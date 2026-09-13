#include "endpanel.h"
#include <QPainter>
EndPanel::EndPanel(bool isLord,bool isWin,QWidget *parent)
    : QWidget{parent}
{
    m_backImage.load(":/images/gameover.png");
    setFixedSize(m_backImage.size());

    m_title=new QLabel(this);
    m_continue=new QPushButton(this);
    m_continue->setFixedSize(231,48);

    m_scorePanel = new ScorePanel(this);
    m_scorePanel->setFixedSize(260, 160);

    if(isLord&&isWin){
        m_title->setPixmap(QPixmap(":/images/lord_win.png"));
    }

    else if (isLord&&!isWin){
        m_title->setPixmap(QPixmap(":/images/lord_fail.png"));
    }


    else if(!isLord&&isWin){
        m_title->setPixmap(QPixmap(":/images/farmer_win.png"));

    }

    else if(!isLord&&!isWin){
         m_title->setPixmap(QPixmap(":/images/farmer_fail.png"));
    }

    connect(m_continue,&QPushButton::clicked,this,&EndPanel::continueGame);

    m_title->move(125,125);
    m_scorePanel->move(75,230);
    m_continue->move(84,429);
    m_scorePanel->setFontStyle(18, QColor(255, 0, 0));  // 红色
    QString style = R"(
        QPushButton{border-image: url(:/images/button_normal.png)}
        QPushButton:hover{border-image: url(:/images/button_hover.png)}
        QPushButton:pressed{border-image: url(:/images/button_pressed.png)}
    )";

    m_continue->setStyleSheet(style);
}

void EndPanel::setPlayers(Player* left, Player* user, Player* right)
{
    m_scorePanel->setPlayers(left, user, right);
}

void EndPanel::setScore(Player *player, int score)
{
    m_scorePanel->setScore(player,score);
}



void EndPanel::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.drawPixmap(rect(),m_backImage);

}
