#include "dedit_graphicaltransition.h"
#include "dedit_graphicalstate.h"
#include <DEA/dea_transition.h>

#include <stdlib.h>
#include <math.h>
// qt
#include <QLineF>
#include <QRect>

int DEdit_GraphicalTransition::m_nLineWidth = 5;
int DEdit_GraphicalTransition::m_nWithItselfTransitionRadius = 20;

DEdit_GraphicalTransition::DEdit_GraphicalTransition()
{
    m_bHovered = FALSE;
    m_bSelected = FALSE;
    m_pStart = NULL;
    m_pEnd = NULL;
}


DEdit_GraphicalTransition::DEdit_GraphicalTransition(DEdit_GraphicalState* start,
                              DEdit_GraphicalState* end)
{
    m_bHovered = FALSE;
    m_bSelected = FALSE;
    m_pStart = start;
    m_pEnd = end;
}

DEdit_GraphicalTransition::~DEdit_GraphicalTransition()
{
    
}



bool DEdit_GraphicalTransition::hasValidPointers() const
{
    bool result = m_pStart && m_pEnd
            && m_pStart->m_pData && m_pEnd->m_pData && m_pData;
    return result;
}


double DEdit_GraphicalTransition::distanceTo(QPoint point) const
{
    // result
    double distance = -1;
    // distances in Triangular/Dreieck Start - End - point
    double distStartToPoint = QLineF(m_pStart->positionToQPoint(), point).length();
    double distEndToPoint = QLineF(m_pEnd->positionToQPoint(), point).length();
    double distStartToEnd = QLineF(m_pEnd->positionToQPoint(),
                                   m_pStart->positionToQPoint()).length();
    distance = distStartToEnd - distStartToPoint- distEndToPoint;
    return fabs(distance);
}


bool DEdit_GraphicalTransition::isPointContained(QPoint pointToCheck)
{
    if(!m_pStart || !m_pEnd)
    {
        return FALSE;
    }
    QPoint imgPoint = pointToCheck;
    imgPoint -= m_pStart->positionToQPoint();
    imgPoint += QPoint(m_cAlphaMask.width()/2, m_cAlphaMask.height()/2);
    QRect rect = m_cAlphaMask.rect();
    //bool result = (distanceTo(pointToCheck) <= 0.1);
    bool result = rect.contains(imgPoint, TRUE);
    result = result && (m_cAlphaMask.pixel(imgPoint) == qRgb(255, 255, 255));
    result = result && !m_pStart->isPointContained(pointToCheck);
    result = result && !m_pEnd->isPointContained(pointToCheck);
    return result;
}

bool DEdit_GraphicalTransition::isConnectedWith(DEdit_GraphicalState* state) const
{
    return (m_pStart == state) || (m_pEnd == state);
}


void DEdit_GraphicalTransition::applySymbolsToDEA_Transitions()
{
    if(!m_pData)
    {
        return;
    }
    m_pData->setInputSymbols(m_szSymbols.toAscii().data());
}

