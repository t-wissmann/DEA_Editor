
#include "dea_state.h"
#include <string.h>
#include <stdio.h>
#include "dea_transition.h"

using namespace std;

DEA_State::DEA_State()
{
    m_bIsFinalState = false;
    m_szName[0] = '\0';
    //printf("DEA_State::DEA_State() - Construct\n");
}


DEA_State::DEA_State(const DEA_State& other)
{
    m_bIsFinalState = other.isFinalState();
    setName(other.name());
    //printf("DEA_State::DEA_State() - Construct\n");
}

DEA_State::DEA_State(char* name, bool finalState)
{
    m_bIsFinalState = finalState;
    setName(name);
    //printf("DEA_State::DEA_State() - Construct\n");
}

DEA_State::~DEA_State()
{
    
    //printf("DEA_State::~DEA_State() - Destruct\n");
}



void DEA_State::setName(const char* name)
{
    strncpy(m_szName, name, 80);
}

const char* DEA_State::name() const
{
    return m_szName;
}


void DEA_State::setFinalState(bool finalstate)
{
    m_bIsFinalState = finalstate;
}


bool DEA_State::isFinalState() const
{
    return m_bIsFinalState;
}


void DEA_State::addTransition(DEA_Transition* transition)
{
    m_TransitionList.push_back(transition);
}


void DEA_State::removeTransition(DEA_Transition* transition)
{
    int index = -1;
    for(int i = 0; i < m_TransitionList.size(); ++i)
    {
        if(m_TransitionList[i] == transition)
        {
            index = i;
            break;
        }
    }
    if(index < 0)
    {// return on invalid index
        return;
    }
    // remove transition from m_TransitionList
    m_TransitionList.erase(m_TransitionList.begin()+index,
                           m_TransitionList.begin()+index+1);
    // only a debug message:
    //printf("transition %d removed\n", transition);
}

vector<DEA_Transition*> DEA_State::connectionList()
{
    return m_TransitionList;
}


DEA_Transition* DEA_State::findTransitionForSymbol(char symbol)
{
    for(int i = 0; i < m_TransitionList.size(); ++i)
    {
        if(m_TransitionList[i]->hasInputSymbol(symbol))
        {
            return m_TransitionList[i];
        }
    }
    return NULL;
}


