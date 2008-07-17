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

// events
class QPaintEvent;
class QMouseEvent;
class QKeyEvent;
class QDragEnterEvent;
class QDropEvent;

class DEdit_Widget : public QWidget
{
    Q_OBJECT;
    
    enum EMode {
        ModeNormal,
        ModeDragState,
        ModeAddTransitionSelectFrom,
        ModeAddTransitionSelectTo,
    };
    
    
public:
    friend class DEdit_WidgetPainter; // needed to paint all items
    DEdit_Widget();
    virtual ~DEdit_Widget();
    
    void setDea(DEA* pDea);
    DEA* dea();
    
    bool isInDragMode() const;
    
    EMode currentMode() const;
    
    int stateIndexAt(QPoint point, bool searchFromTheEnd = FALSE);
        //  --> searchFromTheEnd = true -> begin at last position
        //  --> searchFromTheEnd = false -> begin at index 0;
    DEdit_GraphicalTransition* transitionAt(QPoint point);
    
    QPixmap stateTemplatePixmap() const;
    
    QString dndAddStateCommand() const;
    QString dndMimeFormat() const;
    
    
    
public slots:
    void addState();
    void addState(QPoint atPosition);
    void removeState();
    void addTransition();
    void removeTransition();
    void moveSelectionUp();
    void moveSelectionDown();
protected:
    virtual void paintEvent(QPaintEvent* event);
    // mouse
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    // keyboard
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    // drag and drop..... in this case only drop
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dropEvent(QDropEvent* event);
    
    // functions used by mouseMoveEvent(QMouseEvent*);
    bool handleStateDrag(QMouseEvent* event); // true = requests update, false = no update requested
    
private:
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
    int                         m_nSelectedStateIndex;
    // transitions
    QList<DEdit_GraphicalTransition> m_TransitionList;
    
    // for mode ModeAddTransitionSelectFrom and
    // mode ModeAddTransitionSelectTo
    DEdit_GraphicalState*       m_pAddTransitionStateFrom;
    DEdit_GraphicalState*       m_pAddTransitionStateTo;
    QLine                       m_cNewTransitionLine;
    DEdit_GraphicalTransition*  m_pHoveredTransition;
    DEdit_GraphicalTransition*  m_pSelectedTransition;
    
};


#endif
