
#ifndef __DIA_ABOUT_H_
#define __DIA_ABOUT_H_


#include <QDialog>
#include <multilanguage/translateableobject.h>

class QPushButton;
class QLabel;

class QTabWidget;

// layout 
class QHBoxLayout;
class QVBoxLayout;
class QBoxLayout;

class Dia_About : public QDialog,
      public TranslateableObject
{
    Q_OBJECT
public:
    Dia_About(QWidget* parent = 0);
    ~Dia_About();
    
    void retranslateUi();
    void reloadIcons();
    
private:
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    
    
    QLabel* lblHeader;
    QLabel* lblVersion;
    QLabel* lblTabAbout;
    QLabel* lblTabAuthor;
    QLabel* lblIcon;
    
    QTabWidget* tabMain;
    QPushButton* btnClose;
    // layouts
    
    QBoxLayout*  layoutHeader;
    QHBoxLayout* layoutAbout;
    QVBoxLayout* layoutParent;
    QHBoxLayout* layoutBottom;
    
};


#endif


