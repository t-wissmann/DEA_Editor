
#ifndef __DEDIT_EXECDEAWIDGET_H_
#define __DEDIT_EXECDEAWIDGET_H_

#include <QWidget>
#include <QIcon>

class DEA_State;
class DEdit_GraphicalState;
class DEdit_GraphicalTransition;
// widgets
class DEdit_Widget;
class QLineEdit;
class QLabel;
class QPushButton;
class QToolButton;
class QTextEdit;
class QCheckBox;
class QSpinBox;
// layouts
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
// other qt classes
class QTimer;
#include<QTime>

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
    void tmrExecState_timout();
    void activateTimer(bool on);
    void setTimerInterval(int interval);
    void execToNextBreakPoint();
    void resetExecutionProgressDot();
private:
    // init functions
    void allocateWidgets();
    void createLayouts();
    void connectSlots();
    
    // private setter
    void resetWidgetProperties();
    void removeAllResultIndicators();
    void resetLastTransition(DEdit_GraphicalTransition* transition);
    void resetCurrentState(DEdit_GraphicalState* state);
    
    void printTransitionFound(char symbol, DEA_State* from, DEA_State* to);
    
    
    
    // widgets
    // toolbuttons
    QPushButton* btnStart;
    QPushButton* btnPause;
    QPushButton* btnSingleStep;
    QPushButton* btnStop;
    QPushButton* btnLocked;
    // timer
    QCheckBox*   chkTimerControlled;
    QLabel*      lblTimerInterval;
    QSpinBox*    spinTimerInterval;
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
    QHBoxLayout* layoutTimer;
    QGridLayout* layoutInputAndResult;
    QHBoxLayout* layoutOutputToolButtons;
    
    // members needed while running
    DEdit_GraphicalState* m_pCurrentState;
    QString       m_szStringToCheck;
    int           m_nCurrentPosition;
    QString       m_szAcceptedSymbols;
    bool          m_bErrorOccured;
    DEdit_GraphicalTransition* m_pLastTransition;
    QTime         m_cStateChangedTimestamp;
    
    // icons
    QIcon         m_cIconEditorLocked;
    QIcon         m_cIconEditorUnlocked;
    
    // members
    int           m_nTransitionAnimationTime; // in ms
    bool          m_bRunning;
    bool          m_bPaused;
    DEdit_Widget* m_pDeaWidget;
    QTimer*       m_tmrExecNextState;
};


#endif
