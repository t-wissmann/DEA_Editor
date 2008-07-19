
#include "iconcatcher.h"
#include <QApplication>
#include <QPixmap>

QString IconCatcher::m_szIconDir = "../pic/";
bool IconCatcher::m_bWasInitialized = FALSE;

void IconCatcher::init()
{
    m_bWasInitialized = TRUE;
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

