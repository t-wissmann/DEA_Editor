#ifndef __DEDIT_WIDGETPAINTER_H_ 
#define __DEDIT_WIDGETPAINTER_H_

#include <QColor>

class DEdit_Widget;
#include <DEdit/dedit_graphicalstate.h>
#include <DEdit/dedit_appearance.h>
class DEdit_GraphicalTransition;
// painter
class QPainter;
#include <QPixmap>
#include <QPen>
#include <QPen>
#include <QFont>


class DEdit_WidgetPainter
{
    // friends
    friend class DEdit_Widget;
    friend bool DEdit_GraphicalState::isPointContained(QPoint pointToCheck);
public:
    DEdit_WidgetPainter(DEdit_Widget* widget = 0);
    ~DEdit_WidgetPainter();
    
    // attributes / getter-setter
    void setPaintRichText(bool bPaintRichText);
    bool paintRichText();
    
    // painting functions
    void paint();
    void paintState(QPainter* painter, DEdit_GraphicalState* state);
    void paintStateLabel(QPainter* painter, DEdit_GraphicalState* state);
    static void paintRichTextAt(QPainter* painter, const QRect &rect,
                                int flags, QString &text);
    
    void paintTransition(QPainter* painter, DEdit_GraphicalTransition* transition);
    void repaintTransitionPixmap(DEdit_GraphicalTransition* transition);
    void paintTransition(QPainter* painter, QLineF line, int curve, double execPosition, bool isTransitionPreview = false);
    void paintTransitionLabel(QPainter* painter, DEdit_GraphicalTransition* transition);
    void recomputeTransitionLabelArea(DEdit_GraphicalTransition* transition);
    
    void setAllItemsToNotChanged();
    void setAllItemsToWasChanged();
    
    // create templates
    void recreateAllTemplates();
    void recreateAllStateTemplates();
    void recreateStateHoveredTemplate();
    void recreateTransitionPens();
    void recreateStatePens();
    void recreateStartStateIndicator();
    
    static QPoint middlePointOfCurve(QPoint p1, QPoint p2, int curve);
    static QPixmap recreateStateTemplate(DEdit_ColorTripple colors, int diameter, bool invertedGradient = false);
    static QPixmap recreateStateTemplate(QColor color, int diameter, bool invertedGradient = false);
    
    // general paint functions
    static void drawTangoCircle(QPainter* painter, int centerx, int centery, int radius,
            QColor background, QColor innerborder, QColor outerborder, bool shadow, bool invertedGradient = false);
private:
    // members
    DEdit_Widget* m_pWidget;
    DEdit_Appearance m_cAppearance;
    // attributes
    bool    m_bPaintRichText;
    
    // state template pixmaps
    QPixmap  m_cStateNormalTemplate;
    QPixmap  m_cStateHoveredTemplate;
    QPixmap  m_cStateSelectedTemplate;
    QPixmap  m_cStateDraggedTemplate;
    QPixmap  m_cStateCurrentlyExecutedTemplate;
    QPixmap  m_cStateResultDeniedTemplate;
    QPixmap  m_cStateResultAcceptedTemplate;
    QPixmap  m_cStartStateIndicator;
    QImage   m_cStartStateIndicatorAlphaMask;
    QPoint   m_cStartStateIndicatorPosition; // relativ positon to center
    QPen     m_cStateLabelPen;
    QFont    m_cStateLabelFont;
    QPen     m_cTransitionPen;
    QPen     m_cTransitionPenHovered;
    QPen     m_cTransitionPenSelected;
    QPen     m_cTransitionPenJustExecuted;
    QPen     m_cTransitionLabelPen;
    QFont    m_cTransitionLabelFont;
    QPixmap  m_cWidgetLockedPixmap;
};

#endif
