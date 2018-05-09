
#include "dea_transition.h"
#include "dea_state.h"

#include <stdlib.h>

#include <string.h>

DEA_Transition::DEA_Transition(char inputSymbol)
{
    m_pStart = NULL;
    m_pEnd = NULL;
    m_szInputSymbols[0] = inputSymbol;
    m_szInputSymbols[1] = '\0';
}


DEA_Transition::DEA_Transition(DEA_State* from, DEA_State* to,
                char inputSymbol)
{
    m_pStart = from;
    m_pEnd   = to;
    m_szInputSymbols[0] = inputSymbol;
    m_szInputSymbols[1] = '\0';
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
    m_szInputSymbols[0] = character;
    m_szInputSymbols[1] = '\0';
}

char DEA_Transition::inputSymbol() const
{
    return m_szInputSymbols[0];
}


void DEA_Transition::setInputSymbols(char* symbols)
{
    strncpy(m_szInputSymbols, symbols,
            DEA_TRANSITION_INPUT_SYMBOL_COUNT);
    m_szInputSymbols[DEA_TRANSITION_INPUT_SYMBOL_COUNT-1] = '\0';
}


char* DEA_Transition::inputSymbols()
{
    return m_szInputSymbols;
}


bool DEA_Transition::hasInputSymbol(char symbol)
{
    for(int i = 0; i < DEA_TRANSITION_INPUT_SYMBOL_COUNT; ++i)
    {
        if(m_szInputSymbols[i] == symbol)
        {
            // return true if symbol was found
            return 1;
        }
        if(m_szInputSymbols[i] == '\0')
        {
            // break if end of string has been reached
            break;
        }
    }
    // return false: symbol couldn't be found
    return 0;
}


