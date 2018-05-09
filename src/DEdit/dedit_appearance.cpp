
#include "dedit_appearance.h"


void DEdit_ColorTripple::createStateFromBasicColor(QColor color)
{
    m_cColor[0] = color;
    m_cColor[1] = color.lighter(170);
    m_cColor[2] = color.darker(170);
}


DEdit_Appearance::DEdit_Appearance()
{
    
}

DEdit_Appearance::~DEdit_Appearance()
{
    
}


void DEdit_Appearance::createTangoDefault(DEdit_Appearance* app)
{
    if(!app)
    {
        return;
    }
    // states
    app->m_cStateNormal.createStateFromBasicColor(QColor("#3465A4"));
    app->m_cStateSelected.createStateFromBasicColor(QColor("#75507B"));
    app->m_cStateExecuted.createStateFromBasicColor(QColor("#EDD400"));
    app->m_cStateResultDenied.createStateFromBasicColor(QColor("#EF2929"));
    app->m_cStateResultAccepted.createStateFromBasicColor(QColor("#73D216"));
    app->m_cStateLabelColor = QColor(238, 238, 238);
    app->m_cStateLabelFont = QFont(); // this is the the default font
    app->m_cStateLabelFont.setBold(true); // only difference: bold
    // transitions
    
    app->m_cTransitionNormal = QColor(0, 0, 0);
    app->m_cTransitionHovered = QColor(204, 0, 0);
    app->m_cTransitionSelected = QColor(239, 41, 41);
    app->m_cTransitionExecuted = QColor("#EDD400");
    app->m_cTransitionLabelColor = QColor(238, 238, 238);
    app->m_cTransitionLabelFont = QFont(); // this is the the default font
    app->m_cTransitionLabelFont.setBold(true); // only difference: bold
    
}


