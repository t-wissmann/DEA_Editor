
#include "dedit_propertieswidget.h"

#include <DEA/dea.h>

// widgets
#include <QLabel>
#include <QLineEdit>
// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

DEdit_PropertiesWidget::DEdit_PropertiesWidget(QWidget* parent)
    : QWidget(parent)
{
    // init members
    m_pDea = 0;
    // init gui
    allocateWidget();
    createLayouts();
    connectSlots();
    retranslateUi();
}

DEdit_PropertiesWidget::~DEdit_PropertiesWidget()
{
    
}

void DEdit_PropertiesWidget::setDeaToEdit(DEA* dea)
{
    m_pDea = dea;
}

DEA* DEdit_PropertiesWidget::deaToEdit()
{
    return m_pDea;
}


// init functions
void DEdit_PropertiesWidget::allocateWidget()
{
    lblName = new QLabel;
    txtName = new QLineEdit;
    lblDescription = new QLabel;
    txtDescription = new QLineEdit;
}

void DEdit_PropertiesWidget::createLayouts()
{
    layoutNameDescription = new QGridLayout;
    layoutNameDescription->setMargin(0);
    layoutNameDescription->addWidget(lblName, 0, 0);
    layoutNameDescription->addWidget(txtName, 0, 1);
    layoutNameDescription->addWidget(lblDescription, 1, 0);
    layoutNameDescription->addWidget(txtDescription, 1, 1);
    
    layoutParent = new QVBoxLayout;
    layoutParent->addLayout(layoutNameDescription);
    setLayout(layoutParent);
}

void DEdit_PropertiesWidget::connectSlots()
{
    
}


void DEdit_PropertiesWidget::retranslateUi()
{
    lblName->setText(tr("Name:"));
    lblDescription->setText(tr("Description:"));
}



