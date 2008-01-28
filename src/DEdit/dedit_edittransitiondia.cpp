
#include "dedit_edittransitiondia.h"

#include <DEdit/dedit_widget.h>
#include <DEdit/dedit_graphicaltransition.h>
#include <DEA/dea_state.h>
#include <io/iconcatcher.h>

#include <QMessageBox>
// widgets
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QToolButton>
#include <widgets/commandbuttondnd.h>

// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>


DEdit_EditTransitionDia::DEdit_EditTransitionDia(DEdit_Widget* parent)
    : QDialog(parent)
{
    m_pParent = parent;
    m_pTransitionToEdit = NULL;
    
    // create gui
    allocateWidgets();
    createLayouts();
    connectSlots();
    retranslateUi();
    reloadIcons();
}

DEdit_EditTransitionDia::~DEdit_EditTransitionDia()
{
    
}


void DEdit_EditTransitionDia::allocateWidgets()
{
    boxBottom = new QDialogButtonBox;
    btnOk     = boxBottom->addButton("ok", QDialogButtonBox::AcceptRole);
    btnApply  = boxBottom->addButton("apply", QDialogButtonBox::RejectRole);
    btnCancel = boxBottom->addButton("cancel", QDialogButtonBox::RejectRole);
    // symbols
    lblSymbols = new QLabel;
    txtSymbols = new QLineEdit;
    btnResetCurve = new QToolButton;
    // curve
    lblCurve  = new QLabel;
    slidCurve = new QSlider(Qt::Horizontal);
    spinCurve = new QSpinBox;
    slidCurve->setRange(-10000, 10000);
    spinCurve->setRange(-10000, 10000);
    
    // commadn buttons
    btnAllUpperLetters = new CommandButtonDND("text/plain", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    btnAllLowerLetters = new CommandButtonDND("text/plain", "abcdefghijklmnopqrstuvwxyz");
    btnAllLetters = new CommandButtonDND("text/plain",
                         btnAllLowerLetters->command() + btnAllUpperLetters->command());
    btnAllDigits = new CommandButtonDND("text/plain", "0123456789");
    
}

void DEdit_EditTransitionDia::createLayouts()
{
    layoutSymbols = new QHBoxLayout;
    layoutSymbols->setMargin(0);
    layoutSymbols->addWidget(lblSymbols);
    layoutSymbols->addWidget(txtSymbols);
    
    layoutCommands = new QGridLayout;
    layoutCommands->setMargin(0);
    layoutCommands->addWidget(btnAllLetters, 0, 0);
    layoutCommands->addWidget(btnAllDigits, 0, 1);
    layoutCommands->addWidget(btnAllLowerLetters, 1, 0);
    layoutCommands->addWidget(btnAllUpperLetters, 1, 1);
    
    layoutCurve = new QHBoxLayout;
    layoutCurve->setMargin(0);
    layoutCurve->addWidget(lblCurve);
    layoutCurve->addWidget(slidCurve);
    layoutCurve->addWidget(spinCurve);
    layoutCurve->addWidget(btnResetCurve);
    // new version:
    // curve can be configured by dragging:
    bool bCurveEditVisible = FALSE;
    lblCurve->setVisible(bCurveEditVisible);
    slidCurve->setVisible(bCurveEditVisible);
    spinCurve->setVisible(bCurveEditVisible);
    btnResetCurve->setVisible(bCurveEditVisible);
    
    
    layoutParent = new QVBoxLayout;
    layoutParent->addLayout(layoutSymbols);
    layoutParent->addLayout(layoutCommands);
    layoutParent->addLayout(layoutCurve);
    layoutParent->addWidget(boxBottom);
    setLayout(layoutParent);
}

void DEdit_EditTransitionDia::connectSlots()
{
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btnOk, SIGNAL(clicked()), this, SLOT(ok_clicked()));
    connect(btnApply, SIGNAL(clicked()), this, SLOT(applyChanges()));
    connect(btnResetCurve, SIGNAL(clicked()), this, SLOT(resetCurveToNull()));
    connect(slidCurve, SIGNAL(valueChanged(int)), spinCurve, SLOT(setValue(int)));
    connect(spinCurve, SIGNAL(valueChanged(int)), slidCurve, SLOT(setValue(int)));
    
    // command buttons
    connect(btnAllLetters, SIGNAL(clicked()), this, SLOT(setToAllLetters()));
    connect(btnAllDigits, SIGNAL(clicked()), this, SLOT(setToAllDigits()));
    connect(btnAllLowerLetters, SIGNAL(clicked()), this, SLOT(setToAllLowerLetters()));
    connect(btnAllUpperLetters, SIGNAL(clicked()), this, SLOT(setToAllUpperLetters()));
}

void DEdit_EditTransitionDia::retranslateUi()
{
    setWindowTitle(tr("Edit Transition"));
    btnOk->setText(tr("OK"));
    btnApply->setText(tr("Apply"));
    btnCancel->setText(tr("Cancel"));
    lblSymbols->setText(tr("Symbols to accept:"));
    lblCurve->setText(tr("Curve:"));
    btnResetCurve->setToolTip(tr("Reset Curve to 0"));
    
    btnAllLetters->setText(tr("All letters"));
    btnAllDigits->setText(tr("All Digits"));
    btnAllUpperLetters->setText(tr("All upper letters"));
    btnAllLowerLetters->setText(tr("All lower letters"));
}

void DEdit_EditTransitionDia::reloadIcons()
{
    btnOk->setIcon(IconCatcher::getIcon("button_ok"));
    btnApply->setIcon(IconCatcher::getIcon("apply"));
    btnCancel->setIcon(IconCatcher::getIcon("button_cancel"));
    btnResetCurve->setIcon(IconCatcher::getIcon("undo"));
}


void DEdit_EditTransitionDia::ok_clicked()
{
    if(applyChanges())
    {
        accept();
    }
}

bool DEdit_EditTransitionDia::applyChanges()
{
    if(!m_pTransitionToEdit)
    {
        return FALSE;
    }
    QString symbols = txtSymbols->text();
    QString newSymbols = "";
    // remove all chars that are twice in text;
    while(0 < symbols.length())
    {
        QChar currentChar = symbols[0];
        symbols.remove(currentChar);
        newSymbols += currentChar;
    }
    symbols = newSymbols;
    txtSymbols->setText(symbols);
    
    if(m_pParent)
    {
        // now compare symbols to symbols of other transitions
        // starting at the same state
        QList<DEdit_GraphicalTransition*> transitions
                = m_pParent->graphicalTransitionsWithStart(m_pTransitionToEdit->m_pStart);
        DEdit_GraphicalTransition* currentTransition;
        QString msg = "";
        QString doubledSymbols;
        QString currenTransitionSymbols;
        QString infoLine;
        // check all transitions for double symbols
        for(int i = 0; i < transitions.size(); ++i)
        {
            currentTransition = transitions[i];
            if(!currentTransition)
            {
                continue;
            }
            if(currentTransition == m_pTransitionToEdit)
            {
                // skip transition to edit
                continue;
            }
            doubledSymbols = "";
            currenTransitionSymbols = currentTransition->symbols();
            // get all symbols, that exist twice
            for(int ch = 0; ch < symbols.length(); ++ch)
            {
                if(currenTransitionSymbols.contains(symbols[ch]))
                {
                    doubledSymbols += symbols[ch];
                }
            }
            if(!doubledSymbols.isEmpty())
            {
                infoLine = tr("\'%symbols\' in transition from '\%start\' to \'%end\'") + "\n";
                infoLine.replace("%symbols", doubledSymbols);
                if(currentTransition->hasValidPointers())
                {
                    infoLine.replace("%start", currentTransition->m_pStart->m_pData->name());
                    infoLine.replace("%end",   currentTransition->m_pEnd->m_pData->name());
                }
                msg += infoLine;
            }
        }
        if(!m_pParent->allowNonDeterministic()
         && !msg.isEmpty())
        {// if there are some multible used symbols -> ERROR
            QString text = tr("Couldn't apply symbols, because some "
                    "symbols were already used in other transitions "
                    "starting at the same state \'%statename\'");
            if(m_pTransitionToEdit->hasValidPointers())
            {
                text.replace("%statename", m_pTransitionToEdit->m_pStart->m_pData->name());
            }
            QMessageBox msgbox(QMessageBox::Critical, tr("Error - DEA Editor"), text,
                               QMessageBox::Ok, this);
            msgbox.setDetailedText(msg);
            msgbox.exec();
            return FALSE;
        }
    }
    
    m_pTransitionToEdit->setSymbols(symbols);
    m_pTransitionToEdit->m_nCurve = slidCurve->value();
    m_pTransitionToEdit->setWasChanged();
    if(parentWidget())
    {
        parentWidget()->update();
    }
    return TRUE;
}


void DEdit_EditTransitionDia::setToAllLetters()
{
    txtSymbols->setText("abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

void DEdit_EditTransitionDia::setToAllUpperLetters()
{
    txtSymbols->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

void DEdit_EditTransitionDia::setToAllLowerLetters()
{
    txtSymbols->setText("abcdefghijklmnopqrstuvwxyz");
}

void DEdit_EditTransitionDia::setToAllDigits()
{
    txtSymbols->setText("0123456789");
}



void DEdit_EditTransitionDia::setTransitionToEdit(DEdit_GraphicalTransition* transition)
{
    m_pTransitionToEdit = transition;
    if(!m_pTransitionToEdit)
    {
        return;
    }
    txtSymbols->setText(m_pTransitionToEdit->symbols());
    slidCurve->setValue(m_pTransitionToEdit->m_nCurve);
    
}

DEdit_GraphicalTransition* DEdit_EditTransitionDia::transitionToEdit()
{
    return m_pTransitionToEdit;
}

void DEdit_EditTransitionDia::resetCurveToNull()
{
    slidCurve->setValue(0);
}


