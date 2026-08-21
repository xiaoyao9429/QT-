#include "gamemainwindow.h"
#include "./ui_gamemainwindow.h"
#include "buttongroup.h"
#include "robotplayer.h"
#include "userplayer.h"
#include <QPainter>
#include <QRandomGenerator>
GameMainWindow::GameMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameMainWindow)
{

    ui->setupUi(this);
    ui->scorePanel->setFixedSize(200, 140);
    resize(1200,800);
    ui->buttonGroup->selectPage(ButtonGroup::Panel::CallLord);

    //主窗口标题
    setWindowTitle("小张斗地主");
    //随机获取主窗口背景
    int random = QRandomGenerator::global()->bounded(1, 11);
    QString path= QString(":/images/background-%1.png").arg(random);
    m_bkImage.load(path);
    //实例化游戏控制类
    GameControlInit();

    //玩家得分
    ui->scorePanel->setPlayers(m_playerList[0],m_playerList[1],m_playerList[2]);
    ui->scorePanel->setScore(m_playerList[0],0);
    ui->scorePanel->setScore(m_playerList[1],0);
    ui->scorePanel->setScore(m_playerList[2],0);

    //初始化扑克牌

    //玩家在窗口中的位置
}

GameMainWindow::~GameMainWindow()
{
    delete ui;
}

void GameMainWindow::GameControlInit()
{
    m_gameControl=new GameControl(this);
    RobotPlayer* lrobot=m_gameControl->getLeftRobot();
    RobotPlayer* rrobot=m_gameControl->getRightRobot();
    UserPlayer* user=m_gameControl->getUserPlayer();
    m_playerList << lrobot << user<< rrobot;
}

void GameMainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawPixmap(this->rect(),m_bkImage);

}
