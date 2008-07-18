
#ifndef __DEDIT_EDITTRANSITIONDIA_H_
#define __DEDIT_EDITTRANSITIONDIA_H_

#include <QDialog>

class DEdit_GraphicalTransition;

// widgets
class QPushButton;
class QLabel;
class QLineEdit;
class QDialogButtonBox;

// layouts
class QHBoxLayout;
class QVBoxLayout;

class DEdit_EditTransitionDia : public QDialog
{
    Q_OBJECT
public:
    DEdit_EditTransitionDia(QWidget* parent = 0);
    ~DEdit_EditTransitionDia();
    
    void setTransitionToEdit(DEdit_GraphicalTransition* transition);
    DEdit_GraphicalTransition* transitionToEdit();
    
    void retranslateUi();
public slots:
    
    void applyChanges();
    
private:
    // init functions
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    
    // widgets
    QPushButton* btnOk;
    QPushButton* btnCancel;
    QDialogButtonBox* boxBottom;
    
    QLineEdit* txtSymbols;
    QLabel*    lblSymbols;
    
    // layouts
    QVBoxLayout* layoutParent;
    QHBoxLayout* layoutSymbols;
    
    DEdit_GraphicalTransition* m_pTransitionToEdit;
};



#endif
