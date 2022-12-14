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
    m_bChangedSinceRepaint = true; // at beginning, all transitions has to be repainted
    m_bHovered = false;
    m_bSelected = false;
    m_pStart = NULL;
    m_pEnd = NULL;
    m_bJustExecuted = false;
    m_nCurve = 0;
    m_nDragRotationOffset = 0;
    m_fExecutionProgress = -1.0;
}


DEdit_GraphicalTransition::DEdit_GraphicalTransition(DEdit_GraphicalState* start,
                              DEdit_GraphicalState* end)
{
    m_bChangedSinceRepaint = true; // at beginning, all transitions has to be repainted
    m_bHovered = false;
    m_bSelected = false;
    m_bJustExecuted = false;
    m_pStart = start;
    m_pEnd = end;
    m_nCurve = 0;
    m_nDragRotationOffset = 0;
    m_fExecutionProgress = -1.0;
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
        return false;
    }
    QPoint imgPoint = pointToCheck;
    imgPoint -= m_pStart->positionToQPoint();
    imgPoint += QPoint(m_cAlphaMask.width()/2, m_cAlphaMask.height()/2);
    QRect rect = m_cAlphaMask.rect();
    //bool result = (distanceTo(pointToCheck) <= 0.1);
    bool result = rect.contains(imgPoint, true);
    result = result && (m_cAlphaMask.pixel(imgPoint) == qRgb(255, 255, 255));
    result = result && !m_pStart->isPointContained(pointToCheck);
    result = result && !m_pEnd->isPointContained(pointToCheck);
    return result;
}

bool DEdit_GraphicalTransition::isConnectedWith(DEdit_GraphicalState* state) const
{
    return (m_pStart == state) || (m_pEnd == state);
}

bool DEdit_GraphicalTransition::startEqualsEnd() const
{
    return m_pStart == m_pEnd;
}


QString DEdit_GraphicalTransition::graphicalLabel()
{
    if(!m_pData)
    {
        return "";
    }
    QString result = symbols();
    result.replace(" ", "0x20");
    bool lower = result.contains("abcdefghijklmnopqrstuvwxyz");
    bool upper = result.contains("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    bool digits = result.contains("0123456789");
    
    if(lower)
    {
        result.remove("abcdefghijklmnopqrstuvwxyz");
        // add space
        if(!result.isEmpty())
        {
            result += " ";
        }
        // add abbreviation
        result += "a-z";
    }
    if(upper)
    {
        result.remove("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        // add space
        if(!result.isEmpty())
        {
            result += " ";
        }
        // add abbreviation
        result += "A-Z";
    }
    if(digits)
    {
        result.remove("0123456789");
        // add space
        if(!result.isEmpty())
        {
            result += " ";
        }
        // add abbreviation
        result += "0-9";
    }
    
    return result;
}

void DEdit_GraphicalTransition::setSymbols(QString symbols)
{
    if(!m_pData)
    {
        return;
    }
//     m_pData->setInputSymbols(xmlEncoder::stringToXmlCode(symbols.toLocal8Bit().data()));
    m_pData->setInputSymbols(symbols.toLocal8Bit().data());
}

QString DEdit_GraphicalTransition::symbols()
{
    if(!m_pData)
    {
        return "";
    }
//     qDebug("data is \"%s\"", m_pData->inputSymbols());
//     return QString::fromLocal8Bit(xmlEncoder::xmlCodeToString(m_pData->inputSymbols()));
    return QString::fromLocal8Bit(m_pData->inputSymbols());
}


int DEdit_GraphicalTransition::curveByDragPosition(QPoint dragPos)
{
    if(!m_pStart || !m_pEnd)
    {
        return 0;
    }
    return curveByDragPosition(m_pStart->positionToQPoint(),
                               m_pEnd->positionToQPoint(),
                                       dragPos);
}

int DEdit_GraphicalTransition::curveByDragPosition(QPoint p1, QPoint p2, QPoint dragPos)
{
    if(QLineF(p1, p2).length() == 0)
    {
        // compute angle
        double dx = dragPos.x() - p1.x();
        double dy = dragPos.y() - p1.y();
        double angle;
        if(dx == 0)
        {
            if(dy == 0)
            {
                return 0;
            }
            if(dy < 0)
            {
                angle = M_PI_2; // 90 ??
            }
            else
            {
                angle = M_PI_2 * 3; // 270 ??
            }
        }
        else
        {
            angle = atan(-dy / dx);
            if(dx < 0)
            {
                angle += M_PI;
            }
        }
        int curve = angle / M_PI * 1800 - 450;
        return curve;
    }
    else
    {
        int dxP1P2 = p2.x() - p1.x();
        int dyP1P2 = p2.y() - p1.y();
        int dxP1Drag = dragPos.x() - p1.x();
        int dyP1Drag = dragPos.y() - p1.y();
        
        // ueber kreuz multiplizieren
        dyP1P2 *= dxP1Drag;
        dyP1Drag *= dxP1P2;
        
        QPoint middle = (p1 + p2)/2;
        double distance = QLineF(middle, dragPos).length();
        int sign = 1;
        if(dyP1P2 < dyP1Drag)
        {
            sign = -1;
        }
        double curve = sign * distance/QLineF(p1, p2).length()*1000.0*2.0;
        // range curve
        if(curve > 40000)
        {
            curve = 40000;
        }
        if(curve < -40000)
        {
            curve = -40000;
        }
        return (int)curve;
    }
}

void DEdit_GraphicalTransition::setWasChanged()
{
    m_bChangedSinceRepaint = true;
}

void DEdit_GraphicalTransition::setToNotChanged()
{
    m_bChangedSinceRepaint = false;
}

bool DEdit_GraphicalTransition::wasChangedSinceRepaint() const
{
    bool wasChanged = m_bChangedSinceRepaint;
    if(m_pStart)
    {
        wasChanged = (wasChanged || m_pStart->wasChangedSinceRepaint());
    }
    if(m_pEnd)
    {
        wasChanged = (wasChanged || m_pEnd->wasChangedSinceRepaint());
    }
    return wasChanged;
}

double DEdit_GraphicalTransition::labelRadius()
{
    // radius of big circle of transitions from a state to itself
    double radius = DEdit_GraphicalState::m_nDiameter/2 + 
            DEdit_GraphicalTransition::m_nWithItselfTransitionRadius;
    
    // 2.414213562373095049  = (sqrt(2) + 1)
    return radius * 2.4142135;
}


void DEdit_GraphicalTransition::setExecutionProgress(double progress)
{
    if(progress > 1.0)
    {
        progress = 1.0;
    }
    if(progress < 0.0)
    {
        progress = -1.0;
    }
    if(progress != m_fExecutionProgress)
    {
        m_fExecutionProgress = progress;
        setWasChanged();
    }
}
