
#include "commandbuttondnd.h"
#include <QMouseEvent>
#include <QApplication>
#include <QMimeData>

CommandButtonDND::CommandButtonDND()
    : QPushButton()
{
    m_szMimeType = "text/plain";
}


CommandButtonDND::CommandButtonDND(QString mimeType, QString command)
    : QPushButton()
{
    
    m_szMimeType = mimeType;
    m_szCommand = command;
}


CommandButtonDND::~CommandButtonDND()
{
    
}


void CommandButtonDND::setCommand(QString cmd)
{
    m_szCommand = cmd;
}

QString CommandButtonDND::command() const
{
    return m_szCommand;
}


void CommandButtonDND::mousePressEvent(QMouseEvent* event)
{
    if (event && event->button() == Qt::LeftButton)
    {
        m_cDragStartPosition = event->pos();
    }
    QPushButton::mousePressEvent(event);
}

void CommandButtonDND::mouseMoveEvent(QMouseEvent* event)
{
    if (event && (event->buttons() & Qt::LeftButton))
    {
        int distance = (event->pos() - m_cDragStartPosition).manhattanLength();
        if (distance >= QApplication::startDragDistance())
        {
            startDrag();
        }
    }
}


void CommandButtonDND::setDNDPixmap(QPixmap pixmap)
{
    m_cDNDPixmap = pixmap;
}

QPixmap CommandButtonDND::DNDPixmap() const
{
    return m_cDNDPixmap;
}

void CommandButtonDND::startDrag()
{
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(m_szMimeType, m_szCommand.toAscii());
    QDrag* drag = new QDrag(this);
#ifndef Q_WS_WIN
    // on windows, the image is to large for drag'n'drop pixmap -.-
    drag->setPixmap(m_cDNDPixmap);
    drag->setHotSpot(QPoint(m_cDNDPixmap.width(), m_cDNDPixmap.height())/2);
#endif
    
    drag->setMimeData(mimeData);
    drag->start(Qt::MoveAction);
    clearFocus(); // remove focus from widget after drag and drop
}


void CommandButtonDND::setMimeType(QString type)
{
    m_szMimeType = type;
}

QString CommandButtonDND::mimeType() const
{
    return m_szMimeType;
}




