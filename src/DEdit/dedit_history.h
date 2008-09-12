
#ifndef __DEDIT_HISTORY_H_
#define __DEDIT_HISTORY_H_

#include <QList>

class xmlObject;

class DEdit_Widget;

class DEdit_History
{
public:
    DEdit_History(DEdit_Widget* pEditorWidget = 0);
    ~DEdit_History();
    
    int maxHistorySize() const;
    void setMaxHistorySize(int size);
    
    void setEditorWidget(DEdit_Widget* widget);
    DEdit_Widget* editorWidget();
    
    void clear();
    
    void undo();
    void redo();
    
    bool isUndoPossible() const;
    bool isRedoPossible() const;
    
    void saveCurrentStateToHistory();
    
private:
    void removeItemAt(int index);
    
    int m_nMaxHistorySize;
    int m_nCurrentPosition; // 0 = current
    QList<xmlObject*> m_HistoryList;
    
    DEdit_Widget* m_pEditorWidget;
};

#endif

