#ifndef __DEDIT_GRAPHICALTRANSITION_H_
#define __DEDIT_GRAPHICALTRANSITION_H_

class DEdit_GraphicalState;
class DEA_Transition;
#include <QPoint>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QRect>

class DEdit_GraphicalTransition{
public:
    DEdit_GraphicalTransition();
    DEdit_GraphicalTransition(DEdit_GraphicalState* start,
                              DEdit_GraphicalState* end);
    ~DEdit_GraphicalTransition();
    
    
    // for repaint
    bool m_bChangedSinceRepaint;
    bool wasChangedSinceRepaint() const;
    void setWasChanged();
    void setToNotChanged();
    // properties
    DEdit_GraphicalState* m_pStart;
    DEdit_GraphicalState* m_pEnd;
    DEA_Transition*  m_pData;
    QImage  m_cAlphaMask;
    QPixmap m_cPixmap;
    QRect   m_cLabelArea;
    
    // attributes
    bool m_bHovered;
    bool m_bSelected;
    bool m_bJustExecuted;
    int  m_nCurve;
    int  m_nDragRotationOffset; // rotation offset, when dragging a transition, when start = end
    
    int curveByDragPosition(QPoint dragPos);
    static int curveByDragPosition(QPoint p1, QPoint p2, QPoint dragPos);
    
    // functions
    bool hasValidPointers() const;
    bool isPointContained(QPoint pointToCheck);
    double distanceTo(QPoint point) const;
    bool startEqualsEnd() const;
    bool isConnectedWith(DEdit_GraphicalState* state) const;
    
    void setSymbols(QString symbols);
    QString symbols();
    QString graphicalLabel() const; // this is a shortened version of m_szSymbols
    
    static double labelRadius();
    
    // static attributes
    static int m_nLineWidth;
    static int m_nWithItselfTransitionRadius;
    
};


#endif
