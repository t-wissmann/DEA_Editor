#ifndef __DIA_CONFIGUREDEDITWIDGET_H_
#define __DIA_CONFIGUREDEDITWIDGET_H_

#include <QDialog>
#include <multilanguage/translateableobject.h>

class DEdit_Widget;
class DEdit_MainWindow;
class AppearanceEditWidget;

// widgets
class QLabel;
class QCheckBox;
class QSpinBox;
class QPushButton;
class QDialogButtonBox;
class ColorButton;
class QListWidget;
class QGroupBox;
class QStackedWidget;
class QSplitter;
class QSlider;
class QSplitter;
class TranslationManagerWidget;

// layouts
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class Dia_ConfigureDEditWidget : public QDialog,
      public TranslateableObject
{
    Q_OBJECT
public:
    Dia_ConfigureDEditWidget(QWidget* parent = 0);
    ~Dia_ConfigureDEditWidget();
    
    void retranslateUi();
    void reloadIcons();
    
    void setWidgetToEdit(DEdit_Widget* widget);
    DEdit_Widget* widgetToEdit();
    
    void setMainWindowToEdit(DEdit_MainWindow* mainwindow);
    DEdit_MainWindow* mainWindowToEdit();
    
public slots:
    void applyChanges();
    void setCurrentCategoryName(QString name);
    void restoreDefaults();
    void historyClear();
private:
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    
    // widgets
    QPushButton* btnOk;
    QPushButton* btnCancel;
    QPushButton* btnApply;
    QPushButton* btnRestoreDefaults;
    QDialogButtonBox* boxBottom;
    // list on the left
    QListWidget* lstCategory;
    QGroupBox*   grpCategory;
    
    // options
    // behavior
    QCheckBox*   chkAutoEditNewStates;
    QCheckBox*   chkAutoEditNewTransitions;
    QCheckBox*   chkAlignToGrid;
    QLabel*      lblGridResolution;
    QSpinBox*    spinGridResolution;
    // appearance
    AppearanceEditWidget* wdgAppearance;
    // proportions
    QLabel*      lblStateDiameter;
    QSlider*     slidStateDiameter;
    QSpinBox*    spinStateDiameter;
    QLabel*      lblTransitionLineWidth;
    QSlider*     slidTransitionLineWidth;
    QSpinBox*    spinTransitionLineWidth;
    // history
    QHBoxLayout* layoutHistorySize;
    QLabel*      lblHistorySize;
    QSlider*     slidHistorySize;
    QSpinBox*    spinHistorySize;
    QPushButton* btnHistoryClear;
    // language
    TranslationManagerWidget* wdgTranslations;
    
    
    
    
    
    // layouts
    QVBoxLayout* layoutBehavior;
    QGridLayout* layoutProportions;
    QVBoxLayout* layoutHistory;
    
    QStackedWidget* stackCategory;
    QSplitter*      splitterCentral;
    QVBoxLayout*    layoutParent;
    QHBoxLayout*    layoutGridResolution;
    
    
    DEdit_Widget* m_pWidgetToEdit;
    DEdit_MainWindow* m_pMainWindowToEdit;
};

#endif
