#include "dedit_widget.h"

#include <DEA/dea.h>
#include <DEA/dea_state.h>
#include <DEA/dea_transition.h>
#include <io/debug_msg.h>
#include <io/xmlparser.h>
#include <io/iconcatcher.h>

// dialogs
#include <DEdit/dedit_editstatedia.h>
#include <DEdit/dedit_edittransitiondia.h>
#include <QMessageBox>

// qt classes
#include <QSizePolicy>
#include <QCursor>
// widgets
#include <QMenu>
#include <QAction>

// events
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QContextMenuEvent>

DEdit_Widget::DEdit_Widget() :
  QWidget(),
  m_cWidgetPainter(this)
{
    // init some members
    m_pDea = NULL;
    m_bDeaWasChanged = FALSE;
    m_eMode = ModeNormal;
    m_nGridResolution = 0; // disable grid per default
    m_bAutoEditNewStates = FALSE; // disable auto edit per default
    m_bAutoEditNewTransitions = TRUE;
    m_bAllowNonDeterministic = FALSE;
    // events that call save of history
    m_nHistorySaveReasonFlags = m_nDefaultHistorySaveReasonFlags;
    
    // for states
    m_pDraggedState = NULL;
    m_pHoveredState = NULL;
    m_nSelectedStateIndex = -1;
    m_bAboutToDrop = FALSE;
    m_pStartState = NULL;
    // for transitions
    m_pHoveredTransition = NULL;
    m_pSelectedTransition = NULL;
    m_pDraggedTransition = NULL;
    
    // init dialogs
    m_diaEditState = NULL;
    m_diaEditTransition = NULL;
    
    // set size policy, so that this is the biggest widget in the window
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    // set to Qt::StrongFocus to activate keyPressEvent
    setFocusPolicy(Qt::StrongFocus);
    
    // activate mouse tracking to activate hover effect
    setMouseTracking(TRUE);
    // activate focus to activate keyPressEvent
    setFocusPolicy(Qt::ClickFocus);
    // activate drops
    setAcceptDrops(TRUE);
    // activate context menu
    setContextMenuPolicy(Qt::DefaultContextMenu);
    createContextMenu();
    // init history
    m_cHistory.setEditorWidget(this);
    
    // retranslate etc..
    retranslateUi();
    reloadIcons();
    
}

DEdit_Widget::~DEdit_Widget(){
    
}



void DEdit_Widget::createContextMenu()
{
    // menus
    m_mnuContextMenuState = new QMenu(this);
    m_mnuContextMenuTransition = new QMenu(this);
    m_mnuContextMenuEmptySpace = new QMenu(this);
    // both
    m_mnaRemoveItem = new QAction(NULL);
    m_mnaEditItem = new QAction(NULL);
    // transitions
    m_mnaResetTransitionCurve = new QAction(NULL);
    // states
    m_mnaSetFinalState = new QAction(NULL);
    m_mnaSetFinalState->setCheckable(TRUE);
    m_mnaSetStartState = new QAction(NULL);
    m_mnaSetStartState->setCheckable(TRUE);
    // for state
    m_mnuContextMenuState->addAction(m_mnaSetStartState);
    m_mnuContextMenuState->addAction(m_mnaSetFinalState);
    m_mnuContextMenuState->addSeparator();
    m_mnuContextMenuState->addAction(m_mnaRemoveItem);
    m_mnuContextMenuState->addAction(m_mnaEditItem);
    // for transition
    m_mnuContextMenuTransition->addAction(m_mnaResetTransitionCurve);
    m_mnuContextMenuTransition->addSeparator();
    m_mnuContextMenuTransition->addAction(m_mnaRemoveItem);
    m_mnuContextMenuTransition->addAction(m_mnaEditItem);
    // for empty space
    m_mnaAddState = new QAction(NULL);
    m_mnaAddTransition = new QAction(NULL);
    m_mnuContextMenuEmptySpace->addAction(m_mnaAddState);
    m_mnuContextMenuEmptySpace->addAction(m_mnaAddTransition);
    
    
    // connect slots
    connect(m_mnaRemoveItem, SIGNAL(triggered()), this, SLOT(removeSelectedItem()));
    connect(m_mnaEditItem, SIGNAL(triggered()), this, SLOT(editItem()));
    connect(m_mnaSetStartState, SIGNAL(toggled(bool)), this,
            SLOT(setSelectedState_StartState(bool)));
    connect(m_mnaSetFinalState, SIGNAL(toggled(bool)), this,
            SLOT(setSelectedState_FinalState(bool)));
    connect(m_mnaResetTransitionCurve, SIGNAL(triggered()), this, SLOT(resetTransitionCurve()));
    connect(m_mnaAddState, SIGNAL(triggered()), this, SLOT(addStateAtContextMenuPosition()));
    connect(m_mnaAddTransition, SIGNAL(triggered()), this, SLOT(addTransition()));
    
    
    // slot, to update context menu
    connect(this, SIGNAL(selectedStateIndexChanged(int)), this, SLOT(updateStateContextMenu()));
}


void DEdit_Widget::retranslateUi()
{
    m_mnaRemoveItem->setText(tr("Remove"));
    m_mnaEditItem->setText(tr("Edit"));
    m_mnaSetFinalState->setText(tr("Final State"));
    m_mnaSetStartState->setText(tr("Start State"));
    m_mnaResetTransitionCurve->setText(tr("Reset Curve"));
    
    m_mnaAddState->setText(tr("New State"));
    m_mnaAddTransition->setText(tr("New Transition"));
}

void DEdit_Widget::reloadIcons()
{
    m_mnaAddState->setIcon(IconCatcher::getIcon("add"));
    m_mnaAddTransition->setIcon(IconCatcher::getIcon("add"));
    m_mnaRemoveItem->setIcon(IconCatcher::getIcon("editdelete"));
    m_mnaEditItem->setIcon(IconCatcher::getIcon("edit"));
    m_mnaResetTransitionCurve->setIcon(IconCatcher::getIcon("undo"));
}

void DEdit_Widget::setDea(DEA* pDea)
{
    m_pDea = pDea;
}

DEA* DEdit_Widget::dea()
{
    return m_pDea;
}

DEdit_GraphicalState* DEdit_Widget::graphicalStartState()
{
    return m_pStartState;
}


void DEdit_Widget::addState()
{
    addState(QPoint(width()/2, height()/2));
}


void DEdit_Widget::addStateAtContextMenuPosition()
{
    // add state at the position of the last context menu
    addState(m_cContextMenuPosition);
}

void DEdit_Widget::addState(QPoint atPosition)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(!m_pDea)
    {
        DEBUG_MSG("addState(QPoint)", "m_pDea = 0");
        return;
    }
    
    setCurrentMode(ModeNormal);
    QString stateName = QString::number(m_StateList.size()+1);
    int number = 0;
    // find first free name
    while(findStateByName(QString::number(++number)));
    
    DEA_State* newState = m_pDea->addState(QString::number(number).toAscii().data());
    DEdit_GraphicalState graphicalState(newState);
    graphicalState.m_pWidgetPainter = &m_cWidgetPainter;
    graphicalState.move(atPosition.x(), atPosition.y());
    m_StateList.append(graphicalState);
    // state was added
    if(m_bAutoEditNewStates)
    {
        // popup edit dialog if wanted
        setStateSelected(m_StateList.size()-1);
        editSelectedState();
    }
    
    // state position has Changed
    // so recompute minimum size, so that we can see all states
    recomputeMinimumSize(); 
    // now refresh widget
    update();
    emitDeaWasChanged(); // dea just has been edited
    sendHistoryChangeRequest(ChangeTypeStateAdded); // save current dea to history
}


void DEdit_Widget::removeState()
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
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
    // if state was start state
    if(m_pStartState == state)
    {
        m_pStartState = NULL;
        m_pDea->setStartState(NULL);
    }
    
    // remove state in dea
    m_pDea->removeState(state->m_pData);
    // remove state from graphical state list
    m_StateList.removeAt(m_nSelectedStateIndex);
    // remove selection
    m_pDraggedState = NULL;
    m_nSelectedStateIndex = -1;
    
    // state position has changed
    // so recompute minimum size, so that we can see all states
    recomputeMinimumSize(); 
    // switch back to normal mode
    setCurrentMode(ModeNormal);
    emitDeaWasChanged(); // dea just has been edited
    // now refresh widget
    update();
    sendHistoryChangeRequest(ChangeTypeStateRemoved); // save current dea to history

}

void DEdit_Widget::addTransition()
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(!m_pDea)
    {
        DEBUG_MSG("addTransition()", "m_pDea = 0");
        return;
    }
    if(m_eMode == ModeAddTransitionSelectFrom)
    {
        // nothing to do ;D
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
    
    if(handleStateDrag(event)) // dragging of state is in an own function
    {
        hasToRepaint = TRUE;
    }
    switch(m_eMode)
    {
        case ModeDragState:
        {
            break;
        }
        case ModeDragTransition:
        {
            if(m_pDraggedTransition
               && m_pDraggedTransition->m_pStart
               && m_pDraggedTransition->m_pEnd)
            {
                int curve = m_pDraggedTransition->curveByDragPosition(event->pos());
                m_pDraggedTransition->m_nCurve = curve
                        - m_pDraggedTransition->m_nDragRotationOffset;
                //qDebug("curve = %d;  offset = %d", curve, m_pDraggedTransition->m_nDragRotationOffset);
                m_pDraggedTransition->setWasChanged();
            }
            hasToRepaint = TRUE;
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
                if(m_pHoveredState)
                {
                    // if an state already is hovered
                    // (state has higher hover priority)
                    // then don't hover any transition
                    transition = NULL;
                }
                if(transition != m_pHoveredTransition)
                {
                    if(m_pHoveredTransition)
                    {
                        m_pHoveredTransition->m_bHovered = FALSE;
                        m_pHoveredTransition->setWasChanged();
                    }
                    if(transition)
                    {
                        transition->m_bHovered = TRUE;
                        transition->setWasChanged();
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
        
        // move to axis of grid
        if(m_nGridResolution > 0)
        {
            QPoint newPos = alignPointToGrid(QPoint(newX, newY));
            newX = newPos.x();
            newY = newPos.y();
        }
        
        m_pDraggedState->move(newX, newY);
        m_pDraggedState->setWasChanged();
        hasToRepaint = TRUE;
        recomputeMinimumSize();
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
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(!event)
    {
        return;
    }
    int oldSelectedStateIndex = m_nSelectedStateIndex;
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
            currentState->setWasChanged();
            m_nSelectedStateIndex = i;
            if(m_eMode == ModeNormal)
            {
                // only start drag if there is currently normal mode
                // and only if left mousebutton is pressed
                if(event->button() == Qt::LeftButton)
                {
                    currentState->isDragged = TRUE;
                    currentState->m_nDragOffsetX = event->pos().x() - currentState->m_nX;
                    currentState->m_nDragOffsetY = event->pos().y() - currentState->m_nY;
                    // item was found, we don't need to search anymore
                    m_pDraggedState = currentState;
                    setCurrentMode(ModeDragState);
                }
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
    if(m_pSelectedTransition)
    {
        m_pSelectedTransition->setWasChanged();
    }
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
            currentTransition->setWasChanged();
            m_pSelectedTransition = currentTransition;
            m_pDraggedTransition = currentTransition;
            if(currentTransition->startEqualsEnd())
            {
                // if transition is a transtion from a state to the same state
                // we first have to compute the offset of the angle
                // similar to DEdit_GraphicalState::m_nDragOffsetX
                currentTransition->m_nDragRotationOffset =
                    currentTransition->curveByDragPosition(event->pos()) - 
                        currentTransition->m_nCurve;
            }else
            {
                // set offset to 0 if transition connects to different states
                currentTransition->m_nDragRotationOffset = 0;
            }
            setCurrentMode(ModeDragTransition);
        }
        else
        {
            currentTransition->m_bSelected = FALSE;
        }
    }
    
    // if something has been changed
    if(oldSelectedStateIndex != m_nSelectedStateIndex)
    {
        // then emit signal
        
        emit selectedStateIndexChanged(m_nSelectedStateIndex);
        
    }
    
    //qDebug("item %d was selected", m_nSelectedStateIndex);
    update();
}


void DEdit_Widget::mouseReleaseEvent(QMouseEvent*)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
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
    if(m_pDraggedTransition)
    {
        // m_pDraggedTransition now isn't dragged anymore
        m_pDraggedTransition->setWasChanged();
        m_pDraggedTransition = NULL;
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
            // switch back to normal mode
            setCurrentMode(ModeNormal);
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

void DEdit_Widget::mouseDoubleClickEvent(QMouseEvent*)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    editItem();
}


void DEdit_Widget::recomputeMinimumSize()
{
    int margin = DEdit_GraphicalState::m_nDiameter/2-4;
    int maxX = DEdit_GraphicalState::m_nDiameter;
    int maxY = DEdit_GraphicalState::m_nDiameter;
    for(int i = 0; i < m_StateList.size(); ++i)
    {
        if(m_StateList[i].m_nX > maxX)
        {
            maxX = m_StateList[i].m_nX;
        }
        if(m_StateList[i].m_nY > maxY)
        {
            maxY = m_StateList[i].m_nY;
        }
    }
    
    setMinimumSize(maxX+margin, maxY+margin);
}

bool DEdit_Widget::isInDragMode() const
{
    return (bool)m_pDraggedState;
}

/// START: EVENTS

void DEdit_Widget::keyPressEvent(QKeyEvent* event)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(!event)
    {
        return;
    }
    if(event->key() == Qt::Key_Delete) // if del gets pressed
    {
        // then remove currently selected item
        removeSelectedItem();
    }
    else
    {
        event->ignore();
    }
}

void DEdit_Widget::keyReleaseEvent(QKeyEvent* event)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(!event)
    {
        return;
    }
    event->ignore();
}


void DEdit_Widget::dragEnterEvent(QDragEnterEvent* event)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(event->mimeData()->hasFormat(dndMimeFormat()))
    {
        event->acceptProposedAction();
//#ifdef Q_WS_WIN
        // only needed for drag'n'drop pixmap
        // replacement
        m_bAboutToDrop = TRUE;
//#endif
    }
}

void DEdit_Widget::dragMoveEvent (QDragMoveEvent* event )
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    m_cDropPreviewPosition = alignPointToGrid(event->pos());
    if(m_bAboutToDrop)
    {
        update();
    }
}

void DEdit_Widget::dragLeaveEvent (QDragLeaveEvent* event )
{
    if(m_bAboutToDrop)
    {
        m_bAboutToDrop = FALSE;
        update();
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
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    QString text = event->mimeData()->data(dndMimeFormat());
    if(text == dndAddStateCommand())
    {
        addState(alignPointToGrid(event->pos()));
    }
    m_bAboutToDrop = FALSE;
}


void DEdit_Widget::contextMenuEvent(QContextMenuEvent* event)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    // switch to normal mode
    setCurrentMode(ModeNormal);
    
    // backup context menu position for add state
    // and align this position to grid
    m_cContextMenuPosition = alignPointToGrid(event->pos());
    
    // only show context menu
    // if an item is selected
    if(m_pSelectedTransition)
    {
        // disable dragging transition
        m_mnuContextMenuTransition->exec(event->globalPos());
    }
    else if(m_nSelectedStateIndex >= 0)
    {
        m_mnuContextMenuState->exec(event->globalPos());
    }
    else
    {
        m_mnuContextMenuEmptySpace->exec(event->globalPos());
    }
}

/// END: EVENTS

void DEdit_Widget::setStateSelected(int index)
{
    // deselect old selection
    if(m_nSelectedStateIndex >= 0 && m_nSelectedStateIndex < m_StateList.size())
    {
        m_StateList[m_nSelectedStateIndex].isSelected = FALSE;
    }
    // and also deselect transitions
    if(m_pSelectedTransition)
    {
        m_pSelectedTransition->m_bSelected = FALSE;
    }
    // select new item
    if(index >= 0 && index < m_StateList.size())
    {
        m_StateList[index].isSelected = TRUE;
    }
    // set m_nSelectedStateIndex to new selected index
    m_nSelectedStateIndex = index;
    // request update
    update();
}

void DEdit_Widget::setTransitionSelected(int index)
{
    // deselect old selection
    if(m_pSelectedTransition)
    {
        m_pSelectedTransition->m_bSelected = FALSE;
        m_pSelectedTransition->setWasChanged();
        m_pSelectedTransition = NULL;
    }
    // and also deselect states
    if(m_nSelectedStateIndex >= 0 && m_nSelectedStateIndex < m_StateList.size())
    {
        m_StateList[m_nSelectedStateIndex].isSelected = FALSE;
    }
    m_nSelectedStateIndex = -1;
    // select new transition
    if(index >= 0 && index < m_TransitionList.size())
    {
        m_TransitionList[index].m_bSelected = TRUE;
        m_pSelectedTransition = &m_TransitionList[index];
        m_pSelectedTransition->setWasChanged();
    }
    else
    {
        // no item is selected
        m_pSelectedTransition = NULL;
    }
    
    
}

void DEdit_Widget::moveSelectionUp()
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
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
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
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

void DEdit_Widget::setLocked(bool locked)
{
    if(locked)
    {
        setCurrentMode(ModeLocked);
    }
    else
    {
        setCurrentMode(ModeNormal);
    }
    update();
}

bool DEdit_Widget::isLocked() const
{
    return m_eMode == ModeLocked;
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
    if(m_eMode == ModeDragState)
    {
        // if mode WAS ModeDragState
        // and if now mode has changed
        // -> state was moved
        sendHistoryChangeRequest(ChangeTypeStateMoved);
    }
    
    if(m_eMode == ModeDragTransition)
    {
        // if mode WAS ModeDragTransition
        // and if now mode has changed
        // -> transition curve was changed/dragged
        sendHistoryChangeRequest(ChangeTypeTransitionCurveChanged);
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
        case ModeDragTransition:
        case ModeDragState: {
            newCursor = QCursor(Qt::ClosedHandCursor);
            break;
        }
        case ModeLocked:
        case ModeNormal:
        default: {
            newCursor = QCursor(Qt::ArrowCursor);
            break;
        }
    }
    setCursor(newCursor);
    
    emit currentModeChanged(m_eMode);
}


void DEdit_Widget::setDeaWasChanged(bool wasChanged)
{
    m_bDeaWasChanged = wasChanged;
}

bool DEdit_Widget::isDeaWasChanged()
{
    return m_bDeaWasChanged;
}

void DEdit_Widget::emitDeaWasChanged()
{
    setDeaWasChanged();
    emit deaWasChanged();
}

void DEdit_Widget::createTransition(DEdit_GraphicalState* from, DEdit_GraphicalState* to)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
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
    
    if( !allowNonDeterministic()  // don't check this, if this "hack" is aktivated
        && graphicalTransitionForStartAndEnd(from, to))
    {
        // if transition already exists
        // then don't add a new transition
        QString msg = tr("A Transition from \'%start\' to \'%end\' already exists.");
        if(from->m_pData)
        {
            msg.replace("%start", from->m_pData->name());
        }
        if(to->m_pData)
        {
            msg.replace("%end", to->m_pData->name());
        }
        putErrorMessage(msg);
        return;
    }
    
    
    // create new transition in DEA
    DEA_Transition* transition = m_pDea->createTransition(from->m_pData, to->m_pData, '\0');
    // init new graphical transition
    DEdit_GraphicalTransition newGTransition(from, to);
    newGTransition.m_pData = transition;
    // add new graphical transition to list
    m_TransitionList.append(newGTransition);
    if(m_bAutoEditNewTransitions)
    {
        // select new Transition
        setTransitionSelected(m_TransitionList.size()-1);
        // edit it
        editSelectedTransition();
    }
    //sendHistoryChangeRequest(ChangeTypeTransitionAdded); // save current dea to history
    emitDeaWasChanged(); // dea just has been edited
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


DEdit_GraphicalTransition* DEdit_Widget::graphicalTransitionForData(DEA_Transition* data)
{
    DEdit_GraphicalTransition* foundItem = NULL;
    
    for(int i = m_TransitionList.size()-1; i >= 0; --i){
        if(m_TransitionList[i].m_pData == data)
        {
            foundItem = &(m_TransitionList[i]);
            break;
        }
    }
    return foundItem;
}


DEdit_GraphicalTransition* DEdit_Widget::graphicalTransitionForStartAndEnd
        (DEdit_GraphicalState* start, DEdit_GraphicalState* end)
{
    DEdit_GraphicalTransition* transition;
    for(int i = 0; i < m_TransitionList.size(); ++i)
    {
        transition = &m_TransitionList[i];
        if(transition->m_pStart == start
          && transition->m_pEnd == end)
        {
            return transition;
        }
    }
    return NULL;
}



QList<DEdit_GraphicalTransition*> DEdit_Widget::graphicalTransitionsWithStart
        (DEdit_GraphicalState* start)
{
    QList<DEdit_GraphicalTransition*> result;
    for(int i = 0; i < m_TransitionList.size(); ++i)
    {
        if(m_TransitionList[i].m_pStart == start)
        {
            result.append(&m_TransitionList[i]);
        }
    }
    return result;
}

void DEdit_Widget::removeTransition()
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    removeTransition(m_pSelectedTransition);
    // reset selection
    m_pSelectedTransition = NULL;
    m_pHoveredTransition = NULL;
    update();
    sendHistoryChangeRequest(ChangeTypeTransitionRemoved); // save current dea to history
}

void DEdit_Widget::removeTransition(DEdit_GraphicalTransition* transition)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
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
    
    // remove transition from m_pDea
    m_pDea->removeTransition(transition->m_pData);
    m_TransitionList.removeAt(index);
    emitDeaWasChanged(); // dea just has been edited
    update();
}


void DEdit_Widget::resetTransitionCurve()
{
    if(m_pSelectedTransition)
    {
        m_pSelectedTransition->m_nCurve = 0;
        m_pSelectedTransition->setWasChanged();
    }
    update();
}

QPixmap DEdit_Widget::stateTemplatePixmap() const
{
    return m_cWidgetPainter.m_cStateNormalTemplate;
}

void DEdit_Widget::removeSelectedItem()
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(m_pSelectedTransition)
    {
        removeTransition();
    }
    else
    {
        removeState();
    }
}


void DEdit_Widget::editItem()
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(m_pSelectedTransition)
    {
        editSelectedTransition();
    }
    else if(m_nSelectedStateIndex >= 0)
    {
        editSelectedState();
    }
}


void DEdit_Widget::editSelectedState()
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(!m_pDea)
    {
        DEBUG_MSG("editSelectedState()", "m_pDea = 0");
        return;
    }
    if(m_nSelectedStateIndex < 0 || m_nSelectedStateIndex > m_StateList.size()){
        // no item selected
        return;
    }
    if(!m_diaEditState)
    {
        m_diaEditState = new DEdit_EditStateDia(this);
    }
    DEdit_GraphicalState* state = &m_StateList[m_nSelectedStateIndex];
    m_diaEditState->setStateToEdit(state);
    m_diaEditState->exec();
    setSelectedState_StartState(state->m_bStartState);
    updateStateContextMenu();
    emitDeaWasChanged(); // dea just has been edited
    sendHistoryChangeRequest(ChangeTypeStateEdited);
    update();
}

void DEdit_Widget::editSelectedTransition()
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(!m_pDea)
    {
        DEBUG_MSG("editSelectedTransition()", "m_pDea = 0");
        return;
    }
    
    if(!m_pSelectedTransition)
    {
        // no transition selected
        return;
    }
    if(!m_diaEditTransition)
    {
        m_diaEditTransition = new DEdit_EditTransitionDia(this);
    }
    m_diaEditTransition->setTransitionToEdit(m_pSelectedTransition);
    m_diaEditTransition->exec();
    
    // if edit failed -> no symbol entered
    if(m_pSelectedTransition->symbols().isEmpty())
    {
        // then remove again
        removeTransition(m_pSelectedTransition);
        // reset selection
        m_pSelectedTransition = NULL;
        m_pHoveredTransition = NULL;
        m_pDraggedTransition = NULL;
    }
    emitDeaWasChanged(); // dea just has been edited
    update();
    sendHistoryChangeRequest(ChangeTypeTransitionEdited);
}


DEdit_GraphicalState* DEdit_Widget::findStateByName(QString name)
{
    DEdit_GraphicalState* result = NULL;
    DEdit_GraphicalState* currentItem;
    for(int i = 0; i < m_StateList.size(); ++i)
    {
        currentItem = & m_StateList[i];
        if(currentItem->m_pData
           && name == currentItem->m_pData->name())
        {
            result = currentItem;
            break;
        }
    }
    return result;
}


void DEdit_Widget::writeDeaToFile(xmlObject* file)
{
    if(!file || !m_pDea)
    {
        return;
    }
    //file->setName("test");
    m_pDea->writeToFile(file);
    xmlObject* stateList = file->cAddObject("graphicalstates");
    writeGraphicalStatesToFile(stateList);
    xmlObject* transitionList = file->cAddObject("graphicaltransitions");
    writeGraphicalTransitionsToFile(transitionList);
}

void DEdit_Widget::writeGraphicalStatesToFile(xmlObject* stateList)
{
    if(!stateList)
    {
        return;
    }
    DEdit_GraphicalState* state;
    xmlObject*            object;
    int x, y;
    const char* szName;
    for(int i = 0; i < m_StateList.size(); ++i)
    {
        state = &m_StateList[i];
        if(!state->m_pData)
        {
            // if state is invalid, then continue
            continue;
        }
        // get attributes
        x = state->m_nX;
        y = state->m_nY;
        szName = state->m_pData->name();
        
        // write attributes to statelist
        object = stateList->cAddObject("stateposition");
        object->cAddAttribute("name", szName);
        object->cAddAttribute("x", QString::number(x).toAscii().data());
        object->cAddAttribute("y", QString::number(y).toAscii().data());
    }
}


void DEdit_Widget::writeGraphicalTransitionsToFile(xmlObject* transitionList)
{
    if(!transitionList)
    {
        return;
    }
    DEdit_GraphicalTransition* transition;
    xmlObject*     object;
    int curve;
    const char* startname;
    const char* endname;
    QString symbols;
    for(int i = 0; i < m_TransitionList.size(); ++i)
    {
        transition = &m_TransitionList[i];
        if(!transition->hasValidPointers())
        {
            // if transition is invalid then continue
            continue;
        }
        // get attributes
        startname = transition->m_pStart->m_pData->name();
        endname   = transition->m_pEnd->m_pData->name();
        curve     = transition->m_nCurve;
        symbols   = transition->symbols();
        // create new object
        object = transitionList->cAddObject("transitionoption");
        object->cAddAttribute("start", startname);
        object->cAddAttribute("end",   endname);
        object->cAddAttribute("symbol", symbols.toLocal8Bit().data());
        object->cAddAttribute("curve", QString::number(curve).toAscii().data());
    }
}

bool DEdit_Widget::createDeaFromFile(xmlObject* file)
{
    if(isLocked())
    {
        m_szLastSyntaxError = tr("Editor currently is locked, please unlock first");
        // return if widget was locked
        return FALSE;
    }
    clearCompleteDEA();
    if(!m_pDea)
    {
        m_szLastSyntaxError = "Internal Error: m_pDea = NULL";
        return FALSE;
    }
    /// at first: init m_pDea
    if(!m_pDea->initFromFile(file))
    {
        m_szLastSyntaxError = "Error in state or transition list";
        return FALSE;
    }
    
    xmlObject* graphicalStates = file->cGetObjectByName("graphicalstates");
    if(!graphicalStates)
    {
        m_szLastSyntaxError = "Arbitrary tag <graphicalstates> is missing in <automat>";
        return FALSE;
    }
    if(!createGraphicalStatesFromFile(graphicalStates))
    {
        return FALSE;
    }
    xmlObject* graphicalTransitions = file->cGetObjectByName("graphicaltransitions");
    if(!createGraphicalTransitionsFromFile(graphicalTransitions))
    {
        return FALSE;
    }
    // state position has changed
    // so recompute minimum size, so that we can see all states
    recomputeMinimumSize();
    emitDeaWasChanged(); // dea just has been edited
    update();
    return TRUE;
}


bool DEdit_Widget::createGraphicalStatesFromFile(xmlObject* stateList)
{
    if(isLocked())
    {
        // return if widget was locked
        return FALSE;
    }
    if(!stateList || !m_pDea)
    {
        return FALSE;
    }
    xmlObject* stateObject;
    xmlAttribute* attribute;
    DEdit_GraphicalState graphicalState;
    DEA_State*      pState;
    char*     szStateName;
    int x, y;
    for(int i = 0; i < stateList->nGetObjectCounter(); ++i)
    {
        stateObject = stateList->cGetObjectByIdentifier(i);
        if(!stateObject)
        {
            continue;
        }
        /// read attribute name
        attribute = stateObject->cGetAttributeByName("name");
        if(!attribute)
        {
            m_szLastSyntaxError = "in <stateposition>: attribute name=\"\" is missing";
            return FALSE;
        }
        szStateName = attribute->value();
        pState = m_pDea->findState(szStateName);
        if(!pState)
        {
            m_szLastSyntaxError = "<stateposition> has name " + QString(szStateName) + "\n";
            m_szLastSyntaxError += "but there is no such state in <zustaende>";
            return FALSE;
        }
        /// read attribute positon: x and y
        attribute = stateObject->cGetAttributeByName("x");
        if(!attribute)
        {
            m_szLastSyntaxError = "in <stateposition>: attribute x=\"\" is missing";
            return FALSE;
        }
        x = attribute->nValueToInt();
        attribute = stateObject->cGetAttributeByName("y");
        if(!attribute)
        {
            m_szLastSyntaxError = "in <stateposition>: attribute y=\"\" is missing";
            return FALSE;
        }
        y = attribute->nValueToInt();
        
        /// init graphical state from attribute values
        graphicalState = DEdit_GraphicalState(pState);
        graphicalState.m_pWidgetPainter = &m_cWidgetPainter;
        graphicalState.m_bStartState = (pState == m_pDea->startState());
        graphicalState.move(x, y);
        m_StateList.append(graphicalState);
        // if just added state is start state
        if(graphicalState.m_bStartState)
        {
            m_pStartState = &m_StateList.last();
        }
    }
    
    return TRUE;
}


bool DEdit_Widget::createGraphicalTransitionsFromFile(xmlObject* transitionList)
{
    if(isLocked())
    {
        // return if widget was locked
        return FALSE;
    }
    // info: parameter xmlObject* transitionList not needed yet
    if(!m_pDea)
    {
        return FALSE;
    }
    DEdit_GraphicalTransition graphicalTransition;
    DEA_Transition* transition;
    DEA_State *from, *to;
    int curve;
    DEdit_GraphicalState *graphFrom, *graphTo;
    xmlObject* transitionGuiInfo;
    xmlAttribute* curveAttribute;
    for(int i = 0; i < m_pDea->transitionCount(); ++i)
    {
        transition = m_pDea->transitionAt(i);
        if(!transition)
        {
            continue;
        }
        /// load attributes for graphical transition
        // data start and end
        from = transition->start();
        to   = transition->end();
        if(!from || !to)
        {
            m_szLastSyntaxError = "Creating graphical transitions: Internal Pointer Error: (!from || !to)";
            return FALSE;
        }
        // graphical start and end
        graphFrom = findStateByName(from->name());
        graphTo   = findStateByName(to->name());
        if(!graphFrom || !graphTo)
        {
            m_szLastSyntaxError = "Creating graphical transitions: Internal Pointer Error: (!graphFrom || !graphTo)";
            return FALSE;
        }
        transitionGuiInfo = findGraphicalTransitionObject(transitionList, from->name(), to->name(), transition->inputSymbols());
        if(transitionGuiInfo
           && (curveAttribute = transitionGuiInfo->cGetAttributeByName("curve")))
        {   // if info was found, then set curve
            curve = curveAttribute->nValueToInt();
        }else{
            curve = 0;
        }
        /// init graphical transition
        graphicalTransition = DEdit_GraphicalTransition(graphFrom, graphTo);
        graphicalTransition.m_pData = transition;
        graphicalTransition.m_nCurve = curve;
        m_TransitionList.append(graphicalTransition);
    }
    
    return TRUE;
}


xmlObject* DEdit_Widget::findGraphicalTransitionObject
        (xmlObject* transitionList, const char* start, const char* end, const char* symbols)
{
    xmlObject* result = NULL;
    if(!transitionList || !start || !end || !symbols)
    {
        return result;
    }
    xmlAttribute* attribute;
    xmlObject* currentObj;
    for(int i = 0; i < transitionList->nGetObjectCounter(); ++i)
    {
        //qDebug("i = %d for start=%s end=%s", i, start, end);
        currentObj = transitionList->cGetObjectByIdentifier(i);
        if(!currentObj)
        {
            continue;
        }
        // check all attributes
        // check start
        attribute = currentObj->cGetAttributeByName("start");
        if(!attribute || !!strcmp(attribute->value(), start))
        {
            //qDebug("attribute start not found");
            continue;
        }
        // check end
        attribute = currentObj->cGetAttributeByName("end");
        if(!attribute || !!strcmp(attribute->value(), end))
        {
            //qDebug("attribute end not found");
            continue;
        }
        // check symbols
        attribute = currentObj->cGetAttributeByName("symbol");
        if(!attribute || !!strcmp(attribute->value(), symbols))
        {
            //qDebug("attribute symbol not found");
            continue;
        }
        // if this point is reached,
        // then we have found wanted transition object
        result = currentObj;
        break;
    }
    return result;
}

QString DEdit_Widget::lastSyntaxError()
{
    return m_szLastSyntaxError;
}


void DEdit_Widget::clearCompleteDEA()
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(!m_pDea)
    {
        return;
    }
    
    /// clear GRAPHICAL DEA
    // states
    m_StateList.clear();
    m_pDraggedState = NULL;
    m_pHoveredState = NULL;
    m_pStartState = NULL;
    m_nSelectedStateIndex = -1;
    // transitions
    m_TransitionList.clear();
    m_pAddTransitionStateFrom = NULL;
    m_pAddTransitionStateTo = NULL;
    m_pHoveredTransition = NULL;
    m_pSelectedTransition = NULL;
    /// clear m_pDea
    if(m_pDea)
    {
        m_pDea->setStartState(NULL);
        m_pDea->setStateCount(0);
        m_pDea->setTransitionCount(0);
    }
    // state position has changed
    // so recompute minimum size, so that we can see all states
    recomputeMinimumSize();
    emitDeaWasChanged(); // dea just has been edited
    update();
}


void DEdit_Widget::setSelectedState_FinalState(bool finalState)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(m_nSelectedStateIndex < 0 || m_nSelectedStateIndex >= m_StateList.size())
    {
        return;
    }
    DEdit_GraphicalState* state = &m_StateList[m_nSelectedStateIndex];
    if(state->m_pData)
    {
        state->m_pData->setFinalState(finalState);
    }
    emitDeaWasChanged(); // dea just has been edited
    sendHistoryChangeRequest(ChangeTypeStateEdited);
    update();
}

void DEdit_Widget::setSelectedState_StartState(bool startState)
{
    if(isLocked())
    {
        // return if widget was locked
        return;
    }
    if(m_nSelectedStateIndex < 0 || m_nSelectedStateIndex >= m_StateList.size())
    {
        return;
    }
    DEdit_GraphicalState* state = &m_StateList[m_nSelectedStateIndex];
    state->m_bStartState = startState;
    // if just edited state is the new start state
    if(state->m_bStartState && (m_pStartState != state))
    {
        // then unselect old startstate
        if(m_pStartState)
        {
            m_pStartState->m_bStartState = FALSE;
        }
        // then set just edited state to new start state
        m_pStartState = state;
        // apply changes to dea
        m_pDea->setStartState(m_pStartState->m_pData);
    }
    else if(!state->m_bStartState && (m_pStartState == state))
    {// if just edited state now don't wants to be start state anymore
        m_pDea->setStartState(NULL);
        m_pStartState = NULL;
    }
    
    emitDeaWasChanged(); // dea just has been edited
    sendHistoryChangeRequest(ChangeTypeStateEdited);
    update();
}


void DEdit_Widget::updateStateContextMenu()
{
    if(m_nSelectedStateIndex < 0 || m_nSelectedStateIndex >= m_StateList.size())
    {
        return;
    }
    
    DEdit_GraphicalState* state = &m_StateList[m_nSelectedStateIndex];
    m_mnaSetFinalState->setChecked(state->m_pData && state->m_pData->isFinalState());
    m_mnaSetStartState->setChecked(state->m_bStartState);
}


void DEdit_Widget::putErrorMessage(QString msg)
{
    QMessageBox::critical(window(), tr("DEA Editor"), msg);
}

/// START: HISTORY

void DEdit_Widget::undo()
{
    if(isUndoPossible())
    {
        m_cHistory.undo();
        emitDeaWasChanged();
        emit historyChanged();
    }
}

void DEdit_Widget::redo()
{
    if(isRedoPossible())
    {
        m_cHistory.redo();
        emitDeaWasChanged();
        emit historyChanged();
    }
}


bool DEdit_Widget::isUndoPossible()
{
    return m_cHistory.isUndoPossible();
}

bool DEdit_Widget::isRedoPossible()
{
    return m_cHistory.isRedoPossible();
}


void DEdit_Widget::sendHistoryChangeRequest(EDeaChangeType eType)
{
    if(m_bAutoEditNewTransitions
       && ChangeTypeTransitionAdded == eType
       && (ChangeTypeTransitionEdited & m_nHistorySaveReasonFlags))
    {
        // if a history item is saved on edit
        // we don't need to save it for TransitionAdded,
        // because when a transition gets added, the editTransition() gets called
        // and so history will be saved
        return;
    }
    if(m_bAutoEditNewStates
      && ChangeTypeStateAdded == eType
      && (ChangeTypeStateEdited & m_nHistorySaveReasonFlags))
    {
        // same as for transition added
        return;
    }
    if(m_nHistorySaveReasonFlags & eType)
    {
        // if current type needs a new history item
        // then create a new item
        m_cHistory.saveCurrentStateToHistory();
        emit historyChanged();
    }
}

void DEdit_Widget::setMaxHistorySize(int size)
{
    if(size != m_cHistory.maxHistorySize())
    {
        m_cHistory.setMaxHistorySize(size);
    }
    emit historyChanged();
}

int  DEdit_Widget::maxHistorySize() const
{
    return m_cHistory.maxHistorySize();
}

void DEdit_Widget::clearHistory()
{
    m_cHistory.clear();
    emit historyChanged();
}

/// END: HISTORY


/// START SOME VISUAL OPTIONS
QPoint DEdit_Widget::alignPointToGrid(QPoint p) const
{
    if(m_nGridResolution) // only align, if grid resolution is != 0
    {
        int dX = p.x() % m_nGridResolution;
        int dY = p.y() % m_nGridResolution;
        int halfgrid = m_nGridResolution/2;
        if (dX >= halfgrid)
        {
            dX -= m_nGridResolution;
        }
        if (dY >= halfgrid)
        {
            dY -= m_nGridResolution;
        }
        
        p.setX( p.x() - dX );
        p.setY( p.y() - dY );
    }
    return p;
}

void DEdit_Widget::setGridResolution(int resolution)
{
    m_nGridResolution = resolution;
    if(m_nGridResolution < 0)
    {
        m_nGridResolution = 0;
    }
    if(m_nGridResolution > width()/2)
    {
        m_nGridResolution = width()/2;
    }
}

int DEdit_Widget::gridResolution() const
{
    return m_nGridResolution;
}

void DEdit_Widget::setAutoEditNewStates(bool on)
{
    m_bAutoEditNewStates = on;
}

bool DEdit_Widget::autoEditNewStates() const
{
    return m_bAutoEditNewStates;
}

void DEdit_Widget::setAutoEditNewTransitions(bool on)
{
    m_bAutoEditNewTransitions = on;
}

bool DEdit_Widget::autoEditNewTransitions() const
{
    return m_bAutoEditNewTransitions;
}

DEdit_Appearance* DEdit_Widget::appearance()
{
    return &(m_cWidgetPainter.m_cAppearance);
}

void DEdit_Widget::recreateAllGuiTemplates()
{
    m_cWidgetPainter.recreateAllTemplates();
    // request repaint for all states and transitions
    m_cWidgetPainter.setAllItemsToWasChanged();
    update();
}

void DEdit_Widget::setAllowNonDeterministic(bool bAllow)
{
    m_bAllowNonDeterministic = bAllow;
}

bool DEdit_Widget::allowNonDeterministic() const
{
    return m_bAllowNonDeterministic;
}

/// END SOME VISUAL OPTIONS


