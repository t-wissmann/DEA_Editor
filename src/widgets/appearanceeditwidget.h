

#ifndef __APPEARANCEEDITWIDGET_H_
#define __APPEARANCEEDITWIDGET_H_

#include <QWidget>

class DEdit_Appearance;

// widgets
class ColorTrippleEdit;
class QCheckBox;
class ColorButton;
class QLabel;
class QGroupBox;
// layouts
class QGridLayout;
class QVBoxLayout;

class AppearanceEditWidget : public QWidget
{
    Q_OBJECT
public:
    AppearanceEditWidget(QWidget* parent = 0);
    virtual ~AppearanceEditWidget();
    
    void setAppearanceToEdit(DEdit_Appearance* app);
    DEdit_Appearance* appearanceToEdit();
    
    void applyChanges();
    void retranslateUi();
private:
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    // widgets
    // states
    QGroupBox*        grpStateColors;
    QCheckBox*        chkAdvancedColors;
    ColorTrippleEdit* wdgStateNormal;
    ColorTrippleEdit* wdgStateSelected;
    ColorTrippleEdit* wdgStateExecuted;
    ColorTrippleEdit* wdgStateResultDenied;
    ColorTrippleEdit* wdgStateResultAccepted;
    QLabel*           lblStateLabelColor;
    ColorButton*      btnStateLabelColor;

    // transitions
    QGroupBox*      grpTransitionColors;
    QLabel*         lblTransitionNormal;
    ColorButton*    btnTransitionNormal;
    QLabel*         lblTransitionHovered;
    ColorButton*    btnTransitionHovered;
    QLabel*         lblTransitionSelected;
    ColorButton*    btnTransitionSelected;
    QLabel*         lblTransitionExecuted;
    ColorButton*    btnTransitionExecuted;
    QLabel*         lblTransitionLabelColor;
    ColorButton*    btnTransitionLabelColor;
    
    
    // layouts
    QGridLayout*    layoutStateColors;
    QGridLayout*    layoutTransitionColors;
    QVBoxLayout*    layoutParent;
    
    // members
    DEdit_Appearance* m_pAppearance;
};

#endif
