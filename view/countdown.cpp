#include "countdown.h"
#include <QPainter>
CountDown::CountDown(QWidget *parent)
    : QWidget{parent}
{



    setFixedSize(70,70);
    m_timer=new QTimer;


    connect(m_timer,&QTimer::timeout,this,[=](){
        m_count--;
        if(m_count<10&&m_count>0){
             m_clock.load(":/images/clock.png");
            m_number.load(":/images/number.png");
            m_number=m_number.copy(m_count*(30+10),0,30,42).scaled(20,30);
            update();
        }

        if(m_count==5){
            emit notMuchTimer();
        }

        if(m_count<0){
            m_timer->stop();
            m_count=15;
            m_clock=QPixmap();
            m_number=QPixmap();
            update();
            emit timeOut();
        }

    });



}

void CountDown::showCountDown()
{
    m_count=15;
    m_timer->start(1000);
}

void CountDown::stopCountDown()
{
    m_timer->stop();
    m_count=15;
    m_clock=QPixmap();
    m_number=QPixmap();
    update();

}

void CountDown::paintEvent(QPaintEvent *event)
{

    QPainter p(this);

    p.drawPixmap(rect(),m_clock);
    p.drawPixmap((width()-m_number.width())/2-1,(height()-m_number.height())/2,m_number.width(),m_number.height(),m_number);
}
