
#include "dia_about.h"

#include <io/iconcatcher.h>
#include <QApplication>

#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QFont>
#include <QBoxLayout>
#include <QPixmap>
// layouts
#include <QHBoxLayout>
#include <QVBoxLayout>

Dia_About::Dia_About(QWidget* parent)
    : QDialog(parent)
{
    // close this dialog, if main window gets closed
    setAttribute(Qt::WA_QuitOnClose, false);
    allocateWidgets();
    createLayouts();
    connectSlots();
    retranslateUi();
    reloadIcons();
    resize(400, 240);
}

Dia_About::~Dia_About()
{
    
}


void Dia_About::allocateWidgets()
{
    btnClose = new QPushButton;
    lblHeader = new QLabel;
    lblVersion = new QLabel;
    lblIcon = new QLabel;
    lblIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // tabs
    lblTabAbout = new QLabel;
    lblTabAuthor = new QLabel;
    lblTabLicense = new QLabel;
    tabMain = new QTabWidget;
    
    
    // init labels
    lblTabAuthor->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lblTabAuthor->setIndent(3);
    lblTabAbout->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lblTabAbout->setIndent(3);
    lblTabAbout->setWordWrap(true);
    lblTabAbout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    lblTabLicense->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lblTabLicense->setIndent(3);
    // init font for labels
    QFont font = lblHeader->font();
    font.setPointSize(font.pointSize() + 1);
    font.setBold(true);
    lblHeader->setFont(font);
    font = lblVersion->font();
    font.setPointSize(font.pointSize() - 1);
    lblVersion->setFont(font);
}

void Dia_About::createLayouts()
{
    layoutBottom = new QHBoxLayout;
    layoutBottom->setMargin(0);
    layoutBottom->addStretch();
    layoutBottom->addWidget(btnClose);
    
    layoutHeader = new QBoxLayout(QBoxLayout::LeftToRight);
    layoutHeader->setMargin(0);
    layoutHeader->addWidget(lblHeader);
    layoutHeader->addWidget(lblVersion);
    
    layoutAbout = new QHBoxLayout;
    layoutAbout->setMargin(2);
    layoutAbout->addWidget(lblIcon);
    layoutAbout->addWidget(lblTabAbout);
    QWidget* foo = new QWidget;
    foo->setLayout(layoutAbout);
    // add tabs
    tabMain->addTab(foo, "about");
    tabMain->addTab(lblTabAuthor,  "author");
    tabMain->addTab(lblTabLicense, "license");
    
    layoutParent = new QVBoxLayout;
    layoutParent->setMargin(3);
    layoutParent->addLayout(layoutHeader);
    layoutParent->addWidget(tabMain);
    layoutParent->addLayout(layoutBottom);
    setLayout(layoutParent);
}

void Dia_About::connectSlots()
{
    connect(btnClose, SIGNAL(clicked()), this, SLOT(close()));
}

void Dia_About::retranslateUi()
{
    setWindowTitle(tr("About Dea Editor"));
    lblHeader->setText(tr("Dea Editor"));
    lblVersion->setText(tr("version: %version, compiled on: %date")
                                       .replace("%date", __DATE__)
                                       .replace("%version", qApp->applicationVersion()));
    QString about = "<p>";
    about += tr("Dea Editor - Create and simulate Finate State Machines")
            + "<br><br>(c) Thorsten Wi" + QChar(0x00DF) + "mann";
    about += "</p";
    lblTabAbout->setText(about);
    QString author = QString("Thorsten Wi") + QChar(0x00DF) + "mann";
    author += "<br>&nbsp;&nbsp;<a href=\"mailto:edu@thorsten-wissmann.de\">edu@thorsten-wissmann.de</a>";
    QString license = "<p>";
    license += tr("DEA Editor is under the GPLv3");
    license += "<br><br>" + tr("See %url for more information").replace("%url",
               "<a href=\"http://www.gnu.org/licenses/gpl.txt\">http://www.gnu.org/licenses/gpl.txt</a>");
    license += "</p>";
    lblTabLicense->setText(license);
    
    tabMain->setTabText(0, tr("About"));
    tabMain->setTabText(1, tr("Author"));
    tabMain->setTabText(2, tr("License"));
    lblTabAuthor->setText(author);
    btnClose->setText(tr("Close"));
}

void Dia_About::reloadIcons()
{
    lblIcon->setPixmap(IconCatcher::getIcon("dea_editor",128).pixmap(128, 128));
    btnClose->setIcon(IconCatcher::getIcon("fileclose"));
}


