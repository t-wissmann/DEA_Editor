 

#ifndef _DEA__DEA_STATE_H_
#define _DEA__DEA_STATE_H_

#include <vector>


class DEA_Transition;

class DEA_State
{
public:
    DEA_State();
    DEA_State(const DEA_State& other);
    
    DEA_State(char* name, bool finalState = false);
    ~DEA_State();
    
    void setName(const char* name);
    const char* name() const;
    
    void setFinalState(bool finalstate);
    bool isFinalState() const;
    
    void addTransition(DEA_Transition* transition);
    void removeTransition(DEA_Transition* transition);
    std::vector<DEA_Transition*> connectionList();
    
    DEA_Transition* findTransitionForSymbol(char symbol);
    
private:
    bool    m_bIsFinalState;
    char    m_szName[80];
    std::vector<DEA_Transition*> m_TransitionList;
};


#endif

