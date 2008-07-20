
#include "dedit_execdeawidget.h"
#include "dedit_widget.h"
#include <io/iconcatcher.h>

#include <DEdit/dedit_graphicalstate.h>
#include <DEdit/dedit_graphicaltransition.h>
#include <DEA/dea_state.h>
#include <DEA/dea_transition.h>

// widgtes
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>

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
    m_bRunning = FALSE;
    m_bPaused = FALSE;
    m_bErrorOccured = FALSE;
    m_nCurrentPosition = 0;
    // init gui
    allocateWidgets();
    createLayouts();
    connectSlots();
    resetWidgetProperties();
    
    retranslateUi();
    reloadIcons();
}

DEdit_ExecDeaWidget::~DEdit_ExecDeaWidget()
{
    
}


void DEdit_ExecDeaWidget::allocateWidgets()
{
    btnStart = new QPushButton;
    btnStop = new QPushButton;
    btnPause  = new QPushButton;
    btnPause->setVisible(FALSE);
    btnSingleStep = new QPushButton;
    // output / console
    txtOutput = new QTextEdit;
    txtOutput->setReadOnly(TRUE);
    txtOutput->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    txtOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    txtOutput->setVisible(FALSE);
    btnShowHideOutput = new QPushButton;
    btnShowHideOutput->setCheckable(TRUE);
    btnShowHideOutput->setChecked(FALSE);
    btnClearOutput = new QPushButton;
    btnClearOutput->setVisible(FALSE);
    // input string
    lblInputString = new QLabel;
    txtInputString = new QLineEdit;
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
    
    layoutInputAndResult = new QGridLayout;
    layoutInputAndResult->setMargin(0);
    layoutInputAndResult->addWidget(lblInputString, 0, 0);
    layoutInputAndResult->addWidget(txtInputString, 0, 1, 1, 2); // colspan = 2
    layoutInputAndResult->addWidget(lblResultLabel, 1, 0);
    layoutInputAndResult->addWidget(lblResult, 1, 1);
    layoutInputAndResult->addWidget(lblCurrentResult, 1, 2);
    
    layoutOutputToolButtons = new QHBoxLayout;
    layoutOutputToolButtons->setMargin(0);
    layoutOutputToolButtons->addWidget(btnShowHideOutput);
    layoutOutputToolButtons->addWidget(btnClearOutput);
    layoutOutputToolButtons->addStretch();
    
    
    layoutParent = new QVBoxLayout;
    layoutParent->setMargin(0);
    layoutParent->addLayout(layoutStartStop);
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
    
    // for output / console
    connect(btnShowHideOutput, SIGNAL(toggled(bool)), txtOutput, SLOT(setVisible(bool)));
    connect(btnShowHideOutput, SIGNAL(toggled(bool)), btnClearOutput, SLOT(setVisible(bool)));
    connect(btnClearOutput, SIGNAL(clicked()), txtOutput, SLOT(clear()));
}


void DEdit_ExecDeaWidget::retranslateUi()
{
    btnStart->setText(tr("Start"));
    btnStop->setText(tr("Stop"));
    btnPause->setText(tr("Pause"));
    btnSingleStep->setText(tr("Single Step"));
    lblInputString->setText(tr("Text to recognize:"));
    btnShowHideOutput->setText(tr("Show log"));
    btnClearOutput->setText(tr("Clear log"));
    lblResultLabel->setText(tr("Result:"));
}

void DEdit_ExecDeaWidget::reloadIcons()
{
    btnStart->setIcon(IconCatcher::getIcon("ktstart_all"));
    btnStop->setIcon(IconCatcher::getIcon("ktstop_all"));
    btnPause->setIcon(IconCatcher::getIcon("player_pause"));
    btnShowHideOutput->setIcon(IconCatcher::getIcon("konsole"));
    btnClearOutput->setIcon(IconCatcher::getIcon("editclear"));
    btnSingleStep->setIcon(IconCatcher::getIcon("1rightarrow"));
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
    if(m_pDeaWidget)
    {
        m_pDeaWidget->setLocked(m_bRunning);
    }
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
    putMessage(tr("Starting DEA..."));
    m_bRunning = TRUE;
    m_bPaused = FALSE;
    m_bErrorOccured = FALSE;
    m_pLastTransition = NULL;
    resetWidgetProperties();
    
    m_szStringToCheck = txtInputString->text();
    m_szAcceptedSymbols = "";
    m_nCurrentPosition = 0;
    m_pCurrentState = m_pDeaWidget->graphicalStartState();
    if(!m_pCurrentState)
    {
        m_bErrorOccured = TRUE;
        putMessage(tr("Error when starting: No start state defined!"));
        stop();
        updateResultLabel();
        return;
    }
    // select currentState
    m_pCurrentState->m_bCurrentlyExecutedState = TRUE;
    updateResultLabel();
    m_pDeaWidget->update();
}

void DEdit_ExecDeaWidget::stop()
{
    m_bRunning = FALSE;
    m_bPaused = FALSE;
    
    if(m_pLastTransition)
    {
        // deselect old transition
        m_pLastTransition->m_bJustExecuted = FALSE;
    }
    if(m_pCurrentState)
    {
        // deselect currentState
        m_pCurrentState->m_bCurrentlyExecutedState = FALSE;
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
    m_bPaused = TRUE;
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
        }
        else
        {
            QString text;
            
            text = tr("End of string has been reached but current state \'%statename\' is NOT a final state.").replace("%statename", state->name());
            text += "\n" + tr("\'%inputstring\' DENIED").replace("%inputstring", m_szStringToCheck);
            m_bErrorOccured = TRUE;
            // add space so that in lblResult, we can see a red rect at end of string
            m_szStringToCheck += "  ";
            putMessage(text);
        }
        stop();
        updateResultLabel();
        return;
    }
    char currentSymbol = m_szStringToCheck[m_nCurrentPosition].toAscii();
    
    DEA_Transition* transition = state->findTransitionForSymbol(currentSymbol);
    if(!transition)
    {// if there is no right transition
        QString text;
        text = tr("No Transition found for \'%symbol\' at state \'%statename\'")
                .replace("%symbol", QString(currentSymbol)).replace("%statename", state->name());
        m_bErrorOccured = TRUE;
        text += "\n" + tr("\'%inputstring\' DENIED").replace("%inputstring", m_szStringToCheck);
        putMessage(text);
        stop();
        updateResultLabel();
        return;
    }
    // if transition for current symbol was found
    DEA_State*      newState = transition->end();
    // print some info
    QString transitionInfo = tr("Found transition for \'%symbol\' from \'%stateFrom\' to \'%stateTo\'");
    transitionInfo.replace("%symbol", QString(currentSymbol));
    transitionInfo.replace("%stateFrom", state->name());
    transitionInfo.replace("%stateTo", newState->name());
    putMessage(transitionInfo);
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
    // deselect old transition
    if(m_pLastTransition)
    {
        m_pLastTransition->m_bJustExecuted = FALSE;
    }
    m_pLastTransition = m_pDeaWidget->graphicalTransitionForData(transition);
    // select new transition
    if(m_pLastTransition)
    {
        m_pLastTransition->m_bJustExecuted = TRUE;
    }
    
    // deselect old currentState
    m_pCurrentState->m_bCurrentlyExecutedState = FALSE;
    // select new currentState
    newCurrentState->m_bCurrentlyExecutedState = TRUE;
    // set newCurrentState to m_pCurrentState
    m_pCurrentState = newCurrentState;
    m_szAcceptedSymbols += currentSymbol;
    // go to next symbol
    ++m_nCurrentPosition;
    
    updateResultLabel();
    m_pDeaWidget->update();
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
        currentResult += "#4E9A06\">ACCEPTED";
    }
    else if(m_bErrorOccured)
    {
        currentResult += "#CC0000\">DENIED";
    }
    else
    {
        currentResult += "#EDD400\">RUNNING";
    }
    currentResult += "</font></b>";
    lblCurrentResult->setText(currentResult);
}

