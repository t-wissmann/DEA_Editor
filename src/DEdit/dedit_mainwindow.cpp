
#include "dedit_mainwindow.h"
#include <DEdit/dedit_execdeawidget.h>
#include <DEdit/dedit_propertieswidget.h>
#include <io/xmlparser.h>
#include <io/iconcatcher.h>
#include <io/xmlloader.h>

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

// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>

DEdit_MainWindow::DEdit_MainWindow()
{
    initMembers();
    allocateWidgets();
    createLayouts();
    createActions();
    createMenuBar();
    connectSlots();
    initWidgets();
    retranslateUi();
    reloadIcons();
    // activate drag'n'drop
    setAcceptDrops(TRUE);
    
    resize(800, 600);
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
    dockToolButtons = new QDockWidget;
    // dock properties
    dockProperties =  new QDockWidget;
    wdgProperties = new DEdit_PropertiesWidget;
    
    // exec dea
    dockExecDea = new QDockWidget;
    wdgExecDea = new DEdit_ExecDeaWidget;
    wdgExecDea->setDeaWidget(wdgEditor);
    
    
    // create statusbar
    statusBar();
}

void DEdit_MainWindow::createLayouts()
{
    // dock tool buttons
    layoutToolButtons = new QVBoxLayout;
    layoutToolButtons->setMargin(4);
    layoutToolButtons->addWidget(btnAddState);
    layoutToolButtons->addWidget(btnAddTransition);
    layoutToolButtons->addWidget(btnRemoveItem);
    layoutToolButtons->addWidget(btnEditItem);
    layoutToolButtons->addStretch();
    layoutToolButtons->addWidget(btnMoveUp);
    layoutToolButtons->addWidget(btnMoveDown);
    QWidget* wdgFoo = new QWidget; // only a widget to add a layout to a dockwidget
    wdgFoo->setLayout(layoutToolButtons);
    dockToolButtons->setWidget(wdgFoo);
    addDockWidget(Qt::LeftDockWidgetArea, dockToolButtons);
    
    // dock properties
    dockProperties->setWidget(wdgProperties);
    addDockWidget(Qt::RightDockWidgetArea, dockProperties);
    
    // dock exec dea
    dockExecDea->setWidget(wdgExecDea);
    addDockWidget(Qt::BottomDockWidgetArea, dockExecDea);
    // scroll area
    scrollCentral = new QScrollArea;
    scrollCentral->setWidget(wdgEditor);
    scrollCentral->setWidgetResizable(TRUE);
    scrollCentral->setFrameStyle(QFrame::NoFrame);
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
    // mnuFile
    mnaNewFile = new QAction(NULL);
    mnaOpen = new QAction(NULL);
    mnaSave = new QAction(NULL);
    mnaSaveAs = new QAction(NULL);
    mnaQuit = new QAction(NULL);
    // mnuView
    mnaShowSourceCode = new QAction(NULL);
    mnaShowToolButtonsDock = new QAction(NULL);
    mnaShowToolButtonsDock->setCheckable(TRUE);
    mnaShowToolButtonsDock->setChecked(TRUE);
    mnaShowProperties = new QAction(NULL);
    mnaShowProperties->setCheckable(TRUE);
    mnaShowProperties->setChecked(TRUE);
    mnaShowExecDeaDock = new QAction(NULL);
    mnaShowExecDeaDock->setCheckable(TRUE);
    mnaShowExecDeaDock->setChecked(TRUE);
    // mnuSettings
    mnaShowStatusBar = new QAction(NULL);
    mnaShowStatusBar->setCheckable(TRUE);
    mnaShowStatusBar->setChecked(TRUE);
    mnaConfigureEditor = new QAction(NULL);
    // mnuHelp
    mnaAbout = new QAction(NULL);
    mnaAboutQt = new QAction(NULL);
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
    
    mnuView = menuBar()->addMenu("view");
    mnuView->addAction(mnaShowToolButtonsDock);
    mnuView->addAction(mnaShowProperties);
    mnuView->addAction(mnaShowExecDeaDock);
    mnuView->addSeparator();
    mnuView->addAction(mnaShowSourceCode);
    
    mnuSettings = menuBar()->addMenu("settings");
    mnuSettings->addAction(mnaShowStatusBar);
    mnuSettings->addAction(mnaConfigureEditor);
    
    mnuHelp = menuBar()->addMenu("help");
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
    connect(wdgEditor, SIGNAL(currentModeChanged(DEdit_Widget::EMode)), this,
            SLOT(resetStatusBarText(DEdit_Widget::EMode)));
    connect(wdgEditor, SIGNAL(deaWasChanged()), wdgProperties, SLOT(refreshFromDea()));
    
    // connections for actions
    // mnuFile
    connect(mnaNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(mnaOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(mnaSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(mnaSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(mnaQuit, SIGNAL(triggered()), this, SLOT(close()));
    // mnuView
    connect(mnaShowSourceCode, SIGNAL(triggered()), this, SLOT(showSourceCode()));
    connect(mnaShowToolButtonsDock, SIGNAL(toggled(bool)), dockToolButtons, SLOT(setVisible(bool)));
    connect(dockToolButtons, SIGNAL(visibilityChanged(bool)), mnaShowToolButtonsDock, SLOT(setChecked(bool)));
    connect(mnaShowExecDeaDock, SIGNAL(toggled(bool)), dockExecDea, SLOT(setVisible(bool)));
    connect(dockExecDea, SIGNAL(visibilityChanged(bool)), mnaShowExecDeaDock, SLOT(setChecked(bool)));
    connect(mnaShowProperties, SIGNAL(toggled(bool)), dockProperties, SLOT(setVisible(bool)));
    connect(dockProperties, SIGNAL(visibilityChanged(bool)), mnaShowProperties, SLOT(setChecked(bool)));
    // mnuSettings
    connect(mnaShowStatusBar, SIGNAL(toggled(bool)), statusBar(), SLOT(setVisible(bool)));
    connect(mnaConfigureEditor, SIGNAL(triggered()), this, SLOT(showConfigureEditorDialog()));
    // mnuHelp
    connect(mnaAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(mnaAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void DEdit_MainWindow::initWidgets()
{
    wdgEditor->setDea(&m_cDea);
    wdgProperties->setDea(&m_cDea);
    
    // disable tabs for docks
    setDockOptions(AnimatedDocks);
    
}

void DEdit_MainWindow::retranslateUi()
{
    setWindowTitle(tr("Dea Editor"));
    // dock widgets
    dockToolButtons->setWindowTitle(tr("Tool Buttons"));
    dockExecDea->setWindowTitle(tr("Execute Dea"));
    dockProperties->setWindowTitle(tr("Dea Properties"));
    // tool buttons
    btnAddState->setText(tr("Add State"));
    btnAddTransition->setText(tr("Add Transition"));
    btnRemoveItem->setText(tr("Remove"));
    btnEditItem->setText(tr("Edit"));
    btnMoveUp->setText(tr("Move up"));
    btnMoveDown->setText(tr("Move down"));
    
    
    // actions
    // mnuFile
    mnaNewFile->setText(tr("New"));
    mnaOpen->setText(tr("Open ..."));
    mnaSave->setText(tr("Save"));
    mnaSaveAs->setText(tr("Save as ..."));
    mnaQuit->setText(tr("Quit"));
    // mnuView
    mnaShowToolButtonsDock->setText(tr("Show Tool Buttons"));
    mnaShowExecDeaDock->setText(tr("Show \'Execute Dea\'"));
    mnaShowProperties->setText(tr("Show Properties"));
    mnaShowSourceCode->setText(tr("Show Source Code"));
    // mnuSettings
    mnaShowStatusBar->setText(tr("Show Statusbar"));
    mnaConfigureEditor->setText(tr("Configure Editor"));
    // mnuHelp
    mnaAbout->setText(tr("About Dea Editor"));
    mnaAboutQt->setText(tr("About Qt"));
    // menus
    mnuView->setTitle(tr("&View"));
    mnuFile->setTitle(tr("&File"));
    mnuSettings->setTitle(tr("&Settings"));
    mnuHelp->setTitle(tr("&Help"));
}

void DEdit_MainWindow::reloadIcons()
{
    setWindowIcon(IconCatcher::getIcon("dea_editor", 48));
    // actions
    mnaNewFile->setIcon(IconCatcher::getIcon("filenew"));
    mnaOpen->setIcon(IconCatcher::getIcon("fileopen"));
    mnaSave->setIcon(IconCatcher::getIcon("filesave"));
    mnaSaveAs->setIcon(IconCatcher::getIcon("filesaveas"));
    mnaQuit->setIcon(IconCatcher::getIcon("exit"));
    mnaConfigureEditor->setIcon(IconCatcher::getIcon("configure"));
    mnaAbout->setIcon(IconCatcher::getIcon("dea_editor", 48));
    // tool buttons
    btnAddState->setIcon(IconCatcher::getIcon("add"));
    btnAddTransition->setIcon(IconCatcher::getIcon("add"));
    btnRemoveItem->setIcon(IconCatcher::getIcon("editdelete"));
    btnEditItem->setIcon(IconCatcher::getIcon("edit"));
    btnMoveUp->setIcon(IconCatcher::getIcon("up"));
    btnMoveDown->setIcon(IconCatcher::getIcon("down"));
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
    m_szFilename = ""; // clear current filename
    // clear dea
    wdgEditor->clearCompleteDEA();
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
    QString result = loadFromFile(filename);
    if(!result.isEmpty())
    {
        QMessageBox::critical(this, tr("Error when opening file"), result);
    }
    else
    {// if opening was successfull
        // then we have a new filename
        m_szFilename = filename;
        
    }
    
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
    }
}

