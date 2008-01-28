
#ifndef __COMMANDBUTTONDND_H_
#define __COMMANDBUTTONDND_H_

#include <QPushButton>
#include <QString>
#include <QPoint>
#include <QPixmap>

class QMouseEvent;

class CommandButtonDND : public QPushButton {
    Q_OBJECT
public:
    CommandButtonDND();
    CommandButtonDND(QString mimeType, QString command);
    ~CommandButtonDND();
    
    void setCommand(QString cmd);
    QString command() const;
    
    void setDNDPixmap(QPixmap pixmap);
    QPixmap DNDPixmap() const;
    
    void setMimeType(QString type);
    QString mimeType() const;
    
protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
private:
    
    void startDrag();
    QPoint m_cDragStartPosition;
    QPixmap m_cDNDPixmap;
    
    QString m_szCommand;
    QString m_szMimeType;
    
};

#endif
