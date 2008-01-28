

#ifndef __TRANSLATIONMANAGER_H_
#define __TRANSLATIONMANAGER_H_

#include <QObject>
#include <QStringList>
#include <QTranslator>

class TranslationManager : public QObject
{
    Q_OBJECT
public:
    TranslationManager();
    ~TranslationManager();
    
    void translateTo(QString szLanguage);
    void translateToSystemLanguage();
    static QString translationDirPath();
    static QString systemLanguage();
    
    bool useSystemLanguage() const;
    QString currentLanguage();
    
    QStringList availableLanguages() const;
public slots:
    void refreshTranslationList();
    
private:
    
    bool        m_bUseSystemLanguage;
    QString     m_szCurrentLanguage;
    QTranslator m_AppTranslator;
    QStringList m_AvailableLanguageList;
};


#endif
