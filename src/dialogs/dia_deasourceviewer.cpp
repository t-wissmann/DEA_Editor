
#include "dia_deasourceviewer.h"
#include <io/xmlparser.h>
#include <DEdit/dedit_widget.h>

#include <QPushButton>
#include <QTextEdit>

#include <QHBoxLayout>
#include <QVBoxLayout>


Dia_DeaSourceViewer::Dia_DeaSourceViewer(QWidget* parent)
    : QDialog(parent)
{
    m_pDeaToView = NULL;
    // close this dialog, if main window gets closed
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    createGui();
    retranslateUi();
    resize(600, 360);
}

Dia_DeaSourceViewer::~Dia_DeaSourceViewer()
{
    
}


void Dia_DeaSourceViewer::createGui()
{
    // allocate widgets
    btnRefresh = new QPushButton;
    txtSource = new QTextEdit;
    txtSource->setReadOnly(TRUE);
    txtSource->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    txtSource->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    // create layouts
    layoutToolButtons = new QHBoxLayout;
    layoutToolButtons->setMargin(0);
    layoutToolButtons->addWidget(btnRefresh);
    layoutToolButtons->addStretch();
    
    layoutParent = new QVBoxLayout;
    layoutParent->setMargin(2);
    layoutParent->addLayout(layoutToolButtons);
    layoutParent->addWidget(txtSource);
    setLayout(layoutParent);
    
    // connect slots
    connect(btnRefresh, SIGNAL(clicked()), this, SLOT(refresh()));
}


void Dia_DeaSourceViewer::retranslateUi()
{
    btnRefresh->setText(tr("Refresh"));
    setWindowTitle(tr("DEA - Source Code"));
}

void Dia_DeaSourceViewer::refresh()
{
    if(!m_pDeaToView)
    {
        return;
    }
    xmlObject tree;
    m_pDeaToView->writeDeaToFile(&tree);
    
    // write xml-tree to buf
    char* szBuf;
    unsigned long nBufLength = 1;
    int nReturnValue;
    
    szBuf = (char*) malloc(nBufLength * sizeof(char));
    EditLengthOfBuf (&szBuf, &nBufLength, nBufLength +4);
    szBuf[0] = '\0';
    if ((nReturnValue = WriteObjectToBuf (&tree, &szBuf, &nBufLength)) != 0)
    {
        /// ERROR
        txtSource->setText("INTERNAL ERROR: please contact edu@thorsten-wissmann.de");
    }else
    {
        txtSource->setText(szBuf);
    }
    free (szBuf);
    // write text to widget
}


void Dia_DeaSourceViewer::setDeaToView(DEdit_Widget* dw)
{
    m_pDeaToView = dw;
    refresh();
}

DEdit_Widget* Dia_DeaSourceViewer::dea()
{
    return m_pDeaToView;
}



