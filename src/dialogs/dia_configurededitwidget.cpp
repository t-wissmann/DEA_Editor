
#include "dia_configurededitwidget.h"

#include <DEdit/dedit_widget.h>
#include <io/iconcatcher.h>

// widgets
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <widgets/colorbutton.h>
#include <widgets/appearanceeditwidget.h>

// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>


Dia_ConfigureDEditWidget::Dia_ConfigureDEditWidget(QWidget* parent)
    : QDialog(parent)
{
    m_pWidgetToEdit = NULL;
    
    allocateWidgets();
    createLayouts();
    connectSlots();
    
    retranslateUi();
    reloadIcons();
}

Dia_ConfigureDEditWidget::~Dia_ConfigureDEditWidget()
{
    
}



void Dia_ConfigureDEditWidget::allocateWidgets()
{
    boxBottom = new QDialogButtonBox;
    btnOk     = boxBottom->addButton("ok", QDialogButtonBox::AcceptRole);
    btnApply  = boxBottom->addButton("apply", QDialogButtonBox::ApplyRole);
    btnCancel = boxBottom->addButton("cancel", QDialogButtonBox::RejectRole);
    
    
    
    chkAutoEditNewStates = new QCheckBox;
    chkAutoEditNewTransitions = new QCheckBox;
    chkAlignToGrid = new QCheckBox;
    lblGridResolution = new QLabel;
    spinGridResolution = new QSpinBox;
    spinGridResolution->setRange(1, 100);
    spinGridResolution->setValue(20);
    wdgGridResolution = new QWidget;
    wdgGridResolution->setEnabled(FALSE);
    
    wdgAppearance = new AppearanceEditWidget;
    
}

void Dia_ConfigureDEditWidget::createLayouts()
{
    
    layoutGridResolution = new QHBoxLayout;
    layoutGridResolution->setMargin(0);
    layoutGridResolution->addWidget(lblGridResolution);
    layoutGridResolution->addWidget(spinGridResolution);
    wdgGridResolution->setLayout(layoutGridResolution);
    
    layoutParent = new QVBoxLayout;
    layoutParent->addWidget(chkAutoEditNewStates);
    layoutParent->addWidget(chkAutoEditNewTransitions);
    layoutParent->addWidget(chkAlignToGrid);
    layoutParent->addWidget(wdgGridResolution);
    layoutParent->addWidget(wdgAppearance);
    layoutParent->addWidget(boxBottom);
    
    // new version:
    // chkAutoEditNewTransitions cant be changed->must always be checked
    chkAutoEditNewTransitions->setVisible(FALSE);
    
    setLayout(layoutParent);
}

void Dia_ConfigureDEditWidget::connectSlots()
{
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(this, SIGNAL(accepted()), this, SLOT(applyChanges()));
    connect(btnApply, SIGNAL(clicked()), this, SLOT(applyChanges()));
    connect(chkAlignToGrid, SIGNAL(toggled(bool)), wdgGridResolution, SLOT(setEnabled(bool)));
}

void Dia_ConfigureDEditWidget::retranslateUi()
{
    setWindowTitle(tr("Configure Editor"));
    
    btnOk->setText(tr("OK"));
    btnCancel->setText(tr("Cancel"));
    btnApply->setText(tr("Apply"));
    
    chkAutoEditNewStates->setText(tr("Automatically popup edit dialog for new states"));
    chkAutoEditNewTransitions->setText(tr("Automatically popup edit dialog for new transitions"));
    chkAlignToGrid->setText(tr("Align items to grid"));
    lblGridResolution->setText(tr("Grid resolution:"));
    spinGridResolution->setSuffix(" " + tr("pixel"));
    
}


void Dia_ConfigureDEditWidget::reloadIcons()
{
    btnOk->setIcon(IconCatcher::getIcon("button_ok"));
    btnCancel->setIcon(IconCatcher::getIcon("button_cancel"));
    btnApply->setIcon(IconCatcher::getIcon("apply"));
}


void Dia_ConfigureDEditWidget::setWidgetToEdit(DEdit_Widget* widget)
{
    m_pWidgetToEdit = widget;
    if(!m_pWidgetToEdit)
    {
        return;
    }
    
    // auto edit
    chkAutoEditNewStates->setChecked(widget->autoEditNewStates());
    chkAutoEditNewTransitions->setChecked(widget->autoEditNewTransitions());
    
    // grid
    int gridResolution = m_pWidgetToEdit->gridResolution();
    chkAlignToGrid->setChecked(gridResolution > 0);
    if(gridResolution > 0)
    {
        spinGridResolution->setValue(gridResolution);
    }
    
    //apperance
    wdgAppearance->setAppearanceToEdit(widget->appearance());
}

DEdit_Widget* Dia_ConfigureDEditWidget::widgetToEdit()
{
    return m_pWidgetToEdit;
}


void Dia_ConfigureDEditWidget::applyChanges()
{
    if(!m_pWidgetToEdit)
    {
        return;
    }
    // auto edit
    m_pWidgetToEdit->setAutoEditNewStates(chkAutoEditNewStates->isChecked());
    m_pWidgetToEdit->setAutoEditNewTransitions(chkAutoEditNewTransitions->isChecked());
    
    // grid
    int gridResolution = 0;
    if(chkAlignToGrid->isChecked())
    {
        gridResolution = spinGridResolution->value();
    }
    m_pWidgetToEdit->setGridResolution(gridResolution);
    
    // apply changes to appearance
    wdgAppearance->applyChanges();
    // important: recreate all templates so that the new colors have effect
    m_pWidgetToEdit->recreateAllGuiTemplates();
    
}

