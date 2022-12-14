
#include "dedit_execdeawidget.h"
#include "dedit_widget.h"
#include <io/iconcatcher.h>

#include <DEdit/dedit_graphicalstate.h>
#include <DEdit/dedit_graphicaltransition.h>
#include <DEA/dea_state.h>
#include <DEA/dea_transition.h>

#include <QTimer>
// widgtes
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QToolButton>
#include <QCheckBox>
#include <QSpinBox>

// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

// other qt-classes
#include <QTextCursor>

DEdit_ExecDeaWidget::DEdit_ExecDeaWidget(QWidget* parent)
    : QWidget(parent)
{
    // init some members
    m_pDeaWidget = NULL;
    m_bRunning = false;
    m_bPaused = false;
    m_bErrorOccured = false;
    m_nCurrentPosition = 0;
    m_pCurrentState = NULL;
    m_nTransitionAnimationTime = 1000;
    m_pLastTransition = NULL;
    // init gui
    allocateWidgets();
    createLayouts();
    connectSlots();
    resetWidgetProperties();
    // init timer
    m_tmrExecNextState = new QTimer(this);
    m_tmrExecNextState->setInterval(1000);
    connect(m_tmrExecNextState, SIGNAL(timeout()), this, SLOT(tmrExecState_timout())); 
    
    retranslateUi();
    reloadIcons();
    
    // init widgets
    chkTimerControlled->setChecked(true);
    
}

DEdit_ExecDeaWidget::~DEdit_ExecDeaWidget()
{
    
}


void DEdit_ExecDeaWidget::allocateWidgets()
{
    btnStart = new QPushButton;
    btnStop = new QPushButton;
    btnPause  = new QPushButton;
    btnPause->setVisible(false);
    btnSingleStep = new QPushButton;
    btnLocked = new QPushButton;
    btnLocked->setCheckable(true);
    // output / console
    txtOutput = new QTextEdit;
    txtOutput->setReadOnly(true);
    txtOutput->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    txtOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    txtOutput->setVisible(false);
    btnShowHideOutput = new QPushButton;
    btnShowHideOutput->setCheckable(true);
    btnShowHideOutput->setChecked(false);
    btnClearOutput = new QPushButton;
    btnClearOutput->setVisible(false);
    // timer
    chkTimerControlled = new QCheckBox;
    lblTimerInterval = new QLabel;
    spinTimerInterval = new QSpinBox;
    spinTimerInterval->setRange(0, 10000);
    spinTimerInterval->setValue(1000);
    spinTimerInterval->setSingleStep(100);
    // input string
    lblInputString = new QLabel;
    txtInputString = new QLineEdit;
    btnClearInputString = new QToolButton;
    // result
    lblResultLabel = new QLabel;
    lblResultLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lblResult = new QLabel(" ");
    lblResult->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lblResult->setFrameStyle(QFrame::Panel | QFrame::Shadow_Mask);
    lblResult->setFont(txtInputString->font());
    lblCurrentResult = new QLabel(" ");
    lblCurrentResult->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lblCurrentResult->setFrameStyle(QFrame::Panel | QFrame::Shadow_Mask);
    lblCurrentResult->setFont(txtInputString->font());
    
}

void DEdit_ExecDeaWidget::createLayouts()
{
    layoutStartStop = new QHBoxLayout;
    layoutStartStop->setMargin(0);
    layoutStartStop->addWidget(btnStart);
    layoutStartStop->addWidget(btnPause);
    layoutStartStop->addWidget(btnSingleStep);
    layoutStartStop->addWidget(btnStop);
    layoutStartStop->addStretch();
    layoutStartStop->addWidget(btnLocked);
    
    layoutInputAndResult = new QGridLayout;
    layoutInputAndResult->setMargin(0);
    layoutInputAndResult->addWidget(lblInputString, 0, 0);
    QHBoxLayout* layoutTextbox = new QHBoxLayout;
    layoutTextbox->setMargin(0);
    layoutTextbox->addWidget(txtInputString);
    layoutTextbox->addWidget(btnClearInputString);
    layoutInputAndResult->addLayout(layoutTextbox, 0, 1, 1, 2); // colspan = 2
    layoutInputAndResult->addWidget(lblResultLabel, 1, 0);
    layoutInputAndResult->addWidget(lblResult, 1, 1);
    layoutInputAndResult->addWidget(lblCurrentResult, 1, 2);
    
    layoutOutputToolButtons = new QHBoxLayout;
    layoutOutputToolButtons->setMargin(0);
    layoutOutputToolButtons->addWidget(btnShowHideOutput);
    layoutOutputToolButtons->addWidget(btnClearOutput);
    layoutOutputToolButtons->addStretch();
    
    layoutTimer = new QHBoxLayout;
    layoutTimer->setMargin(0);
    layoutTimer->addWidget(chkTimerControlled);
    layoutTimer->addWidget(lblTimerInterval);
    layoutTimer->addWidget(spinTimerInterval);
    layoutTimer->addStretch();
    
    layoutParent = new QVBoxLayout;
    layoutParent->setMargin(2);
    layoutParent->addLayout(layoutStartStop);
    layoutParent->addLayout(layoutTimer);
    layoutParent->addLayout(layoutInputAndResult);
    layoutParent->addWidget(txtOutput);
    layoutParent->addLayout(layoutOutputToolButtons);
    
    setLayout(layoutParent);
    
}

void DEdit_ExecDeaWidget::connectSlots()
{
    connect(btnStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(btnStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(btnPause, SIGNAL(clicked()), this, SLOT(pause()));
    connect(btnSingleStep, SIGNAL(clicked()), this, SLOT(executeToNextState()));
    connect(btnLocked, SIGNAL(toggled(bool)), this, SLOT(setEditorLocked(bool)));
    // for timer
    connect(chkTimerControlled, SIGNAL(toggled(bool)), this, SLOT(activateTimer(bool)));
    connect(spinTimerInterval, SIGNAL(valueChanged(int)), this, SLOT(setTimerInterval(int)));
    // for output / console
    connect(btnShowHideOutput, SIGNAL(toggled(bool)), txtOutput, SLOT(setVisible(bool)));
    connect(btnShowHideOutput, SIGNAL(toggled(bool)), btnClearOutput, SLOT(setVisible(bool)));
    connect(btnClearOutput, SIGNAL(clicked()), txtOutput, SLOT(clear()));
    connect(btnClearInputString, SIGNAL(clicked()), txtInputString, SLOT(clear()));
    
    
    // for dev
    QTimer* tmr = new QTimer(this);
    connect(tmr, SIGNAL(timeout()), this, SLOT(resetExecutionProgressDot()));
    tmr->setInterval(40);
    tmr->start();
}

void DEdit_ExecDeaWidget::retranslateUi()
{
    btnStart->setText(tr("Start"));
    btnStop->setText(tr("Stop"));
    btnPause->setText(tr("Pause"));
    btnSingleStep->setText(tr("Single Step"));
    btnLocked->setText(tr("Lock Editor"));
    lblInputString->setText(tr("Text to recognize:"));
    btnClearInputString->setToolTip(tr("Clear Field"));
    btnShowHideOutput->setText(tr("Show log"));
    btnClearOutput->setText(tr("Clear log"));
    lblResultLabel->setText(tr("Result:"));
    // timer
    spinTimerInterval->setSuffix(tr(" ms"));
    spinTimerInterval->setSpecialValueText(tr("immediately"));
    chkTimerControlled->setText(tr("Timer controlled"));
    lblTimerInterval->setText(tr("Interval between two states: "));
    
}

void DEdit_ExecDeaWidget::reloadIcons()
{
    btnStart->setIcon(IconCatcher::getIcon("ktstart_all"));
    btnStop->setIcon(IconCatcher::getIcon("ktstop_all"));
    btnPause->setIcon(IconCatcher::getIcon("player_pause"));
    btnLocked->setIcon(IconCatcher::getIcon("unlock"));
    btnClearInputString->setIcon(IconCatcher::getIcon("editclear"));
    btnShowHideOutput->setIcon(IconCatcher::getIcon("konsole"));
    btnClearOutput->setIcon(IconCatcher::getIcon("editclear"));
    btnSingleStep->setIcon(IconCatcher::getIcon("1rightarrow"));
    
    m_cIconEditorLocked = IconCatcher::getIcon("lock");
    m_cIconEditorUnlocked = IconCatcher::getIcon("unlock");
}

void DEdit_ExecDeaWidget::setDeaWidget(DEdit_Widget* widget)
{
    m_pDeaWidget = widget;
}

DEdit_Widget* DEdit_ExecDeaWidget::deaWidget()
{
    return m_pDeaWidget;
}


void DEdit_ExecDeaWidget::putMessage(QString msg)
{
    txtOutput->append(msg);
    // set cursor to end of output
    QTextCursor cursor = txtOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    txtOutput->setTextCursor(cursor);
}

void DEdit_ExecDeaWidget::resetWidgetProperties()
{
    btnStart->setEnabled(!m_bRunning || m_bPaused);
    btnPause->setEnabled(m_bRunning && !m_bPaused);
    btnStop->setEnabled(m_bRunning);
    btnSingleStep->setEnabled(m_bRunning);
    txtInputString->setReadOnly(m_bRunning);
    btnClearInputString->setEnabled(!m_bRunning);
    if(m_bRunning)
    {
        btnLocked->setChecked(true);
    }
    btnLocked->setEnabled(!m_bRunning);
}


bool DEdit_ExecDeaWidget::isRunning() const
{
    return m_bRunning;
}

bool DEdit_ExecDeaWidget::isPaused() const
{
    return m_bPaused;
}


void DEdit_ExecDeaWidget::start()
{
    if(!m_pDeaWidget)
    {
        return;
    }
    removeAllResultIndicators();
    putMessage(tr("Starting DEA..."));
    m_bRunning = true;
    m_bPaused = false;
    m_bErrorOccured = false;
    resetLastTransition(m_pLastTransition);
    resetWidgetProperties();
    
    m_szStringToCheck = txtInputString->text();
    m_szAcceptedSymbols = "";
    m_nCurrentPosition = 0;
    
    // select currentState
    resetCurrentState(m_pDeaWidget->graphicalStartState()); // set m_pCurrentState to start
    if(!m_pCurrentState)
    {
        m_bErrorOccured = true;
        putMessage(tr("Error when starting: No start state defined!"));
        stop();
        updateResultLabel();
        return;
    }
    updateResultLabel();
    m_pDeaWidget->update();
    // execute to end if wanted
    if(chkTimerControlled->isChecked()
       && spinTimerInterval->value() == 0)
    {
        execToNextBreakPoint();
    }
}

void DEdit_ExecDeaWidget::stop()
{
    m_bRunning = false;
    m_bPaused = false;
    
    resetLastTransition(NULL);
    if(m_pCurrentState)
    {
        m_pCurrentState->m_bCurrentlyExecutedState = false;
    }
    
    
    if(m_pDeaWidget)
    {
        m_pDeaWidget->update();
    }
    putMessage(tr("Stopped DEA"));
    resetWidgetProperties();
}


void DEdit_ExecDeaWidget::pause()
{
    m_bPaused = true;
    resetWidgetProperties();
}


void DEdit_ExecDeaWidget::executeToNextState()
{
    if(!m_pDeaWidget || !m_pDeaWidget->dea() || !m_pCurrentState || !m_pCurrentState->m_pData)
    {
        putMessage("Internal Pointer Error");
        stop();
        updateResultLabel();
        return;
    }
    
    DEA_State* state = m_pCurrentState->m_pData;
    if(m_nCurrentPosition >= m_szStringToCheck.size())
    {
        // if end of string has been reached
        if(state->isFinalState()){
            QString text;
            text = tr("End of string has been reached and current state \'%statename\' is a final state.").replace("%statename", state->name());
            text += "\n" + tr("\'%inputstring\' ACCEPTED").replace("%inputstring", m_szStringToCheck);
            putMessage(text);
            m_pCurrentState->m_eResultIndicator = DEdit_GraphicalState::ResultAccepted;
        }
        else
        {
            QString text;
            
            text = tr("End of string has been reached but current state \'%statename\' is NOT a final state.").replace("%statename", state->name());
            text += "\n" + tr("\'%inputstring\' DENIED").replace("%inputstring", m_szStringToCheck);
            m_bErrorOccured = true;
            // add space so that in lblResult, we can see a red rect at end of string
            m_szStringToCheck += "  ";
            putMessage(text);
            m_pCurrentState->m_eResultIndicator = DEdit_GraphicalState::ResultDenied;
        }
        stop();
        updateResultLabel();
        return;
    }
    char currentSymbol = QChar(m_szStringToCheck[m_nCurrentPosition]).toLatin1();
    
    DEA_Transition* transition = state->findTransitionForSymbol(currentSymbol);
    if(!transition)
    {// if there is no right transition
        QString text;
        text = tr("No Transition found for \'%symbol\' at state \'%statename\'")
                .replace("%symbol", QString(currentSymbol)).replace("%statename", state->name());
        m_bErrorOccured = true;
        text += "\n" + tr("\'%inputstring\' DENIED").replace("%inputstring", m_szStringToCheck);
        putMessage(text);
        m_pCurrentState->m_eResultIndicator = DEdit_GraphicalState::ResultDenied;
        // abort
        stop();
        updateResultLabel();
        return;
    }
    // if transition for current symbol was found
    DEA_State*      newState = transition->end();
    // print some info
    printTransitionFound(currentSymbol, state, newState);
    
    if(!newState)
    {
        putMessage("Internal pointer error...");
        return;
    }
    DEdit_GraphicalState* newCurrentState = m_pDeaWidget->findStateByName(newState->name());
    if(!newCurrentState)
    {
        putMessage("Internal pointer error...");
        return;
    }
    resetCurrentState(newCurrentState);
    resetLastTransition(m_pDeaWidget->graphicalTransitionForData(transition));
    m_szAcceptedSymbols += currentSymbol;
    // go to next symbol
    ++m_nCurrentPosition;
    
    updateResultLabel();
    m_pDeaWidget->update();
}


void DEdit_ExecDeaWidget::printTransitionFound(char symbol, DEA_State* from, DEA_State* to)
{
    if(!from || !to)
    {
        return;
    }
    QString transitionInfo = tr("Found transition for \'%symbol\' from \'%stateFrom\' to \'%stateTo\'");
    transitionInfo.replace("%symbol", QString(symbol));
    transitionInfo.replace("%stateFrom", from->name());
    transitionInfo.replace("%stateTo", to->name());
    putMessage(transitionInfo);
}

void DEdit_ExecDeaWidget::resetLastTransition(DEdit_GraphicalTransition* transition)
{
    
    // deselect old transition
    if(m_pLastTransition)
    {
        m_pLastTransition->m_bJustExecuted = false;
        m_pLastTransition->setExecutionProgress(-1.0);
        m_pLastTransition->setWasChanged();
    }
    // reset last transition
    m_pLastTransition = transition;
    // select new transition
    if(m_pLastTransition)
    {
        m_pLastTransition->m_bJustExecuted = true;
        m_pLastTransition->setExecutionProgress(0.0);
        m_pLastTransition->setWasChanged();
    }
    // reset execution progress
    m_cStateChangedTimestamp.start();
}

void DEdit_ExecDeaWidget::resetCurrentState(DEdit_GraphicalState* state)
{
    
    // deselect old currentState
    if(m_pCurrentState)
    {
        m_pCurrentState->m_bCurrentlyExecutedState = false;
    }
    // select new currentState
    if(state)
    {
        state->m_bCurrentlyExecutedState = true;
    }
    // set newCurrentState to m_pCurrentState
    m_pCurrentState = state;
}


void DEdit_ExecDeaWidget::updateResultLabel()
{
    QString text = "<font style=\"background-color: #8AE234;\">";
    text += QString(m_szAcceptedSymbols).replace("<", "&lt;").replace(" ", "&nbsp;");
    QString color = "#FCE94F";
    if(m_bErrorOccured)
    {
        color = "#EF2929";
    }
    text += "</font><font style=\"background-color: " + color + ";\">";
    text += m_szStringToCheck.right(m_szStringToCheck.size() - m_nCurrentPosition).replace("<", "&lt;").replace(" ", "&nbsp;");
    text += "</font>";
    lblResult->setText(text);
    
    QString currentResult = "<b><font color=\"";
    if(!m_bRunning && !m_bErrorOccured)
    {
        currentResult += "#4E9A06\">" + tr("ACCEPTED");
    }
    else if(m_bErrorOccured)
    {
        currentResult += "#CC0000\">" + tr("DENIED");
    }
    else
    {
        currentResult += "#EDD400\">" + tr("RUNNING");
    }
    currentResult += "</font></b>";
    lblCurrentResult->setText(currentResult);
}


void DEdit_ExecDeaWidget::setEditorLocked(bool locked)
{
    if(m_pDeaWidget)
    {
        m_pDeaWidget->setLocked(locked);
    }
    if(locked)
    {
        btnLocked->setIcon(m_cIconEditorLocked);
    }
    else
    {
        btnLocked->setIcon(m_cIconEditorUnlocked);
        removeAllResultIndicators();
    }
}

void DEdit_ExecDeaWidget::removeAllResultIndicators()
{
    if(m_pCurrentState)
    {
        m_pCurrentState->m_eResultIndicator = DEdit_GraphicalState::NoResult;
    }
}


void DEdit_ExecDeaWidget::execToNextBreakPoint()
{
    while(m_bRunning)
    {
        executeToNextState();
    }
}

void DEdit_ExecDeaWidget::tmrExecState_timout()
{
    if(m_bRunning)
    {
        executeToNextState();
    }
}


void DEdit_ExecDeaWidget::activateTimer(bool on)
{
    if(m_tmrExecNextState->isActive() == on)
    {
        // return if nothing would bechanged
        return;
    }
    if(m_tmrExecNextState->isActive())
    {
        m_tmrExecNextState->stop();
    }
    if(on && spinTimerInterval->value() != 0)
    {
        m_tmrExecNextState->start();
    }
    if(chkTimerControlled->isChecked()
       && spinTimerInterval->value() == 0)
    {
        execToNextBreakPoint();
    }
}

void DEdit_ExecDeaWidget::setTimerInterval(int interval)
{
    if(interval == 0)
    {
        m_tmrExecNextState->stop();
        m_nTransitionAnimationTime = 1000;
    }
    else
    {
        m_nTransitionAnimationTime = interval;
        m_tmrExecNextState->setInterval(interval);
        if(!m_tmrExecNextState->isActive()
            && chkTimerControlled->isChecked())
        {
            m_tmrExecNextState->start();
        }
    }
    if(chkTimerControlled->isChecked()
       && spinTimerInterval->value() == 0)
    {
        execToNextBreakPoint();
    }
}



void DEdit_ExecDeaWidget::resetExecutionProgressDot()
{
    if(!m_pDeaWidget || !m_pLastTransition)
    {
        return;
    }
    double progress = m_cStateChangedTimestamp.elapsed();
    progress /= (double)m_nTransitionAnimationTime;
    m_pLastTransition->setExecutionProgress(progress);
    m_pDeaWidget->update();
}

