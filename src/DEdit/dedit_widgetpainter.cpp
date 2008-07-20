
#include "dedit_widgetpainter.h"
#include "dedit_widget.h"
#include "dedit_graphicalstate.h"
#include "dedit_graphicaltransition.h"
#include <DEA/dea_state.h>
#include <io/debug_msg.h>
#include <math.h>

// painter
#include <QPainter>
#include <QColor>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPen>
#include <QPainterPath>
#include <QFontMetrics>

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
            recomputeTransitionLabelArea(&(*transitionList)[i]);
            paintTransition(&imagePainter, &(*transitionList)[i]);
        }
        // paint selected transition
        //imagePainter.setPen(m_cTransitionPenHovered);
        recomputeTransitionLabelArea(m_pWidget->m_pHoveredTransition);
        paintTransition(&imagePainter, m_pWidget->m_pHoveredTransition);
        
        // paint states
        QList<DEdit_GraphicalState>* stateList = &m_pWidget->m_StateList;
        // init pen
        imagePainter.setPen(Qt::NoPen);
        imagePainter.setBrush(Qt::NoBrush);
        imagePainter.setFont(m_cStateLabelFont);
        for(int i = 0; i < stateList->size(); ++i)
        {
            paintState(&imagePainter, &(*stateList)[i]);
        }
        
        if(m_pWidget->m_eMode == DEdit_Widget::ModeAddTransitionSelectTo)
        {
            // paint line of new transition
            int stateRadius = DEdit_GraphicalState::m_nDiameter/2;
            imagePainter.setPen(m_cTransitionPen);
            imagePainter.setBrush(m_cTransitionPen.brush());
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
        if(m_pWidget->m_bAboutToDrop) // if there is an state, that will be dropped so
        {
            int width = m_cStateDraggedTemplate.width();
            QRectF source(0, 0, width, width);
            QRectF target(m_pWidget->m_cDropPreviewPosition.x()-width/2,
                        m_pWidget->m_cDropPreviewPosition.y()-width/2, width, width);
            imagePainter.drawPixmap(target, m_cStateNormalTemplate, source);
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
    
    painter->setPen(Qt::NoPen);
    
    int width = m_cStateNormalTemplate.width();
    QRectF source(0, 0, width, width);
    QRectF target(state->m_nX-width/2, state->m_nY-width/2, width, width);
    
    
    if(state->m_bCurrentlyExecutedState)
    {
        painter->drawPixmap(target, m_cStateCurrentlyExecutedTemplate, source);
    }
    else if(state->isDragged)
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
    
    
    // draw label
    painter->setPen(m_cStateLabelPen);
    paintStateLabel(painter, state);
}


void DEdit_WidgetPainter::paintStateLabel(QPainter* painter, DEdit_GraphicalState* state)
{
    if(!painter  || !state  || !state->m_pData){
        return;
    }
    int margin = DEdit_GraphicalState::m_nDiameter*0.3/2;
    int radius = DEdit_GraphicalState::m_nDiameter/2 - margin;
    QRect labelArea(state->m_nX-radius, state->m_nY-radius,
                    radius*2, radius*2);
    QString label = state->m_pData->name();
    int labelFlags = Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWrapAnywhere;
    painter->drawText(labelArea, labelFlags, label);
    
    
    
    if(state->m_pData->isFinalState())
    {
        QRect finalStateIndicator(state->m_nX-radius, state->m_nY-radius,
                                radius*2, radius*2);
        // now illustrate attributes
        painter->drawEllipse(finalStateIndicator);
    }
    if(state->m_bStartState)
    {
        QRect source(0, 0,
                     m_cStartStateIndicator.width(),
                     m_cStartStateIndicator.height());
        QRect target(source);
        target.moveTo(state->positionToQPoint()+m_cStartStateIndicatorPosition);
        painter->drawPixmap(target, m_cStartStateIndicator, source);
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
    if(transition->m_bJustExecuted)
    {
        painter->setPen(m_cTransitionPenJustExecuted);
    }
    else if(transition->m_bSelected)
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
    // paint line and label background to painter
    paintTransition(painter, line);
    QRect lblArea = transition->m_cLabelArea;
    if(lblArea.isValid())
    {
        painter->setBrush(painter->pen().brush());
        painter->drawRect(lblArea);
    }
    
    // paint label
    painter->setPen(m_cTransitionLabelPen);
    painter->setFont(m_cTransitionLabelFont);
    paintTransitionLabel(painter, transition);
    
    
    // paint to alpha mask
    int alphaMaskWidth = abs(line.dx())*2+lineWidth*3;
    int alphaMaskHeight = abs(line.dy())*2+lineWidth*3;
    // ensure, that label is within alpha mask
    if(alphaMaskWidth < transition->m_cLabelArea.width())
    {
        alphaMaskWidth = transition->m_cLabelArea.width();
    }
    if(alphaMaskHeight < transition->m_cLabelArea.height())
    {
        alphaMaskHeight = transition->m_cLabelArea.height();
    }
    
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
    
    
    // paint line and label background to alpha
    int dx = offsetX - line.x1();
    int dy = offsetY -line.y1();
    alphaPainter.drawRect(transition->m_cLabelArea.adjusted(dx, dy, dx, dy));
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


void DEdit_WidgetPainter::paintTransitionLabel
        (QPainter* painter, DEdit_GraphicalTransition* transition)
{
    if(!painter || !transition || !transition->hasValidPointers())
    {
        return;
    }
    if(transition->graphicalLabel().isEmpty())
    {
        // nothing to do if label would be empty
        return;
    }
    QString label = transition->graphicalLabel();
    int flags = Qt::AlignVCenter | Qt::AlignHCenter;
    painter->drawText(transition->m_cLabelArea, flags, label);
}


void DEdit_WidgetPainter::recomputeTransitionLabelArea
        (DEdit_GraphicalTransition* transition)
{
    if(!transition)
    {
        return;
    }
    QRect result;
    if(transition->graphicalLabel().isEmpty())
    {
        result.setRect(0, 0, 0, 0);
    }else
    {
        int margin = m_cTransitionPen.width()/2;
        QFontMetrics fm(m_cTransitionLabelFont);
        result.setWidth(fm.width(transition->graphicalLabel()) + 2*margin);
        result.setHeight(fm.height() + margin);
        QPoint lblPos = (transition->m_pStart->positionToQPoint()*0.55 + 
                transition->m_pEnd->positionToQPoint()*0.45);
        if(transition->m_pStart->positionToQPoint()
           != transition->m_pEnd->positionToQPoint())
        {
            result.moveCenter(lblPos);
        }
        else
        {
            int radius = DEdit_GraphicalState::m_nDiameter/2 + 
                    DEdit_GraphicalTransition::m_nWithItselfTransitionRadius;
            
            /*
            calculation of HALF_SQRT_OF_2plus1
            HALF_SQRT_OF_2plus1 = 1 + sqrt(2) / 2;
            1 : move to center of big circle
            sqrt(2)/2 : move to line of circle ( sqrt(2)/2 = sin(45°))
            */
#define HALF_SQRT_OF_2plus1 1.70710
            int delta = (HALF_SQRT_OF_2plus1 * radius);
            result.moveCenter(lblPos + QPoint(delta, -delta));
#undef HALF_SQRT_OF_2plus1 // undef, because only needed for int delta
        }
    }
    transition->m_cLabelArea = result;
}

void DEdit_WidgetPainter::paintTransition(QPainter* painter, QLineF line,
                                          bool bigDotAtStart)
{
    int penwidth = m_cTransitionPen.width();
    QPointF arrowPos = line.p2(); // arrowposition
    qreal arrowAngle = -90;
    if(bigDotAtStart)
    {
        painter->drawEllipse(line.x1()-penwidth, line.y1()-penwidth,
                    penwidth*2, penwidth*2);
    }
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
    // states
    recreateStatePens();
    recreateStateNormalTemplate();
    recreateStateHoveredTemplate();
    recreateStateDraggedTemplate();
    recreateStateSelectedTemplate();
    recreateStateCurrentlyExecutedTemplate();
    
    // transitions
    recreateTransitionPens();
    // start state indicator after recreateTransitionPens
    // because recreateStartStateIndicator() needs some transition pens
    recreateStartStateIndicator();
}

void DEdit_WidgetPainter::recreateStatePens()
{
    m_cStateLabelPen.setWidth(3);
    m_cStateLabelPen.setColor(QColor(238, 238, 238));
    m_cStateLabelFont.setBold(TRUE);
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

void DEdit_WidgetPainter::recreateStateCurrentlyExecutedTemplate()
{
    m_cStateCurrentlyExecutedTemplate = recreateStateTemplate(
            QColor("#73D216"), DEdit_GraphicalState::m_nDiameter, TRUE);
}
void DEdit_WidgetPainter::recreateStartStateIndicator()
{
    int lineWidth = DEdit_GraphicalTransition::m_nLineWidth;
    // init font
    QFont font = m_cTransitionLabelFont;
    QString text = QObject::tr("Start");
    QFontMetrics fm(font);
    int textmarginX = m_cTransitionPen.width();
    int textmarginY = m_cTransitionPen.width()/2;
    int arrowspace = 14; // in px
    int textwidth = fm.width(text) + textmarginX;
    int textheight = fm.height() + textmarginY;
    int width = textwidth + lineWidth*4 + arrowspace;
    int height = textheight + lineWidth*4;
    QRect labelBackground(lineWidth, lineWidth*2, textwidth, textheight);
    // init pixmap
    m_cStartStateIndicator = QPixmap(width, height);
    m_cStartStateIndicator.fill(QColor(0, 0, 0, 0));
    m_cStartStateIndicatorPosition = QPoint
            (-width-DEdit_GraphicalState::m_nDiameter/2+lineWidth, -height/2);
    
    QPainter painter(&m_cStartStateIndicator);
    painter.setFont(font);
    painter.setPen(m_cTransitionPen);
    painter.setBrush(m_cTransitionPen.brush());
    painter.setRenderHint(QPainter::Antialiasing);
    QLine line(lineWidth*2, height/2,
               width-lineWidth, height/2);
    int arrowheight = m_cTransitionPen.width();
   // draw label background
    painter.drawRect(labelBackground); 
    
    // draw arrow
    painter.drawLine(line);
    QLine arrowLine(line.p2(), line.p2()-QPoint(arrowheight*2,arrowheight));
    painter.drawLine(arrowLine);
    arrowLine = QLine(arrowLine.p1(), arrowLine.p2()+QPoint(0, arrowheight*2));
    painter.drawLine(arrowLine);
    
    // draw text
    painter.setBrush(Qt::NoBrush);
    painter.setPen(m_cTransitionLabelPen);
    int flags = Qt::AlignHCenter | Qt::AlignVCenter;
    painter.drawText(labelBackground, flags, text);
    
    // draw alpha mask
    {
        m_cStartStateIndicatorAlphaMask = QImage(width, height, QImage::Format_Mono);
        QPainter alphaPainter(&m_cStartStateIndicatorAlphaMask);
        QPen alphaPen(m_cTransitionPen);
        alphaPen.setColor(QColor(255, 255, 255));
        alphaPainter.setPen(alphaPen);
        alphaPainter.fillRect(m_cStartStateIndicatorAlphaMask.rect(),
                              QBrush(QColor(0, 0, 0)));
        alphaPainter.drawLine(line);
        alphaPainter.drawLine(arrowLine);
        arrowLine = QLine(arrowLine.p1(), arrowLine.p2()-QPoint(0, arrowheight*2));
        alphaPainter.drawLine(arrowLine);
        alphaPainter.drawRect(labelBackground);
        alphaPainter.end();
        /// only for debug
        //m_cStartStateIndicatorAlphaMask.save("/home/thorsten/alphamask.png");
    }
    painter.end();
}


void DEdit_WidgetPainter::recreateTransitionPens()
{
    // black
    m_cTransitionPen.setCapStyle(Qt::RoundCap);
    m_cTransitionPen.setJoinStyle(Qt::RoundJoin);
    m_cTransitionPen.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPen.setCapStyle(Qt::RoundCap);
    m_cTransitionPen.setColor(QColor(0, 0, 0));
    // light red
    m_cTransitionPenHovered.setCapStyle(Qt::RoundCap);
    m_cTransitionPenHovered.setJoinStyle(Qt::RoundJoin);
    m_cTransitionPenHovered.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPenHovered.setCapStyle(Qt::RoundCap);
    m_cTransitionPenHovered.setColor(QColor(204, 0, 0));
    // dark red
    m_cTransitionPenSelected.setCapStyle(Qt::RoundCap);
    m_cTransitionPenSelected.setJoinStyle(Qt::RoundJoin);
    m_cTransitionPenSelected.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPenSelected.setCapStyle(Qt::RoundCap);
    m_cTransitionPenSelected.setColor(QColor(239, 41, 41));
    // green
    m_cTransitionPenJustExecuted.setCapStyle(Qt::RoundCap);
    m_cTransitionPenJustExecuted.setJoinStyle(Qt::RoundJoin);
    m_cTransitionPenJustExecuted.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPenJustExecuted.setCapStyle(Qt::RoundCap);
    m_cTransitionPenJustExecuted.setColor(QColor(78, 154, 6));
    
    // for label
    m_cTransitionLabelPen.setColor(QColor(238, 238, 238));
    m_cTransitionLabelFont.setBold(TRUE);
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


