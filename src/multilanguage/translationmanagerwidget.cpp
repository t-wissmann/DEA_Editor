
#include "translationmanagerwidget.h"

#include "translationmanager.h"


// widgets
#include <QListWidget>
#include <QLabel>
#include <QRadioButton>
// layouts
#include <QVBoxLayout>


QString TranslationManagerWidgetItem::toString() const
{
    return m_szEnglishName
                 + " " + TranslationManagerWidget::tr("(")
                 + m_szCustomName + TranslationManagerWidget::tr(")")
                 /*
                 // currently deactivated
                 + " " + TranslationManagerWidget::tr("by") + " " + m_szAuthor
                 */
                 ;
}


TranslationManagerWidget::TranslationManagerWidget()
{
    // init some members
    m_pTranslationManager = NULL;
    
    createGui();
    retranslateUi();
}

TranslationManagerWidget::~TranslationManagerWidget()
{
    
}

void TranslationManagerWidget::createGui()
{
    // allocate widgets
    m_radUseSystemLanguage = new QRadioButton;
    m_radUseCustomLanguage = new QRadioButton;
    m_lblLanguage = new QLabel;
    m_lblLanguage->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_listTranslations = new QListWidget;
    
    // create layouts
    m_layoutRow = new QHBoxLayout;
    m_layoutRow->setMargin(5);
    m_layoutRow->addWidget(m_lblLanguage);
    m_layoutRow->addWidget(m_listTranslations);
    
    m_layoutParent = new QVBoxLayout;
    m_layoutParent->addWidget(m_radUseSystemLanguage);
    m_layoutParent->addWidget(m_radUseCustomLanguage);
    m_layoutParent->addLayout(m_layoutRow);
    
    setLayout(m_layoutParent);
    
    // connect slots
    connect(m_radUseCustomLanguage, SIGNAL(toggled(bool)), m_lblLanguage, SLOT(setEnabled(bool)));
    connect(m_radUseCustomLanguage, SIGNAL(toggled(bool)), m_listTranslations, SLOT(setEnabled(bool)));
    // init widgets
    m_radUseCustomLanguage->setChecked(TRUE);
}

void TranslationManagerWidget::retranslateUi()
{
    m_radUseSystemLanguage->setText(tr("Use system's default language"));
    m_radUseCustomLanguage->setText(tr("Use a custom language"));
    m_lblLanguage->setText(tr("Language:"));
}


void TranslationManagerWidget::setTranslationManager(TranslationManager* pManager)
{
    m_pTranslationManager = pManager;
    if(!m_pTranslationManager)
    {
        return;
    }
    refresh();// refresh, so that all availible languages are in the list widget
    
    if(m_pTranslationManager->useSystemLanguage())
    {
        m_radUseSystemLanguage->setChecked(TRUE);
    }
    else
    {
        m_radUseCustomLanguage->setChecked(TRUE);
        m_listTranslations->setCurrentRow(
                        indexOfLanguage(
                     m_pTranslationManager->currentLanguage()));
    }
}


void TranslationManagerWidget::applyChanges()
{
    if(!m_pTranslationManager)
    {
        return;
    }
    
    if(m_radUseSystemLanguage->isChecked())
    {
        m_pTranslationManager->translateToSystemLanguage();
    }
    else
    {
        TranslationManagerWidgetItem* newTranslation = translationItemAt(m_listTranslations->currentRow());
        if(newTranslation)
        {
            m_pTranslationManager->translateTo(newTranslation->m_szName);
        }
    }
}

void TranslationManagerWidget::restoreDefaults()
{
    m_radUseSystemLanguage->setChecked(TRUE);
}


TranslationManagerWidgetItem* TranslationManagerWidget::translationItemAt(int index)
{
    if(index < 0 || index > m_TranslationItemList.size())
    {
        return NULL;
    }
    return &(m_TranslationItemList[index]);
}


void TranslationManagerWidget::refresh()
{
    if(!m_pTranslationManager)
    {
        return;
    }
    m_pTranslationManager->refreshTranslationList();
    refreshTranslationItemList();
    refreshTranslationListWidget();
}

void TranslationManagerWidget::refreshTranslationItemList()
{
    if(!m_pTranslationManager)
    {
        return;
    }
    QStringList languages = m_pTranslationManager->availableLanguages();
    
    m_TranslationItemList.clear(); // clear old list
    // now fill it again
    TranslationManagerWidgetItem newItem;
    for(int i = 0; i < languages.size(); ++i)
    {
        initTranslationItemByLanguageName(&newItem, languages[i]);
        m_TranslationItemList.append(newItem);
    }
}



void TranslationManagerWidget::initTranslationItemByLanguageName
        (TranslationManagerWidgetItem* pItem, QString szName)
{
    if(!pItem || szName.isEmpty())
    {
        return;
    }
    QString szFileName = "dea_editor_";
    szFileName += szName;
    QTranslator translator;
    translator.load(szFileName, TranslationManager::translationDirPath());
    pItem->m_szName = szName;
    pItem->m_szEnglishName = translator.translate("TranslationManagerWidget",
        "English name of your language", "This is the name of your translation's language, f.e. \"German\"");
    pItem->m_szCustomName = translator.translate("TranslationManagerWidget", "Custom name of your language",
        "this is the name of your translation's language in its own language, f.e. \"Deutsch\"");
    pItem->m_szAuthor = translator.translate("TranslationManagerWidget",
        "Author's Name", "here, you can put your name, f.e.\"Thorsten W.\" ;D");
}

void TranslationManagerWidget::refreshTranslationListWidget()
{
    m_listTranslations->clear();
    for(int i = 0; i < m_TranslationItemList.size(); ++i)
    {
        m_listTranslations->addItem(m_TranslationItemList[i].toString());
    }
}

int TranslationManagerWidget::indexOfLanguage(const QString& szLanguage)
{
    int nResultIndex;
    for(int i = 0; i < m_TranslationItemList.size(); ++i)
    {
        if(m_TranslationItemList[i].m_szName == szLanguage)
        {
            nResultIndex = i;
            break;
        }
    }
    return nResultIndex;
}


