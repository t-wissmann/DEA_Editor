#ifndef __DEDIT_GRAPHICALSTATE_H_ 
#define __DEDIT_GRAPHICALSTATE_H_

#include <QPoint>
class DEA_State;

class DEdit_GraphicalState
{
public:
    DEdit_GraphicalState(DEA_State* state = 0);
    ~DEdit_GraphicalState();
    DEA_State* m_pData;
    
    int m_nX;
    int m_nY;
    // attributs
    bool isHovered;
    bool isDragged;
    bool isSelected;
    // only needed for drag
    int m_nDragOffsetX;
    int m_nDragOffsetY;
    // functions
    void move(int x, int y);
    bool isPointContained(QPoint pointToCheck);
    QPoint positionToQPoint() const;
    // static attributes
    static int m_nDiameter;
};

#endif
