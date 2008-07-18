#ifndef __DEDIT_MAINWINDOW_H_ 
#define __DEDIT_MAINWINDOW_H_ 

#include <QMainWindow>

#include <DEA/dea.h>

// dialogs
class Dia_DeaSourceViewer;

// widgets
#include<DEdit/dedit_widget.h>
class QPushButton;
class CommandButtonDND;
class QStatusBar;
class QDockWidget;

// menus
class QMenuBar;
class QMenu;
class QAction;

// layouts
class QHBoxLayout;
class QVBoxLayout;


class DEdit_MainWindow : public QMainWindow{
    Q_OBJECT;
public:
    DEdit_MainWindow();
    virtual ~DEdit_MainWindow();
    
    void retranslateUi();
public slots:
    void resetStatusBarText(DEdit_Widget::EMode mode);
    void showSourceCode();
private:
    void initMembers();
    void allocateWidgets();
    void createLayouts();
    void createActions();
    void createMenuBar();
    void connectSlots();
    void initWidgets();
    
    // widgets
    DEdit_Widget* wdgEditor;
    // tool buttons
    CommandButtonDND* btnAddState;
    QPushButton* btnAddTransition;
    QPushButton* btnRemoveItem;
    QPushButton* btnEditItem;
    QPushButton* btnMoveUp;
    QPushButton* btnMoveDown;
    
    // dock widgets
    QDockWidget* dockToolButtons;
    
    // actions
    // mnuFile
    QAction*     mnaQuit;
    // mnuView
    QAction*     mnaShowToolButtonsDock;
    QAction*     mnaShowSourceCode;
    // mnuSettings
    QAction*     mnaShowStatusBar;
    // menus in menubar
    QMenu*       mnuFile;
    QMenu*       mnuView;
    QMenu*       mnuSettings;
    
    // layouts
    QVBoxLayout* layoutToolButtons;
    QHBoxLayout* layoutParent;
    
    // dialogs
    Dia_DeaSourceViewer* m_diaSourceViewer;
    
    // members
    DEA m_cDea;
};


#endif
