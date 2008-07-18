
#include "dedit_edittransitiondia.h"

#include <DEdit/dedit_graphicaltransition.h>

// widgets
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QLineEdit>

// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>


DEdit_EditTransitionDia::DEdit_EditTransitionDia(QWidget* parent)
    : QDialog(parent)
{
    m_pTransitionToEdit = NULL;
    
    // create gui
    allocateWidgets();
    createLayouts();
    connectSlots();
    retranslateUi();
}

DEdit_EditTransitionDia::~DEdit_EditTransitionDia()
{
    
}


void DEdit_EditTransitionDia::allocateWidgets()
{
    boxBottom = new QDialogButtonBox;
    btnOk     = boxBottom->addButton("ok", QDialogButtonBox::AcceptRole);
    btnCancel = boxBottom->addButton("cancel", QDialogButtonBox::RejectRole);
    
    lblSymbols = new QLabel;
    txtSymbols = new QLineEdit;
    
}

void DEdit_EditTransitionDia::createLayouts()
{
    layoutSymbols = new QHBoxLayout;
    layoutSymbols->setMargin(0);
    layoutSymbols->addWidget(lblSymbols);
    layoutSymbols->addWidget(txtSymbols);
    
    layoutParent = new QVBoxLayout;
    layoutParent->addLayout(layoutSymbols);
    layoutParent->addWidget(boxBottom);
    setLayout(layoutParent);
}

void DEdit_EditTransitionDia::connectSlots()
{
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btnOk, SIGNAL(clicked()), this, SLOT(applyChanges()));
}

void DEdit_EditTransitionDia::retranslateUi()
{
    setWindowTitle(tr("Edit Transition"));
    btnOk->setText(tr("OK"));
    btnCancel->setText(tr("Cancel"));
    lblSymbols->setText(tr("Symbols to accept:"));
}

void DEdit_EditTransitionDia::applyChanges()
{
    if(!m_pTransitionToEdit)
    {
        return;
    }
    m_pTransitionToEdit->m_szSymbols = txtSymbols->text();
    m_pTransitionToEdit->applySymbolsToDEA_Transitions();
    
    accept();
}



void DEdit_EditTransitionDia::setTransitionToEdit(DEdit_GraphicalTransition* transition)
{
    m_pTransitionToEdit = transition;
    if(!m_pTransitionToEdit)
    {
        return;
    }
    txtSymbols->setText(m_pTransitionToEdit->m_szSymbols);
}

DEdit_GraphicalTransition* DEdit_EditTransitionDia::transitionToEdit()
{
    return m_pTransitionToEdit;
}


