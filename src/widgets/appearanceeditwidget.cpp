
#include "appearanceeditwidget.h"

#include <DEdit/dedit_appearance.h>
// widgets
#include "colortrippleedit.h"
#include <QLabel>
#include <QCheckBox>
// layouts
#include <QGridLayout>


AppearanceEditWidget::AppearanceEditWidget(QWidget* parent)
    : QWidget(parent)
{
    m_pAppearance = NULL;
    allocateWidgets();
    createLayouts();
    connectSlots();
    retranslateUi();
    chkAdvancedColors->setChecked(TRUE);
}

AppearanceEditWidget::~AppearanceEditWidget()
{
    
}



void AppearanceEditWidget::allocateWidgets()
{
    wdgStateNormal = new ColorTrippleEdit;
    wdgStateSelected = new ColorTrippleEdit;
    wdgStateExecuted = new ColorTrippleEdit;
    wdgStateResultDenied = new ColorTrippleEdit;
    wdgStateResultAccepted = new ColorTrippleEdit;
    chkAdvancedColors = new QCheckBox;
}

void AppearanceEditWidget::createLayouts()
{
    int row = 0;
    layoutParent = new QGridLayout;
    layoutParent->addWidget(chkAdvancedColors, row++, 2, 1, 2);
    wdgStateNormal->addToGridLayout(layoutParent, row++);
    wdgStateSelected->addToGridLayout(layoutParent, row++);
    wdgStateExecuted->addToGridLayout(layoutParent, row++);
    wdgStateResultDenied->addToGridLayout(layoutParent, row++);
    wdgStateResultAccepted->addToGridLayout(layoutParent, row++);
    
    setLayout(layoutParent);
}

void AppearanceEditWidget::connectSlots()
{
    connect(chkAdvancedColors, SIGNAL(toggled(bool)), wdgStateNormal, SLOT(setAdvancedColorsEnabled(bool)));
    connect(chkAdvancedColors, SIGNAL(toggled(bool)), wdgStateSelected, SLOT(setAdvancedColorsEnabled(bool)));
    connect(chkAdvancedColors, SIGNAL(toggled(bool)), wdgStateExecuted, SLOT(setAdvancedColorsEnabled(bool)));
    connect(chkAdvancedColors, SIGNAL(toggled(bool)), wdgStateResultDenied, SLOT(setAdvancedColorsEnabled(bool)));
    connect(chkAdvancedColors, SIGNAL(toggled(bool)), wdgStateResultAccepted, SLOT(setAdvancedColorsEnabled(bool)));
}

void AppearanceEditWidget::retranslateUi()
{
    wdgStateNormal->setText(tr("Normal:"));
    wdgStateSelected->setText(tr("Selected:"));
    wdgStateExecuted->setText(tr("Executed:"));
    wdgStateResultDenied->setText(tr("Denied:"));
    wdgStateResultAccepted->setText(tr("Accepted:"));
    chkAdvancedColors->setText(tr("Custom Border"));
    
}

void AppearanceEditWidget::setAppearanceToEdit(DEdit_Appearance* app)
{
    m_pAppearance = app;
    if(!m_pAppearance)
    {
        return;
    }
    wdgStateNormal->setColorTripple(&app->m_cStateNormal);
    wdgStateSelected->setColorTripple(&app->m_cStateSelected);
    wdgStateExecuted->setColorTripple(&app->m_cStateExecuted);
    wdgStateResultDenied->setColorTripple(&app->m_cStateResultDenied);
    wdgStateResultAccepted->setColorTripple(&app->m_cStateResultAccepted);
}

DEdit_Appearance* AppearanceEditWidget::appearanceToEdit()
{
    return m_pAppearance;
}



void AppearanceEditWidget::applyChanges()
{
    if(!m_pAppearance)
    {
        return;
    }
    wdgStateNormal->applyChanges();
    wdgStateSelected->applyChanges();
    wdgStateExecuted->applyChanges();
    wdgStateResultDenied->applyChanges();
    wdgStateResultAccepted->applyChanges();
    
}

