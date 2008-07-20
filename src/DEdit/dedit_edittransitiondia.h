
#ifndef __DEDIT_EDITTRANSITIONDIA_H_
#define __DEDIT_EDITTRANSITIONDIA_H_

#include <QDialog>

class DEdit_GraphicalTransition;

// widgets
class QPushButton;
class QLabel;
class QLineEdit;
class QDialogButtonBox;
class CommandButtonDND;

// layouts
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class DEdit_EditTransitionDia : public QDialog
{
    Q_OBJECT
public:
    DEdit_EditTransitionDia(QWidget* parent = 0);
    ~DEdit_EditTransitionDia();
    
    void setTransitionToEdit(DEdit_GraphicalTransition* transition);
    DEdit_GraphicalTransition* transitionToEdit();
    
    void retranslateUi();
    void reloadIcons();
public slots:
    
    void applyChanges();
    void setToAllLetters();
    void setToAllUpperLetters();
    void setToAllLowerLetters();
    void setToAllDigits();
private:
    // init functions
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    
    // widgets
    QPushButton* btnOk;
    QPushButton* btnApply;
    QPushButton* btnCancel;
    QDialogButtonBox* boxBottom;
    // commandbuttons
    CommandButtonDND* btnAllLetters;
    CommandButtonDND* btnAllUpperLetters;
    CommandButtonDND* btnAllLowerLetters;
    CommandButtonDND* btnAllDigits;
    
    QLineEdit* txtSymbols;
    QLabel*    lblSymbols;
    
    // layouts
    QVBoxLayout* layoutParent;
    QGridLayout* layoutCommands;
    QHBoxLayout* layoutSymbols;
    
    DEdit_GraphicalTransition* m_pTransitionToEdit;
};



#endif
