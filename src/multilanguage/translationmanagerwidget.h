
#ifndef __TRANSLATIONMANAGERWIDGET_H_
#define __TRANSLATIONMANAGERWIDGET_H_

#include <QWidget>
#include <multilanguage/translateableobject.h>
#include <QString>
#include <QList>

class TranslationManager;

// widgets
class QRadioButton;
class QListWidget;
class QLabel;
// layouts
class QVBoxLayout;
class QHBoxLayout;

class TranslationManagerWidgetItem
{
public:
         // Description      // example for a german translation
    QString m_szName;        // "german"
    QString m_szEnglishName; // "German"
    QString m_szCustomName;  // "Deutsch"
    QString m_szAuthor;      // "Thorsten Wißmann"
    QString toString() const;
};


class TranslationManagerWidget
        : public QWidget,
          public TranslateableObject
{
    Q_OBJECT
public:
    TranslationManagerWidget();
    ~TranslationManagerWidget();
    
    void retranslateUi();
    void setTranslationManager(TranslationManager* pManager);
    TranslationManagerWidgetItem* translationItemAt(int index);
    void refreshTranslationItemList();
    void refreshTranslationListWidget();
    void initTranslationItemByLanguageName(TranslationManagerWidgetItem* pItem, QString szName);
    int  indexOfLanguage(const QString& szLanguage);
public slots:
    void applyChanges();
    void restoreDefaults();
    void refresh();
private:
    void createGui();
    // widgets
    QRadioButton* m_radUseSystemLanguage;
    QRadioButton* m_radUseCustomLanguage;
    QLabel*       m_lblLanguage;
    QListWidget*  m_listTranslations;
    // layouts
    QVBoxLayout*  m_layoutParent;
    QHBoxLayout*  m_layoutRow;
    // members
    QList<TranslationManagerWidgetItem> m_TranslationItemList;
    TranslationManager*                 m_pTranslationManager;
};


#endif


