
#include "dedit_widget.h"
#include "dedit_history.h"
#include <io/xmlparser.h>

DEdit_History::DEdit_History(DEdit_Widget* pEditorWidget)
{
    m_nMaxHistorySize = 5;
    m_pEditorWidget = pEditorWidget;
    m_nCurrentPosition = 0;
}

DEdit_History::~DEdit_History()
{
    clear();
}



int DEdit_History::maxHistorySize() const
{
    return m_nMaxHistorySize;
}

void DEdit_History::setMaxHistorySize(int size)
{
    if(size < 0)
    {
        size = 0;
    }
    m_nMaxHistorySize = size;
    xmlObject* currentObject;
    while(m_nMaxHistorySize < m_HistoryList.size())
    {
        currentObject = m_HistoryList.last();
        if(currentObject)
        {
            delete currentObject;
        }
        m_HistoryList.removeLast();
    }
    if(m_nCurrentPosition >= size)
    {
        m_nCurrentPosition = size;
    }
}


void DEdit_History::setEditorWidget(DEdit_Widget* widget)
{
    m_pEditorWidget = widget;
}


void DEdit_History::clear()
{
    //qDebug("clearing history...");
    xmlObject* currentObject;
    for(int i = 0; i < m_HistoryList.size(); ++i)
    {
        currentObject = m_HistoryList[i];
        if(currentObject)
        {
            delete currentObject;
        }
    }
    m_HistoryList.clear();
}

DEdit_Widget* DEdit_History::editorWidget()
{
    return m_pEditorWidget;
}



bool DEdit_History::isUndoPossible() const
{
    //qDebug("undo possible = %d", (m_nCurrentPosition < (m_HistoryList.size()-1)));
    return (m_nCurrentPosition < (m_HistoryList.size()-1));
}

bool DEdit_History::isRedoPossible() const
{
    //qDebug("redo possible = %d", (m_nCurrentPosition > 0));
    return (m_nCurrentPosition > 0);
}


void DEdit_History::saveCurrentStateToHistory()
{
    if(!m_pEditorWidget || m_nMaxHistorySize <= 0)
    {
        return;
    }
    // delete items before current position
    for(int i = m_nCurrentPosition; i > 0; --i)
    {
        removeItemAt(i);
    }
    m_nCurrentPosition = 0;
    
    // init new item
    xmlObject* pCurrentState = new xmlObject;
    m_pEditorWidget->writeDeaToFile(pCurrentState);
    m_HistoryList.prepend(pCurrentState);
    removeItemAt(m_nMaxHistorySize); // remove items, if there are to much
    //qDebug("history size = %d", m_HistoryList.size());
}


void DEdit_History::removeItemAt(int index)
{
    if(index < 0 || index >= m_HistoryList.size())
    {
        return;
    }
    if(m_HistoryList[index])
    {
        delete m_HistoryList[index];
    }
    m_HistoryList.removeAt(index);
}


void DEdit_History::undo()
{
    if(!isUndoPossible())
    {
        return;
    }
    //qDebug("undoing..");
    m_pEditorWidget->createDeaFromFile(m_HistoryList[m_nCurrentPosition+1]);
    
    // go to next item
    m_nCurrentPosition++;
}

void DEdit_History::redo()
{
    if(!isRedoPossible())
    {
        return;
    }
    m_pEditorWidget->createDeaFromFile(m_HistoryList[m_nCurrentPosition-1]);
    
    // go to item before
    m_nCurrentPosition--;
}


