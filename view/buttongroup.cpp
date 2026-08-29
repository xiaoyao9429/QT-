#include "buttongroup.h"
#include "ui_buttongroup.h"
#include "mybutton.h"
#include <QVector>


ButtonGroup::ButtonGroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ButtonGroup)
{
    ui->setupUi(this);
    initButtons();
}

ButtonGroup::~ButtonGroup()
{
    delete ui;
}

void ButtonGroup::initButtons()
{
    //开始游戏
    ui->start->setImage(":/images/start-1.png",":/images/start-3.png",":/images/start-2.png");
    //开局出牌
    ui->playCard->setImage(":/images/chupai_btn-1.png",":/images/chupai_btn-3.png",":/images/chupai_btn-2.png");
    //过程中出牌
    ui->playCard_2->setImage(":/images/chupai_btn-1.png",":/images/chupai_btn-3.png",":/images/chupai_btn-2.png");
    //不要
    ui->pass->setImage(":/images/pass_btn-1.png",":/images/pass_btn-3.png",":/images/pass_btn-2.png");
    //不抢
    ui->giveUp->setImage(":/images/buqiang-1.png",":/images/buqiang-3.png",":/images/buqiang-2.png");
    //1,2,3分
    ui->oneScore->setImage(":/images/1fen-1.png",":/images/1fen-3.png",":/images/1fen-2.png");
    ui->twoScore->setImage(":/images/2fen-1.png",":/images/2fen-3.png",":/images/2fen-2.png");
    ui->threeScore->setImage(":/images/3fen-1.png",":/images/3fen-3.png",":/images/3fen-2.png");

    //设置按钮大小
    QVector<MyButton*> buttons={ui->start,ui->playCard,ui->playCard_2,ui->pass,ui->giveUp,ui->oneScore,ui->twoScore,ui->threeScore};
    for(auto button:buttons)
    {
        button->setFixedSize(90,45);
    }

    connect(ui->start,&QPushButton::clicked,this,&ButtonGroup::startGame);
    connect(ui->playCard,&QPushButton::clicked,this,&ButtonGroup::playHand);
    connect(ui->playCard_2,&QPushButton::clicked,this,&ButtonGroup::playHand);
    connect(ui->pass,&QPushButton::clicked,this,&ButtonGroup::pass);
    connect(ui->giveUp,&QPushButton::clicked,this,[=](){emit betPoint(0);});
    connect(ui->oneScore,&QPushButton::clicked,this,[=](){emit betPoint(1);});
    connect(ui->twoScore,&QPushButton::clicked,this,[=](){emit betPoint(2);});
    connect(ui->threeScore,&QPushButton::clicked,this,[=](){emit betPoint(3);});

}
//只有用户会触发这个函数
void ButtonGroup::selectPage(Panel Type,int curBet)
{

    switch (Type) {
    case Panel::Start:
        ui->stackedWidget->setCurrentWidget(ui->startPage);
        break;
    case Panel::Empty:
        ui->stackedWidget->setCurrentWidget(ui->nullPage);
        break;
    case Panel::CallLord://只有这个case会用到curBet
        ui->stackedWidget->setCurrentWidget(ui->callLordPage);
        switch (curBet)
        {
        case 0://显示所有按钮
            
            break;
        case 1://显示2,3分按钮
            ui->oneScore->hide();
            break;

        case 2://显示3分按钮
            ui->oneScore->hide();
            ui->twoScore->hide();
            break;
        default:
            break;
        }

        break;
    case Panel::PassOrPlay:
        ui->stackedWidget->setCurrentWidget(ui->passOrPlayPage);
        break;
    case Panel::PlayCard:
        ui->stackedWidget->setCurrentWidget(ui->playCradPage);
        break;
    default:
        break;
    }

}
