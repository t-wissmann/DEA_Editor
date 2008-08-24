

#ifndef __COLORTRIPPLEEDIT_H_
#define __COLORTRIPPLEEDIT_H_

#include <QWidget>

class ColorButton;
class QLabel;
class QGridLayout;
class DEdit_ColorTripple;

class ColorTrippleEdit : public QWidget
{
    Q_OBJECT
public:
    ColorTrippleEdit(QWidget* parent = 0);
    ~ColorTrippleEdit();
    
    void setText(QString text);
    void addToGridLayout(QGridLayout* layout, int row, int col = 0);
    void applyChanges();
    
    void setColorTripple(DEdit_ColorTripple* colors);
    DEdit_ColorTripple* colorTripple();
    void initColorsFromColorTripple(DEdit_ColorTripple* colors);
public slots:
    void setAdvancedColorsEnabled(bool enabled);
private:
    QLabel*      lblDescription;
    ColorButton* btnColor0;
    ColorButton* btnColor1;
    ColorButton* btnColor2;
    // members
    DEdit_ColorTripple* m_pColors;
};


#endif

