
#include "dea_transition.h"
#include "dea_state.h"

#include <stdlib.h>

DEA_Transition::DEA_Transition(char inputSymbol)
{
    m_pStart = NULL;
    m_pEnd = NULL;
    m_chInputSymbol = inputSymbol;
}


DEA_Transition::DEA_Transition(DEA_State* from, DEA_State* to,
                char inputSymbol)
{
    m_pStart = from;
    m_pEnd   = to;
    m_chInputSymbol = inputSymbol;
}

DEA_Transition::~DEA_Transition()
{
    if(m_pStart)
    {
        m_pStart->removeTransition(this);
    }
}


void DEA_Transition::setStart(DEA_State* start)
{
    m_pStart = start;
}

DEA_State* DEA_Transition::start() const
{
    return m_pStart;
}


void DEA_Transition::setEnd(DEA_State* end)
{
    m_pEnd = end;
}

DEA_State* DEA_Transition::end() const
{
    return m_pEnd;
}

void DEA_Transition::setInputSymbol(char character)
{
    m_chInputSymbol = character;
}

char DEA_Transition::inputSymbol() const
{
    return m_chInputSymbol;
}


