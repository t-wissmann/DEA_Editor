
#ifndef __TRANSLATEABLEOBJECT_H_
#define __TRANSLATEABLEOBJECT_H_

#include <QList>

class TranslateableObject
{
public:
    TranslateableObject();
    virtual ~TranslateableObject();
    
    virtual void retranslateUi(); // reimplement this function
    
    static void retranslateAllObjects();
private:
    static QList<TranslateableObject*> m_ObjectList;
};


#endif

