
#include "dedit_mainwindow.h"

// widgets
#include <DEdit/dedit_widget.h>
#include <QPushButton>
#include <widgets/commandbuttondnd.h>

// other qt-classes
#include <QPixmap>

// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>

DEdit_MainWindow::DEdit_MainWindow()
{
    initMembers();
    allocateWidgets();
    createLayouts();
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
    
}

void DEdit_MainWindow::allocateWidgets()
{
    wdgEditor = new DEdit_Widget;
    // tool buttons
    btnAddState = new CommandButtonDND(wdgEditor->dndMimeFormat(), wdgEditor->dndAddStateCommand() );
    btnAddState->setDNDPixmap(wdgEditor->stateTemplatePixmap());
    btnAddTransition = new QPushButton;
    btnRemoveState = new QPushButton;
    btnRemoveTransition = new QPushButton;
    btnMoveUp = new QPushButton;
    btnMoveDown = new QPushButton;
}

void DEdit_MainWindow::createLayouts()
{
    layoutToolButtons = new QVBoxLayout;
    layoutToolButtons->addWidget(btnAddState);
    layoutToolButtons->addWidget(btnRemoveState);
    layoutToolButtons->addWidget(btnAddTransition);
    layoutToolButtons->addWidget(btnRemoveTransition);
    layoutToolButtons->addWidget(btnMoveUp);
    layoutToolButtons->addWidget(btnMoveDown);
    
    layoutParent = new QHBoxLayout;
    layoutParent->addLayout(layoutToolButtons);
    layoutParent->addWidget(wdgEditor);
    
    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layoutParent);
    setCentralWidget(centralWidget);
}

void DEdit_MainWindow::connectSlots()
{
    connect(btnAddState, SIGNAL(clicked()), wdgEditor, SLOT(addState()));
    connect(btnRemoveState, SIGNAL(clicked()), wdgEditor, SLOT(removeState()));
    connect(btnRemoveTransition, SIGNAL(clicked()), wdgEditor, SLOT(removeTransition()));
    connect(btnMoveUp, SIGNAL(clicked()), wdgEditor, SLOT(moveSelectionUp()));
    connect(btnMoveDown, SIGNAL(clicked()), wdgEditor, SLOT(moveSelectionDown()));
    connect(btnAddTransition, SIGNAL(clicked()), wdgEditor, SLOT(addTransition()));
}

void DEdit_MainWindow::initWidgets()
{
    wdgEditor->setDea(&m_cDea);
}

void DEdit_MainWindow::retranslateUi()
{
    setWindowTitle(tr("DEA Editor"));
    // tool buttons
    btnAddState->setText(tr("Add State"));
    btnRemoveState->setText(tr("Remove State"));
    btnAddTransition->setText(tr("Add Transition"));
    btnRemoveTransition->setText(tr("Remove Transition"));
    btnMoveUp->setText(tr("Move up"));
    btnMoveDown->setText(tr("Move down"));
}

