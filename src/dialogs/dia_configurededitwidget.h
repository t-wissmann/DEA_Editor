#ifndef __DIA_CONFIGUREDEDITWIDGET_H_
#define __DIA_CONFIGUREDEDITWIDGET_H_

#include <QDialog>

class DEdit_Widget;

// widgets
class QLabel;
class QCheckBox;
class QSpinBox;
class QPushButton;
class QDialogButtonBox;
class ColorButton;

// layouts
class QHBoxLayout;
class QVBoxLayout;

class Dia_ConfigureDEditWidget : public QDialog
{
    Q_OBJECT
public:
    Dia_ConfigureDEditWidget(QWidget* parent = 0);
    ~Dia_ConfigureDEditWidget();
    
    void retranslateUi();
    void reloadIcons();
    
    void setWidgetToEdit(DEdit_Widget* widget);
    DEdit_Widget* widgetToEdit();
    
public slots:
    void applyChanges();
private:
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    
    // widgets
    QPushButton* btnOk;
    QPushButton* btnCancel;
    QPushButton* btnApply;
    QDialogButtonBox* boxBottom;
    // options
    QCheckBox*   chkAutoEditNewStates;
    QCheckBox*   chkAutoEditNewTransitions;
    QCheckBox*   chkAlignToGrid;
    QLabel*      lblGridResolution;
    QSpinBox*    spinGridResolution;
    QWidget*     wdgGridResolution;
    
    
    
    // layouts
    QVBoxLayout* layoutParent;
    QHBoxLayout* layoutGridResolution;
    
    
    DEdit_Widget* m_pWidgetToEdit;
};

#endif
