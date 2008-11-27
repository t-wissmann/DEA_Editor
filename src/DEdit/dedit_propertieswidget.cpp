
#include "dedit_propertieswidget.h"

#include <DEA/dea.h>
#include <DEA/dea_state.h>
#include <DEA/dea_transition.h>

// widgets
#include <QLabel>
#include <QFrame>
#include <QLineEdit>
// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
// other qt-classes
#include <QStringList>
#include <QFont>

DEdit_PropertiesWidget::DEdit_PropertiesWidget(QWidget* parent)
    : QWidget(parent)
{
    // init members
    m_pDea = 0;
    // init gui
    allocateWidget();
    createLayouts();
    initWidgets();
    connectSlots();
    retranslateUi();
}

DEdit_PropertiesWidget::~DEdit_PropertiesWidget()
{
    
}

// init functions
void DEdit_PropertiesWidget::allocateWidget()
{
    lblName = new QLabel;
    txtName = new QLineEdit;
    lblDescription = new QLabel;
    txtDescription = new QLineEdit;
    
    // properties
    frmPropeties = new QFrame;
    lblStates = new QLabel;
    txtStates = new QLineEdit;
    lblTransitions = new QLabel;
    txtTransitions = new QLineEdit;
    lblAlphabet = new QLabel;
    txtAlphabet = new QLineEdit;
    lblStartState = new QLabel;
    txtStartState = new QLineEdit;
    lblFinalStates = new QLabel;
    txtFinalStates = new QLineEdit;
}

void DEdit_PropertiesWidget::createLayouts()
{
    layoutNameDescription = new QGridLayout;
    layoutNameDescription->setMargin(0);
    layoutNameDescription->addWidget(lblName, 0, 0);
    layoutNameDescription->addWidget(txtName, 0, 1);
    layoutNameDescription->addWidget(lblDescription, 1, 0);
    layoutNameDescription->addWidget(txtDescription, 1, 1);
    
    int row = 0;
    layoutProperties = new QGridLayout;
    layoutProperties->setMargin(0);
    layoutProperties->setSpacing(1);
    layoutProperties->addWidget(lblStates,      row,    0);
    layoutProperties->addWidget(txtStates,      row++,  1);
    layoutProperties->addWidget(lblAlphabet,    row,    0);
    layoutProperties->addWidget(txtAlphabet,    row++,  1);
    layoutProperties->addWidget(lblTransitions, row,    0);
    layoutProperties->addWidget(txtTransitions, row++,  1);
    layoutProperties->addWidget(lblStartState,  row,    0);
    layoutProperties->addWidget(txtStartState,  row++,  1);
    layoutProperties->addWidget(lblFinalStates, row,    0);
    layoutProperties->addWidget(txtFinalStates, row++,  1);
    frmPropeties->setLayout(layoutProperties);
    
    layoutParent = new QVBoxLayout;
    layoutParent->addLayout(layoutNameDescription);
    layoutParent->addWidget(frmPropeties);
    setLayout(layoutParent);
}

void DEdit_PropertiesWidget::connectSlots()
{
    
}

void DEdit_PropertiesWidget::initWidgets()
{
    initPropertyValueWidgets(txtStates);
    initPropertyValueWidgets(txtTransitions);
    initPropertyValueWidgets(txtAlphabet);
    initPropertyValueWidgets(txtStartState);
    initPropertyValueWidgets(txtFinalStates);
    
    initPropertyNameWidgets(lblStates);
    initPropertyNameWidgets(lblTransitions);
    initPropertyNameWidgets(lblAlphabet);
    initPropertyNameWidgets(lblStartState);
    initPropertyNameWidgets(lblFinalStates);
    
    frmPropeties->setFrameShape(QFrame::Box);
    frmPropeties->setFrameShadow(QFrame::Sunken);
    
    // currently disable name and description
    txtName->setVisible(FALSE);
    lblName->setVisible(FALSE);
    txtDescription->setVisible(FALSE);
    lblDescription->setVisible(FALSE);
    
}

void DEdit_PropertiesWidget::initPropertyValueWidgets(QLineEdit* widget)
{
    if(!widget)
    {
        return;
    }
    /*
    widget->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    widget->setFrameShape(QFrame::Box);
    widget->setFrameShadow(QFrame::Sunken);
    widget->setWordWrap(TRUE);
    */
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    widget->setReadOnly(TRUE);
}


void DEdit_PropertiesWidget::initPropertyNameWidgets(QLabel* widget)
{
    if(!widget)
    {
        return;
    }
    widget->setFrameShape(QFrame::Box);
    widget->setFrameShadow(QFrame::Sunken);
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QFont font = widget->font();
    font.setPointSize(13);
    widget->setFont(font);
}


void DEdit_PropertiesWidget::retranslateUi()
{
    lblName->setText(tr("Name:"));
    lblDescription->setText(tr("Description:"));
    
    lblStates->setToolTip(tr("States"));
    lblFinalStates->setToolTip(tr("Final States"));
    lblStartState->setToolTip(tr("Start State"));
    lblAlphabet->setToolTip(tr("Alphabet"));
    lblTransitions->setToolTip(tr("Transitions"));
    
    txtStates->setToolTip(lblStates->toolTip());
    txtFinalStates->setToolTip(lblFinalStates->toolTip());
    txtStartState->setToolTip(lblStartState->toolTip());
    txtAlphabet->setToolTip(lblAlphabet->toolTip());
    txtTransitions->setToolTip(lblTransitions->toolTip());
    
    lblAlphabet->setText(QString(QChar(0x03A3))); //  0x03A3 = sigma
    lblStates->setText("<i>Q</i>");
    lblTransitions->setText(QString(QChar(0x03B4))); // 0x03B4 = delta
    lblStartState->setText("<i>q0</i>");
    lblFinalStates->setText("<i>F</i>");
}




void DEdit_PropertiesWidget::setDea(DEA* dea)
{
    m_pDea = dea;
    refreshFromDea();
}

DEA* DEdit_PropertiesWidget::dea()
{
    return m_pDea;
}


void DEdit_PropertiesWidget::refreshFromDea()
{
    if(!m_pDea)
    {
        return;
    }
    QString startState = "";
    if(m_pDea->startState())
    {
        startState = m_pDea->startState()->name(); 
    }
    txtStartState->setText(startState);
    txtStartState->setCursorPosition(0);
    QString allStates = "";
    QString finalStates = "";
    DEA_State* currentState;
    DEA_State** stateBuf = m_pDea->stateBuf();
    if(stateBuf)
    {
        for(unsigned int i = 0; i < m_pDea->stateCount(); ++i)
        {
            currentState = stateBuf[i];
            if(currentState)
            {
                if(!allStates.isEmpty())
                {
                    allStates += ", ";
                }
                allStates += currentState->name();
                if(currentState->isFinalState())
                {
                    if(!finalStates.isEmpty())
                    {
                        finalStates += ", ";
                    }
                    finalStates += currentState->name();
                }
            }
        }
    }
    txtStates->setText(allStates);
    txtStates->setCursorPosition(0);
    txtFinalStates->setText(finalStates);
    txtFinalStates->setCursorPosition(0);
    
    txtAlphabet->setText(QString::fromLocal8Bit("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "0123456789,;.:-_#'+*~!\"§$%&/()=\\{[]}"));
    txtAlphabet->setCursorPosition(0);
    
    QString transitions = "";
    QString currentTransition;
    DEA_Transition** transitionBuf = m_pDea->transitionBuf();
    DEA_Transition* transition;
    if(transitionBuf)
    {
        for(unsigned int i = 0; i < m_pDea->transitionCount(); ++i)
        {
            transition = transitionBuf[i];
            if(!transition || !transition->start() || !transition->end())
            {
                continue;
            }
            currentTransition += "(";
            currentTransition += transition->start()->name();
            currentTransition += ", ";
            currentTransition += transition->inputSymbols();
            currentTransition += ", ";
            currentTransition += transition->end()->name();
            currentTransition += ")";
            if(transitions.isEmpty())
            {
                transitions += currentTransition;
            }else
            {
                transitions += ", " + currentTransition;
            }
        }
    }
    txtTransitions->setText(transitions);
    txtTransitions->setCursorPosition(0);
}




