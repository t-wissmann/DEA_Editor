#include "dea.h"
#include "dea_transition.h"
#include "dea_state.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <io/xmlparser.h>

using namespace std;

DEA::DEA()
{
    m_uStateCount = 0;
    m_pStateBuf   = NULL;
    m_pStartState = NULL;
    m_bDebugMode  = true;
    
    m_uTransitionCount = 0;
    m_pTransitionBuf   = NULL;
}

DEA::~DEA()
{
    // free all states
    setStateCount(0);
    // free all connections
    setTransitionCount(0);
}


void DEA::setStateCount(unsigned int count)
{
    if(count == m_uStateCount)
    {
        // nothing to do
        return;
    }
    DEA_State** pOldBuf = m_pStateBuf;
    unsigned int uOldCount = m_uStateCount;
    
    m_uStateCount = count;
    
    // allocate new memory
    if(m_uStateCount != 0)
    {
        m_pStateBuf = (DEA_State**) malloc(sizeof(DEA_State*) * m_uStateCount);
    }
    else
    {
        m_pStateBuf = NULL;
    }
    // copy existing items
    if(m_uStateCount != 0 && uOldCount != 0)
    {
        memcpy(m_pStateBuf, pOldBuf, sizeof(DEA_State*) * 
               ((uOldCount < m_uStateCount) ? uOldCount : m_uStateCount));
    }
    // init new memory
    for(int i = uOldCount; i < m_uStateCount; ++i)
    {
        m_pStateBuf[i] = NULL;
    }
    // free old buf & old states
    //printf("statecount = %d,,, oldcount = %d\n", m_uStateCount, uOldCount);
    for(int i = m_uStateCount; i < uOldCount; ++i)
    {
        if(pOldBuf[i])
        {
            //printf("i = %d., conent = %d \n", i, pOldBuf[i]);
            delete pOldBuf[i];
        }
    }
    if(pOldBuf)
    {
        free(pOldBuf);
    }
    //printf("State count = %d\n", m_uStateCount);
}

unsigned int DEA::stateCount()
{
    return m_uStateCount;
}


DEA_State* DEA::addState(const DEA_State& state)
{
    setStateCount(stateCount()+1);
    return (m_pStateBuf[m_uStateCount-1] = new DEA_State(state));
}


void DEA::setStartState(DEA_State* state)
{
    m_pStartState = state;
}

DEA_State* DEA::startState()
{
    return m_pStartState;
}


void DEA::setTransitionCount(unsigned int count)
{
    if(count == m_uTransitionCount)
    {
        // nothing to do
        return;
    }
    DEA_Transition** pOldBuf = m_pTransitionBuf;
    unsigned int uOldCount = m_uTransitionCount;
    
    m_uTransitionCount = count;
    
    // allocate new memory
    if(m_uTransitionCount != 0)
    {
        m_pTransitionBuf = (DEA_Transition**)
                malloc(sizeof(DEA_Transition*) * m_uTransitionCount);
    }
    else
    {
        m_pTransitionBuf = NULL;
    }
    // copy existing items
    if(m_uTransitionCount != 0 && uOldCount != 0)
    {
        memcpy(m_pTransitionBuf, pOldBuf, sizeof(DEA_Transition*) *
                ((uOldCount < m_uTransitionCount) ?
                       uOldCount : m_uTransitionCount));
    }
    // init new memory
    for(int i = uOldCount; i < m_uTransitionCount; ++i)
    {
        m_pTransitionBuf[i] = NULL;
    }
    // free old buf & old Transitions
    for(int i = m_uTransitionCount; i < uOldCount; ++i)
    {
        if(pOldBuf[i])
        {
            delete pOldBuf[i];
        }
    }
    if(pOldBuf)
    {
        free(pOldBuf);
    }
    //printf("Transition count set to %d\n", m_uTransitionCount);
}

unsigned int DEA::connectionCount()
{
    return m_uTransitionCount;
}


DEA_Transition* DEA::createTransition(DEA_State* start, DEA_State* end, char symbol)
{
    // init connection
    DEA_Transition* transition = new DEA_Transition(start, end, symbol);
    if(start)
    {
        start->addTransition(transition);
    }
    
    /*
    printf("creating transition from %s to %s via symbol %c\n",
           start ? start->name() : "NULL",
           end   ? end  ->name() : "NULL",
           symbol);
    */
    
    // add connection to Buf
    setTransitionCount(m_uTransitionCount+1);
    m_pTransitionBuf[m_uTransitionCount-1] = transition;
    return transition;
}

bool DEA::run(char* inputString)
{
    bool returnState = false;
    if(!m_pStartState)
    {
        putDebugMessage("Run: No Start State Defined");
        return false;
    }
    DEA_State* pCurrentState = m_pStartState;
    unsigned int uSymbolIndex = 0;
    vector<DEA_Transition*> connectionList;
    DEA_Transition*         pCurrentTransition;
    while(inputString[uSymbolIndex] != '\0')
    {
        if(!pCurrentState)
        {
            break;
        }
        connectionList = pCurrentState->connectionList();
        // clear current state
        pCurrentState = NULL;
        // find right transition
        for(int i = 0; i < connectionList.size(); ++i)
        {
            pCurrentTransition = connectionList.at(i);
            if(!pCurrentTransition)
            {
                continue;
            }
            if(pCurrentTransition->hasInputSymbol(inputString[uSymbolIndex]))
            {
                pCurrentState = pCurrentTransition->end();
                break;
            }
        }// for
        if(!pCurrentState)
        {
            break;
        }
        
        // go to next symbol in input stream
        uSymbolIndex++;
    }// while
    if(inputString[uSymbolIndex] == '\0')
    {
        returnState = (pCurrentState && pCurrentState->isFinalState());
        if(!returnState)
        {
            putDebugMessage("Run: String is read completely, but current state is no final state");
        }
    }
    else
    {
        
        printf("Run: No right transition found for char %c at position %d\n",
               inputString[uSymbolIndex], uSymbolIndex+1);
        returnState = false;
    }
    return returnState;
}


bool DEA::initFromFile(xmlObject* file)
{
    if(!file)
    {
        putDebugMessage("ERROR: Invalid File Pointer");
        return false;
    }
    // clear current StateBuf and TransitionBuf
    setStateCount(0);
    setTransitionCount(0);
    
    if(strcmp(file->name(), "automat"))
    {
        putDebugMessage("ERROR: Parent Tag must be \'automat\'");
        return false;
    }
    xmlObject* states = file->cGetObjectByName("zustaende");
    if(!initStatesFromStateList(states))
    {
        putDebugMessage("ERROR when parsing states");
        return false;
    }
    xmlObject* transitions = file->cGetObjectByName("uebergaenge");
    if(!initTransitionsFromTransitionList(transitions))
    {
        putDebugMessage("ERROR when parsing transitions");
    }
    
    return true;
}


bool DEA::initStatesFromStateList(xmlObject* stateList)
{
    if(!stateList)
    {
        return false;
    }
    xmlObject* currentState;
    xmlAttribute* currentAttribute;
    DEA_State currentStateObject;
    for(int i = 0; i < stateList->nGetObjectCounter(); ++i)
    {
        currentState = stateList->cGetObjectByIdentifier(i);
        if(!currentState)
        {
            continue;
        }
        if(strcmp(currentState->name(), "zustand"))
        {
            return false;
        }
        if(!(currentAttribute = currentState->cGetAttributeByName("name")))
        {
            return false;
        }
        currentStateObject.setName(currentAttribute->value());
        if(!(currentAttribute = currentState->cGetAttributeByName("finalzustand")))
        {
            return false;
        }
        currentStateObject.setFinalState(currentAttribute->nValueToInt());
        
        DEA_State* newState = addState(currentStateObject);
        //printf("state at %d added\n", newState);
        
    }
    
    // find out start state
    if(!(currentAttribute = stateList->cGetAttributeByName("start")))
    {
        putDebugMessage("ERROR: Needed attribute \'start\' in \'zustaende\' not existing");
        return false;
    }
    
    DEA_State* startState = findState(currentAttribute->value());
    setStartState(startState);
    if(!startState)
    {
        printf("ERROR: requested start state \'%s\' not existing in \'zustaende\'\n", currentAttribute->value());
        return false;
    }
    
    
    return true;
}

bool DEA::initTransitionsFromTransitionList(xmlObject* transitionList)
{
    if(!transitionList)
    {
        return false;
    }
    xmlObject* currentTransition;
    xmlAttribute* currentAttribute;
    DEA_State*  pStateStart;
    DEA_State*  pStateEnd;
    char        chInputSymbol;
    for(int i = 0; i < transitionList->nGetObjectCounter(); ++i)
    {
        currentTransition = transitionList->cGetObjectByIdentifier(i);
        if(!currentTransition)
        {
            continue;
        }
        if(strcmp(currentTransition->name(), "uebergang"))
        {
            return false;
        }
        if(!(currentAttribute = currentTransition->cGetAttributeByName("from")))
        {
            return false;
        }
        if(!(pStateStart = findState(currentAttribute->value())))
        {
            return false;
        }
        if(!(currentAttribute = currentTransition->cGetAttributeByName("to")))
        {
            return false;
        }
        if(!(pStateEnd = findState(currentAttribute->value())))
        {
            return false;
        }
        if(!(currentAttribute = currentTransition->cGetAttributeByName("symbol")))
        {
            return false;
        }
        char* symbols = currentAttribute->value();
        DEA_Transition* transition = createTransition(pStateStart, pStateEnd, '\0');
        transition->setInputSymbols(symbols);
        
    }
    
    return true;
}


DEA_State* DEA::findState(char* name)
{
    DEA_State* foundItem = NULL;
    
    for(int i = 0; i < m_uStateCount; ++i)
    {
        if(!m_pStateBuf[i])
        {
            continue;
        }
        if(!strcmp(m_pStateBuf[i]->name(), name))
        {
            foundItem = m_pStateBuf[i];
            break;
        }
    }
    return foundItem;
}


void DEA::putDebugMessage(char* msg)
{
    if(m_bDebugMode)
    {
        printf("%s\n", msg);
    }
}

void DEA::removeState(DEA_State* state)
{
    int index = indexOf(state);
    if(index < 0 || index >= m_uStateCount)
    {
        return;
    }
    // move all items behind index by 1 to front
    for(int i = index+1; i < m_uStateCount; ++i)
    {
        m_pStateBuf[i-1] = m_pStateBuf[i];
    }
    // and push item, that should be deleted to the end
    m_pStateBuf[m_uStateCount-1] = state;
    // now decrease buf length by 1
    setStateCount(m_uStateCount-1);
}

int  DEA::indexOf(DEA_State* state)
{
    int result = -1;
    for(int i = 0; i < m_uStateCount; ++i)
    {
        if(m_pStateBuf[i] == state){
            result = i;
            break;
        }
    }
    return result;
}

void DEA::removeTransition(DEA_Transition* transition)
{
    int index = indexOf(transition);
    if((index < 0) || (index >= m_uTransitionCount))
    {
        return;
    }
    // move all items behind index by 1 to front
    for(int i = index+1; i < m_uTransitionCount; ++i)
    {
        m_pTransitionBuf[i-1] = m_pTransitionBuf[i];
    }
    m_pTransitionBuf[m_uTransitionCount-1] = transition;
    // now decrease buf length by 1 -> last transition gets deleted
    setTransitionCount(m_uTransitionCount-1);
}

int DEA::indexOf(DEA_Transition* transition)
{
    for(int i = 0; i < m_uTransitionCount; ++i)
    {
        if(m_pTransitionBuf[i] == transition)
        {
            return i;
        }
    }
    return -1;
}


void DEA::writeToFile(xmlObject* file)
{
    if(!file)
    {
        return;
    }
    file->setName("automat");
    xmlObject* states = file->cAddObject("zustaende");
    states->cAddAttribute("start", m_pStartState ? m_pStartState->name() : "");
    writeStatesToFile(states);
    
    xmlObject* transitions = file->cAddObject("uebergaenge");
    writeTransitionsToFile(transitions);
    
}


void DEA::writeStatesToFile(xmlObject* stateList)
{
    if(!stateList)
    {
        return;
    }
    DEA_State* currentState;
    xmlObject* stateXmlObject;
    for(int i = 0; i < m_uStateCount; ++i)
    {
        currentState = m_pStateBuf[i];
        if(!currentState)
        {
            continue;
        }
        stateXmlObject = stateList->cAddObject("zustand");
        stateXmlObject->cAddAttribute("name", currentState->name());
        stateXmlObject->cAddAttribute("finalzustand",
                                      currentState->isFinalState() ? "1" : "0");
    }
    
}

void DEA::writeTransitionsToFile(xmlObject* transitionList)
{
    if(!transitionList)
    {
        return;
    }
    DEA_Transition* currentTransition;
    xmlObject* transitionXmlObject;
    DEA_State* pStateTmp;
    for(int i = 0; i < m_uTransitionCount; ++i)
    {
        currentTransition = m_pTransitionBuf[i];
        if(!currentTransition)
        {
            continue;
        }
        transitionXmlObject = transitionList->cAddObject("uebergang");
        pStateTmp = currentTransition->start();
        transitionXmlObject->cAddAttribute("from", pStateTmp ? pStateTmp->name() : "");
        pStateTmp = currentTransition->end();
        transitionXmlObject->cAddAttribute("to", pStateTmp ? pStateTmp->name() : "");
        transitionXmlObject->cAddAttribute("symbol", currentTransition->inputSymbols());
    }
}


