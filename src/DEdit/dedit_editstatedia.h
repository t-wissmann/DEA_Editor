
#ifndef __DEDIT_EDITSTATEDIA_H_
#define __DEDIT_EDITSTATEDIA_H_

#include <QDialog>

class DEdit_GraphicalState;
class DEdit_Widget;

// widgets
class QPushButton;
class QLineEdit;
class QLabel;
class QCheckBox;

// layouts
class QHBoxLayout;
class QVBoxLayout;
class QDialogButtonBox;


class DEdit_EditStateDia : public QDialog
{
    Q_OBJECT
public:
    DEdit_EditStateDia(DEdit_Widget* parent = 0);
    ~DEdit_EditStateDia();
    
    void setStateToEdit(DEdit_GraphicalState* state);
    DEdit_GraphicalState* stateToEdit();
    
    void retranslateUi();
    void reloadIcons();
public slots:
    bool applyChanges();
    void ok_clicked();
private:
    // init functions
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    
    // widgets
    QLabel* lblName;
    QLineEdit* txtName;
    QCheckBox* chkIsFinalState;
    QCheckBox* chkIsStartState;
    
    // buttons on the bottom
    QDialogButtonBox* boxBottom;
    QPushButton* btnOk;
    QPushButton* btnApply;
    QPushButton* btnCancel;
    
    // layouts
    QVBoxLayout* layoutParent;
    QHBoxLayout* layoutName;
    
    
    
    // members
    DEdit_Widget*         m_pParent;
    DEdit_GraphicalState* m_pStateToEdit;
};

#endif


