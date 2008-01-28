
#include "translationmanager.h"

#include <multilanguage/translateableobject.h>
#include <QApplication>
#include <QDir>
#include <QLocale>


TranslationManager::TranslationManager()
{
    refreshTranslationList();
    m_bUseSystemLanguage = TRUE;
    m_szCurrentLanguage = systemLanguage();
}

TranslationManager::~TranslationManager()
{

}


QString TranslationManager::translationDirPath()
{
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("translations");
    return dir.absolutePath() + QDir::separator();
}


void TranslationManager::refreshTranslationList()
{
    QDir dir(translationDirPath());
    m_AvailableLanguageList = dir.entryList(
                QStringList() << "dea_editor_*.qm", // name filtesr
                QDir::Files, // filters
                QDir::Name // sorting by name
              );
    m_AvailableLanguageList.replaceInStrings("dea_editor_", "");
    m_AvailableLanguageList.replaceInStrings(".qm", "");
    m_AvailableLanguageList.removeAll(""); // remove empty entries
}



QString TranslationManager::systemLanguage()
{
    return QLocale::languageToString(QLocale::system().language()).toLower();
}


void TranslationManager::translateToSystemLanguage()
{
    translateTo(systemLanguage());
    m_bUseSystemLanguage = TRUE;
}

void TranslationManager::translateTo(QString szLanguage)
{
    QString dirpath = translationDirPath();
    QString filename = QString("dea_editor_") + szLanguage;
    
    QApplication::removeTranslator(&m_AppTranslator);
    m_AppTranslator.load(filename, dirpath);
    QApplication::installTranslator(&m_AppTranslator);
    
    TranslateableObject::retranslateAllObjects();
    // reset  members
    m_bUseSystemLanguage = FALSE;
    m_szCurrentLanguage = szLanguage;
}


bool TranslationManager::useSystemLanguage() const
{
    return m_bUseSystemLanguage;
}

QString TranslationManager::currentLanguage()
{
    return m_szCurrentLanguage;
}


QStringList TranslationManager::availableLanguages() const
{
    return m_AvailableLanguageList;
}

