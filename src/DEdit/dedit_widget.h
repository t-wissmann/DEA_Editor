#ifndef __DEDIT_WIDGET_H_
#define __DEDIT_WIDGET_H_


#include <QWidget>
#include <multilanguage/translateableobject.h>

#include <QList>
// graphical
#include "dedit_widgetpainter.h"
#include <QLine>
#include <QPoint>
#include <QPixmap>


// core
class DEA;
class DEA_State;
class DEA_Transition;
#include "dedit_graphicalstate.h"
#include "dedit_graphicaltransition.h"
#include "dedit_history.h"

class xmlObject;

// menus
class QMenu;
class QAction;

// dialogs
class DEdit_EditStateDia;
class DEdit_EditTransitionDia;

// events
class QPaintEvent;
class QMouseEvent;
class QKeyEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QContextMenuEvent;

class DEdit_Widget : public QWidget,
      public TranslateableObject
{
    Q_OBJECT;
    
public:
    enum EMode {
        ModeNormal,
        ModeDragState,
        ModeDragTransition,
        ModeAddTransitionSelectFrom,
        ModeAddTransitionSelectTo,
        ModeLocked
    };
    
    enum EDeaChangeType {
        ChangeTypeStateAdded                = 0x00000001,
        ChangeTypeStateRemoved              = 0x00000002,
        ChangeTypeStateEdited               = 0x00000004,
        ChangeTypeStateMoved                = 0x00000008, // TODO
        ChangeTypeTransitionAdded           = 0x00000010,
        ChangeTypeTransitionEdited          = 0x00000020,
        ChangeTypeTransitionRemoved         = 0x00000040,
        ChangeTypeTransitionCurveChanged    = 0x00000080  // TODO
    };
    static const int m_nDefaultHistorySaveReasonFlags =
            ChangeTypeStateAdded |
            ChangeTypeStateRemoved |
            ChangeTypeStateEdited |
            ChangeTypeStateMoved |
            ChangeTypeTransitionAdded |
            ChangeTypeTransitionEdited |
            ChangeTypeTransitionRemoved |
            ChangeTypeTransitionCurveChanged;
signals:
    void currentModeChanged(DEdit_Widget::EMode);
    void selectedStateIndexChanged(int index);
    void deaWasChanged();
    void historyChanged();
public:
    friend class DEdit_WidgetPainter; // needed to paint all items
    DEdit_Widget();
    virtual ~DEdit_Widget();
    
    void setDea(DEA* pDea);
    DEA* dea();
    DEdit_GraphicalState* graphicalStartState();
    
    bool isInDragMode() const;
    
    EMode currentMode() const;
    
    int stateIndexAt(QPoint point, bool searchFromTheEnd = FALSE);
        //  --> searchFromTheEnd = true -> begin at last position
        //  --> searchFromTheEnd = false -> begin at index 0;
    DEdit_GraphicalTransition* transitionAt(QPoint point);
    DEdit_GraphicalTransition* graphicalTransitionForData(DEA_Transition* data);
    DEdit_GraphicalTransition* graphicalTransitionForStartAndEnd
            (DEdit_GraphicalState* start, DEdit_GraphicalState* end);
    QList<DEdit_GraphicalTransition*> graphicalTransitionsWithStart(DEdit_GraphicalState* start);
    QPixmap stateTemplatePixmap() const;
    
    QString dndAddStateCommand() const;
    QString dndMimeFormat() const;
    
    DEdit_GraphicalState* findStateByName(QString name);
    
    // write and read functions
    bool createDeaFromFile(xmlObject* file);
    bool createGraphicalStatesFromFile(xmlObject* stateList);
    bool createGraphicalTransitionsFromFile(xmlObject* transitionList);
    static xmlObject* findGraphicalTransitionObject(xmlObject* transitionList, const char* start, const char* end, const char* symbols);
    QString lastSyntaxError();
    void writeDeaToFile(xmlObject* file);
    void writeGraphicalStatesToFile(xmlObject* stateList);
    void writeGraphicalTransitionsToFile(xmlObject* transitionList);
    
    void retranslateUi();
    void reloadIcons();
    
    
    // some visual options
    QPoint alignPointToGrid(QPoint p) const;
    void setGridResolution(int resolution);
    int gridResolution() const;
    void setAutoEditNewStates(bool on);
    bool autoEditNewStates() const;
    void setAutoEditNewTransitions(bool on);
    bool autoEditNewTransitions() const;
    bool isLocked() const;
    DEdit_Appearance* appearance();
    void recreateAllGuiTemplates();
    void setAllowNonDeterministic(bool bAllow);
    bool allowNonDeterministic() const;
    bool isDeaWasChanged();
    // for history
    bool isUndoPossible();
    bool isRedoPossible();
    void setMaxHistorySize(int size);
    int  maxHistorySize() const;
public slots:
    void addState();
    void addStateAtContextMenuPosition();
    void addState(QPoint atPosition);
    void addTransition();
    void removeState();
    void removeTransition();
    void removeSelectedItem();
    void editItem();
    void editSelectedState();
    void editSelectedTransition();
    void moveSelectionUp();
    void moveSelectionDown();
    void setStateSelected(int index);
    void setTransitionSelected(int index);
    void resetTransitionCurve();
    void setSelectedState_FinalState(bool finalState);
    void setSelectedState_StartState(bool startState);
    void updateStateContextMenu();
    void clearCompleteDEA();
    void setLocked(bool locked);
    void recomputeMinimumSize();
    void putErrorMessage(QString msg);
    void setDeaWasChanged(bool wasChanged = TRUE);
    // for history
    void undo();
    void redo();
    void clearHistory();
protected:
    virtual void paintEvent(QPaintEvent* event);
    // mouse
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    // keyboard
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    // drag and drop..... in this case only drop
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragMoveEvent (QDragMoveEvent* event );
    virtual void dragLeaveEvent (QDragLeaveEvent* event );
    virtual void dropEvent(QDropEvent* event);
    // context menu
    virtual void contextMenuEvent(QContextMenuEvent* event);
    
    // functions used by mouseMoveEvent(QMouseEvent*);
    bool handleStateDrag(QMouseEvent* event); // true = requests update, false = no update requested
    
private:
    // init functions
    void createContextMenu();
    
    // other functions
    void setCurrentMode(EMode mode);
    void emitDeaWasChanged();
    void createTransition(DEdit_GraphicalState* from, DEdit_GraphicalState* to);
    void removeTransition(DEdit_GraphicalTransition* transition);
    
    void sendHistoryChangeRequest(EDeaChangeType eType);
    
    DEA*    m_pDea;
    bool    m_bDeaWasChanged;
    EMode   m_eMode;
    DEdit_WidgetPainter m_cWidgetPainter;
    DEdit_History       m_cHistory;
    int                 m_nHistorySaveReasonFlags; // reasons to create a new history item, i.e. to call m_cWidgetPainter.saveCurrentHistory();
    // states
    QList<DEdit_GraphicalState> m_StateList;
    DEdit_GraphicalState*       m_pDraggedState;
    DEdit_GraphicalState*       m_pHoveredState;
    DEdit_GraphicalState*       m_pStartState;
    int                         m_nSelectedStateIndex;
    QPoint                      m_cDropPreviewPosition;
    bool                        m_bAboutToDrop;
    // transitions
    QList<DEdit_GraphicalTransition> m_TransitionList;
    // for mode ModeAddTransitionSelectFrom and
    // mode ModeAddTransitionSelectTo
    DEdit_GraphicalState*       m_pAddTransitionStateFrom;
    DEdit_GraphicalState*       m_pAddTransitionStateTo;
    QLine                       m_cNewTransitionLine;
    DEdit_GraphicalTransition*  m_pHoveredTransition;
    DEdit_GraphicalTransition*  m_pSelectedTransition;
    DEdit_GraphicalTransition*  m_pDraggedTransition;
    
    // some properties
    int             m_nGridResolution;
    bool            m_bAutoEditNewStates;
    bool            m_bAutoEditNewTransitions;
    bool            m_bAllowNonDeterministic;
    QString         m_szLastSyntaxError; // needed for createDeaFromFile()
    
    // context menu
    QPoint          m_cContextMenuPosition;
    QMenu*          m_mnuContextMenuState;
    QMenu*          m_mnuContextMenuTransition;
    QMenu*          m_mnuContextMenuEmptySpace;
    QAction*        m_mnaAddState;
    QAction*        m_mnaAddTransition;
    QAction*        m_mnaRemoveItem;
    QAction*        m_mnaEditItem;
    QAction*        m_mnaResetTransitionCurve;
    QAction*        m_mnaSetFinalState;
    QAction*        m_mnaSetStartState;
    
    
    // dialogs
    DEdit_EditStateDia* m_diaEditState;
    DEdit_EditTransitionDia* m_diaEditTransition;
    
};


#endif
