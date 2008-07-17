#ifndef _DEA__DEA_H_
#define _DEA__DEA_H_

class DEA_Transition;
class DEA_State;
class xmlObject;

class DEA {
public:
    DEA();
    ~DEA();
    bool initFromFile(xmlObject* file);
    bool initStatesFromStateList(xmlObject* stateList);
    bool initTransitionsFromTransitionList(xmlObject* transitionList);
    
    void setStateCount(unsigned int count);
    unsigned int stateCount();
    
    void setTransitionCount(unsigned int count);
    unsigned int connectionCount();
    
    DEA_State* addState(const DEA_State& state);
    void removeState(DEA_State* state);
    int  indexOf(DEA_State* state);
    DEA_State* findState(char* name);
    
    void setStartState(DEA_State* state);
    DEA_State* startState();
    
    bool run(char* inputString);
    
    DEA_Transition* createTransition(DEA_State* start, DEA_State* end, char symbol);
    void removeTransition(DEA_Transition* transition);
    int indexOf(DEA_Transition* transition);
    
    void putDebugMessage(char* msg);
private:
    unsigned int m_uStateCount;
    DEA_State**  m_pStateBuf;
    DEA_State*   m_pStartState;
    bool         m_bDebugMode;
    
    unsigned int      m_uTransitionCount;
    DEA_Transition**  m_pTransitionBuf;
    
};


#endif
