#ifndef __DEDIT_WIDGETPAINTER_H_ 
#define __DEDIT_WIDGETPAINTER_H_

#include <QColor>

class DEdit_Widget;
class DEdit_GraphicalState;
class DEdit_GraphicalTransition;
// painter
class QPainter;
#include <QPixmap>
#include <QPen>


class DEdit_WidgetPainter
{
    // friends
    friend class DEdit_Widget;
public:
    DEdit_WidgetPainter(DEdit_Widget* widget = 0);
    ~DEdit_WidgetPainter();
    
    void paint();
    void paintState(QPainter* painter, DEdit_GraphicalState* state);
    
    // create templates
    void recreateAllTemplates();
    void recreateStateNormalTemplate();
    void recreateStateHoveredTemplate();
    void recreateStateDraggedTemplate();
    void recreateStateSelectedTemplate();
    void recreateTransitionPens();
    
    void paintTransition(QPainter* painter, DEdit_GraphicalTransition* transition);
    void paintTransition(QPainter* painter, QLineF line);
    static QPixmap recreateStateTemplate(QColor color, int diameter, bool invertedGradient = FALSE);
    
    // general paint functions
    static void drawTangoCircle(QPainter* painter, int centerx, int centery, int radius,
            QColor background, QColor innerborder, QColor outerborder, bool shadow, bool invertedGradient = FALSE);
private:
    // members
    DEdit_Widget* m_pWidget;
    
    // state template pixmaps
    QPixmap  m_cStateNormalTemplate;
    QPixmap  m_cStateHoveredTemplate;
    QPixmap  m_cStateDraggedTemplate;
    QPixmap  m_cStateSelectedTemplate;
    QPen     m_cTransitionPen;
    QPen     m_cTransitionPenHovered;
    QPen     m_cTransitionPenSelected;
};

#endif
