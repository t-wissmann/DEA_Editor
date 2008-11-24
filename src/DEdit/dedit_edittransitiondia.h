
#ifndef __DEDIT_EDITTRANSITIONDIA_H_
#define __DEDIT_EDITTRANSITIONDIA_H_

#include <QDialog>
#include <multilanguage/translateableobject.h>

class DEdit_GraphicalTransition;
class DEdit_Widget;

// widgets
class QPushButton;
class QLabel;
class QLineEdit;
class QDialogButtonBox;
class CommandButtonDND;
class QSlider;
class QToolButton;
class QSpinBox;

// layouts
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class DEdit_EditTransitionDia : public QDialog,
      public TranslateableObject
{
    Q_OBJECT
public:
    DEdit_EditTransitionDia(DEdit_Widget* parent = 0);
    ~DEdit_EditTransitionDia();
    
    void setTransitionToEdit(DEdit_GraphicalTransition* transition);
    DEdit_GraphicalTransition* transitionToEdit();
    
    void retranslateUi();
    void reloadIcons();
public slots:
    void ok_clicked();
    bool applyChanges();
    void setToAllLetters();
    void setToAllUpperLetters();
    void setToAllLowerLetters();
    void setToAllDigits();
    void resetCurveToNull();
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
    
    // symbols
    QLineEdit* txtSymbols;
    QLabel*    lblSymbols;
    // curve
    QLabel*    lblCurve;
    QSlider*   slidCurve;
    QSpinBox*   spinCurve;
    QToolButton* btnResetCurve;
    
    // layouts
    QVBoxLayout* layoutParent;
    QGridLayout* layoutCommands;
    QHBoxLayout* layoutSymbols;
    QHBoxLayout* layoutCurve;
    
    DEdit_GraphicalTransition* m_pTransitionToEdit;
    DEdit_Widget*              m_pParent;
};



#endif
