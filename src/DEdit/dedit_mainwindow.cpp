
#include "dedit_mainwindow.h"

// dialogs
#include <dialogs/dia_deasourceviewer.h>

// widgets
#include <DEdit/dedit_widget.h>
#include <QPushButton>
#include <widgets/commandbuttondnd.h>
#include <QStatusBar>

// other qt-classes
#include <QPixmap>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QDockWidget>

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
    
    resize(800, 600);
    move(0, 0);
}

DEdit_MainWindow::~DEdit_MainWindow()
{
    
}


void DEdit_MainWindow::initMembers()
{
    // init dialogs
    m_diaSourceViewer = NULL;
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
    btnMoveDown = new QPushButton;
    dockToolButtons = new QDockWidget;
    
    // create statusbar
    statusBar();
}

void DEdit_MainWindow::createLayouts()
{
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
    
    layoutParent = new QHBoxLayout;
    layoutParent->setMargin(0);
    layoutParent->setSpacing(0);
    layoutParent->addWidget(wdgEditor);
    
    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layoutParent);
    setCentralWidget(centralWidget);
}

void DEdit_MainWindow::createActions()
{
    // mnuFile
    mnaQuit = new QAction(NULL);
    // mnuView
    mnaShowSourceCode = new QAction(NULL);
    mnaShowToolButtonsDock = new QAction(NULL);
    mnaShowToolButtonsDock->setCheckable(TRUE);
    mnaShowToolButtonsDock->setChecked(TRUE);
    // mnuSettings
    mnaShowStatusBar = new QAction(NULL);
    mnaShowStatusBar->setCheckable(TRUE);
    mnaShowStatusBar->setChecked(TRUE);
}

void DEdit_MainWindow::createMenuBar()
{
    mnuFile = menuBar()->addMenu("file");
    mnuFile->addAction(mnaQuit);
    
    mnuView = menuBar()->addMenu("view");
    mnuView->addAction(mnaShowToolButtonsDock);
    mnuView->addSeparator();
    mnuView->addAction(mnaShowSourceCode);
    
    mnuSettings = menuBar()->addMenu("settings");
    mnuSettings->addAction(mnaShowStatusBar);
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
    
    // connections for actions
    // mnuFile
    connect(mnaQuit, SIGNAL(triggered()), this, SLOT(close()));
    // mnuView
    connect(mnaShowSourceCode, SIGNAL(triggered()), this, SLOT(showSourceCode()));
    connect(mnaShowToolButtonsDock, SIGNAL(toggled(bool)),
            dockToolButtons, SLOT(setVisible(bool)));
    connect(dockToolButtons, SIGNAL(visibilityChanged(bool)),
            mnaShowToolButtonsDock, SLOT(setChecked(bool)));
    // mnuSettings
    connect(mnaShowStatusBar, SIGNAL(toggled(bool)), statusBar(), SLOT(setVisible(bool)));
    
}

void DEdit_MainWindow::initWidgets()
{
    wdgEditor->setDea(&m_cDea);
}



void DEdit_MainWindow::retranslateUi()
{
    setWindowTitle(tr("DEA Editor"));
    // tool buttons
    dockToolButtons->setWindowTitle(tr("Tool Buttons"));
    btnAddState->setText(tr("Add State"));
    btnAddTransition->setText(tr("Add Transition"));
    btnRemoveItem->setText(tr("Remove"));
    btnEditItem->setText(tr("Edit"));
    btnMoveUp->setText(tr("Move up"));
    btnMoveDown->setText(tr("Move down"));
    // actions
    // mnuFile
    mnaQuit->setText(tr("Quit"));
    // mnuView
    mnaShowToolButtonsDock->setText(tr("Show Tool Buttons"));
    mnaShowSourceCode->setText(tr("Show Source Code"));
    // mnuSettings
    mnaShowStatusBar->setText(tr("Show Statusbar"));
    // menus
    mnuView->setTitle(tr("&View"));
    mnuFile->setTitle(tr("&File"));
    mnuSettings->setTitle(tr("&Settings"));
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
        case DEdit_Widget::ModeAddTransitionSelectFrom:{
            msg = tr("Click on the first state, and keep the mouse button pressed");
            break;
        }
        case DEdit_Widget::ModeAddTransitionSelectTo:{
            msg = tr("Now, drag transition to second state and release the mouse button");
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

