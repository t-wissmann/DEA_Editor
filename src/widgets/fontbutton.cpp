


#include "fontbutton.h"

#include <QFontDialog>

FontButton::FontButton(QWidget* parent)
    :QPushButton(parent)
{
    setText("abc ABC 123");
    connect(this, SIGNAL(clicked()), this, SLOT(selectFontFromDialog()));
}

FontButton::~FontButton()
{
        
}



void FontButton::selectFontFromDialog()
{
    bool bAccepted = false; // if user clicked OK in font dialog
    QFont newFont = QFontDialog::getFont(&bAccepted, font(), this);
    if(bAccepted)
    {
        setFont(newFont);
    }
}






