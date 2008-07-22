#ifndef __DEDIT_MAINWINDOW_H_ 
#define __DEDIT_MAINWINDOW_H_ 

#include <QMainWindow>

#include <DEA/dea.h>

// dialogs
class Dia_DeaSourceViewer;
class Dia_ConfigureDEditWidget;

// widgets
#include<DEdit/dedit_widget.h>
class QPushButton;
class CommandButtonDND;
class QStatusBar;
class QDockWidget;
class DEdit_ExecDeaWidget;
class QScrollArea;
class DEdit_PropertiesWidget;

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
    void reloadIcons();
    
    
    QString loadFromFile(QString filename); // returns errormsg
    QString saveToFile(QString filename); // returns errormsg
public slots:
    void newFile();
    void openFile();
    void resetStatusBarText(DEdit_Widget::EMode mode);
    void showSourceCode();
    void showConfigureEditorDialog();
    void saveFile();
    void saveFileAs();
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
    // dock properties
    QDockWidget* dockProperties;
    DEdit_PropertiesWidget* wdgProperties;
    
    
    //dock run
    QDockWidget* dockExecDea;
    DEdit_ExecDeaWidget* wdgExecDea;
    
    // actions
    // mnuFile
    QAction*     mnaNewFile;
    QAction*     mnaOpen;
    QAction*     mnaSave;
    QAction*     mnaSaveAs;
    QAction*     mnaQuit;
    // mnuView
    QAction*     mnaShowToolButtonsDock;
    QAction*     mnaShowExecDeaDock;
    QAction*     mnaShowProperties;
    QAction*     mnaShowSourceCode;
    // mnuSettings
    QAction*     mnaShowStatusBar;
    QAction*     mnaConfigureEditor;
    // menus in menubar
    QMenu*       mnuFile;
    QMenu*       mnuView;
    QMenu*       mnuSettings;
    
    // layouts
    QVBoxLayout* layoutToolButtons;
    QScrollArea* scrollCentral;
    QHBoxLayout* layoutParent;
    
    // dialogs
    Dia_DeaSourceViewer*      m_diaSourceViewer;
    Dia_ConfigureDEditWidget* m_diaConfigureDEditWidget;
    
    // members
    QString m_szFilename;
    DEA m_cDea;
};


#endif
