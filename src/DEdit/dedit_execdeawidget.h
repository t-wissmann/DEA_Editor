
#ifndef __DEDIT_EXECDEAWIDGET_H_
#define __DEDIT_EXECDEAWIDGET_H_

#include <QWidget>
#include <QIcon>

class DEdit_GraphicalState;
class DEdit_GraphicalTransition;
// widgets
class DEdit_Widget;
class QLineEdit;
class QLabel;
class QPushButton;
class QToolButton;
class QTextEdit;

// layouts
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class DEdit_ExecDeaWidget : public QWidget
{
    Q_OBJECT
public:
    DEdit_ExecDeaWidget(QWidget* parent = 0);
    ~DEdit_ExecDeaWidget();
    
    void retranslateUi();
    void reloadIcons();
    
    void setDeaWidget(DEdit_Widget* widget);
    DEdit_Widget* deaWidget();
    
    bool isRunning() const;
    bool isPaused() const;
public slots:
    void putMessage(QString msg);
    void start();
    void pause();
    void stop();
    void executeToNextState();
    void updateResultLabel();
    void setEditorLocked(bool locked);
private:
    // init functions
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    
    // private setter
    void resetWidgetProperties();
    void removeAllResultIndicators();
    
    // widgets
    // toolbuttons
    QPushButton* btnStart;
    QPushButton* btnPause;
    QPushButton* btnSingleStep;
    QPushButton* btnStop;
    QPushButton* btnLocked;
    // inputstring
    QLabel*      lblInputString;
    QLineEdit*   txtInputString;
    QToolButton* btnClearInputString;
    
    // console
    QPushButton* btnShowHideOutput;
    QPushButton* btnClearOutput;
    QTextEdit*   txtOutput;
    // result
    QLabel*      lblResultLabel;
    QLabel*      lblResult;
    QLabel*      lblCurrentResult;
    
    // layouts
    QVBoxLayout* layoutParent;
    QHBoxLayout* layoutStartStop;
    QGridLayout* layoutInputAndResult;
    QHBoxLayout* layoutOutputToolButtons;
    
    // members needed while running
    DEdit_GraphicalState* m_pCurrentState;
    QString       m_szStringToCheck;
    int           m_nCurrentPosition;
    QString       m_szAcceptedSymbols;
    bool          m_bErrorOccured;
    DEdit_GraphicalTransition* m_pLastTransition;
    
    // icons
    QIcon         m_cIconEditorLocked;
    QIcon         m_cIconEditorUnlocked;
    
    // members
    bool          m_bRunning;
    bool          m_bPaused;
    DEdit_Widget* m_pDeaWidget;
};


#endif
