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
    m_curMovePos = 0;

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

    connect(m_gameControl,&GameControl::playerStatusChanged,this,&GameMainWindow::onPlayerStatusChanged);
    connect(m_gameControl,&GameControl::notifyGrabLordBet,this,&GameMainWindow::onGrabLordBet);
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
    connect(ui->buttonGroup,&ButtonGroup::betPoint,this,[=](int bet){
        m_gameControl->getUserPlayer()->grabLordBet(bet);
    });//抢地主
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
        //设置底牌图片
        {   CardList last3Card=m_gameControl->initialCards().toCardList();//剩下三张底牌
            for(int i=0;i<last3Card.size();++i){
                QPixmap front=m_cardMap[last3Card[i]]->image();
                m_last3Cards[i]->setImage(front);

            }

            //开始叫地主
            m_gameControl->startCallLord();

        }


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
           m_contextMap[m_playerList[i]].isFront=false;  // 机器人显示牌背
       }
       else
       {
           m_contextMap[m_playerList[i]].isFront=true;   // 用户显示牌面
       }
   }

   //重置玩家卡牌
   m_gameControl->reset();

   //重置发牌动画位移
   m_curMovePos = 0;

   //启动定时器
   m_timer->start(15);

   //背景音乐;



}

void GameMainWindow::onDispatchCard()
{
    //base牌移动到curMovePos*step个像素
    //当前玩家
    Player* currentPlayer=m_gameControl->getCurrentPlayer();
    if (currentPlayer == nullptr) {
        m_timer->stop();
        return;
    }

    // 每一张新牌开始动画前，先把动画牌移动到发牌区并显示
    if (m_curMovePos == 0) {
        m_moveCard->move(m_baseCardPos);
        m_moveCard->show();
    }

    //移动扑克牌到玩家位置
    m_curMovePos+=15;

    //绘制发牌动画
    drawDispatchCard(currentPlayer,m_curMovePos);

    if(m_curMovePos>=100)//给当前玩家发一张牌
    {
        Cards& deck = m_gameControl->initialCards();
        // 只剩 3 张时停止发牌，作为底牌保留，不再分发
        if (deck.cardCount() <= 3) {
            m_timer->stop();
            gameStatusProcess(GameControl::CallingLord);//切换状态到叫地主阶段
            return;
        }
        Card card = deck.takeRandomCard();
        currentPlayer->addCard(card);
        dispatchCardHandle(currentPlayer,currentPlayer->cards());
        m_gameControl->setCurrentPlayer(currentPlayer->nextPlayer());//切换到下家
        m_curMovePos=0;
    }

}

void GameMainWindow::drawDispatchCard(Player* player,int curPos)
{
    int index = m_playerList.indexOf(player);
    if (index < 0 || index >= m_playerList.size()) {
        return;
    }

    // 分别取出三个玩家的放牌区域
    QRect leftRect   = m_contextMap[m_playerList[0]].cradRect;
    QRect userRect   = m_contextMap[m_playerList[1]].cradRect;
    QRect rightRect  = m_contextMap[m_playerList[2]].cradRect;

    // 直接按 curPos/100 的进度插值，避免 (距离/100)*curPos 的整数除法截断误差
    int distLeftX  = m_baseCardPos.x() - leftRect.right();                 // 左侧机器人: X方向距离
    int distUserY  = userRect.top() - m_baseCardPos.y() - m_basePanel->height(); // 用户: Y方向距离
    int distRightX = rightRect.left() - m_baseCardPos.x() - m_basePanel->width(); // 右侧机器人: X方向距离

    //计算move牌每次移动到的坐标（先乘后除，减少整数除法误差）
    QPoint pos[] = {
        QPoint(m_baseCardPos.x() - curPos * distLeftX / 100,  m_baseCardPos.y()),                   // 左侧机器人
        QPoint(m_baseCardPos.x(),                          m_baseCardPos.y() + curPos * distUserY / 100),  // 用户
        QPoint(m_baseCardPos.x() + curPos * distRightX / 100, m_baseCardPos.y())                    // 右侧机器人
    };

    //移动
    m_moveCard->move(pos[index]);

    //如果移动的距离已超过，则隐藏（发牌动画结束，下一张开始时在onDispatchCard中重新show）
    if (curPos >= 100) {
        m_moveCard->hide();
    }
}

void GameMainWindow::dispatchCardHandle(Player *player,  const Cards &cards)
{
    CardList list=cards.toCardList();
    for(int i=0;i<list.size();++i){
        CardPanel* panel=m_cardMap[list[i]];
        panel->setOwner(player);
    }

    //在主窗口中显示
    updatePlayerCards(player);

}

void GameMainWindow::updatePlayerCards(Player *player)
{
    Cards cards=player->cards();
    CardList list=cards.toCardList();
    //取出放牌区域
    QRect cardsRect=m_contextMap[player].cradRect;
    int cradSpace=20;
    for(int i=0;i<list.size();++i){//效率有点低
        CardPanel* panel =m_cardMap[list[i]];
        panel->setFrontSide(m_contextMap[player].isFront);
        //水平或垂直展示
        if(m_contextMap[player].align==CardAlign::horizontal)//水平
        {
            int leftx=cardsRect.left()+(cardsRect.width()-m_cardSize.width()-(list.size()-1)*cradSpace)/2;
            int y=cardsRect.top()+(cardsRect.height()-m_cardSize.height())/2;
            panel->move(leftx+i*cradSpace,y);
        }

        else{//垂直
            int topy=cardsRect.top()+(cardsRect.height()-m_cardSize.height()-(list.size()-1)*cradSpace)/2;
            int x=cardsRect.left()+(cardsRect.width()-m_cardSize.width())/2;
            panel->move(x,topy+i*cradSpace);
        }

        panel->show();
        panel->raise();
    }

}

void GameMainWindow::onPlayerStatusChanged(Player *player, GameControl::PlayerStatus status)
{
    switch (status) {
    case GameControl::ThinkingForCallLord:
        if(player==m_gameControl->getUserPlayer()){
            ui->buttonGroup->selectPage(ButtonGroup::Panel::CallLord);
        }

        break;

    case GameControl::ThinkingForPlayHand:

        break;

    case GameControl::Winning:
        break;
    default:
        break;
    }
}

void GameMainWindow::onGrabLordBet(Player *player, int bet, bool isFrist)
{
    //显示抢地主的提示信息
    PlayerContext context=m_contextMap[player];
    if(bet==0){//不抢
        context.info->setPixmap(QPixmap(":/images/buqinag.png"));
    }

    else if(isFrist){
        context.info->setPixmap(QPixmap(":/images/jiaodizhu.png"));
    }


    else{
        context.info->setPixmap(QPixmap(":/images/qiangdizhu.png"));
    }

    context.info->show();

    //显示叫地主的分数

    //背景音乐
}



void GameMainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawPixmap(this->rect(),m_bkImage);

}
