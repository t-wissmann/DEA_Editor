#ifndef __DEDIT_GRAPHICALSTATE_H_ 
#define __DEDIT_GRAPHICALSTATE_H_

#include <QPoint>
class DEA_State;
class DEdit_WidgetPainter;

class DEdit_GraphicalState
{
public:
    DEdit_GraphicalState(DEA_State* state = 0);
    ~DEdit_GraphicalState();
    DEA_State* m_pData;
    
    int m_nX;
    int m_nY;
    bool m_bStartState;
    // attributes
    bool isHovered;
    bool isDragged;
    bool isSelected;
    bool m_bCurrentlyExecutedState; // current state in execution of dea
    // only needed for drag
    int m_nDragOffsetX;
    int m_nDragOffsetY;
    // needed for isPointContained
    DEdit_WidgetPainter* m_pWidgetPainter;
    
    enum EResultIndicator {
        NoResult,
        ResultAccepted,
        ResultDenied
    };
    EResultIndicator m_eResultIndicator;
    
    // functions
    void move(int x, int y);
    bool isPointContained(QPoint pointToCheck);
    QPoint positionToQPoint() const;
    // static attributes
    static int m_nDiameter;
};

#endif
