

#ifndef __APPEARANCEEDITWIDGET_H_
#define __APPEARANCEEDITWIDGET_H_

#include <QWidget>

class DEdit_Appearance;

// widgets
class ColorTrippleEdit;
class QCheckBox;
// layouts
class QGridLayout;


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
    QCheckBox*        chkAdvancedColors;
    ColorTrippleEdit* wdgStateNormal;
    ColorTrippleEdit* wdgStateSelected;
    ColorTrippleEdit* wdgStateExecuted;
    ColorTrippleEdit* wdgStateResultDenied;
    ColorTrippleEdit* wdgStateResultAccepted;
    
    // layouts
    QGridLayout*    layoutParent;
    
    // members
    DEdit_Appearance* m_pAppearance;
};

#endif
