#ifndef __ICONCATCHER_H_
#define __ICONCATCHER_H_

#include <QIcon>
#include <QString>

class IconCatcher
{
private:
    IconCatcher() {};
    ~IconCatcher() {};
    
    static bool m_bWasInitialized;
    
public:
    static void init();
    static bool wasInitialized() { return m_bWasInitialized; };
    static QIcon getIcon(QString name, QString extension, int size = 16);
    static QIcon getIcon(QString name, int size = 16);
    
    static QString m_szIconDir;
    
};



#endif
