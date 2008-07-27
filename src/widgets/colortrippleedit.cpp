
#include "colortrippleedit.h"
#include <DEdit/dedit_appearance.h> // for ColorTripple

#include <widgets/colorbutton.h>
#include <QLabel>
#include <QHBoxLayout>
#include <QSizePolicy>

ColorTrippleEdit::ColorTrippleEdit(QWidget *parent)
    : QWidget(parent)
{
    // init members
    m_pColors = NULL;
    // init gui
    lblDescription = new QLabel(this);
    btnColor0 = new ColorButton(this);
    btnColor1 = new ColorButton(this);
    btnColor2 = new ColorButton(this);
    btnColor0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    btnColor1->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    btnColor2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

ColorTrippleEdit::~ColorTrippleEdit()
{
    
}



void ColorTrippleEdit::setText(QString text)
{
    lblDescription->setText(text);
}

void ColorTrippleEdit::addToGridLayout(QGridLayout* layout, int row, int col)
{
    layout->addWidget(lblDescription, row, col++);
    layout->addWidget(btnColor0, row, col++);
    layout->addWidget(btnColor1, row, col++);
    layout->addWidget(btnColor2, row, col++);
}


void ColorTrippleEdit::setAdvancedColorsEnabled(bool enabled)
{
    btnColor1->setEnabled(enabled);
    btnColor2->setEnabled(enabled);
}

void ColorTrippleEdit::setColorTripple(DEdit_ColorTripple* colors)
{
    m_pColors = colors;
    if(!m_pColors)
    {
        return;
    }
    btnColor0->setColor(colors->m_cColor[0]);
    btnColor1->setColor(colors->m_cColor[1]);
    btnColor2->setColor(colors->m_cColor[2]);
}

DEdit_ColorTripple* ColorTrippleEdit::colorTripple()
{
    return m_pColors;
}


void ColorTrippleEdit::applyChanges()
{
    if(!m_pColors)
    {
        return;
    }
    if(btnColor1->isEnabled()) // if is in advanced mode
    {
        m_pColors->m_cColor[0] = btnColor0->color();
        m_pColors->m_cColor[1] = btnColor1->color();
        m_pColors->m_cColor[2] = btnColor2->color();
    }
    else
    {
        m_pColors->createStateFromBasicColor(btnColor0->color());
    }
}


