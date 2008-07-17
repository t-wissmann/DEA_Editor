#include "dedit_widget.h"

#include <DEA/dea.h>
#include <DEA/dea_state.h>
#include <DEA/dea_transition.h>
#include <io/debug_msg.h>

// qt classes
#include <QSizePolicy>
#include <QCursor>
// events
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDropEvent>

DEdit_Widget::DEdit_Widget() :
  QWidget(),
  m_cWidgetPainter(this)
{
    // init some members
    m_pDea = NULL;
    m_eMode = ModeNormal;
    // for states
    m_pDraggedState = NULL;
    m_pHoveredState = NULL;
    m_nSelectedStateIndex = -1;
    // for transitions
    m_pHoveredTransition = NULL;
    m_pSelectedTransition = NULL;
    
    // set size policy, so that this is the biggest widget in the window
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    // set to Qt::StrongFocus to activate keyPressEvent
    setFocusPolicy(Qt::StrongFocus);
    
    // activate mouse tracking to activate hover effect
    setMouseTracking(TRUE);
    // activate drops
    setAcceptDrops(TRUE);
}

DEdit_Widget::~DEdit_Widget(){
    
}




void DEdit_Widget::setDea(DEA* pDea)
{
    m_pDea = pDea;
}

DEA* DEdit_Widget::dea()
{
    return m_pDea;
}



void DEdit_Widget::addState()
{
    addState(QPoint(width()/2, height()/2));
}


void DEdit_Widget::addState(QPoint atPosition)
{
    if(!m_pDea)
    {
        DEBUG_MSG("addState(QPoint)", "m_pDea = 0");
        return;
    }
    setCurrentMode(ModeNormal);
    DEA_State* newState = m_pDea->addState("New State");
    DEdit_GraphicalState graphicalState(newState);
    graphicalState.move(atPosition.x(), atPosition.y());
    m_StateList.append(graphicalState);
    // state was added
    // now refresh widget
    update();
}


void DEdit_Widget::removeState()
{
    if(!m_pDea)
    {
        DEBUG_MSG("removeState()", "m_pDea = 0");
        return;
    }
    if(m_nSelectedStateIndex < 0)
    {
        return;
    }
    
    DEdit_GraphicalState* state = &(m_StateList[m_nSelectedStateIndex]);
    
    // remove all transitions, that use state
    for(int i = 0; i < m_TransitionList.size(); ++i)
    {
        if(m_TransitionList[i].isConnectedWith(state))
        {
            removeTransition(&(m_TransitionList[i]));
            i--;
        }
    }
    
    // remove state in dea
    m_pDea->removeState(state->m_pData);
    // remove state from graphical state list
    m_StateList.removeAt(m_nSelectedStateIndex);
    // remove selection
    m_pDraggedState = NULL;
    m_nSelectedStateIndex = -1;
    // now refresh widget
    update();
}

void DEdit_Widget::addTransition()
{
    if(!m_pDea)
    {
        DEBUG_MSG("addTransition()", "m_pDea = 0");
        return;
    }
    if(m_eMode != ModeNormal)
    {
        DEBUG_MSG("addTransition()", "m_eMode != ModeNormal)");
        return;
    }
    
    setCurrentMode(ModeAddTransitionSelectFrom);
    // clear points..
    m_pAddTransitionStateFrom = NULL;
    m_pAddTransitionStateTo = NULL;
}


void DEdit_Widget::paintEvent(QPaintEvent*)
{
    m_cWidgetPainter.paint();
}

void DEdit_Widget::mouseMoveEvent(QMouseEvent* event)
{
    if(!event)
    {
        return;
    }
    bool hasToRepaint = FALSE;
    
    if(handleStateDrag(event))
    {
        hasToRepaint = TRUE;
    }
    switch(m_eMode)
    {
        case ModeDragState:
        {
            break;
        }
        case ModeAddTransitionSelectFrom:
        {
            break;
        }
        case ModeAddTransitionSelectTo:
        {
            // reset line
            QPoint p2 = event->pos();
            if(m_pHoveredState) // if there is an state hovered
            {
                p2 = QPoint(m_pHoveredState->m_nX, m_pHoveredState->m_nY);
            }
            m_cNewTransitionLine = QLine(m_cNewTransitionLine.p1(), p2);
            hasToRepaint = TRUE; // request for repaint

            break;
        }
        case ModeNormal:
        default: {
            // handle hover effect for transitions
            // only in normal mode
            {
                DEdit_GraphicalTransition* transition = transitionAt(event->pos());
                if(transition != m_pHoveredTransition)
                {
                    if(m_pHoveredTransition)
                    {
                        m_pHoveredTransition->m_bHovered = FALSE;
                    }
                    if(transition)
                    {
                        transition->m_bHovered = TRUE;
                    }
                    m_pHoveredTransition = transition;
                    hasToRepaint = TRUE;
                }
            }
            break;
        }
    }
    
    if(hasToRepaint)
    {
        update();
    }
}

bool DEdit_Widget::handleStateDrag(QMouseEvent* event)
{
    if(!event)
    {
        return FALSE;
    }
    bool hasToRepaint = FALSE;
    // if is in dragged mode
    if(m_pDraggedState)
    {
        // move dragged item
        int newX = event->pos().x() - m_pDraggedState->m_nDragOffsetX;
        int newY = event->pos().y() - m_pDraggedState->m_nDragOffsetY;
        // only allow drags within the widget
        newX = (newX < 0) ? 0 : newX;
        newX = (newX >= width()) ? width() : newX;
        newY = (newY < 0) ? 0 : newY;
        newY = (newY >= height()) ? height() : newY;
        
        m_pDraggedState->move(newX, newY);
        hasToRepaint = TRUE;
    }
    else
    {
        // set hoveredState to NULL -> find hovered state again
        m_pHoveredState = NULL;
        // only check if a state is hovered
        // if we aren't in drag mode at the moment
        bool currentStateIsHovered = FALSE;
        bool newHoverValue;
        DEdit_GraphicalState* currentState;
         // start with last item
        for(int i = m_StateList.size()-1; i >= 0; --i)
        {
            currentState = &m_StateList[i];
            // if hovered item was found
            if(currentStateIsHovered)
            {// then deactivate all other items
                newHoverValue = FALSE;
            }
            else // else check, if current item is hovered item
            {
                newHoverValue = currentState->isPointContained(event->pos());
                currentStateIsHovered = newHoverValue;
            }
            // only apply if something would change
            if(newHoverValue != currentState->isHovered)
            {
                // if isHovered has to be changed
                currentState->isHovered = newHoverValue;
                // then repaint
                hasToRepaint = TRUE;
            }
            if(newHoverValue)
            {
                // if currentState is hovered
                // set m_pHoveredState to currentState
                m_pHoveredState = currentState;
            }
        }
    }
    return hasToRepaint;
}


void DEdit_Widget::mousePressEvent(QMouseEvent* event)
{
    if(!event)
    {
        return;
    }
    m_nSelectedStateIndex = -1;
    DEdit_GraphicalState* currentState;
    // start with last item
    int i;
    for(i = m_StateList.size()-1; i >= 0; --i)
    {
        currentState = &m_StateList[i];
        // if current item is hovered
        if(currentState->isHovered)
        {
            // then hovered item has to be dragged
            // activate item
            currentState->isSelected = TRUE;
            m_nSelectedStateIndex = i;
            if(m_eMode == ModeNormal)
            {
                // only start drag if there is currently normal mode
                currentState->isDragged = TRUE;
                currentState->m_nDragOffsetX = event->pos().x() - currentState->m_nX;
                currentState->m_nDragOffsetY = event->pos().y() - currentState->m_nY;
                // item was found, we don't need to search anymore
                m_pDraggedState = currentState;
                setCurrentMode(ModeDragState);
            }
            if(m_eMode == ModeAddTransitionSelectFrom)
            {
                // if is in add transition mode
                // then we found our first state for the new transition
                m_cNewTransitionLine = QLine(currentState->m_nX, currentState->m_nY,
                                             currentState->m_nX, currentState->m_nY);
                // and then we have to switch to ModeAddTransitionSelectTo
                setCurrentMode(ModeAddTransitionSelectTo);
                // set currentState to "From" for new Transition
                m_pAddTransitionStateFrom = currentState;
                m_pAddTransitionStateTo = currentState;
                //qDebug("Switchted to mode ModeAddTransitionSelectTo");
            }
            break;
        }
        else
        {
            // deselect others 
            currentState->isSelected = FALSE;
        }
    }
    // unselect all other items
    while(i > 0)
    {
        i--;
        m_StateList[i].isSelected = FALSE;
    }
    // clear selection
    m_pSelectedTransition = NULL;
    // loop for transitions
    DEdit_GraphicalTransition* currentTransition;
    for(i = m_TransitionList.size()-1; i >= 0; --i)
    {
        currentTransition = &m_TransitionList[i];
        // if current item is hovered
        if(currentTransition->m_bHovered)
        {
            currentTransition->m_bSelected = TRUE;
            m_pSelectedTransition = currentTransition;
        }
        else
        {
            currentTransition->m_bSelected = FALSE;
        }
    }
    
    //qDebug("item %d was selected", m_nSelectedStateIndex);
    update();
}


void DEdit_Widget::mouseReleaseEvent(QMouseEvent*)
{
    if(m_pDraggedState)
    {
        // set isDragged Attribute to false
        m_pDraggedState->isDragged = FALSE;
        // m_pDraggedState now isn't dragged anymore
        m_pDraggedState = NULL;
        setCurrentMode(ModeNormal);
        // refresh screen
        update();
    }
    if(m_eMode == ModeAddTransitionSelectFrom)
    {
        // if mouse was released in SelectFrom
        // then user has clicked to somewhere...
        // then abort ModeAddTransitionSelectFrom
        setCurrentMode(ModeNormal);
    }
    if(m_eMode == ModeAddTransitionSelectTo)
    {
        if(m_pHoveredState) // if there was an item hovered
        {// then create the new transition
            // currently hovered item is "To" for new Transition
            m_pAddTransitionStateTo = m_pHoveredState;
            createTransition(m_pAddTransitionStateFrom, m_pAddTransitionStateTo);
            
        }else
        {
            //qDebug("Adding transition aborted..");
        }
        
        // switch back to normal mode
        setCurrentMode(ModeNormal);
        // refresh screen
        update();
    }
}


bool DEdit_Widget::isInDragMode() const
{
    return (bool)m_pDraggedState;
}

void DEdit_Widget::keyPressEvent(QKeyEvent* event)
{
    if(!event)
    {
        return;
    }
    event->ignore();
}

void DEdit_Widget::keyReleaseEvent(QKeyEvent* event)
{
    if(!event)
    {
        return;
    }
    event->ignore();
}


void DEdit_Widget::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasFormat(dndMimeFormat()))
    {
        event->acceptProposedAction();
    }
}
QString DEdit_Widget::dndMimeFormat() const
{
    return "dea_editor/command_add_state";
}

QString DEdit_Widget::dndAddStateCommand() const
{
    return "cmd:dea_edit_add_command";
}

void DEdit_Widget::dropEvent(QDropEvent* event)
{
    QString text = event->mimeData()->data(dndMimeFormat());
    if(text == dndAddStateCommand())
    {
        addState(event->pos());
    }
}

void DEdit_Widget::moveSelectionUp()
{
    // high index -> painted at last -> on the top (up)
    // move up -> draw later -> higher index
    if((m_nSelectedStateIndex >= 0)
       && (m_nSelectedStateIndex < (m_StateList.size()-1)))
    {
        m_StateList.move(m_nSelectedStateIndex, m_nSelectedStateIndex+1);
        ++m_nSelectedStateIndex;
        //qDebug("moving up to %d", m_nSelectedStateIndex);
        update();
    }
}

void DEdit_Widget::moveSelectionDown()
{
    // low index -> painted at first -> on the bottom (down)
    // move down -> draw first -> lower index
    if((m_nSelectedStateIndex  > 0)
       && (m_nSelectedStateIndex < m_StateList.size()))
    {
        m_StateList.move(m_nSelectedStateIndex, m_nSelectedStateIndex-1);
        --m_nSelectedStateIndex;
        update();
    }
}


DEdit_Widget::EMode DEdit_Widget::currentMode() const
{
    return m_eMode;
}


int DEdit_Widget::stateIndexAt(QPoint point, bool searchFromTheEnd)
{
    int index = -1;
    DEdit_GraphicalState* currentState;
    
    if(searchFromTheEnd)
    {
        for(int i = m_StateList.size()-1; i >= 0 ; --i)
        {
            currentState = &m_StateList[i];
            if(currentState->isPointContained(point))
            {
                // break if index was found
                index = i;
                break;
            }

        }
    }
    else
    {
        for(int i = 0; i < m_StateList.size(); ++i)
        {
            currentState = &m_StateList[i];
            if(currentState->isPointContained(point))
            {
                // break if index was found
                index = i;
                break;
            }
        }
    }
    return index;
}



void DEdit_Widget::setCurrentMode(EMode mode)
{
    if(m_eMode == mode)
    {// nothing to do
        return;
    }
    m_eMode = mode;
    QCursor newCursor;
    switch(m_eMode)
    {
        case ModeAddTransitionSelectFrom:
        case ModeAddTransitionSelectTo:
        {
            newCursor = QCursor(Qt::CrossCursor);
            break;
        }
        case ModeDragState: {
            newCursor = QCursor(Qt::ClosedHandCursor);
            break;
        }
        case ModeNormal:
        default: {
            newCursor = QCursor(Qt::ArrowCursor);
            break;
        }
    }
    setCursor(newCursor);
}



void DEdit_Widget::createTransition(DEdit_GraphicalState* from, DEdit_GraphicalState* to)
{
    if(!m_pDea)
    {
        DEBUG_MSG("addTransition()", "m_pDea = 0");
        return;
    }
    if(!from || !to)
    {
        DEBUG_MSG("addTransition()",
                  "Invalid parameters: DEdit_GraphicalState* from == NULL || DEdit_GraphicalState* to == NULL ");
        return;
    }
    // create new transition in DEA
    DEA_Transition* transition = m_pDea->createTransition(from->m_pData, to->m_pData, '\0');
    // init new graphical transition
    DEdit_GraphicalTransition newGTransition(from, to);
    newGTransition.m_DataList.append(transition);
    // add new graphical transition to list
    m_TransitionList.append(newGTransition);
    update();
}


DEdit_GraphicalTransition* DEdit_Widget::transitionAt(QPoint point)
{
    DEdit_GraphicalTransition* foundItem = NULL;
    
    for(int i = m_TransitionList.size()-1; i >= 0; --i){
        if(m_TransitionList[i].isPointContained(point))
        {
            foundItem = &(m_TransitionList[i]);
            break;
        }
    }
    return foundItem;
}

void DEdit_Widget::removeTransition()
{
    removeTransition(m_pSelectedTransition);
    // reset selection
    m_pSelectedTransition = NULL;
    m_pHoveredTransition = NULL;
    update();
}

void DEdit_Widget::removeTransition(DEdit_GraphicalTransition* transition)
{
    if(!m_pDea)
    {
        DEBUG_MSG("removeTransition(DEdit_GraphicalState*)", "m_pDea = 0");
        return;
    }
    if(!transition)
    {// return if no transition is selected
        return;
    }
    // get index
    int index = -1;
    for(int i = 0; i < m_TransitionList.size(); ++i)
    {
        if(&(m_TransitionList[i]) == transition)
        {
            index = i;
            break;
        }
    }
    if(index < 0)
    {
        return;
    }
    
    // remove ALL transitions from m_pDea 
    for(int i = 0; i < transition->m_DataList.size(); ++i)
    {
        m_pDea->removeTransition(transition->m_DataList[i]);
    }
    m_TransitionList.removeAt(index);
}






QPixmap DEdit_Widget::stateTemplatePixmap() const
{
    return m_cWidgetPainter.m_cStateNormalTemplate;
}




