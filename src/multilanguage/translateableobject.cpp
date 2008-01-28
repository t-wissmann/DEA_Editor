
#include "translateableobject.h"

QList<TranslateableObject*> TranslateableObject::m_ObjectList;

TranslateableObject::TranslateableObject()
{
    // append this object to list
    TranslateableObject::m_ObjectList.append(this);
}


TranslateableObject::~TranslateableObject()
{
    // remove it from list again
    TranslateableObject::m_ObjectList.removeAll(this);
}

void TranslateableObject::retranslateUi()
{
    // foo....
}


void TranslateableObject::retranslateAllObjects()
{
    TranslateableObject* m_pCurrent;
    for(int i = 0; i < m_ObjectList.size(); ++i)
    {
        m_pCurrent = m_ObjectList[i];
        if(m_pCurrent)
        {
            m_pCurrent->retranslateUi();
        }
    }
}

