
#include "iconcatcher.h"
#include <QApplication>
#include <QPixmap>

QString IconCatcher::m_szIconDir = "../pic/";
bool IconCatcher::m_bWasInitialized = false;

void IconCatcher::init()
{
    m_bWasInitialized = true;
    m_szIconDir = QApplication::applicationDirPath() + "/../pic/";
}


QIcon IconCatcher::getIcon(QString name, QString extension, int size)
{
    return QIcon(QPixmap(m_szIconDir + name + QString::number(size) + extension));
}

QIcon IconCatcher::getIcon(QString name, int size)
{
    return getIcon(name, ".png", size);
}

QIcon IconCatcher::getMenuBarToolBarIcon(QString name)
{
    QString extension = ".png";
    QIcon icon;
    // 16 x 16
    icon.addFile(m_szIconDir + name + "16" + extension, QSize(16, 16));
    // 22 x 22
    icon.addFile(m_szIconDir + name + "22" + extension, QSize(22, 22));
    return icon;
}
