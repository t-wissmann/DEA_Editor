
#ifndef _DEA__DEA_TRANSITION_H_
#define _DEA__DEA_TRANSITION_H_

#define DEA_TRANSITION_INPUT_SYMBOL_COUNT 256

class DEA_State;

class DEA_Transition{
public:
    DEA_Transition(char inputSymbol = '\0');
    DEA_Transition(DEA_State* from, DEA_State* to,
                   char inputSymbol = '\0');
    ~DEA_Transition();
    
    void setStart(DEA_State* start);
    DEA_State* start() const;
    
    void setEnd(DEA_State* end);
    DEA_State* end() const;
    
    void setInputSymbol(char character);
    char inputSymbol() const;
    
    void setInputSymbols(char* symbols);
    char* inputSymbols();
    
    bool hasInputSymbol(char symbol);
    
private:
    DEA_State*   m_pStart;
    DEA_State*   m_pEnd;
    char         m_szInputSymbols[DEA_TRANSITION_INPUT_SYMBOL_COUNT];
};

#endif

