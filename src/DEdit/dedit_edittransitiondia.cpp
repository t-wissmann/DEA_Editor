
#include "dedit_edittransitiondia.h"

#include <DEdit/dedit_graphicaltransition.h>
#include <io/iconcatcher.h>

// widgets
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <widgets/commandbuttondnd.h>

// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>


DEdit_EditTransitionDia::DEdit_EditTransitionDia(QWidget* parent)
    : QDialog(parent)
{
    m_pTransitionToEdit = NULL;
    
    // create gui
    allocateWidgets();
    createLayouts();
    connectSlots();
    retranslateUi();
    reloadIcons();
}

DEdit_EditTransitionDia::~DEdit_EditTransitionDia()
{
    
}


void DEdit_EditTransitionDia::allocateWidgets()
{
    boxBottom = new QDialogButtonBox;
    btnOk     = boxBottom->addButton("ok", QDialogButtonBox::AcceptRole);
    btnApply  = boxBottom->addButton("apply", QDialogButtonBox::RejectRole);
    btnCancel = boxBottom->addButton("cancel", QDialogButtonBox::RejectRole);
    
    lblSymbols = new QLabel;
    txtSymbols = new QLineEdit;
    
    // commadn buttons
    btnAllUpperLetters = new CommandButtonDND("text/plain", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    btnAllLowerLetters = new CommandButtonDND("text/plain", "abcdefghijklmnopqrstuvwxyz");
    btnAllLetters = new CommandButtonDND("text/plain",
                         btnAllLowerLetters->command() + btnAllUpperLetters->command());
    btnAllDigits = new CommandButtonDND("text/plain", "0123456789");
    
}

void DEdit_EditTransitionDia::createLayouts()
{
    layoutSymbols = new QHBoxLayout;
    layoutSymbols->setMargin(0);
    layoutSymbols->addWidget(lblSymbols);
    layoutSymbols->addWidget(txtSymbols);
    
    layoutCommands = new QGridLayout;
    layoutCommands->addWidget(btnAllLetters, 0, 0);
    layoutCommands->addWidget(btnAllDigits, 0, 1);
    layoutCommands->addWidget(btnAllLowerLetters, 1, 0);
    layoutCommands->addWidget(btnAllUpperLetters, 1, 1);
    
    layoutParent = new QVBoxLayout;
    layoutParent->addLayout(layoutSymbols);
    layoutParent->addLayout(layoutCommands);
    layoutParent->addWidget(boxBottom);
    setLayout(layoutParent);
}

void DEdit_EditTransitionDia::connectSlots()
{
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(this, SIGNAL(accepted()), this, SLOT(applyChanges()));
    connect(btnApply, SIGNAL(clicked()), this, SLOT(applyChanges()));
    
    // command buttons
    connect(btnAllLetters, SIGNAL(clicked()), this, SLOT(setToAllLetters()));
    connect(btnAllDigits, SIGNAL(clicked()), this, SLOT(setToAllDigits()));
    connect(btnAllLowerLetters, SIGNAL(clicked()), this, SLOT(setToAllLowerLetters()));
    connect(btnAllUpperLetters, SIGNAL(clicked()), this, SLOT(setToAllUpperLetters()));
}

void DEdit_EditTransitionDia::retranslateUi()
{
    setWindowTitle(tr("Edit Transition"));
    btnOk->setText(tr("OK"));
    btnApply->setText(tr("Apply"));
    btnCancel->setText(tr("Cancel"));
    lblSymbols->setText(tr("Symbols to accept:"));
    
    btnAllLetters->setText(tr("All letters"));
    btnAllDigits->setText(tr("All Digits"));
    btnAllUpperLetters->setText(tr("All upper letters"));
    btnAllLowerLetters->setText(tr("All lower letters"));
}

void DEdit_EditTransitionDia::reloadIcons()
{
    btnOk->setIcon(IconCatcher::getIcon("button_ok"));
    btnApply->setIcon(IconCatcher::getIcon("apply"));
    btnCancel->setIcon(IconCatcher::getIcon("button_cancel"));
}

void DEdit_EditTransitionDia::applyChanges()
{
    if(!m_pTransitionToEdit)
    {
        return;
    }
    m_pTransitionToEdit->setSymbols(txtSymbols->text());
    if(parentWidget())
    {
        parentWidget()->update();
    }
}


void DEdit_EditTransitionDia::setToAllLetters()
{
    txtSymbols->setText("abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

void DEdit_EditTransitionDia::setToAllUpperLetters()
{
    txtSymbols->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

void DEdit_EditTransitionDia::setToAllLowerLetters()
{
    txtSymbols->setText("abcdefghijklmnopqrstuvwxyz");
}

void DEdit_EditTransitionDia::setToAllDigits()
{
    txtSymbols->setText("0123456789");
}



void DEdit_EditTransitionDia::setTransitionToEdit(DEdit_GraphicalTransition* transition)
{
    m_pTransitionToEdit = transition;
    if(!m_pTransitionToEdit)
    {
        return;
    }
    txtSymbols->setText(m_pTransitionToEdit->symbols());
}

DEdit_GraphicalTransition* DEdit_EditTransitionDia::transitionToEdit()
{
    return m_pTransitionToEdit;
}


