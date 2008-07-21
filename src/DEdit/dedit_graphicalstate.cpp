#include "dedit_graphicalstate.h"
#include <DEA/dea_state.h>
#include "dedit_widgetpainter.h"
#include <QLineF>

int DEdit_GraphicalState::m_nDiameter = 100;

DEdit_GraphicalState::DEdit_GraphicalState(DEA_State* state)
{
    m_pData = state;
    m_pWidgetPainter = NULL;
    m_nX = 0;
    m_nY = 0;
    
    m_bStartState = 0;
    m_bCurrentlyExecutedState = 0;
    m_eResultIndicator = NoResult;
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
    if(m_bStartState && m_pWidgetPainter)
    {
        QPoint pixelpos = pointToCheck
                - (positionToQPoint() + m_pWidgetPainter->m_cStartStateIndicatorPosition);
        
        if(m_pWidgetPainter->m_cStartStateIndicatorAlphaMask.rect().contains(pixelpos))
        {// only check, if position is WITHIN m_cStartStateIndicatorAlphaMask
            bool overStartState = (qRgb(255, 255, 255)
                    == m_pWidgetPainter->m_cStartStateIndicatorAlphaMask.pixel(pixelpos));
             result = result || overStartState;
        }
    }
    
    return result;
}

QPoint DEdit_GraphicalState::positionToQPoint() const
{
    return QPoint(m_nX, m_nY);
}

