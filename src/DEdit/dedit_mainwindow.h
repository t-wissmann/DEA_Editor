#ifndef __DEDIT_MAINWINDOW_H_ 
#define __DEDIT_MAINWINDOW_H_ 

#include <QMainWindow>

#include <DEA/dea.h>
#include <multilanguage/translateableobject.h>
#include <multilanguage/translationmanager.h>
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
class QSpacerItem;

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
class QBoxLayout;
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
    
    virtual QMenu*  createPopupMenu();
    
    bool userReallyWantsToCloseFile();
    bool hasDataChanged() const;
    
    bool areToolButtonsStretched() const;
    bool isStretchToolButtonsButtonVisible() const;
    TranslationManager* translationManager();
    void setFrameVisible(bool visible);
    bool isFrameVisible() const;
    void setBackgroundColor(QString color);
    QString backgroundColor() const;
    void setMoveUpMoveDownButtonsVisible(bool visible);
    bool areMoveUpMoveDownButtonsVisible() const;
    void setCentralWidgetMargin(int nMargin);
    int centralWidgetMargin() const;
    bool isToolsAlignmentHorizontal() const;
    void setIsAddingTransition(bool adding); // set if user currently wants to add an transition
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
    void setStretchToolButtons(bool on);
    void setStretchToolButtonsButtonVisible(bool visible);
    void setToolsAlignmentHorizontal(bool horizontal);
    void setDataHasChanged(bool bChanged = TRUE);
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
    QSpacerItem* itemToolButtonStretch;
    QPushButton* btnStretchToolButtons;
    
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
    QAction*     mnaStetchToolButtons;
    QAction*     mnaAlignToolsHorizontal;
    QAction*     mnaShowSourceCode;
    // mnuSettings
    QAction*     mnaShowToolBar;
    QAction*     mnaShowStatusBar;
    QAction*     mnaShowMenuBar;
    QAction*     mnaConfigureEditor;
    // mnuHelp
    QAction*     mnaWhatsThis;
    QAction*     mnaAboutQt;
    QAction*     mnaAbout;
    // menus in menubar
    QMenu*       mnuFile;
    QMenu*       mnuEdit;
    QMenu*       mnuView;
    QMenu*       mnuSettings;
    QMenu*       mnuHelp;
    // popup menu
    QMenu*       mnuPopupMenu;
    // layouts
    QBoxLayout*  layoutToolButtons;
    QScrollArea* scrollCentral;
    QHBoxLayout* layoutParent;
    
    // dialogs
    Dia_DeaSourceViewer*      m_diaSourceViewer;
    Dia_ConfigureDEditWidget* m_diaConfigureDEditWidget;
    Dia_About*                m_diaAbout;
    
    // members
    TranslationManager m_TranslationManager;
    QString m_szBackgroundColor;
    QString m_szFilename;
    bool    m_bDataChanged;
    DEA m_cDea;
};


#endif
