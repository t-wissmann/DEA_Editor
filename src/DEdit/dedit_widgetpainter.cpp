
#include "dedit_widgetpainter.h"
#include "dedit_widget.h"
#include "dedit_graphicalstate.h"
#include "dedit_graphicaltransition.h"
#include <io/debug_msg.h>
#include <math.h>

// painter
#include <QPainter>
#include <QColor>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPen>
#include <QPainterPath>

DEdit_WidgetPainter::DEdit_WidgetPainter(DEdit_Widget* widget)
{
    m_pWidget = widget;
    recreateAllTemplates();
    
    
}

DEdit_WidgetPainter::~DEdit_WidgetPainter()
{
    
}


void DEdit_WidgetPainter::paint()
{
    if(!m_pWidget)
    {
        DEBUG_MSG("paint()", "m_pWidget = 0");
        return;
    }
    QPixmap pixmap(m_pWidget->rect().width(), m_pWidget->rect().height());
    pixmap.fill(QColor(0, 0, 0, 0));
    // paint to image
    {
        QPainter imagePainter(&pixmap);
        imagePainter.setRenderHint(QPainter::Antialiasing);
        
        // paint transitions
        QList<DEdit_GraphicalTransition>* transitionList = &m_pWidget->m_TransitionList;
        //imagePainter.setPen(m_cTransitionPen);
        for(int i = 0; i < transitionList->size(); ++i)
        {
            if(&(*transitionList)[i] == m_pWidget->m_pHoveredTransition)
            {
                // skip hovered transition
                // it will be painted at last
                continue;
            }
            paintTransition(&imagePainter, &(*transitionList)[i]);
        }
        // paint selected transition
        //imagePainter.setPen(m_cTransitionPenHovered);
        paintTransition(&imagePainter, m_pWidget->m_pHoveredTransition);
        
        // paint states
        QList<DEdit_GraphicalState>* stateList = &m_pWidget->m_StateList;
        // init pen
        imagePainter.setPen(Qt::NoPen);
        for(int i = 0; i < stateList->size(); ++i)
        {
            paintState(&imagePainter, &(*stateList)[i]);
        }
        
        if(m_pWidget->m_eMode == DEdit_Widget::ModeAddTransitionSelectTo)
        {
            // paint line of new transition
            int stateRadius = DEdit_GraphicalState::m_nDiameter/2;
            imagePainter.setPen(m_cTransitionPen);
            
            QLineF newLine (m_pWidget->m_cNewTransitionLine);
            
            // if another item is hovered
            // then shorten arrow by stateRadius
            if(m_pWidget->m_pHoveredState &&
               (m_pWidget->m_pAddTransitionStateTo != m_pWidget->m_pHoveredState)) 
            {
                newLine.setLength(newLine.length() - stateRadius);
            }
            imagePainter.setPen(m_cTransitionPen);
            paintTransition(&imagePainter, newLine);
        }
        
        imagePainter.end();
    }
    // paint to widget
    {
        QPainter widgetpainter(m_pWidget);
        widgetpainter.drawPixmap(m_pWidget->rect(), pixmap, m_pWidget->rect());
        widgetpainter.end();
    }
}

void DEdit_WidgetPainter::paintState(QPainter* painter, DEdit_GraphicalState* state)
{
    if(!painter  || !state){
        return;
    }
    int width = m_cStateNormalTemplate.width();
    QRectF source(0, 0, width, width);
    QRectF target(state->m_nX-width/2, state->m_nY-width/2, width, width);
    
    if(state->isDragged)
    {
        painter->drawPixmap(target, m_cStateDraggedTemplate, source);
    }
    else if(state->isSelected)
    {
        painter->drawPixmap(target, m_cStateSelectedTemplate, source);
    }
    else
    {
        painter->drawPixmap(target, m_cStateNormalTemplate, source);
    }
    
    if(state->isHovered)
    {
        painter->drawPixmap(target, m_cStateHoveredTemplate, source);
    }
}


void DEdit_WidgetPainter::paintTransition
        (QPainter* painter, DEdit_GraphicalTransition* transition)
{
    if(!painter || !transition)
    {
        return;
    }
    if(!transition->hasValidPointers())
    {
        return;
    }
    
    // select pen according to the current attributes of transition
    if(transition->m_bSelected)
    {
        painter->setPen(m_cTransitionPenSelected);
    }
    else if(transition->m_bHovered)
    {
        painter->setPen(m_cTransitionPenHovered);
    }
    else
    {
        painter->setPen(m_cTransitionPen);
    }
    
    int lineWidth = DEdit_GraphicalTransition::m_nLineWidth;
    // init line
    QLineF line(transition->m_pStart->positionToQPoint(),
                transition->m_pEnd  ->positionToQPoint());
    // shorten transition if it connects to different states
    if(line.length() != 0) 
    {
        line.setLength(line.length() - DEdit_GraphicalState::m_nDiameter/2);
    }
    // paint to painter
    paintTransition(painter, line);
    
    // paint to alpha mask
    int alphaMaskWidth = abs(line.dx())*2+lineWidth*3;
    int alphaMaskHeight = abs(line.dy())*2+lineWidth*3;
    if(line.length() == 0)
    {
        alphaMaskWidth = DEdit_GraphicalState::m_nDiameter + DEdit_GraphicalTransition::m_nWithItselfTransitionRadius * 2 +
                DEdit_GraphicalTransition::m_nLineWidth;
        alphaMaskWidth *= 2;
        alphaMaskHeight = alphaMaskWidth;
    }
    QImage alphaMask(alphaMaskWidth, alphaMaskHeight,
                     QImage::Format_Mono);
    QPainter alphaPainter(&alphaMask);
    QPen alphaPen(m_cTransitionPen);
    alphaPen.setColor(QColor(255, 255, 255));
    alphaPainter.setPen(alphaPen);
    alphaPainter.fillRect(QRect(0, 0, alphaMask.width(), alphaMask.height()),
                         QBrush(QColor(0, 0, 0)));
    int offsetX = alphaMask.width()/2;
    int offsetY = alphaMask.height()/2;
    
    line = QLineF(offsetX, offsetY, offsetX+line.dx(),
                  offsetY+line.dy());
    
    paintTransition(&alphaPainter, line);
    alphaPainter.end();
    {
        /// very useful to understand code above
        //painter->drawImage(33, 33, alphaMask);
    }
    transition->m_cAlphaMask = alphaMask;
    
}

void DEdit_WidgetPainter::paintTransition(QPainter* painter, QLineF line)
{
    int penwidth = m_cTransitionPen.width();
    QPointF arrowPos = line.p2(); // arrowposition
    qreal arrowAngle = -90;
    painter->drawEllipse(line.x1()-penwidth, line.y1()-penwidth,
                    penwidth*2, penwidth*2);
    if(line.length() != 0)
    {
        // simply paint line,
        // if length != 0
        painter->drawLine(line);
        arrowAngle =  180 - line.angle(QLineF(0, 0, 100, 0)); // angle to x axis
        if(line.dy() > 0)
        {
            // add sign to angle if angle should be negative
            arrowAngle *= -1;
        }
    }
    else
    {// else a state is connected with itself
        int radius = DEdit_GraphicalState::m_nDiameter/2 + 
                DEdit_GraphicalTransition::m_nWithItselfTransitionRadius;
        QRect rect(line.x1(), line.y1(), radius*2, -radius*2);
        // from state center to -90° of big circle
        painter->drawLine(line.x1(), line.y1(), line.x1()+radius, line.y1());
        // draw big dircle from -90° to +180°
        painter->drawArc(rect, -90*16, 270*16);
        // from +180° to top of state
        painter->drawLine(line.x1(), line.y1()-radius,
                          line.x1(), line.y1()-DEdit_GraphicalState::m_nDiameter/2);
        arrowPos = QPoint(line.x1(), line.y1()-DEdit_GraphicalState::m_nDiameter/2);
    }
    //painter->drawText(100, 100, QString("Arrow angle: ") + QString::number(arrowAngle));
    // compute from degrees to rad
    arrowAngle *= M_PI / 180;
    
    // draw arrow
    double arrowlength = 15;
    double arrowwidth = M_PI/8; // in rad
    int x, y;
    x = arrowlength*cos(arrowAngle+arrowwidth);
    y = arrowlength*sin(arrowAngle+arrowwidth);
    painter->drawLine(arrowPos.x(), arrowPos.y(),
                      arrowPos.x()+x, arrowPos.y()+y);
    
    x = arrowlength*cos(arrowAngle-arrowwidth);
    y = arrowlength*sin(arrowAngle-arrowwidth);
    
    painter->drawLine(arrowPos.x(), arrowPos.y(),
                      arrowPos.x()+x, arrowPos.y()+y);
    
}


void DEdit_WidgetPainter::recreateAllTemplates()
{
    recreateStateNormalTemplate();
    recreateStateHoveredTemplate();
    recreateStateDraggedTemplate();
    recreateStateSelectedTemplate();
    recreateTransitionPens();
}

void DEdit_WidgetPainter::recreateStateNormalTemplate()
{
    m_cStateNormalTemplate = recreateStateTemplate(
            QColor("#3465A4"), DEdit_GraphicalState::m_nDiameter);
}
void DEdit_WidgetPainter::recreateStateHoveredTemplate()
{
    m_cStateHoveredTemplate = recreateStateTemplate(
            QColor(255,255,255,33), DEdit_GraphicalState::m_nDiameter);
}

void DEdit_WidgetPainter::recreateStateDraggedTemplate()
{
    m_cStateDraggedTemplate = recreateStateTemplate(
            QColor("#75507B"), DEdit_GraphicalState::m_nDiameter, TRUE);
}

void DEdit_WidgetPainter::recreateStateSelectedTemplate(){
    m_cStateSelectedTemplate = recreateStateTemplate(
            QColor("#75507B"), DEdit_GraphicalState::m_nDiameter, TRUE);
}


void DEdit_WidgetPainter::recreateTransitionPens()
{
    
    m_cTransitionPen.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPen.setCapStyle(Qt::RoundCap);
    m_cTransitionPen.setColor(QColor(0, 0, 0));
    m_cTransitionPenHovered.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPenHovered.setCapStyle(Qt::RoundCap);
    m_cTransitionPenHovered.setColor(QColor(66, 90, 130));
    m_cTransitionPenSelected.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPenSelected.setCapStyle(Qt::RoundCap);
    m_cTransitionPenSelected.setColor(QColor(111, 67, 117));
}

QPixmap DEdit_WidgetPainter::recreateStateTemplate(QColor color, int diameter, bool invertedGradient)
{
    int pix_width = diameter * 1.1; // *1.1 : need space for shadow
    int circle_radius = diameter / 2;
    bool shadows = color.alpha() > 125; // for low alpha: shadow is ugly and must be disabled ;D
    // init pixmap
    QPixmap resultPixmap(pix_width, pix_width);
    resultPixmap.fill(QColor(0, 0, 0, 0)); // clear completely
    
    // init painter
    QPainter pixPainter(&resultPixmap);
    pixPainter.setRenderHint(QPainter::Antialiasing, TRUE);
    // init colors
    QColor innerborder = color.lighter(170);
    QColor outerborder = color.darker(170);
    // start paint
    int half_pix_width = pix_width/2; // half of the width
    drawTangoCircle(&pixPainter, half_pix_width, half_pix_width, circle_radius,
                     color, innerborder, outerborder, shadows, invertedGradient);
    return resultPixmap;
}


void DEdit_WidgetPainter::drawTangoCircle(QPainter* painter, int centerx, int centery, int radius,
    QColor background, QColor innerborder, QColor outerborder, bool shadow, bool invertedGradient)
{
    if(!painter){
        return;
    }
    
    int innerBorderWidth = 4;
    if(innerborder.alpha() == 0)
    {
        innerBorderWidth = 0;
    }
    // reduce radius by border width
    // so that circle including borders has diameter 2*radius
    radius -= innerBorderWidth;
    
    
    // init gradients etc..
    QLinearGradient bgGradient(centerx-radius, centery-radius, centerx-radius, centery+radius);
    if(invertedGradient)
    {
        bgGradient.setColorAt(0, background.darker(140));
        bgGradient.setColorAt(1, background);
    }
    else
    {
        bgGradient.setColorAt(0, background);
        bgGradient.setColorAt(1, background.darker(140));
    }
    QBrush bgbrush(bgGradient);
    QPen   borderpen;
    borderpen.setWidthF(4);
    borderpen.setColor(outerborder);
    
    QRect circle(centerx - radius, centery-radius, radius*2, radius*2);
    
    //draw shadow
    if(shadow)
    {
        QRect shadowrect(circle);
        int widthdelta = (int)(shadowrect.width() * 0.05);
        shadowrect.adjust(widthdelta/(-2), widthdelta/(-2), widthdelta/(2), widthdelta/(2));
        shadowrect.setWidth(shadowrect.width() + widthdelta);
        shadowrect.setHeight(shadowrect.height() + widthdelta);
        
        
        QRadialGradient shadowGradient(QPointF(shadowrect.center()), (shadowrect.width()/2));
        shadowGradient.setColorAt(0,   QColor(0, 0, 0, 200));
        shadowGradient.setColorAt(0.9, QColor(0, 0, 0, 150));
        shadowGradient.setColorAt(1,   QColor(0, 0, 0, 0));
        
        QBrush shadowbrush(shadowGradient);
        painter->setPen(Qt::NoPen);
        painter->setBrush(shadowbrush);
        painter->drawEllipse(shadowrect);
    }
    
    //draw circle
    painter->setPen(borderpen);
    if(innerBorderWidth)
    {
        painter->setBrush(QBrush(innerborder));
    }
    else
    {
        painter->setBrush(bgbrush);
    }
    
    painter->drawEllipse(circle);
    
    if(innerBorderWidth)
    {
        circle.adjust(innerBorderWidth, innerBorderWidth, -innerBorderWidth, -innerBorderWidth);
        painter->setPen(Qt::NoPen);
        painter->setBrush(bgbrush);
        painter->drawEllipse(circle);
    }
}


