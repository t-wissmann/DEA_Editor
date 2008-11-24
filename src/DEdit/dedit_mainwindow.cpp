
#include "dedit_mainwindow.h"
#include <DEdit/dedit_execdeawidget.h>
#include <DEdit/dedit_propertieswidget.h>
#include <io/xmlparser.h>
#include <io/iconcatcher.h>
#include <io/xmlloader.h>
#include <io/configio.h>

// dialogs
#include <dialogs/dia_deasourceviewer.h>
#include <dialogs/dia_configurededitwidget.h>
#include <dialogs/dia_about.h>
#include <QMessageBox>
#include <QFileDialog>

// widgets
#include <DEdit/dedit_widget.h>
#include <QPushButton>
#include <widgets/commandbuttondnd.h>
#include <QStatusBar>
#include <QMenuBar>
#include <QDockWidget>
#include <QScrollArea>
#include <QToolBar>
#include <QSpacerItem>

// other qt-classes
#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QPixmap>
#include <QDir>
#include <QFileInfo>
#include <QSizePolicy>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QUrl>
#include <QKeyEvent>
#include <QWhatsThis>

// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>

DEdit_MainWindow::DEdit_MainWindow()
    : QMainWindow()
{
    
    // enable vertical tabbar for docks
    setDockOptions(AnimatedDocks | AllowNestedDocks | AllowTabbedDocks | VerticalTabs);
    initMembers();
    allocateWidgets();
    createLayouts();
    createActions();
    createToolBars();
    createMenuBar();
    connectSlots();
    initWidgets();
    retranslateUi();
    reloadIcons();
    parseArguments();
    // activate drag'n'drop
    setAcceptDrops(TRUE);
    
    resize(800, 600);
    
    // now load configuration
    loadConfig();
    
}

DEdit_MainWindow::~DEdit_MainWindow()
{
    
}


void DEdit_MainWindow::initMembers()
{
    // init dialogs
    m_diaSourceViewer = NULL;
    m_diaConfigureDEditWidget = NULL;
    m_diaAbout = NULL;
    // other members
    m_szBackgroundColor = "Window";
}

void DEdit_MainWindow::allocateWidgets()
{
    wdgEditor = new DEdit_Widget;
    // tool buttons
    btnAddState = new CommandButtonDND(wdgEditor->dndMimeFormat(), wdgEditor->dndAddStateCommand() );
    btnAddState->setDNDPixmap(wdgEditor->stateTemplatePixmap());
    btnAddTransition = new QPushButton;
    btnRemoveItem = new QPushButton;
    btnEditItem = new QPushButton;
    btnMoveUp = new QPushButton;
    btnMoveUp->setVisible(FALSE);
    btnMoveDown = new QPushButton;
    btnMoveDown->setVisible(FALSE);
    itemToolButtonStretch = new QSpacerItem(1, 1);
    
    btnStretchToolButtons = new QPushButton;
    btnStretchToolButtons->setCheckable(TRUE);
    
    // create statusbar
    statusBar();
}

void DEdit_MainWindow::createLayouts()
{
    
    // dock tool buttons
    dockToolButtons = new QDockWidget;
    layoutToolButtons = new QVBoxLayout;
    layoutToolButtons->setMargin(4);
    layoutToolButtons->addWidget(btnAddState);
    layoutToolButtons->addWidget(btnAddTransition);
    layoutToolButtons->addWidget(btnRemoveItem);
    layoutToolButtons->addWidget(btnEditItem);
    //layoutToolButtons->addStretch();
    layoutToolButtons->addWidget(btnMoveUp);
    layoutToolButtons->addWidget(btnMoveDown);
    layoutToolButtons->addWidget(btnStretchToolButtons);
    layoutToolButtons->addItem(itemToolButtonStretch);
    QWidget* wdgFoo = new QWidget; // only a widget to add a layout to a dockwidget
    wdgFoo->setLayout(layoutToolButtons);
    dockToolButtons->setWidget(wdgFoo);
    addDockWidget(Qt::LeftDockWidgetArea, dockToolButtons);
    
    // dock properties
    dockProperties =  new QDockWidget;
    wdgProperties = new DEdit_PropertiesWidget;
    dockProperties->setWidget(wdgProperties);
    addDockWidget(Qt::RightDockWidgetArea, dockProperties);
    
    // dock exec dea
    dockExecDea = new QDockWidget;
    wdgExecDea = new DEdit_ExecDeaWidget;
    dockExecDea->setWidget(wdgExecDea);
    addDockWidget(Qt::BottomDockWidgetArea, dockExecDea);
    // scroll area
    scrollCentral = new QScrollArea;
    scrollCentral->setWidget(wdgEditor);
    scrollCentral->setWidgetResizable(TRUE);
    
    //scrollCentral->setFrameStyle(QFrame::NoFrame);
    scrollCentral->setLineWidth(0);
    
    QPalette pal = scrollCentral->palette();
    QColor bgColor = pal.color(QPalette::Window);
    bgColor.setAlpha(0);
    pal.setColor(QPalette::Window, bgColor); // make background transparent
    scrollCentral->setPalette(pal);
    
    layoutParent = new QHBoxLayout;
    layoutParent->setMargin(0);
    layoutParent->setSpacing(0);
    layoutParent->addWidget(scrollCentral);
    
    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layoutParent);
    setCentralWidget(centralWidget);
}

void DEdit_MainWindow::createActions()
{
    QWidget* pActionParent = this;
    // mnuFile
    mnaNewFile = new QAction(pActionParent);
    mnaOpen = new QAction(pActionParent);
    mnaSave = new QAction(pActionParent);
    mnaSaveAs = new QAction(pActionParent);
    mnaQuit = new QAction(pActionParent);
    // mnuEdit
    mnaUndo = new QAction(pActionParent);
    mnaRedo = new QAction(pActionParent);
    // mnuView
    mnaShowSourceCode = new QAction(pActionParent);
    mnaShowToolButtonsDock = dockToolButtons->toggleViewAction();
    mnaShowProperties = dockProperties->toggleViewAction();
    mnaShowExecDeaDock = dockExecDea->toggleViewAction();
    mnaStetchToolButtons = new QAction(pActionParent);
    mnaStetchToolButtons->setCheckable(TRUE);
    // mnuSettings
    mnaShowToolBar = NULL; // will be set in createToolBars();
    mnaShowStatusBar = new QAction(pActionParent);
    mnaShowStatusBar->setCheckable(TRUE);
    mnaShowStatusBar->setChecked(TRUE);
    mnaShowMenuBar = new QAction(pActionParent);
    mnaShowMenuBar->setCheckable(TRUE);
    mnaShowMenuBar->setChecked(TRUE);
    mnaConfigureEditor = new QAction(pActionParent);
    // mnuHelp
    mnaWhatsThis = QWhatsThis::createAction(pActionParent);
    mnaAbout = new QAction(pActionParent);
    mnaAboutQt = new QAction(pActionParent);
}

void DEdit_MainWindow::createToolBars()
{
    tlbMainToolBar = addToolBar("main toolbar");
    tlbMainToolBar->addAction(mnaNewFile);
    tlbMainToolBar->addAction(mnaOpen);
    tlbMainToolBar->addAction(mnaSave);
    tlbMainToolBar->addAction(mnaSaveAs);
    tlbMainToolBar->addSeparator();
    tlbMainToolBar->addAction(mnaUndo);
    tlbMainToolBar->addAction(mnaRedo);
    
    mnaShowToolBar = tlbMainToolBar->toggleViewAction();
}
void DEdit_MainWindow::createMenuBar()
{
    mnuFile = menuBar()->addMenu("file");
    mnuFile->addAction(mnaNewFile);
    mnuFile->addAction(mnaOpen);
    mnuFile->addSeparator();
    mnuFile->addAction(mnaSave);
    mnuFile->addAction(mnaSaveAs);
    mnuFile->addSeparator();
    mnuFile->addAction(mnaQuit);
    
    mnuEdit = menuBar()->addMenu("edit");
    mnuEdit->addAction(mnaUndo);
    mnuEdit->addAction(mnaRedo);
    
    mnuView = menuBar()->addMenu("view");
    mnuView->addAction(mnaShowToolButtonsDock);
    mnuView->addAction(mnaShowProperties);
    mnuView->addAction(mnaShowExecDeaDock);
    mnuView->addAction(mnaStetchToolButtons);
    mnuView->addSeparator();
    mnuView->addAction(mnaShowSourceCode);
    
    mnuSettings = menuBar()->addMenu("settings");
    mnuSettings->addAction(mnaShowToolBar);
    mnuSettings->addAction(mnaShowMenuBar);
    mnuSettings->addAction(mnaShowStatusBar);
    mnuSettings->addSeparator();
    mnuSettings->addAction(mnaConfigureEditor);
    
    mnuHelp = menuBar()->addMenu("help");
    mnuHelp->addAction(mnaWhatsThis);
    mnuHelp->addSeparator();
    mnuHelp->addAction(mnaAbout);
    mnuHelp->addAction(mnaAboutQt);
}


void DEdit_MainWindow::connectSlots()
{
    connect(btnAddState, SIGNAL(clicked()), wdgEditor, SLOT(addState()));
    connect(btnRemoveItem, SIGNAL(clicked()), wdgEditor, SLOT(removeSelectedItem()));
    connect(btnMoveUp, SIGNAL(clicked()), wdgEditor, SLOT(moveSelectionUp()));
    connect(btnMoveDown, SIGNAL(clicked()), wdgEditor, SLOT(moveSelectionDown()));
    connect(btnAddTransition, SIGNAL(clicked()), wdgEditor, SLOT(addTransition()));
    connect(btnEditItem, SIGNAL(clicked()), wdgEditor, SLOT(editItem()));
    connect(btnStretchToolButtons, SIGNAL(toggled(bool)), this, SLOT(setStretchToolButtons(bool)));
    
    
    connect(wdgEditor, SIGNAL(currentModeChanged(DEdit_Widget::EMode)), this,
            SLOT(resetStatusBarText(DEdit_Widget::EMode)));
    connect(wdgEditor, SIGNAL(deaWasChanged()), wdgProperties, SLOT(refreshFromDea()));
    connect(wdgEditor, SIGNAL(historyChanged()), this, SLOT(reinitEditMenu()));
    
    // connections for actions
    // mnuFile
    connect(mnaNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(mnaOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(mnaSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(mnaSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(mnaQuit, SIGNAL(triggered()), this, SLOT(close()));
    // mnaEdit
    connect(mnaUndo, SIGNAL(triggered()), this, SLOT(undo()));
    connect(mnaRedo, SIGNAL(triggered()), this, SLOT(redo()));
    // mnuView
    connect(mnaStetchToolButtons, SIGNAL(toggled(bool)), this, SLOT(setStretchToolButtons(bool)));
    connect(mnaShowSourceCode, SIGNAL(triggered()), this, SLOT(showSourceCode()));
    // mnuSettings
    // mnaShowToolBar was already connected in createToolBars()
    connect(mnaShowStatusBar, SIGNAL(toggled(bool)), statusBar(), SLOT(setVisible(bool)));
    connect(mnaShowMenuBar, SIGNAL(toggled(bool)), menuBar(), SLOT(setVisible(bool)));
    connect(mnaConfigureEditor, SIGNAL(triggered()), this, SLOT(showConfigureEditorDialog()));
    // mnuHelp
    connect(mnaAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(mnaAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void DEdit_MainWindow::initWidgets()
{
    wdgEditor->setDea(&m_cDea);
    wdgProperties->setDea(&m_cDea);
    wdgExecDea->setDeaWidget(wdgEditor);
    
    // set object names
    dockToolButtons->setObjectName("DockToolButtons");
    dockProperties->setObjectName("DockProperties");
    dockExecDea->setObjectName("DockExecDea");
    tlbMainToolBar->setObjectName("ToolBarMain");
    
    // activate stretch per default
    setStretchToolButtons(TRUE);
    // hide stretch button in dockwidget per default
    setStretchToolButtonsButtonVisible(FALSE);
    
    setFrameVisible(FALSE);
    setBackgroundColor("Window");
    // init undo / redo
    reinitEditMenu();
}

void DEdit_MainWindow::retranslateUi()
{
    resetWindowTitle();
    // dock widgets
    
    dockToolButtons->setWindowTitle(tr("Tool Buttons"));
    dockExecDea->setWindowTitle(tr("Execute Dea"));
    dockProperties->setWindowTitle(tr("Dea Properties"));
    
    // tool bars
    tlbMainToolBar->setWindowTitle(tr("Main Tool Bar"));
    
    // tool buttons
    btnAddState->setText(tr("Add State"));
    btnAddState->setWhatsThis(tr("This adds a new State.\n"
				 "You also can drag this button to the position, where the State shall be added."));
    btnAddTransition->setText(tr("Add Transition"));
    btnAddTransition->setWhatsThis(tr("Click to add a new transition.\n"
				 "Then click on the first state of the transition, then on the second state."));
    btnRemoveItem->setText(tr("Remove"));
    btnRemoveItem->setWhatsThis(tr("Removes the currently selected State or Transition"));
    btnEditItem->setText(tr("Edit"));
    btnEditItem->setWhatsThis(tr("Edits the currently selected State or Transition"));
    btnMoveUp->setText(tr("Move up"));
    btnMoveDown->setText(tr("Move down"));
    btnStretchToolButtons->setText(tr("Stretch"));
    
    
    // actions
    // mnuFile
    mnaNewFile->setText(tr("New"));
    mnaOpen->setText(tr("Open ..."));
    mnaSave->setText(tr("Save"));
    mnaSaveAs->setText(tr("Save as ..."));
    mnaQuit->setText(tr("Quit"));
    mnaQuit->setShortcut(tr("Ctrl+Q"));
    // mnuEdit
    mnaUndo->setText(tr("Undo"));
    mnaUndo->setShortcut(tr("Ctrl+Z"));
    mnaRedo->setText(tr("Redo"));
    mnaRedo->setShortcut(tr("Ctrl+Shift+Z"));
    // mnuView
    mnaShowToolButtonsDock->setText(tr("Show Tool Buttons"));
    mnaShowExecDeaDock->setText(tr("Show \'Execute Dea\'"));
    mnaShowProperties->setText(tr("Show Properties"));
    mnaStetchToolButtons->setText(tr("Stretch Tool Buttons"));
    mnaShowSourceCode->setText(tr("Show Source Code"));
    // mnuSettings
    mnaShowToolBar->setText(tr("Show Toolbar"));
    mnaShowMenuBar->setText(tr("Show Menubar"));
    mnaShowMenuBar->setShortcut(tr("Ctrl+M"));
    mnaShowStatusBar->setText(tr("Show Statusbar"));
    mnaConfigureEditor->setText(tr("Configure Editor"));
    // mnuHelp
    mnaWhatsThis->setText(tr("What's this?"));
    mnaAbout->setText(tr("About Dea Editor"));
    mnaAboutQt->setText(tr("About Qt"));
    // menus
    mnuFile->setTitle(tr("&File"));
    mnuEdit->setTitle(tr("&Edit"));
    mnuView->setTitle(tr("&View"));
    mnuSettings->setTitle(tr("&Settings"));
    mnuHelp->setTitle(tr("&Help"));
}

void DEdit_MainWindow::reloadIcons()
{
    setWindowIcon(IconCatcher::getIcon("dea_editor", 48));
    // actions
    mnaNewFile->setIcon(IconCatcher::getMenuBarToolBarIcon("filenew"));
    mnaOpen->setIcon(IconCatcher::getMenuBarToolBarIcon("fileopen"));
    mnaSave->setIcon(IconCatcher::getMenuBarToolBarIcon("filesave"));
    mnaSaveAs->setIcon(IconCatcher::getMenuBarToolBarIcon("filesaveas"));
    mnaQuit->setIcon(IconCatcher::getIcon("exit"));
    mnaUndo->setIcon(IconCatcher::getMenuBarToolBarIcon("undo"));
    mnaRedo->setIcon(IconCatcher::getMenuBarToolBarIcon("redo"));
    
    mnaConfigureEditor->setIcon(IconCatcher::getIcon("configure"));
    mnaWhatsThis->setIcon(IconCatcher::getIcon("help", 16));
    mnaAbout->setIcon(IconCatcher::getIcon("dea_editor", 48));
    // tool buttons
    btnAddState->setIcon(IconCatcher::getIcon("add"));
    btnAddTransition->setIcon(IconCatcher::getIcon("add"));
    btnRemoveItem->setIcon(IconCatcher::getIcon("editdelete"));
    btnEditItem->setIcon(IconCatcher::getIcon("edit"));
    btnMoveUp->setIcon(IconCatcher::getIcon("up"));
    btnMoveDown->setIcon(IconCatcher::getIcon("down"));
}


void DEdit_MainWindow::parseArguments()
{
    QStringList args = QApplication::arguments();
    QString fileToLoad = "";
    for(int i = 0; i < args.size(); ++i)
    {
        if(args[i].endsWith(".xml"))
        {
            fileToLoad = args[i];
        }
    }
    if(!fileToLoad.isEmpty())
    {
        QString result = loadFromFile(fileToLoad);
        if(!result.isEmpty())
        {
            QMessageBox::critical(this, tr("Error when saving file"), result);
        }
        else
        {   // if opening was successfull
            // then we have a new filename
            m_szFilename = fileToLoad;
            resetWindowTitle();
        }
    }
}

void DEdit_MainWindow::keyPressEvent(QKeyEvent* event)
{
    if((event->key() == Qt::Key_M) && (event->modifiers() & Qt::ControlModifier) )
    {
        mnaShowMenuBar->setChecked(!mnaShowMenuBar->isChecked());
        event->accept();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}


void DEdit_MainWindow::resetStatusBarText(DEdit_Widget::EMode mode)
{
    QString msg;
    
    switch(mode)
    {
        case DEdit_Widget::ModeDragState:{
            msg = tr("Drag State to wanted position");
            break;
        }
        case DEdit_Widget::ModeDragTransition:{
            msg = tr("Drag Transition to wanted curve");
            break;
        }
        case DEdit_Widget::ModeAddTransitionSelectFrom:{
            msg = tr("Click on the first state, and keep the mouse button pressed");
            break;
        }
        case DEdit_Widget::ModeAddTransitionSelectTo:{
            msg = tr("Now, drag transition to second state and release the mouse button");
            break;
        }
        case DEdit_Widget::ModeLocked:{
            msg = tr("DEA is running, so it is currently not possible to modify the DEA");
            break;
        }
        default: {
            msg = "";
            break;
        }
    }
    statusBar()->showMessage(msg);
}


void DEdit_MainWindow::showSourceCode()
{
    if(!m_diaSourceViewer)
    {
        m_diaSourceViewer = new Dia_DeaSourceViewer(this);
        m_diaSourceViewer->setDeaToView(wdgEditor);
    }
    m_diaSourceViewer->refresh();
    m_diaSourceViewer->show();
}


void DEdit_MainWindow::showConfigureEditorDialog()
{
    if(!m_diaConfigureDEditWidget)
    {
        m_diaConfigureDEditWidget = new Dia_ConfigureDEditWidget(this);
        // close this dialog, if main window gets closed
        m_diaConfigureDEditWidget->setAttribute(Qt::WA_QuitOnClose, FALSE);
    }
    m_diaConfigureDEditWidget->setWidgetToEdit(wdgEditor);
    m_diaConfigureDEditWidget->setMainWindowToEdit(this);
    m_diaConfigureDEditWidget->show();
}


void DEdit_MainWindow::saveFile()
{
    if(m_szFilename.isEmpty())
    {
        saveFileAs();
    }
    else
    {
        QString result = saveToFile(m_szFilename);
        if(!result.isEmpty())
        {
            QMessageBox::critical(this, tr("Error when saving file"), result);
        }
    }
}

void DEdit_MainWindow::saveFileAs()
{
    QString otherFilename = QFileDialog::getSaveFileName(this, tr("Save File"),
            m_szFilename,
            tr("DEA Files (*.xml)"));
    if(otherFilename.isEmpty())
    {
        // if cancel was clicked
        return;
    }
    if(!otherFilename.endsWith(".xml"))
    {
        otherFilename += ".xml";
    }
    
    QString result = saveToFile(otherFilename);
    if(!result.isEmpty())
    {
        QMessageBox::critical(this, tr("Error when saving file"), result);
    }
    else
    {
        // if file save was successfull
        // then we have a new filename
        m_szFilename = otherFilename;
        // filename has been changed
        resetWindowTitle(); // so reset filename in window title
    }
}

QString DEdit_MainWindow::saveToFile(QString filename) // returns errormsg
{
    QString szResult = "";
    xmlObject tree;
    wdgEditor->writeDeaToFile(&tree);
    int nResult = WriteClassToFile(filename.toLocal8Bit().data(), &tree);
    if(nResult != 0)
    {
        szResult = tr("Error when writing to \'%filename\'").replace("%filename", filename);
        szResult += "\n" + tr("You propably don't have the necessery Write-Rights there.");
    }
    else
    {
        statusBar()->showMessage(tr("File %filename successfully written").replace("%filename", filename), 3000);
    }
    
    return szResult;
}


void DEdit_MainWindow::newFile()
{
    if(userReallyWantsToCloseFile())
    {
        m_szFilename = ""; // clear current filename
        // clear dea
        wdgEditor->clearCompleteDEA();
        resetWindowTitle();
    }
}

void DEdit_MainWindow::openFile()
{
    QString openDir = m_szFilename;
    if(openDir.isEmpty())
    {
        openDir = QDir::homePath();
    }
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
            openDir, tr("DEA Files (*.xml)"));
    if(filename.isEmpty())
    {
        return;
    }
    
    if(!userReallyWantsToCloseFile())
    {
        return;
    }
    QString result = loadFromFile(filename);
    if(!result.isEmpty())
    {
        QMessageBox::critical(this, tr("Error when opening file"), result);
    }
    else
    {   // if opening was successfull
        // then we have a new filename
        m_szFilename = filename;
    }
    resetWindowTitle();
    
}

QString DEdit_MainWindow::loadFromFile(QString filename) // returns errormsg
{
    QString szResult = "";
    xmlLoader loader;
    xmlObject tree;
    if(!loader.loadFileToClass(filename.toLocal8Bit().data(), &tree))
    {
        szResult = tr("Error when loading \'%filename\'").replace("%filename", filename) + "\n";
        szResult += tr("Propably you donnot have the necessary Read-Rights "
                            "or the file doesnot exist.");
    }
    else if(!wdgEditor->createDeaFromFile(&tree))
    {
        szResult += tr("Error when parsing \'%filename\'").replace("%filename", filename) + "\n";
        szResult += tr("Syntax error:") + "\n";
        szResult += wdgEditor->lastSyntaxError();
    }
    else
    {
        // new file, new history
        wdgEditor->clearHistory();
        // print success to status bar :)
        statusBar()->showMessage(tr("File %filename successfully loaded").replace("%filename", filename), 3000);
    }
    return szResult;
}


void DEdit_MainWindow::showAboutDialog()
{
    if(!m_diaAbout)
    {
        m_diaAbout = new Dia_About(this);
    }
    m_diaAbout->show();
}


void DEdit_MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
    {
        QList<QUrl> urls = event->mimeData()->urls();
        if(urls.isEmpty())
        {
            return;
        }
        QString filename = urls.first().toLocalFile();
        if(filename.endsWith(".xml"))
        {
            event->acceptProposedAction();
        }
    }
}

void DEdit_MainWindow::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
    {
        return;
    }
    QString filename = urls.first().toLocalFile();
    if(filename.isEmpty())
    {
        return;
    }
    QString result = loadFromFile(filename);
    if(!result.isEmpty())
    {
        QMessageBox::critical(this, tr("Error when opening file"), result);
    }
    else
    {// if opening was successfull
        // then we have a new filename
        m_szFilename = filename;
        resetWindowTitle();
    }
}



void DEdit_MainWindow::closeEvent(QCloseEvent* event)
{
    if(!event)
    {
        return;
    }
    if(userReallyWantsToCloseFile())
    {
        saveConfig();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}


void DEdit_MainWindow::undo()
{
    wdgEditor->undo();
}

void DEdit_MainWindow::redo()
{
    wdgEditor->redo();
}

void DEdit_MainWindow::reinitEditMenu()
{
    //qDebug("reinit edit menu");
    mnaUndo->setEnabled(wdgEditor->isUndoPossible());
    mnaRedo->setEnabled(wdgEditor->isRedoPossible());
}


void DEdit_MainWindow::saveConfig()
{
    ConfigIO config;
    config.setMainWindow(this);
    config.setEditorWidget(wdgEditor);
    config.saveConfig();
}

void DEdit_MainWindow::loadConfig()
{
    ConfigIO config;
    config.setMainWindow(this);
    config.setEditorWidget(wdgEditor);
    config.loadConfig();
}



bool DEdit_MainWindow::userReallyWantsToCloseFile()
{
    return TRUE;
}


void DEdit_MainWindow::resetWindowTitle()
{
    QString szNewWindowTitle;
    QString szFilename = tr("New File");
    if(!m_szFilename.isEmpty())
    {
        QStringList list = m_szFilename.split(QDir::separator());
        if(list.size() > 0)
        {
            szFilename = list.last();
        }
    }
    szNewWindowTitle = szFilename;
    szNewWindowTitle += " - ";
    szNewWindowTitle += tr("Dea Editor");
    setWindowTitle(szNewWindowTitle);
}


TranslationManager* DEdit_MainWindow::translationManager()
{
    return &m_TranslationManager;
}


void DEdit_MainWindow::setStretchToolButtons(bool on)
{
    QSizePolicy newPolicy = btnAddState->sizePolicy();
    if(on)
    {
        itemToolButtonStretch->changeSize(0, 0);
        newPolicy = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    else
    {
        itemToolButtonStretch->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        newPolicy = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    // apply new size policy to buttons
    btnAddState->setSizePolicy(newPolicy);
    btnAddTransition->setSizePolicy(newPolicy);
    btnRemoveItem->setSizePolicy(newPolicy);
    btnEditItem->setSizePolicy(newPolicy);
    btnMoveUp->setSizePolicy(newPolicy);
    btnMoveDown->setSizePolicy(newPolicy);
    btnStretchToolButtons->setSizePolicy(newPolicy);
    // update layout
    layoutToolButtons->update();
    
    mnaStetchToolButtons->setChecked(on);
    btnStretchToolButtons->setChecked(on);
}


bool DEdit_MainWindow::areToolButtonsStretched() const
{
    return btnStretchToolButtons->isChecked();
}



bool DEdit_MainWindow::isStretchToolButtonsButtonVisible() const
{
    return btnStretchToolButtons->isVisible();
}

void DEdit_MainWindow::setStretchToolButtonsButtonVisible(bool visible)
{
    btnStretchToolButtons->setVisible(visible);
}


void DEdit_MainWindow::setFrameVisible(bool visible)
{
    if(visible)
    {
        scrollCentral->setLineWidth(1);
        // set to default frame style of scroll views
        scrollCentral->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    }
    else
    {
        scrollCentral->setFrameStyle(QFrame::NoFrame);
        scrollCentral->setLineWidth(0);
    }
}

bool DEdit_MainWindow::isFrameVisible() const
{
    return (0 != scrollCentral->lineWidth());
}


void DEdit_MainWindow::setBackgroundColor(QString color)
{
    QBrush newBackground = wdgEditor->palette().window();
    if(color.toLower() == "window")
    {
        newBackground = QBrush(QColor(255, 255, 255, 0)); // transparent
    }
    else if(color.toLower() == "base")
    {
        newBackground = palette().base();
    }
    else if(QColor(color).isValid())
    {
        newBackground = QBrush(QColor(color));
    }
    else
    {
        return;
    }
    // apply changes
    m_szBackgroundColor = color;
    QPalette pal = wdgEditor->palette();
    pal.setBrush(QPalette::Window, newBackground);
    wdgEditor->setPalette(pal);
    
}

QString DEdit_MainWindow::backgroundColor() const
{
    return m_szBackgroundColor;
}


void DEdit_MainWindow::setMoveUpMoveDownButtonsVisible(bool visible)
{
    btnMoveUp->setVisible(visible);
    btnMoveDown->setVisible(visible);
}


bool DEdit_MainWindow::areMoveUpMoveDownButtonsVisible() const
{
    return  btnMoveUp->isVisible();
}

