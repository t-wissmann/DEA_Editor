
#ifndef __FONTBUTTON_H_
#define __FONTBUTTON_H_

#include <QPushButton>

class FontButton : public QPushButton
{
    Q_OBJECT
public:
    FontButton(QWidget* parent = 0);
    ~FontButton();   
public slots:
    void selectFontFromDialog();
    
    
};



#endif
