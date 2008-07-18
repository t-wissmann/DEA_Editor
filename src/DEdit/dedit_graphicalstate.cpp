#include "dedit_graphicalstate.h"
#include <DEA/dea_state.h>

#include <QLineF>

int DEdit_GraphicalState::m_nDiameter = 100;

DEdit_GraphicalState::DEdit_GraphicalState(DEA_State* state)
{
    m_pData = state;
    m_nX = 0;
    m_nY = 0;
    
    m_bStartState = 0;
    
    isHovered = 0;
    isDragged = 0;
    isSelected = 0;
    
    m_nDragOffsetX = 0;
    m_nDragOffsetY = 0;
}

DEdit_GraphicalState::~DEdit_GraphicalState()
{
    
    
}


void DEdit_GraphicalState::move(int x, int y)
{
    m_nX = x;
    m_nY = y;
}



bool DEdit_GraphicalState::isPointContained(QPoint pointToCheck)
{
    bool result;
    QLineF distance(m_nX, m_nY, pointToCheck.x(), pointToCheck.y());
    result = distance.length() <= (((double)m_nDiameter)/2);
    
    return result;
}

QPoint DEdit_GraphicalState::positionToQPoint() const
{
    return QPoint(m_nX, m_nY);
}

