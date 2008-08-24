
#include "dedit_widgetpainter.h"
#include "dedit_widget.h"
#include "dedit_graphicalstate.h"
#include "dedit_graphicaltransition.h"
#include <DEA/dea_state.h>
#include <io/debug_msg.h>
#include <io/iconcatcher.h>
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
    // recreate default color theme
    DEdit_Appearance::createTangoDefault(&m_cAppearance);
    // recreate templates
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
    /*
    QPixmap pixmap(m_pWidget->rect().width(), m_pWidget->rect().height());
    pixmap.fill(QColor(0, 0, 0, 0));*/
    // paint to image
    {
        //QPainter imagePainter(&pixmap);
        QPainter imagePainter(m_pWidget);
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
            imagePainter.setBrush(Qt::NoBrush);
            QLineF newLine (m_pWidget->m_cNewTransitionLine);
            
            // if another item is hovered
            // then shorten arrow by stateRadius
            if(m_pWidget->m_pHoveredState &&
               (m_pWidget->m_pAddTransitionStateTo != m_pWidget->m_pHoveredState)) 
            {
                newLine.setLength(newLine.length() - stateRadius);
            }
            imagePainter.setPen(m_cTransitionPen);
            paintTransition(&imagePainter, newLine, 0, -1.0, TRUE);
        }
        if(m_pWidget->m_bAboutToDrop) // if there is an state, that will be dropped so
        {
            int width = m_cStateDraggedTemplate.width();
            QRectF source(0, 0, width, width);
            QRectF target(m_pWidget->m_cDropPreviewPosition.x()-width/2,
                        m_pWidget->m_cDropPreviewPosition.y()-width/2, width, width);
            imagePainter.drawPixmap(target, m_cStateNormalTemplate, source);
        }
        
        // draw icon if is locked
        if(m_pWidget->isLocked())
        {
            QRect source = m_cWidgetLockedPixmap.rect();
            QRect target = source;
            target.moveTo(10,10);
            imagePainter.drawPixmap(target, m_cWidgetLockedPixmap, source);
        }
        
        imagePainter.end();
    }
    
    /*
    // paint to widget
    {
        QPainter widgetpainter(m_pWidget);
        widgetpainter.drawPixmap(m_pWidget->rect(), pixmap, m_pWidget->rect());
        widgetpainter.end();
    }
    */
    setAllItemsToNotChanged();
}


void DEdit_WidgetPainter::setAllItemsToNotChanged()
{
    if(!m_pWidget)
    {
        DEBUG_MSG("setAllItemsToNotChanged()", "m_pWidget = 0");
        return;
    }
    // all states
    for(int i = 0; i < m_pWidget->m_StateList.size(); ++i)
    {
        m_pWidget->m_StateList[i].setToNotChanged();
    }
    // all transitions
    for(int i = 0; i < m_pWidget->m_TransitionList.size(); ++i)
    {
        m_pWidget->m_TransitionList[i].setToNotChanged();
    }
}


void DEdit_WidgetPainter::setAllItemsToWasChanged()
{
    if(!m_pWidget)
    {
        DEBUG_MSG("setAllItemsToWasChanged()", "m_pWidget = 0");
        return;
    }
    // all states
    for(int i = 0; i < m_pWidget->m_StateList.size(); ++i)
    {
        m_pWidget->m_StateList[i].setWasChanged();
    }
    // all transitions
    for(int i = 0; i < m_pWidget->m_TransitionList.size(); ++i)
    {
        m_pWidget->m_TransitionList[i].setWasChanged();
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
    
    
    
    
    if(state->m_eResultIndicator == DEdit_GraphicalState::ResultDenied)
    {
        painter->drawPixmap(target, m_cStateResultDeniedTemplate, source);
    }
    else if(state->m_eResultIndicator == DEdit_GraphicalState::ResultAccepted)
    {
        painter->drawPixmap(target, m_cStateResultAcceptedTemplate, source);
    }
    else if(state->m_bCurrentlyExecutedState)
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



QPoint DEdit_WidgetPainter::middlePointOfCurve(QPoint p1, QPoint p2, int curve)
{
    //QPoint middle = (p1 * 0.55 + p2 * 0.45);
    QPoint middle = (p1+ p2)/2;
    // compute delta
    int dx = p2.x()-p1.x();
    int dy = p2.y()-p1.y();
    // invert sign of one delta
    dx *= -1;
    // swap deltas
    int tmp = dx;
    dx = dy;
    dy = tmp;
    // now stretch it
    dx = ((double)dx) / 1000.0 * curve;
    dy = ((double)dy) / 1000.0 * curve;
    // now move middle
    middle += QPoint(dx, dy);
    return middle;
}



void DEdit_WidgetPainter::paintTransition
        (QPainter* painter, DEdit_GraphicalTransition* transition)
{
    if(!painter || !transition || !transition->hasValidPointers())
    {
        return;
    }
    
    if(transition->wasChangedSinceRepaint())
    {// update if something has changed since last paint
        recomputeTransitionLabelArea(transition);
        repaintTransitionPixmap(transition);
    }
    QRect source = transition->m_cPixmap.rect();
    QRect target = source;
    target.moveCenter(transition->m_pStart->positionToQPoint());
    painter->drawPixmap(target, transition->m_cPixmap, source);
    
    
    {/// very useful to understand code about alpha mask
        //painter->drawImage(33, 33, transition->m_cAlphaMask);
    }
}


void DEdit_WidgetPainter::repaintTransitionPixmap(DEdit_GraphicalTransition* transition)
{
    if(!transition || !transition->hasValidPointers())
    {
        return;
    }
    /// init some variables
    int lineWidth = DEdit_GraphicalTransition::m_nLineWidth;
    // init line
    QLineF line(transition->m_pStart->positionToQPoint(),
                transition->m_pEnd  ->positionToQPoint());
    /// compute size of pixmaps
    int pixmapWidth = abs(line.dx())*2+lineWidth*3 + transition->m_cLabelArea.width();
    int pixmapHeight = abs(line.dy())*2+lineWidth*3 + transition->m_cLabelArea.height();
    // add height and width for curve
    pixmapWidth  += abs(line.dy())/1000.0*fabs(transition->m_nCurve);
    pixmapHeight += abs(line.dx())/1000.0*fabs(transition->m_nCurve);
    if(line.length() == 0)
    {
        pixmapWidth = DEdit_GraphicalTransition::m_nLineWidth;
        pixmapWidth += DEdit_GraphicalTransition::labelRadius();
        pixmapWidth += transition->m_cLabelArea.width()/2;
        pixmapWidth *= 2;
        pixmapHeight = pixmapWidth;
    }
    // for execution dot
    if(transition->m_bJustExecuted)
    {
        pixmapWidth += DEdit_GraphicalState::m_nDiameter/2;
        pixmapHeight += DEdit_GraphicalState::m_nDiameter/2;
    }
    
    /// compute offset
    int offsetX = pixmapWidth/2;
    int offsetY = pixmapHeight/2;
    /// now move the line by offset
    line = QLineF(offsetX, offsetY, offsetX+line.dx(),
                  offsetY+line.dy());
    // now resize pixmap
    transition->m_cPixmap = QPixmap(pixmapWidth, pixmapHeight);
    transition->m_cPixmap.fill(QColor(0, 0, 0, 0));
    /// init painter
    QPainter painter(&transition->m_cPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    // select pen according to the current attributes of transition
    if(transition->m_bJustExecuted){
        painter.setPen(m_cTransitionPenJustExecuted);
    }else if(transition->m_bSelected){
        painter.setPen(m_cTransitionPenSelected);
    }else if(transition->m_bHovered){
        painter.setPen(m_cTransitionPenHovered);
    }else{
        painter.setPen(m_cTransitionPen);
    }
    
    // paint line and label background to painter
    paintTransition(&painter, line, transition->m_nCurve, transition->m_fExecutionProgress, FALSE);
    // reset Label Area
    QRect lblArea = transition->m_cLabelArea;
    lblArea.moveTo(QPoint(offsetX, offsetY)
            + lblArea.topLeft() - transition->m_pStart->positionToQPoint());
    transition->m_cLabelArea = lblArea;
    if(lblArea.isValid()){
        painter.setBrush(painter.pen().brush());
        painter.drawRect(lblArea);
    }
    
    // paint label
    painter.setPen(m_cTransitionLabelPen);
    painter.setFont(m_cTransitionLabelFont);
    paintTransitionLabel(&painter, transition);
    
    
    // paint to alpha mask
    QImage alphaMask(pixmapWidth, pixmapHeight,
                     QImage::Format_Mono);
    QPainter alphaPainter(&alphaMask);
    QPen alphaPen(m_cTransitionPen);
    alphaPen.setColor(QColor(255, 255, 255));
    alphaPainter.setPen(alphaPen);
    alphaPainter.fillRect(QRect(0, 0, pixmapWidth, pixmapHeight),
                          QBrush(QColor(0, 0, 0)));
    
    // paint line and label background to alpha
    alphaPainter.drawRect(lblArea);
    alphaPainter.setBrush(Qt::NoBrush);
    paintTransition(&alphaPainter, line, transition->m_nCurve, transition->m_fExecutionProgress, FALSE);
    
    alphaPainter.end();
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
    if(!transition || !transition->hasValidPointers())
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
        QPoint lblPos = middlePointOfCurve(transition->m_pStart->positionToQPoint(),
                                           transition->m_pEnd->positionToQPoint(),
                                           transition->m_nCurve/2);
        if(transition->m_pStart->positionToQPoint()
           != transition->m_pEnd->positionToQPoint())
        {
            result.moveCenter(lblPos);
        }
        else
        {
            double labelRadius = DEdit_GraphicalTransition::labelRadius();
            // angle in 0.1°
            int angle = 450 + transition->m_nCurve; // start at 45°
            double angle_rad = ((double)angle)/1800 * M_PI;
            
            int dx = (int)(labelRadius * cos(angle_rad));
            int dy = (int)(labelRadius * sin(angle_rad));
            result.moveCenter(lblPos + QPoint(dx, -dy));
        }
    }
    transition->m_cLabelArea = result;
}

void DEdit_WidgetPainter::paintTransition(QPainter* painter, QLineF line,
                int curve, double execPosition, bool isTransitionPreview)
{
    int penwidth = m_cTransitionPen.width();
    double arrowlength = 15;
    double arrowwidth = M_PI/8; // in rad
    QPointF arrowPos = line.p2(); // arrowposition
    qreal arrowAngle = +90;
    painter->drawEllipse(line.x1()-penwidth, line.y1()-penwidth,
                         penwidth*2, penwidth*2);
    QPainterPath path; // path
    if(line.length() != 0)
    {
        // simply paint line,
        // if length != 0
        QPoint middle = middlePointOfCurve(line.p1().toPoint(), line.p2().toPoint(), curve);
        path.moveTo(line.p1());
        path.quadTo(middle, line.p2());
        QPointF arrowEnd = line.p1();
        if(!isTransitionPreview)
        {
            // if is not a transition preview, then move arrowposition
            qreal arrowPositionLen = path.length() - DEdit_GraphicalState::m_nDiameter/2;
            arrowPos = path.pointAtPercent(path.percentAtLength(arrowPositionLen));
            arrowEnd = path.pointAtPercent(path.percentAtLength(arrowPositionLen-arrowlength));
        }
        // angle of line from p2 to controlpoint of curve, i.e. QPoint middle to x-axis
        double dx = arrowEnd.x() - arrowPos.x();
        double dy = (-1)*(arrowEnd.y() - arrowPos.y());
        if(dx == 0)
        {
            arrowAngle = ((dy > 0) ? +90 : -90);
        }
        else
        {
            arrowAngle = atan(dy / dx) / M_PI * 180;
        }
        if(dx < 0)
        {
            arrowAngle += 180;
        }
        //painter->drawText(20, 20, QString("angle = ") + QString::number(arrowAngle));
    }
    else
    {// else a state is connected with itself
        int radius = DEdit_GraphicalState::m_nDiameter/2 + 
                DEdit_GraphicalTransition::m_nWithItselfTransitionRadius;
        path.moveTo(line.p1());
        double angle_rad = ((double)curve)/1800 * M_PI;
        double cos_angle = cos(angle_rad);
        double sin_angle = sin(angle_rad);
        double circle_center_radius = sqrt(2) * radius;
        // rect for circle
        QRectF rect(0, 0, radius*2, -radius*2);
        QPointF center(line.x1()+cos(angle_rad+M_PI_4)*circle_center_radius,
                       line.y1()-sin(angle_rad+M_PI_4)*circle_center_radius);
        rect.moveCenter(center.toPoint());
        // from state center to -90° of big circle
        path.lineTo(line.x1()+radius*cos_angle, line.y1()-radius*sin_angle);
        // draw big dircle from -90° to +180°
        path.arcTo(rect, ((qreal)curve)/-10+90,(qreal)-270);
        // from +180° to top of state
        arrowPos = QPoint(line.x1()-DEdit_GraphicalState::m_nDiameter/2*sin_angle,
                          line.y1()-DEdit_GraphicalState::m_nDiameter/2*cos_angle);
        if(isTransitionPreview)
        {
            path.lineTo(arrowPos.x(), arrowPos.y());
        }
        else
        {
            path.lineTo(line.p1());
        }
        arrowAngle = 90 + ((double)curve/10);
    }
    // draw calculated path
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
    if(execPosition >= 0.0 && execPosition <= 1.0)
    {
        int diameter = DEdit_GraphicalState::m_nDiameter/3;
        QRect dotRect(0, 0, diameter, diameter);
        dotRect.moveCenter(path.pointAtPercent(execPosition).toPoint());
        
        painter->setBrush(painter->pen().brush());
        painter->drawEllipse(dotRect);
    }
    
    
    //painter->drawText(100, 100, QString("Arrow angle: ") + QString::number(arrowAngle));
    // compute from degrees to rad
    arrowAngle *= M_PI / 180;
    
    // draw arrow
    int x, y;
    x = arrowlength*cos(arrowAngle+arrowwidth);
    y = arrowlength*sin(arrowAngle+arrowwidth);
    painter->drawLine(arrowPos.x(), arrowPos.y(),
                      arrowPos.x()+x, arrowPos.y()-y);
    
    x = arrowlength*cos(arrowAngle-arrowwidth);
    y = arrowlength*sin(arrowAngle-arrowwidth);
    
    painter->drawLine(arrowPos.x(), arrowPos.y(),
                      arrowPos.x()+x, arrowPos.y()-y);
    
}


void DEdit_WidgetPainter::recreateAllTemplates()
{
    // states
    recreateStatePens();
    recreateAllStateTemplates();
    
    // transitions
    recreateTransitionPens();
    // start state indicator after recreateTransitionPens
    // because recreateStartStateIndicator() needs some transition pens
    recreateStartStateIndicator();
    
    // "icons"
    m_cWidgetLockedPixmap = IconCatcher::getIcon("lock").pixmap(16,16);
}

void DEdit_WidgetPainter::recreateStatePens()
{
    m_cStateLabelPen.setWidth(3);
    m_cStateLabelPen.setColor(m_cAppearance.m_cStateLabelColor);
    m_cStateLabelFont.setBold(TRUE);
}



void DEdit_WidgetPainter::recreateAllStateTemplates()
{
    int diameter = DEdit_GraphicalState::m_nDiameter;
    m_cStateNormalTemplate = recreateStateTemplate(m_cAppearance.m_cStateNormal, diameter);
    recreateStateHoveredTemplate();
    m_cStateSelectedTemplate = recreateStateTemplate(m_cAppearance.m_cStateSelected, diameter, TRUE);
    m_cStateDraggedTemplate = m_cStateSelectedTemplate;
    m_cStateCurrentlyExecutedTemplate = recreateStateTemplate(m_cAppearance.m_cStateExecuted, diameter, TRUE);
    m_cStateResultDeniedTemplate = recreateStateTemplate(m_cAppearance.m_cStateResultDenied, diameter, TRUE);
    m_cStateResultAcceptedTemplate = recreateStateTemplate(m_cAppearance.m_cStateResultAccepted, diameter, TRUE);
}

void DEdit_WidgetPainter::recreateStateHoveredTemplate()
{
    m_cStateHoveredTemplate = recreateStateTemplate(
            QColor(255,255,255,33), DEdit_GraphicalState::m_nDiameter);
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
    // black = normal
    m_cTransitionPen.setCapStyle(Qt::RoundCap);
    m_cTransitionPen.setJoinStyle(Qt::RoundJoin);
    m_cTransitionPen.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPen.setCapStyle(Qt::RoundCap);
    m_cTransitionPen.setColor(m_cAppearance.m_cTransitionNormal);
    // light red = hovered
    m_cTransitionPenHovered.setCapStyle(Qt::RoundCap);
    m_cTransitionPenHovered.setJoinStyle(Qt::RoundJoin);
    m_cTransitionPenHovered.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPenHovered.setCapStyle(Qt::RoundCap);
    m_cTransitionPenHovered.setColor(m_cAppearance.m_cTransitionHovered);
    // dark red = selected
    m_cTransitionPenSelected.setCapStyle(Qt::RoundCap);
    m_cTransitionPenSelected.setJoinStyle(Qt::RoundJoin);
    m_cTransitionPenSelected.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPenSelected.setCapStyle(Qt::RoundCap);
    m_cTransitionPenSelected.setColor(m_cAppearance.m_cTransitionSelected);
    // executed
    m_cTransitionPenJustExecuted.setCapStyle(Qt::RoundCap);
    m_cTransitionPenJustExecuted.setJoinStyle(Qt::RoundJoin);
    m_cTransitionPenJustExecuted.setWidth(DEdit_GraphicalTransition::m_nLineWidth);
    m_cTransitionPenJustExecuted.setCapStyle(Qt::RoundCap);
    m_cTransitionPenJustExecuted.setColor(m_cAppearance.m_cTransitionExecuted);
    
    // for label
    m_cTransitionLabelPen.setColor(m_cAppearance.m_cTransitionLabelColor);
    m_cTransitionLabelFont.setBold(TRUE);
}



QPixmap DEdit_WidgetPainter::recreateStateTemplate(DEdit_ColorTripple colors, int diameter, bool invertedGradient)
{
    int pix_width = diameter * 1.1; // *1.1 : need space for shadow
    int circle_radius = diameter / 2;
    bool shadows = colors.m_cColor[0].alpha() > 125; // for low alpha: shadow is ugly and must be disabled ;D
    // init pixmap
    QPixmap resultPixmap(pix_width, pix_width);
    resultPixmap.fill(QColor(0, 0, 0, 0)); // clear completely
    
    // init painter
    QPainter pixPainter(&resultPixmap);
    pixPainter.setRenderHint(QPainter::Antialiasing, TRUE);
    // start paint
    int half_pix_width = pix_width/2; // half of the width
    drawTangoCircle(&pixPainter, half_pix_width, half_pix_width, circle_radius,
                     colors.m_cColor[0], colors.m_cColor[1], colors.m_cColor[2], shadows, invertedGradient);
    return resultPixmap;
}

QPixmap DEdit_WidgetPainter::recreateStateTemplate(QColor color, int diameter, bool invertedGradient)
{
    DEdit_ColorTripple colortripple;
    colortripple.createStateFromBasicColor(color);
    return recreateStateTemplate(colortripple, diameter, invertedGradient);
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



