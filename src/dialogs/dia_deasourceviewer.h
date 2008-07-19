
#ifndef __DIA_DEASOURCEVIEWER_H_
#define __DIA_DEASOURCEVIEWER_H_

#include <QDialog>

class QPushButton;
class QTextEdit;

class QHBoxLayout;
class QVBoxLayout;

class DEdit_Widget;

class Dia_DeaSourceViewer : public QDialog
{
    Q_OBJECT
public:
    Dia_DeaSourceViewer(QWidget* parent = 0);
    ~Dia_DeaSourceViewer();
    
    void retranslateUi();
    void reloadIcons();
    
    void setDeaToView(DEdit_Widget* dw);
    DEdit_Widget* dea();
    
public slots:
    void refresh();
    
private:
    void createGui();
    
    QPushButton* btnRefresh;
    QPushButton* btnClose;
    QTextEdit* txtSource;
    
    QVBoxLayout* layoutParent;
    QHBoxLayout* layoutToolButtons;
    
    // members
    DEdit_Widget* m_pDeaToView;
};


#endif
