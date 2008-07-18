#ifndef __DEDIT_GRAPHICALTRANSITION_H_
#define __DEDIT_GRAPHICALTRANSITION_H_

class DEdit_GraphicalState;
class DEA_Transition;
#include <QList>
#include <QPoint>
#include <QImage>
#include <QString>
#include <QRect>

class DEdit_GraphicalTransition{
public:
    DEdit_GraphicalTransition();
    DEdit_GraphicalTransition(DEdit_GraphicalState* start,
                              DEdit_GraphicalState* end);
    ~DEdit_GraphicalTransition();
    
    
    DEdit_GraphicalState* m_pStart;
    DEdit_GraphicalState* m_pEnd;
    QString m_szSymbols;
    QList<DEA_Transition*>  m_DataList;
    QImage  m_cAlphaMask;
    QRect   m_cLabelArea;
    
    // attributes
    bool m_bHovered;
    bool m_bSelected;
    
    // functions
    bool hasValidPointers() const;
    bool isPointContained(QPoint pointToCheck);
    double distanceTo(QPoint point) const;
    bool isConnectedWith(DEdit_GraphicalState* state) const;
    void applySymbolsToDEA_Transitions();
    
    
    // static attributes
    static int m_nLineWidth;
    static int m_nWithItselfTransitionRadius;
};


#endif
