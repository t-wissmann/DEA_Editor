#ifndef __DEDIT_MAINWINDOW_H_ 
#define __DEDIT_MAINWINDOW_H_ 

#include <QMainWindow>

#include <DEA/dea.h>
#include <multilanguage/translateableobject.h>
// dialogs
class Dia_DeaSourceViewer;
class Dia_ConfigureDEditWidget;
class Dia_About;

// widgets
#include<DEdit/dedit_widget.h>
class QPushButton;
class CommandButtonDND;
class QStatusBar;
class QDockWidget;
class DEdit_ExecDeaWidget;
class QScrollArea;
class DEdit_PropertiesWidget;
class QToolBar;

// events
class QDragEnterEvent;
class QDropEvent;
class QKeyEvent;
class QCloseEvent;

// menus
class QMenuBar;
class QMenu;
class QAction;

// layouts
class QHBoxLayout;
class QVBoxLayout;


class DEdit_MainWindow
    : public QMainWindow,
      public TranslateableObject
{
    friend class ConfigIO;
    Q_OBJECT
public:
    DEdit_MainWindow();
    virtual ~DEdit_MainWindow();
    
    void retranslateUi();
    void reloadIcons();
    
    QString loadFromFile(QString filename); // returns errormsg
    QString saveToFile(QString filename); // returns errormsg
    
    
    void saveConfig();
    void loadConfig();
    
    bool userReallyWantsToCloseFile();
    
public slots:
    void newFile();
    void openFile();
    void resetStatusBarText(DEdit_Widget::EMode mode);
    void showSourceCode();
    void showConfigureEditorDialog();
    void saveFile();
    void saveFileAs();
    void showAboutDialog();
    void undo();
    void redo();
    void reinitEditMenu();
    void resetWindowTitle();
protected:
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dropEvent(QDropEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void closeEvent(QCloseEvent* event);
    
private:
    void initMembers();
    void allocateWidgets();
    void createLayouts();
    void createActions();
    void createToolBars();
    void createMenuBar();
    void connectSlots();
    void initWidgets();
    void parseArguments();
    
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
    // toolbars
    QToolBar*    tlbMainToolBar;
    
    // actions
    // mnuFile
    QAction*     mnaNewFile;
    QAction*     mnaOpen;
    QAction*     mnaSave;
    QAction*     mnaSaveAs;
    QAction*     mnaQuit;
    // mnuEdit
    QAction*     mnaUndo;
    QAction*     mnaRedo;
    // mnuView
    QAction*     mnaShowToolButtonsDock;
    QAction*     mnaShowExecDeaDock;
    QAction*     mnaShowProperties;
    QAction*     mnaShowSourceCode;
    // mnuSettings
    QAction*     mnaShowToolBar;
    QAction*     mnaShowStatusBar;
    QAction*     mnaShowMenuBar;
    QAction*     mnaConfigureEditor;
    // mnuHelp
    QAction*     mnaAboutQt;
    QAction*     mnaAbout;
    // menus in menubar
    QMenu*       mnuFile;
    QMenu*       mnuEdit;
    QMenu*       mnuView;
    QMenu*       mnuSettings;
    QMenu*       mnuHelp;
    
    // layouts
    QVBoxLayout* layoutToolButtons;
    QScrollArea* scrollCentral;
    QHBoxLayout* layoutParent;
    
    // dialogs
    Dia_DeaSourceViewer*      m_diaSourceViewer;
    Dia_ConfigureDEditWidget* m_diaConfigureDEditWidget;
    Dia_About*                m_diaAbout;
    
    // members
    QString m_szFilename;
    DEA m_cDea;
};


#endif
