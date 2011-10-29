
#ifndef __DEDIT_PROPERTIESWIDGET_H_
#define __DEDIT_PROPERTIESWIDGET_H_


#include <QWidget>
#include <multilanguage/translateableobject.h>

class DEA;
// widgets
class QLabel;
class QFrame;
class QLineEdit;
class QTextEdit;
#include <QTextCursor>
// layouts
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

#define DEdit_Properties_TextWidget QTextEdit
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
    static void initPropertyValueWidgets(DEdit_Properties_TextWidget* lineedit);
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
    DEdit_Properties_TextWidget* txtName;
    QLabel* lblDescription;
    DEdit_Properties_TextWidget* txtDescription;
    // properties
    QFrame*    frmPropeties;
    QLabel*    lblStates;
    DEdit_Properties_TextWidget*    txtStates;
    QLabel*    lblTransitions;
    DEdit_Properties_TextWidget*    txtTransitions;
    QLabel*    lblAlphabet;
    DEdit_Properties_TextWidget*    txtAlphabet;
    QLabel*    lblStartState;
    DEdit_Properties_TextWidget*    txtStartState;
    QLabel*    lblFinalStates;
    DEdit_Properties_TextWidget*    txtFinalStates;
    QTextCursor cCursor;
    
    // layouts
    QGridLayout* layoutNameDescription;
    QGridLayout* layoutProperties;
    
    QVBoxLayout* layoutParent;
    
    DEA* m_pDea;
};


#endif


