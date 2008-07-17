#ifndef __DEDIT_MAINWINDOW_H_ 
#define __DEDIT_MAINWINDOW_H_ 

#include <QMainWindow>

#include <DEA/dea.h>



// widgets
class DEdit_Widget;
class QPushButton;
class CommandButtonDND;

// layouts
class QHBoxLayout;
class QVBoxLayout;


class DEdit_MainWindow : public QMainWindow{
    Q_OBJECT;
public:
    DEdit_MainWindow();
    virtual ~DEdit_MainWindow();
    
    void retranslateUi();
    
private:
    void initMembers();
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    void initWidgets();
    
    // widgets
    DEdit_Widget* wdgEditor;
    // tool buttons
    CommandButtonDND* btnAddState;
    QPushButton* btnAddTransition;
    QPushButton* btnRemoveState;
    QPushButton* btnRemoveTransition;
    QPushButton* btnMoveUp;
    QPushButton* btnMoveDown;
    
    // layouts
    QVBoxLayout* layoutToolButtons;
    QHBoxLayout* layoutParent;
    
    // members
    DEA m_cDea;
};


#endif
