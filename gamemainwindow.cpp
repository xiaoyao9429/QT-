#include "gamemainwindow.h"
#include "./ui_gamemainwindow.h"
#include "buttongroup.h"
#include "robotplayer.h"
#include "userplayer.h"
#include <QPainter>
#include <QRandomGenerator>
#include "cards.h"
#include "QPoint"
GameMainWindow::GameMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameMainWindow)
{

    ui->setupUi(this);
    ui->scorePanel->setFixedSize(200, 140);
    resize(1200,800);
    ui->buttonGroup->selectPage(ButtonGroup::Panel::Start);
    connectButtonGroup();

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
    initCardMap();

    //玩家在窗口的上下文环境
    initPlayerContext();
     //初始化游戏场景
    initGameScene();
    //实例化定时器
    m_timer=new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GameMainWindow::onDispatchCard);

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

void GameMainWindow::initCardMap()
{
    //加载大图
    QPixmap pixmap(":/images/card.png");
    //单张牌尺寸：图集 13 列 × 5 行
    m_cardSize.setWidth(pixmap.width() / 13);
    m_cardSize.setHeight(pixmap.height() / 5);

    //卡牌背面图：第 5 行(row=4)、第 3 列(col=2)
    //TODO: 如背面图位置不对，需根据实际 card.png 调整行列号
    m_cardBackImage = pixmap.copy(m_cardSize.width() * 2, m_cardSize.height() * 4,
                                  m_cardSize.width(), m_cardSize.height());

    m_cardMap.clear();
    //按图集顺序填充：每行一种花色，行内点数 3→2
    // 行0:♦ 行1:♣ 行2:♥ 行3:♠
    for (int suit = static_cast<int>(CardSuit::Diamond);
         suit <= static_cast<int>(CardSuit::Spade); ++suit) {
        int row = suit - 1;   // Diamond=1 → row 0
        for (int point = static_cast<int>(CardPoint::Card_3);
             point <= static_cast<int>(CardPoint::Card_2); ++point) {
            int col = point - 1;  // Card_3=1 → col 0

            QPixmap temp = pixmap.copy(m_cardSize.width() * col,
                                        m_cardSize.height() * row,
                                        m_cardSize.width(),
                                        m_cardSize.height());
            Card card(static_cast<CardPoint>(point),
                      static_cast<CardSuit>(suit));
            auto cp = new CardPanel(this);
            cp->hide();
            cp->setCard(card);
            cp->setBackImage(m_cardBackImage);
            cp->setImage(temp);
            m_cardMap.insert(card, cp);
        }
    }

    //大小王：第 5 行(row=4)
    //小王 col 0
    {
        QPixmap temp = pixmap.copy(0, m_cardSize.height() * 4,
                                    m_cardSize.width(), m_cardSize.height());
        Card card(CardPoint::Card_SJ, CardSuit::Suit_Begin);
        auto cp = new CardPanel(this);
        cp->hide();
        cp->setCard(card);
        cp->setBackImage(m_cardBackImage);
        cp->setImage(temp);
        m_cardMap.insert(card, cp);
    }
    //大王 col 1
    {
        QPixmap temp = pixmap.copy(m_cardSize.width(), m_cardSize.height() * 4,
                                    m_cardSize.width(), m_cardSize.height());
        Card card(CardPoint::Card_BJ, CardSuit::Suit_Begin);
        auto cp = new CardPanel(this);
        cp->hide();
        cp->setCard(card);
        cp->setBackImage(m_cardBackImage);
        cp->setImage(temp);
        m_cardMap.insert(card, cp);
    }
}

void GameMainWindow::connectButtonGroup()
{
    connect(ui->buttonGroup,&ButtonGroup::startGame,this,[=](){//开始游戏
        //隐藏按钮组
        ui->buttonGroup->selectPage(ButtonGroup::Panel::Empty);
        //更新游戏状态
        gameStatusProcess(GameControl::DispatchCard);


    });
    connect(ui->buttonGroup,&ButtonGroup::betPoint,this,[=](){});//抢地主
    connect(ui->buttonGroup,&ButtonGroup::pass,this,[=](){});//不要
    connect(ui->buttonGroup,&ButtonGroup::playHand,this,[=](){}); //出牌
}

void GameMainWindow::initPlayerContext()
{
    //玩家放置扑克牌的位置
    QRect cardsRect[] ={
        QRect(90,130,100,height()-200),//左侧机器人
        QRect(250,rect().bottom()-120,width()-500,100),//用户
        QRect(rect().right()-190,130,100,height()-200)//右侧机器人
    };
    //玩家出牌的区域
    QRect playerRect[] ={
        QRect(260,150,100,100),//左侧机器人
        QRect(150,rect().bottom()-290,width()-300,100),//用户
        QRect(rect().right()-360,150,100,100)//右侧机器人
    };
    //玩家头像显式的位置
    QPoint roleImgPos[]={
        QPoint(cardsRect[0].left()-80,cardsRect[0].height()/2+20),//左侧机器人
        QPoint(cardsRect[1].right()-10,cardsRect[1].bottom()-10),//用户
        QPoint(cardsRect[2].right()+10,cardsRect[2].height()+20),//右侧机器人
    };

    for(int i=0;i<m_playerList.size();++i)//左机器人，用户，右机器人
    {
        PlayerContext temp;
        if(i!=1){
            temp.align=CardAlign::vertical;//垂直
              temp.isFront=false;//背面
        }
        else{
            temp.align=CardAlign::horizontal;//水平
              temp.isFront=true;//背面
        }
        temp.playHandRect=playerRect[i];//出牌区域
        temp.cradRect=cardsRect[i];//放牌区域
        temp.info=new QLabel(this);//每个玩家的操作的提示信息
        temp.info->resize(160,98);
        //放到出牌区域的中间
        QRect rect=playerRect[i];
        QPoint point=QPoint(rect.left()+(rect.width()-temp.info->width())/2,rect.top()+(rect.height()-temp.info->height())/2);
        temp.info->move(point);
        temp.info->hide();
        //玩家头像
        temp.roleImg=new QLabel(this);
        temp.roleImg->resize(84,120);
        temp.roleImg->hide();
        temp.roleImg->move(roleImgPos[i]);

        //填充
        m_contextMap.insert(m_playerList[i],temp);

    }
}

void GameMainWindow::initGameScene()
{
    //发牌区的牌
    m_basePanel=new CardPanel(this);
    m_basePanel->setImage(m_cardBackImage);
    m_basePanel->setBackImage(m_cardBackImage);
    //发牌过程中移动效果的扑克牌
    m_moveCard=new CardPanel(this);
    m_moveCard->setImage(m_cardBackImage);
    m_moveCard->setBackImage(m_cardBackImage);
    //三张底牌
    for(int  i=0;i<3;++i){
        CardPanel* panel=new CardPanel(this);
        panel->setImage(m_cardBackImage);
        panel->setBackImage(m_cardBackImage);
        m_last3Cards.push_back(panel);
    }

    //base牌的位置
    m_baseCardPos=QPoint((width()-m_cardSize.width())/2,(height()-m_cardSize.height())/2-100);
    m_basePanel->move(m_baseCardPos);
    //m_basePanel->hide();
    m_moveCard->move(m_baseCardPos);
    //m_moveCard->hide();

    //三张底牌的位置
    int base=(width()-3*m_cardSize.width()-2*10)/2;
    for(int i=0;i<m_last3Cards.size();++i){
        m_last3Cards[i]->move(base+i*(m_cardSize.width()+10),20);
        m_last3Cards[i]->hide();
    }

}

void GameMainWindow::gameStatusProcess(GameControl::GameStatus status)
{
    m_gameStatus=status;
    switch (status) {
    case GameControl::GameStatus::CallingLord:

        break;
    case GameControl::GameStatus::DispatchCard:
        dispatchCards();
        break;

    case GameControl::GameStatus::PlayingHand:
    default:
        break;
    }
}

void GameMainWindow::dispatchCards()
{
   // m_gameControl->dispatchCards();


   //刷新卡牌属性  //这些重置操作我觉得应该放在再来一局按钮的槽函数中，现在这样太耦合了
   for(auto f:m_cardMap){
       f->setSelected(false);
       f->setFrontSide(true);
       f->hide();
   }

   //隐藏底牌
   for(auto f:m_last3Cards){
       f->hide();
   }

   //重置玩家窗口上下文
   for(int i=0;i<m_playerList.size();++i)//左机器人，用户，右机器人
   {
       m_contextMap[m_playerList[i]].lastCard.clear();
       m_contextMap[m_playerList[i]].info->hide();
       m_contextMap[m_playerList[i]].roleImg->hide();
       if(i!=1){
           m_contextMap[m_playerList[i]].isFront=true;
       }
       else
       {
           m_contextMap[m_playerList[i]].isFront=false;
       }
   }

   //重置玩家卡牌
   m_gameControl->reset();

   //启动定时器
   m_timer->start(10);

   //背景音乐;



}

void GameMainWindow::onDispatchCard()
{
    //base牌移动到curMovePos*step个像素
    static int curMovePos=0;
    //当前玩家
    Player* currentPlayer=m_gameControl->getCurrentPlayer();
    
    //移动扑克牌到玩家位置
    curMovePos+=15;

    //绘制发牌动画
    drawDispatchCard(currentPlayer,curMovePos);

    if(curMovePos>=100)//给当前玩家发一张牌
    {
        Card card=m_gameControl->initialCards().takeRandomCard();
        currentPlayer->addCard(card);
        m_gameControl->setCurrentPlayer(currentPlayer->nextPlayer());//切换到下家
        curMovePos=0;
    }

    if(m_gameControl->initCardsCount()<=3 ){//停止发牌
        m_timer->stop();//停止定时器
        gameStatusProcess(GameControl::CallingLord);//切换状态到叫地主阶段
    }

}

void GameMainWindow::drawDispatchCard(Player* player,int curPos)
{

    //得到每个玩家的放牌区域
    QRect rect=m_contextMap[player].cradRect;
    //每个玩家的单元步长step,curMovePos代表格子，step代表每个格子的像素距离
    int step[]={
        (m_baseCardPos.x()-rect.right())/100,//左侧机器人
        (rect.top()-m_baseCardPos.y()-m_basePanel->height())/100,//用户
        (rect.left()-m_baseCardPos.x()-m_basePanel->width())/100,//右侧机器人
        
    };

    //计算move牌每次移动到的坐标
    QPoint pos[]={
        QPoint(m_baseCardPos.x()-curPos*step[0],m_baseCardPos.y()),//左侧机器人
        QPoint(m_baseCardPos.x(),m_baseCardPos.y()+curPos*step[1]),//用户
        QPoint(m_baseCardPos.x()+curPos*step[2],m_baseCardPos.y()),//右侧机器人
    };

    //移动
    int index=m_playerList.indexOf(player);
    m_moveCard->move(pos[index]);

    if(curPos==0){
        m_moveCard->show();
    }

    //如果移动的距离已超过，则隐藏
    if(curPos>=100){
        m_moveCard->hide();
    }


}



void GameMainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawPixmap(this->rect(),m_bkImage);

}
