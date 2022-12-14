
#include "dia_configurededitwidget.h"

#include <DEdit/dedit_widget.h>
#include <DEdit/dedit_mainwindow.h>
#include <io/iconcatcher.h>

#include <DEdit/dedit_graphicalstate.h>
#include <DEdit/dedit_graphicaltransition.h>

// widgets
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QStackedWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSplitter>
#include <widgets/colorbutton.h>
#include <widgets/appearanceeditwidget.h>
#include <multilanguage/translationmanagerwidget.h>

// layouts
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

// other
#include <QList>

Dia_ConfigureDEditWidget::Dia_ConfigureDEditWidget(QWidget* parent)
    : QDialog(parent)
{
    m_pWidgetToEdit = NULL;
    
    allocateWidgets();
    createLayouts();
    connectSlots();
    
    retranslateUi();
    reloadIcons();
    // init widgets
    lstCategory->setCurrentRow(0);
    splitterCentral->setSizes(QList<int>() << 180 << (splitterCentral->width() - 180));
    chkAlignToGrid->setChecked(true); // force that signal is emitted
    chkAlignToGrid->setChecked(false);
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
    btnRestoreDefaults = boxBottom->addButton("defaults", QDialogButtonBox::ResetRole);
    
    
    
    chkAutoEditNewStates = new QCheckBox;
    chkAutoEditNewTransitions = new QCheckBox;
    chkAlignToGrid = new QCheckBox;
    lblGridResolution = new QLabel;
    spinGridResolution = new QSpinBox;
    spinGridResolution->setRange(1, 100);
    spinGridResolution->setValue(20);
    
    wdgAppearance = new AppearanceEditWidget;
    // proportions
    lblStateDiameter = new QLabel;
    slidStateDiameter = new QSlider(Qt::Horizontal);
    spinStateDiameter = new QSpinBox;
    slidStateDiameter->setRange(22, 500);
    spinStateDiameter->setRange(22, 500);
    
    lblTransitionLineWidth = new QLabel;
    slidTransitionLineWidth = new QSlider(Qt::Horizontal);
    spinTransitionLineWidth = new QSpinBox;
    slidTransitionLineWidth->setRange(1, 40);
    spinTransitionLineWidth->setRange(1, 40);
    
    // history
    lblHistorySize = new QLabel;
    slidHistorySize = new QSlider(Qt::Horizontal);
    spinHistorySize = new QSpinBox;
    spinHistorySize->setRange(0, 40);
    slidHistorySize->setRange(0, 40);
    btnHistoryClear = new QPushButton;
    
    // language / translations
    wdgTranslations = new TranslationManagerWidget;
    
    // listbox
    lstCategory = new QListWidget;
    //lstCategory->setViewMode(QListView::IconMode);
    lstCategory->setIconSize(QSize(200, 84));
    lstCategory->setMovement(QListView::Static);
    lstCategory->setMinimumWidth(140);
    lstCategory->setMaximumWidth(140);
    lstCategory->setSpacing(0);
    lstCategory->setAlternatingRowColors(true);
    
    QListWidgetItem *currentItem = new QListWidgetItem(lstCategory);
    currentItem->setText("behavior");
    currentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    currentItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    currentItem = new QListWidgetItem(lstCategory);
    currentItem->setText("colors");
    currentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    currentItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    currentItem = new QListWidgetItem(lstCategory);
    currentItem->setText("sizes");
    currentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    currentItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    currentItem = new QListWidgetItem(lstCategory);
    currentItem->setText("history");
    currentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    currentItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    currentItem = new QListWidgetItem(lstCategory);
    currentItem->setText("language");
    currentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    currentItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    
    grpCategory = new QGroupBox;
    grpCategory->setFlat(true);
}

void Dia_ConfigureDEditWidget::createLayouts()
{
    // behavior
    layoutGridResolution = new QHBoxLayout;
    layoutGridResolution->setMargin(0);
    layoutGridResolution->addWidget(lblGridResolution);
    layoutGridResolution->addWidget(spinGridResolution);
    
    layoutBehavior = new QVBoxLayout;
    //layoutBehavior->setMargin(0);
    layoutBehavior->addWidget(chkAutoEditNewStates);
    layoutBehavior->addWidget(chkAutoEditNewTransitions);
    layoutBehavior->addWidget(chkAlignToGrid);
    layoutBehavior->addLayout(layoutGridResolution);
    QWidget* wdgBehavior = new QWidget; // create extra widget
    wdgBehavior->setLayout(layoutBehavior); // because stacked layout has no addLayout()
    
    // proportions
    layoutProportions = new QGridLayout;
    int row = 0;
    layoutProportions->addWidget(lblStateDiameter, row, 0);
    layoutProportions->addWidget(slidStateDiameter, row, 1);
    layoutProportions->addWidget(spinStateDiameter, row, 2);
    row++;
    layoutProportions->addWidget(lblTransitionLineWidth, row, 0);
    layoutProportions->addWidget(slidTransitionLineWidth, row, 1);
    layoutProportions->addWidget(spinTransitionLineWidth, row, 2);
    QWidget* wdgProportions = new QWidget;
    wdgProportions->setLayout(layoutProportions);
    
    // history
    layoutHistorySize = new QHBoxLayout;
    layoutHistorySize->setMargin(0);
    layoutHistorySize->addWidget(lblHistorySize);
    layoutHistorySize->addWidget(slidHistorySize);
    layoutHistorySize->addWidget(spinHistorySize);
    
    layoutHistory = new QVBoxLayout;
    layoutHistory->addLayout(layoutHistorySize);
    layoutHistory->addWidget(btnHistoryClear);
    
    QWidget* wdgHistory = new QWidget;
    wdgHistory->setLayout(layoutHistory);
    
    
    //
    stackCategory = new QStackedWidget;
    stackCategory->addWidget(wdgBehavior);
    stackCategory->addWidget(wdgAppearance);
    stackCategory->addWidget(wdgProportions);
    stackCategory->addWidget(wdgHistory);
    stackCategory->addWidget(wdgTranslations);
    
    
    QHBoxLayout* layoutStack = new QHBoxLayout; // create extra layout
    layoutStack->setMargin(0);
    layoutStack->addWidget(stackCategory); // because QGroupBox has no setWidget()
    grpCategory->setLayout(layoutStack);
    
    splitterCentral = new QSplitter;
    splitterCentral->addWidget(lstCategory);
    splitterCentral->addWidget(grpCategory);
    
    layoutParent = new QVBoxLayout;
    layoutParent->setSizeConstraint(QLayout::SetMinimumSize);
    layoutParent->addWidget(splitterCentral);
    layoutParent->addWidget(boxBottom);
    
    // new version:
    // chkAutoEditNewTransitions cant be changed->must always be checked
    chkAutoEditNewTransitions->setVisible(false);
    
    setLayout(layoutParent);
}

void Dia_ConfigureDEditWidget::connectSlots()
{
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnRestoreDefaults, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(this, SIGNAL(accepted()), this, SLOT(applyChanges()));
    connect(btnApply, SIGNAL(clicked()), this, SLOT(applyChanges()));
    connect(chkAlignToGrid, SIGNAL(toggled(bool)), lblGridResolution, SLOT(setEnabled(bool)));
    connect(chkAlignToGrid, SIGNAL(toggled(bool)), spinGridResolution, SLOT(setEnabled(bool)));
    connect(lstCategory, SIGNAL(currentRowChanged(int)), stackCategory, SLOT(setCurrentIndex(int)));
    connect(lstCategory, SIGNAL(currentTextChanged(QString)), this, SLOT(setCurrentCategoryName(QString)));
    connect(slidStateDiameter, SIGNAL(valueChanged(int)), spinStateDiameter, SLOT(setValue(int)));
    connect(spinStateDiameter, SIGNAL(valueChanged(int)), slidStateDiameter, SLOT(setValue(int)));
    connect(slidTransitionLineWidth, SIGNAL(valueChanged(int)), spinTransitionLineWidth, SLOT(setValue(int)));
    connect(spinTransitionLineWidth, SIGNAL(valueChanged(int)), slidTransitionLineWidth, SLOT(setValue(int)));
    connect(slidHistorySize, SIGNAL(valueChanged(int)), spinHistorySize, SLOT(setValue(int)));
    connect(spinHistorySize, SIGNAL(valueChanged(int)), slidHistorySize, SLOT(setValue(int)));
    connect(btnHistoryClear, SIGNAL(clicked()), this, SLOT(historyClear()));
    
}



void Dia_ConfigureDEditWidget::retranslateUi()
{
    setWindowTitle(tr("Configure Editor"));
    
    btnOk->setText(tr("OK"));
    btnCancel->setText(tr("Cancel"));
    btnApply->setText(tr("Apply"));
    btnRestoreDefaults->setText(tr("Defaults"));
    
    chkAutoEditNewStates->setText(tr("Automatically popup edit dialog for new states"));
    chkAutoEditNewTransitions->setText(tr("Automatically popup edit dialog for new transitions"));
    chkAlignToGrid->setText(tr("Align items to grid"));
    lblGridResolution->setText(tr("Grid resolution:"));
    spinGridResolution->setSuffix(" " + tr("pixel"));
    
    // proportions
    lblStateDiameter->setText(tr("State diameter:"));
    spinStateDiameter->setSuffix(" " + tr("px"));
    lblTransitionLineWidth->setText(tr("Transition line width:"));
    spinTransitionLineWidth->setSuffix(" " + tr("px"));
    
    // history
    lblHistorySize->setText(tr("History size:"));
    btnHistoryClear->setText(tr("Clear History"));
    
    // categories
    QListWidgetItem* item;
    item = lstCategory->item(0);
    if(item) item->setText(tr("Behavior"));
    item = lstCategory->item(1);
    if(item) item->setText(tr("Colors"));
    item = lstCategory->item(2);
    if(item) item->setText(tr("Sizes"));
    item = lstCategory->item(3);
    if(item) item->setText(tr("History"));
    item = lstCategory->item(4);
    if(item) item->setText(tr("Language"));
    
}


void Dia_ConfigureDEditWidget::reloadIcons()
{
    btnOk->setIcon(IconCatcher::getIcon("button_ok"));
    btnCancel->setIcon(IconCatcher::getIcon("button_cancel"));
    btnApply->setIcon(IconCatcher::getIcon("apply"));
    btnRestoreDefaults->setIcon(IconCatcher::getIcon("undo"));
    // categories
    QListWidgetItem* item;
    item = lstCategory->item(0);
    if(item) item->setIcon(IconCatcher::getIcon("systemsettings", 48));
    item = lstCategory->item(1);
    if(item) item->setIcon(IconCatcher::getIcon("colors", 48));
    item = lstCategory->item(2);
    if(item) item->setIcon(IconCatcher::getIcon("applications-accessories", 48));
    item = lstCategory->item(3);
    if(item) item->setIcon(IconCatcher::getIcon("history", 48));
    item = lstCategory->item(4);
    if(item) item->setIcon(IconCatcher::getIcon("config-language", 48));
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
    
    // proportions
    spinStateDiameter->setValue(DEdit_GraphicalState::m_nDiameter - 4); // minus 4 for shadows
    spinTransitionLineWidth->setValue(DEdit_GraphicalTransition::m_nLineWidth);
    
    // history
    spinHistorySize->setValue(m_pWidgetToEdit->maxHistorySize());
    
}

DEdit_Widget* Dia_ConfigureDEditWidget::widgetToEdit()
{
    return m_pWidgetToEdit;
}


void Dia_ConfigureDEditWidget::setMainWindowToEdit(DEdit_MainWindow* mainwindow)
{
    m_pMainWindowToEdit = mainwindow;
    if(!m_pMainWindowToEdit)
    {
        return;
    }
    wdgTranslations->setTranslationManager(m_pMainWindowToEdit->translationManager());
}

DEdit_MainWindow* Dia_ConfigureDEditWidget::mainWindowToEdit()
{
    return m_pMainWindowToEdit;
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
    // apply changes to proportions
    DEdit_GraphicalState::m_nDiameter = 4 + spinStateDiameter->value();// plus 4 for shadows
    DEdit_GraphicalTransition::m_nLineWidth = spinTransitionLineWidth->value();
    
    // apply changes to history
    m_pWidgetToEdit->setMaxHistorySize(spinHistorySize->value());
    
    // translate ;D
    wdgTranslations->applyChanges();
    
    // important: recreate all templates so that the new colors have effect
    m_pWidgetToEdit->recreateAllGuiTemplates();
    
}


void Dia_ConfigureDEditWidget::setCurrentCategoryName(QString name)
{
    grpCategory->setTitle(name);
}



void Dia_ConfigureDEditWidget::restoreDefaults()
{
    
    chkAutoEditNewStates->setChecked(false);
    chkAutoEditNewTransitions->setChecked(true);
    
    chkAlignToGrid->setChecked(false);
    spinGridResolution->setValue(20);
    
    spinStateDiameter->setValue(100);
    spinTransitionLineWidth->setValue(5);
    
    
    spinHistorySize->setValue(5);
    
    wdgAppearance->restoreDefaults();
    
    wdgTranslations->restoreDefaults();
}



void Dia_ConfigureDEditWidget::historyClear()
{
    if(!m_pWidgetToEdit)
    {
        return;
    }
    m_pWidgetToEdit->clearHistory();
}



