

#ifndef __APPEARANCEEDITWIDGET_H_
#define __APPEARANCEEDITWIDGET_H_

#include <QWidget>

#include <multilanguage/translateableobject.h>
class DEdit_Appearance;

// widgets
class ColorTrippleEdit;
class QCheckBox;
class ColorButton;
class QLabel;
class QGroupBox;
class FontButton;
// layouts
class QGridLayout;
class QVBoxLayout;

class AppearanceEditWidget : public QWidget,
      public TranslateableObject
{
    Q_OBJECT
public:
    AppearanceEditWidget(QWidget* parent = 0);
    virtual ~AppearanceEditWidget();
    
    void setAppearanceToEdit(DEdit_Appearance* app);
    DEdit_Appearance* appearanceToEdit();
    void initColorsFromAppearance(DEdit_Appearance* app);
    
    void applyChanges();
    void retranslateUi();
    void restoreDefaults();
    
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
    QLabel*           lblStateLabelFont;
    FontButton*       btnStateLabelFont;

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
    QLabel*         lblTransitionLabelFont;
    FontButton*     btnTransitionLabelFont;
    
    
    // layouts
    QGridLayout*    layoutStateColors;
    QGridLayout*    layoutTransitionColors;
    QVBoxLayout*    layoutParent;
    
    // members
    DEdit_Appearance* m_pAppearance;
};

#endif
