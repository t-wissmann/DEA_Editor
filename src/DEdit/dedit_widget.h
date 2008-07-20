#ifndef __DEDIT_WIDGET_H_
#define __DEDIT_WIDGET_H_


#include <QWidget>

#include <QList>
// graphical
#include "dedit_widgetpainter.h"
#include <QLine>
#include <QPixmap>
// core
class DEA;
class DEA_State;
class DEA_Transition;
#include "dedit_graphicalstate.h"
#include "dedit_graphicaltransition.h"

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
class QDropEvent;
class QContextMenuEvent;

class DEdit_Widget : public QWidget
{
    Q_OBJECT;
    
public:
    enum EMode {
        ModeNormal,
        ModeDragState,
        ModeAddTransitionSelectFrom,
        ModeAddTransitionSelectTo,
        ModeLocked
    };

signals:
    void currentModeChanged(DEdit_Widget::EMode);
    void selectedStateIndexChanged(int index);
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
    
    QPixmap stateTemplatePixmap() const;
    
    QString dndAddStateCommand() const;
    QString dndMimeFormat() const;
    
    DEdit_GraphicalState* findStateByName(QString name);
    
    // write and read functions
    bool createDeaFromFile(xmlObject* file);
    bool createGraphicalStatesFromFile(xmlObject* stateList);
    bool createGraphicalTransitionsFromFile(xmlObject* transitionList);
    QString lastSyntaxError();
    void writeDeaToFile(xmlObject* file);
    void writeGraphicalStatesToFile(xmlObject* stateList);
    
    void retranslateUi();
    void reloadIcons();
    
    
    // some options
    void setGridResolution(int resolution);
    int gridResolution() const;
    void setAutoEditNewStates(bool on);
    bool autoEditNewStates() const;
    void setAutoEditNewTransitions(bool on);
    bool autoEditNewTransitions() const;
    bool isLocked() const;
public slots:
    void addState();
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
    void setSelectedState_FinalState(bool finalState);
    void setSelectedState_StartState(bool startState);
    void updateStateContextMenu();
    void clearCompleteDEA();
    void setLocked(bool locked);
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
    
    void createTransition(DEdit_GraphicalState* from, DEdit_GraphicalState* to);
    void removeTransition(DEdit_GraphicalTransition* transition);
    
    DEA*    m_pDea;
    EMode   m_eMode;
    DEdit_WidgetPainter m_cWidgetPainter;
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
    
    // some properties
    int             m_nGridResolution;
    bool            m_bAutoEditNewStates;
    bool            m_bAutoEditNewTransitions;
    QString         m_szLastSyntaxError; // needed for createDeaFromFile()
    
    // context menu
    QMenu*          m_mnuContextMenuState;
    QMenu*          m_mnuContextMenuTransition;
    QAction*        m_mnaRemoveItem;
    QAction*        m_mnaEditItem;
    QAction*        m_mnaSetFinalState;
    QAction*        m_mnaSetStartState;
    
    // dialogs
    DEdit_EditStateDia* m_diaEditState;
    DEdit_EditTransitionDia* m_diaEditTransition;
    
};


#endif
