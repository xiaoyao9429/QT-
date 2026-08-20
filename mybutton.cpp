#include "mybutton.h"
#include <QPainter>
#include <qevent.h>
MyButton::MyButton(QWidget *parent): QPushButton(parent) {}

void MyButton::setImage(QString normal, QString hover, QString pressed)
{
    this->m_normal=normal;
    this->m_hover=hover;
    this->m_pressed=pressed;
    m_pixmap.load(normal);
}

void MyButton::mousePressEvent(QMouseEvent *ev)
{

    QPushButton::mousePressEvent(ev);
    if(ev->button()==Qt::LeftButton)
    {
        m_pixmap.load(m_pressed);
        update();
    }
    
    
}

void MyButton::mouseReleaseEvent(QMouseEvent *ev)
{
    QPushButton::mouseReleaseEvent(ev);
    if(ev->button()==Qt::LeftButton)
    {
        m_pixmap.load(m_normal);
        update();
    }
    
}

void MyButton::enterEvent(QEnterEvent *ev)
{
    QPushButton::enterEvent(ev);

    m_pixmap.load(m_hover);
    update();
}

void MyButton::leaveEvent(QEvent *ev)
{
    QPushButton::leaveEvent(ev);
    m_pixmap.load(m_normal);
    update();
}

void MyButton::paintEvent(QPaintEvent *ev)
{

    QPainter painter(this);
    painter.drawPixmap(this->rect(),m_pixmap);
}
