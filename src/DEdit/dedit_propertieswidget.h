
#ifndef __DEDIT_PROPERTIESWIDGET_H_
#define __DEDIT_PROPERTIESWIDGET_H_


#include <QWidget>
#include <multilanguage/translateableobject.h>

class DEA;
// widgets
class QLabel;
class QFrame;
class QLineEdit;
// layouts
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;


class DEdit_PropertiesWidget : public QWidget,
      public TranslateableObject
{
    Q_OBJECT
public:
    DEdit_PropertiesWidget(QWidget* parent = 0);
    virtual ~DEdit_PropertiesWidget();
    
    void setDea(DEA* dea);
    DEA* dea();
    
    void retranslateUi();
    static void initPropertyValueWidgets(QLineEdit* lineedit);
    static void initPropertyNameWidgets(QLabel* lineedit);
public slots:
    void refreshFromDea();
    
private:
    // init functions
    void allocateWidget();
    void createLayouts();
    void connectSlots();
    void initWidgets();
    
    // widgets
    QLabel* lblName;
    QLineEdit* txtName;
    QLabel* lblDescription;
    QLineEdit* txtDescription;
    // properties
    QFrame*    frmPropeties;
    QLabel*    lblStates;
    QLineEdit*    txtStates;
    QLabel*    lblTransitions;
    QLineEdit*    txtTransitions;
    QLabel*    lblAlphabet;
    QLineEdit*    txtAlphabet;
    QLabel*    lblStartState;
    QLineEdit*    txtStartState;
    QLabel*    lblFinalStates;
    QLineEdit*    txtFinalStates;
    
    
    // layouts
    QGridLayout* layoutNameDescription;
    QGridLayout* layoutProperties;
    
    QVBoxLayout* layoutParent;
    
    DEA* m_pDea;
};


#endif


