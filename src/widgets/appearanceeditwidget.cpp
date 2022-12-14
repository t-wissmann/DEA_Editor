
#include "appearanceeditwidget.h"

#include <DEdit/dedit_appearance.h>
// widgets
#include "colortrippleedit.h"
#include <QLabel>
#include <widgets/colorbutton.h>
#include <widgets/fontbutton.h>
#include <QCheckBox>
#include <QGroupBox>
// layouts
#include <QGridLayout>
#include <QHBoxLayout>


AppearanceEditWidget::AppearanceEditWidget(QWidget* parent)
    : QWidget(parent)
{
    m_pAppearance = NULL;
    allocateWidgets();
    createLayouts();
    connectSlots();
    retranslateUi();
    chkAdvancedColors->setChecked(true);
}

AppearanceEditWidget::~AppearanceEditWidget()
{
    
}



void AppearanceEditWidget::allocateWidgets()
{
    // states
    grpStateColors = new QGroupBox;
    wdgStateNormal = new ColorTrippleEdit;
    wdgStateSelected = new ColorTrippleEdit;
    wdgStateExecuted = new ColorTrippleEdit;
    wdgStateResultDenied = new ColorTrippleEdit;
    wdgStateResultAccepted = new ColorTrippleEdit;
    chkAdvancedColors = new QCheckBox;
    lblStateLabelColor = new QLabel;
    btnStateLabelColor = new ColorButton;
    btnStateLabelColor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lblStateLabelFont = new QLabel;
    btnStateLabelFont = new FontButton;
    // transitions
    grpTransitionColors = new QGroupBox;
    
    lblTransitionNormal = new QLabel;
    btnTransitionNormal = new ColorButton;
    btnTransitionNormal->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lblTransitionHovered = new QLabel;
    btnTransitionHovered = new ColorButton;
    btnTransitionHovered->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lblTransitionSelected = new QLabel;
    btnTransitionSelected = new ColorButton;
    btnTransitionSelected->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lblTransitionExecuted = new QLabel;
    btnTransitionExecuted = new ColorButton;
    btnTransitionExecuted->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lblTransitionLabelColor = new QLabel;
    btnTransitionLabelColor = new ColorButton;
    btnTransitionLabelColor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lblTransitionLabelFont = new QLabel;
    btnTransitionLabelFont = new FontButton;
    
}

void AppearanceEditWidget::createLayouts()
{
    // states
    int row = 0;
    layoutStateColors = new QGridLayout;
    layoutStateColors->addWidget(chkAdvancedColors, row++, 2, 1, 2);
    wdgStateNormal->addToGridLayout(layoutStateColors, row++);
    wdgStateSelected->addToGridLayout(layoutStateColors, row++);
    wdgStateExecuted->addToGridLayout(layoutStateColors, row++);
    wdgStateResultDenied->addToGridLayout(layoutStateColors, row++);
    wdgStateResultAccepted->addToGridLayout(layoutStateColors, row++);
    
    layoutStateColors->addWidget(lblStateLabelColor, row, 0);
    layoutStateColors->addWidget(btnStateLabelColor, row++, 1);
    
    layoutStateColors->addWidget(lblStateLabelFont, row, 0);
    layoutStateColors->addWidget(btnStateLabelFont, row++, 1);
    
    grpStateColors->setLayout(layoutStateColors);
    
    // transitions
    row = 0;
    layoutTransitionColors = new QGridLayout;
    layoutTransitionColors->addWidget(lblTransitionNormal, row, 0);
    layoutTransitionColors->addWidget(btnTransitionNormal, row++, 1);
    layoutTransitionColors->addWidget(lblTransitionHovered, row, 0);
    layoutTransitionColors->addWidget(btnTransitionHovered, row++, 1);
    layoutTransitionColors->addWidget(lblTransitionSelected, row, 0);
    layoutTransitionColors->addWidget(btnTransitionSelected, row++, 1);
    layoutTransitionColors->addWidget(lblTransitionExecuted, row, 0);
    layoutTransitionColors->addWidget(btnTransitionExecuted, row++, 1);
    layoutTransitionColors->addWidget(lblTransitionLabelColor, row, 0);
    layoutTransitionColors->addWidget(btnTransitionLabelColor, row++, 1);
    layoutTransitionColors->addWidget(lblTransitionLabelFont, row, 0);
    layoutTransitionColors->addWidget(btnTransitionLabelFont, row++, 1);
    
    grpTransitionColors->setLayout(layoutTransitionColors);
    
    // layout parent
    layoutParent = new QVBoxLayout;
    layoutParent->addWidget(grpStateColors);
    layoutParent->addWidget(grpTransitionColors);
    
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
    lblStateLabelColor->setText(tr("Label color:"));
    lblStateLabelFont->setText(tr("Font:"));
    grpStateColors->setTitle(tr("Colors for States"));
    
    grpTransitionColors->setTitle(tr("Colors for Transitions"));
    lblTransitionNormal->setText(tr("Normal:"));
    lblTransitionHovered->setText(tr("Hovered:"));
    lblTransitionSelected->setText(tr("Selected:"));
    lblTransitionExecuted->setText(tr("Executed:"));
    lblTransitionLabelColor->setText(tr("Label color:"));
    lblTransitionLabelFont->setText(tr("Font:"));
    
}

void AppearanceEditWidget::setAppearanceToEdit(DEdit_Appearance* app)
{
    m_pAppearance = app;
    initColorsFromAppearance(m_pAppearance);
    if(!app)
    {
        return;
    }
    // states
    wdgStateNormal->setColorTripple(&app->m_cStateNormal);
    wdgStateSelected->setColorTripple(&app->m_cStateSelected);
    wdgStateExecuted->setColorTripple(&app->m_cStateExecuted);
    wdgStateResultDenied->setColorTripple(&app->m_cStateResultDenied);
    wdgStateResultAccepted->setColorTripple(&app->m_cStateResultAccepted);
}


void AppearanceEditWidget::initColorsFromAppearance(DEdit_Appearance* app)
{
    if(!app)
    {
        return;
    }
    
    // states
    wdgStateNormal->initColorsFromColorTripple(&app->m_cStateNormal);
    wdgStateSelected->initColorsFromColorTripple(&app->m_cStateSelected);
    wdgStateExecuted->initColorsFromColorTripple(&app->m_cStateExecuted);
    wdgStateResultDenied->initColorsFromColorTripple(&app->m_cStateResultDenied);
    wdgStateResultAccepted->initColorsFromColorTripple(&app->m_cStateResultAccepted);
    btnStateLabelColor->setColor(app->m_cStateLabelColor);
    btnStateLabelFont->setFont(app->m_cStateLabelFont);
    chkAdvancedColors->setChecked(true);
    
    // transitions
    btnTransitionNormal->setColor(app->m_cTransitionNormal);
    btnTransitionHovered->setColor(app->m_cTransitionHovered);
    btnTransitionSelected->setColor(app->m_cTransitionSelected);
    btnTransitionExecuted->setColor(app->m_cTransitionExecuted);
    btnTransitionLabelColor->setColor(app->m_cTransitionLabelColor);
    btnTransitionLabelFont->setFont(app->m_cTransitionLabelFont);
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
    // states
    wdgStateNormal->applyChanges();
    wdgStateSelected->applyChanges();
    wdgStateExecuted->applyChanges();
    wdgStateResultDenied->applyChanges();
    wdgStateResultAccepted->applyChanges();
    m_pAppearance->m_cStateLabelColor = btnStateLabelColor->color();
    m_pAppearance->m_cStateLabelFont  = btnStateLabelFont->font();
    // transitions
    m_pAppearance->m_cTransitionNormal = btnTransitionNormal->color();
    m_pAppearance->m_cTransitionHovered = btnTransitionHovered->color();
    m_pAppearance->m_cTransitionSelected = btnTransitionSelected->color();
    m_pAppearance->m_cTransitionExecuted = btnTransitionExecuted->color();
    m_pAppearance->m_cTransitionLabelColor = btnTransitionLabelColor->color();
    m_pAppearance->m_cTransitionLabelFont  = btnTransitionLabelFont->font();
}


void AppearanceEditWidget::restoreDefaults()
{
    DEdit_Appearance defaultAppearance;
    DEdit_Appearance::createTangoDefault(&defaultAppearance);
    
    initColorsFromAppearance(&defaultAppearance);
}

