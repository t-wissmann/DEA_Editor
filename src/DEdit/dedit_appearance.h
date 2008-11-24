#ifndef __DEDIT_APPEARANCE_H_
#define __DEDIT_APPEARANCE_H_

#include <QColor>
#include <QFont>


class DEdit_ColorTripple
{
public:
    DEdit_ColorTripple() {};
    ~DEdit_ColorTripple() {};
    
    QColor m_cColor[3];
    // for states:
    // 0 = fill color
    // 1 = inner border
    // 2 = outer border
    void operator=(const DEdit_ColorTripple& other)
    {
        m_cColor[0] = other.m_cColor[0];
        m_cColor[1] = other.m_cColor[1];
        m_cColor[2] = other.m_cColor[2];
    };
    void createStateFromBasicColor(QColor color);
};

class DEdit_Appearance
{
public:
    DEdit_Appearance();
    ~DEdit_Appearance();
    
    DEdit_ColorTripple m_cStateNormal;
    DEdit_ColorTripple m_cStateSelected;
    DEdit_ColorTripple m_cStateExecuted;
    DEdit_ColorTripple m_cStateResultDenied;
    DEdit_ColorTripple m_cStateResultAccepted;
    QColor             m_cStateLabelColor;
    QFont              m_cStateLabelFont;
    
    
    QColor          m_cTransitionNormal;
    QColor          m_cTransitionHovered;
    QColor          m_cTransitionSelected;
    QColor          m_cTransitionExecuted;
    QColor          m_cTransitionLabelColor;
    QFont           m_cTransitionLabelFont;
    
    static void createTangoDefault(DEdit_Appearance* app);
    
};

#endif
