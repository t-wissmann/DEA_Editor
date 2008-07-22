
#ifndef __DEDIT_PROPERTIESWIDGET_H_
#define __DEDIT_PROPERTIESWIDGET_H_


#include <QWidget>

class DEA;
// widgets
class QLabel;
class QLineEdit;
// layouts
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;


class DEdit_PropertiesWidget : public QWidget
{
    Q_OBJECT
public:
    DEdit_PropertiesWidget(QWidget* parent = 0);
    virtual ~DEdit_PropertiesWidget();
    
    void setDeaToEdit(DEA* dea);
    DEA* deaToEdit();
    
    void retranslateUi();
    
private:
    // init functions
    void allocateWidget();
    void createLayouts();
    void connectSlots();
    
    // widgets
    QLabel* lblName;
    QLineEdit* txtName;
    QLabel* lblDescription;
    QLineEdit* txtDescription;
    
    // layouts
    QGridLayout* layoutNameDescription;
    QVBoxLayout* layoutParent;
    
    DEA* m_pDea;
};


#endif


