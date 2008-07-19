#include "dedit_editstatedia.h"

#include <DEdit/dedit_graphicalstate.h>
#include <DEdit/dedit_widget.h>
#include <DEA/dea_state.h>
#include <io/iconcatcher.h>
// widgets
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

// dialogs
#include <QMessageBox>

DEdit_EditStateDia::DEdit_EditStateDia(DEdit_Widget* parent)
    : QDialog(parent)
{
    m_pParent = parent;
    m_pStateToEdit = NULL;
    // init gui
    allocateWidgets();
    createLayouts();
    connectSlots();
    retranslateUi();
    reloadIcons();
}

DEdit_EditStateDia::~DEdit_EditStateDia()
{
    
}

void DEdit_EditStateDia::allocateWidgets()
{
    
    lblName = new QLabel;
    txtName = new QLineEdit;
    chkIsFinalState = new QCheckBox;
    chkIsStartState = new QCheckBox;
    
    // buttons on the bottom
    boxBottom = new QDialogButtonBox;
    btnOk     = boxBottom->addButton("ok", QDialogButtonBox::AcceptRole);
    btnCancel = boxBottom->addButton("cancel", QDialogButtonBox::RejectRole);
}

void DEdit_EditStateDia::createLayouts()
{
    
    layoutName = new QHBoxLayout;
    layoutName->setMargin(0);
    layoutName->addWidget(lblName);
    layoutName->addWidget(txtName);
    
    
    layoutParent = new QVBoxLayout;
    layoutParent->addLayout(layoutName);
    layoutParent->addWidget(chkIsStartState);
    layoutParent->addWidget(chkIsFinalState);
    layoutParent->addWidget(boxBottom);
    
    setLayout(layoutParent);
}

void DEdit_EditStateDia::connectSlots()
{
    connect(btnOk, SIGNAL(clicked()), this, SLOT(applyChanges()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


void DEdit_EditStateDia::retranslateUi()
{
    setWindowTitle(tr("Edit State"));
    
    lblName->setText(tr("Name:"));
    chkIsStartState->setText(tr("Start State"));
    chkIsFinalState->setText(tr("Final State"));
    // buttons on the bottom
    btnOk->setText(tr("OK"));
    btnCancel->setText(tr("Cancel"));
}

void DEdit_EditStateDia::reloadIcons()
{
    btnOk->setIcon(IconCatcher::getIcon("button_ok"));
    btnCancel->setIcon(IconCatcher::getIcon("button_cancel"));
}

void DEdit_EditStateDia::setStateToEdit(DEdit_GraphicalState* state)
{
    m_pStateToEdit = state;
    if(!m_pStateToEdit || !m_pStateToEdit->m_pData)
    {
        return;
    }
    txtName->setText(m_pStateToEdit->m_pData->name());
    chkIsFinalState->setChecked(m_pStateToEdit->m_pData->isFinalState());
    chkIsStartState->setChecked(m_pStateToEdit->m_bStartState);
}

DEdit_GraphicalState* DEdit_EditStateDia::stateToEdit()
{
    return m_pStateToEdit;
}


void DEdit_EditStateDia::applyChanges()
{
    if(!m_pStateToEdit || !m_pStateToEdit->m_pData)
    {
        return;
    }
    QString name = txtName->text();
    DEdit_GraphicalState* item = m_pParent ? m_pParent->findStateByName(name) : NULL;
    if(item != m_pStateToEdit && item != NULL)
    {
        QString msg, title;
        title = tr("Name already exists");
        msg = tr("There already is a state named %statename")
                .replace("%statename", name) + "\n";
        msg += tr("Please choose an other name");
        QMessageBox::critical(this, title, msg);
        return;
    }
    m_pStateToEdit->m_pData->setName(name.toAscii().data());
    m_pStateToEdit->m_pData->setFinalState(chkIsFinalState->isChecked());
    m_pStateToEdit->m_bStartState = chkIsStartState->isChecked();
    accept();
}

