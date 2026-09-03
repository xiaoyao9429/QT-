#include "animationwindow.h"
#include "ui_animationwindow.h"
#include <QPainter>
#include <QTimer>
AnimationWindow::AnimationWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AnimationWindow)
{
    ui->setupUi(this);
}

AnimationWindow::~AnimationWindow()
{
    delete ui;
}

void AnimationWindow::setBetImage(int score)
{
    if(score==1){
        m_image.load(":/images/score1.png");
    }
    else if(score==2){
        m_image.load(":/images/score2.png");
    }
    else if(score==3){
        m_image.load(":/images/score3.png");
    }

    update();
    // QTimer::singleShot(2000,this,[=](){
    //     this->hide();
    // });
}

void AnimationWindow::paintEvent(QPaintEvent *event)
{

    QPainter p(this);
    p.drawPixmap(0,0,m_image.width(),m_image.height(),m_image);
}
