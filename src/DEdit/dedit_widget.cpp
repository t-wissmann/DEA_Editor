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
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QContextMenuEvent>

DEdit_Widget::DEdit_Widget() :
  QWidget(),
  m_cWidgetPainter(this)
{
    // init some members
    m_pDea = NULL;
    m_eMode = ModeNormal;
    m_nGridResolution = 0; // disable grid per default
    // for states
    m_pDraggedState = NULL;
    m_pHoveredState = NULL;
    m_nSelectedStateIndex = -1;
    m_bAboutToDrop = FALSE;
    m_pStartState = NULL;
    // for transitions
    m_pHoveredTransition = NULL;
    m_pSelectedTransition = NULL;
    
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
    
    // actions
    m_mnaRemoveItem = new QAction(NULL);
    m_mnaEditItem = new QAction(NULL);
    
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
    m_mnuContextMenuTransition->addAction(m_mnaRemoveItem);
    m_mnuContextMenuTransition->addAction(m_mnaEditItem);
    
    // connect slots
    connect(m_mnaRemoveItem, SIGNAL(triggered()), this, SLOT(removeSelectedItem()));
    connect(m_mnaEditItem, SIGNAL(triggered()), this, SLOT(editItem()));
    connect(m_mnaSetStartState, SIGNAL(toggled(bool)), this,
            SLOT(setSelectedState_StartState(bool)));
    connect(m_mnaSetFinalState, SIGNAL(toggled(bool)), this,
            SLOT(setSelectedState_FinalState(bool)));
    // slot, to update context menu
    connect(this, SIGNAL(selectedStateIndexChanged(int)), this, SLOT(updateStateContextMenu()));
}


void DEdit_Widget::retranslateUi()
{
    m_mnaRemoveItem->setText(tr("Remove"));
    m_mnaEditItem->setText(tr("Edit"));
    m_mnaSetFinalState->setText(tr("Final State"));
    m_mnaSetStartState->setText(tr("Start State"));
    
}

void DEdit_Widget::reloadIcons()
{
    m_mnaRemoveItem->setIcon(IconCatcher::getIcon("remove"));
    m_mnaEditItem->setIcon(IconCatcher::getIcon("edit"));
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
        
        // move to axis of grid
        if(m_nGridResolution > 0)
        {
            newX = newX - newX % m_nGridResolution;
            newY = newY - newY % m_nGridResolution;
        }
        
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

void DEdit_Widget::mouseDoubleClickEvent(QMouseEvent*)
{
    editItem();
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
#ifdef Q_WS_WIN
        // only needed for drag'n'drop pixmap
        // replacement
        m_bAboutToDrop = TRUE;
#endif
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
    m_bAboutToDrop = FALSE;
}


void DEdit_Widget::dragMoveEvent (QDragMoveEvent* event )
{
    m_cDropPreviewPosition = event->pos();
    if(m_bAboutToDrop)
    {
        update();
    }
}

void DEdit_Widget::contextMenuEvent(QContextMenuEvent* event)
{
    // only show context menu
    // if an item is selected
    if(m_pSelectedTransition)
    {
        m_mnuContextMenuTransition->exec(event->globalPos());
    }
    else if(m_nSelectedStateIndex >= 0)
    {
        m_mnuContextMenuState->exec(event->globalPos());
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
    
    emit currentModeChanged(m_eMode);
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
    newGTransition.m_pData = transition;
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
    
    // remove transition from m_pDe
    m_pDea->removeTransition(transition->m_pData);
    m_TransitionList.removeAt(index);
}




QPixmap DEdit_Widget::stateTemplatePixmap() const
{
    return m_cWidgetPainter.m_cStateNormalTemplate;
}

void DEdit_Widget::removeSelectedItem()
{
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
    
    update();
}

void DEdit_Widget::editSelectedTransition()
{
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
    update();
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

bool DEdit_Widget::createDeaFromFile(xmlObject* file)
{
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
    if(!createGraphicalTransitionsFromFile(file))
    {
        return FALSE;
    }
    update();
    return TRUE;
}


bool DEdit_Widget::createGraphicalStatesFromFile(xmlObject* stateList)
{
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
    }
    
    return TRUE;
}


bool DEdit_Widget::createGraphicalTransitionsFromFile(xmlObject*)
{
    // info: parameter xmlObject* transitionList not needed yet
    if(!m_pDea)
    {
        return FALSE;
    }
    DEdit_GraphicalTransition graphicalTransition;
    DEA_Transition* transition;
    DEA_State *from, *to;
    DEdit_GraphicalState *graphFrom, *graphTo;
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
        
        /// init graphical transition
        graphicalTransition = DEdit_GraphicalTransition(graphFrom, graphTo);
        graphicalTransition.m_pData = transition;
        graphicalTransition.m_szSymbols = transition->inputSymbols();
        m_TransitionList.append(graphicalTransition);
    }
    
    return TRUE;
}

QString DEdit_Widget::lastSyntaxError()
{
    return m_szLastSyntaxError;
}


void DEdit_Widget::clearCompleteDEA()
{
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
    update();
}


void DEdit_Widget::setSelectedState_FinalState(bool finalState)
{
    if(m_nSelectedStateIndex < 0 || m_nSelectedStateIndex >= m_StateList.size())
    {
        return;
    }
    DEdit_GraphicalState* state = &m_StateList[m_nSelectedStateIndex];
    if(state->m_pData)
    {
        state->m_pData->setFinalState(finalState);
    }
    update();
}

void DEdit_Widget::setSelectedState_StartState(bool startState)
{
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



/// START SOME VISUAL OPTIONS
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
/// END SOME VISUAL OPTIONS
