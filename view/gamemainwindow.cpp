#include "gamemainwindow.h"
#include "./ui_gamemainwindow.h"
#include "buttongroup.h"
#include "robotplayer.h"
#include "userplayer.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QRubberBand>
#include "cards.h"
#include "playhand.h"
#include "QPoint"
#include "endpanel.h"
#include "bgmcontroller.h"
#include <QPropertyAnimation>
GameMainWindow::GameMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameMainWindow)
{

    ui->setupUi(this);
    ui->scorePanel->setFixedSize(200, 140);
    resize(1200,800);
    ui->buttonGroup->selectPage(ButtonGroup::Panel::Start,0);
    connectButtonGroup();

    //主窗口标题
    setWindowTitle("小张斗地主");
    //随机获取主窗口背景
    int random = QRandomGenerator::global()->bounded(1, 11);
    QString path= QString(":/images/background-%1.png").arg(random);
    m_bkImage.load(path);
    //实例化游戏控制类
    GameControlInit();

    //动画窗口类
    m_animationWindow=new AnimationWindow(this);

    //玩家得分
    ui->scorePanel->setPlayers(m_playerList[0],m_playerList[1],m_playerList[2]);
    ui->scorePanel->setScore(m_playerList[0],0);
    ui->scorePanel->setScore(m_playerList[1],0);
    ui->scorePanel->setScore(m_playerList[2],0);


    //初始化扑克牌
    initCardMap();

    //初始化闹钟
    initCountDown();


    //玩家在窗口的上下文环境
    initPlayerContext();
     //初始化游戏场景
    initGameScene();
    //实例化发牌动画控制器
    m_animator = new DealAnimator(this);
    m_animator->setMoveCard(m_moveCard);
    m_animator->setBasePos(m_baseCardPos);
    //设置三个玩家的发牌目标位置
    QRect leftRect  = m_contextMap[m_playerList[0]].cradRect;
    QRect userRect  = m_contextMap[m_playerList[1]].cradRect;
    QRect rightRect = m_contextMap[m_playerList[2]].cradRect;
    m_animator->setTargetPos(m_playerList[0], QPoint(leftRect.right(), m_baseCardPos.y()));
    m_animator->setTargetPos(m_playerList[1], QPoint(m_baseCardPos.x(), userRect.top() - m_cardSize.height()));
    m_animator->setTargetPos(m_playerList[2], QPoint(rightRect.left() - m_cardSize.width(), m_baseCardPos.y()));
    connect(m_animator, &DealAnimator::cardArrived, this, &GameMainWindow::onCardArrived);

    //启动欢迎背景音乐（单例，后续各事件点通过 BGMController::instance() 控制）
    BGMController::instance()->playBgm(BGMController::Scene::Welcome);

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
    connect(m_gameControl,&GameControl::grabLordBetDecided,this,&GameMainWindow::onGrabLordBet);
    connect(m_gameControl,&GameControl::notifyGameStatusChanged,this,&GameMainWindow::gameStatusProcess);
    //出牌阶段相关信号
    connect(m_gameControl,&GameControl::notifyPlayCards,this,&GameMainWindow::onPlayCards);
    connect(m_gameControl,&GameControl::notifyPass,this,&GameMainWindow::onPass);
    connect(m_gameControl,&GameControl::notifyLordConfirmed,this,&GameMainWindow::onLordConfirmed);
    connect(m_gameControl,&GameControl::notifyGameOver,this,&GameMainWindow::onGameOver);
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

    //所有牌面板的点击信号统一连到主窗口（选中逻辑由主窗口处理）
    for (auto cp : m_cardMap) {
        connect(cp, &CardPanel::cardClicked, this, &GameMainWindow::onCardClicked);
    }
}

void GameMainWindow::connectButtonGroup()
{
    connect(ui->buttonGroup,&ButtonGroup::startGame,this,[=](){//开始游戏
        //隐藏按钮组
        ui->buttonGroup->selectPage(ButtonGroup::Panel::Empty,0);
        //更新游戏状态
        gameStatusProcess(GameControl::DispatchCard);


    });
    connect(ui->buttonGroup,&ButtonGroup::betPoint,this,[=](int bet){
        // 用户点击按钮：直接提交给 GameControl 的统一入口，不再走 Player 中转
        m_gameControl->playerBet(m_gameControl->getUserPlayer(), bet);
    });//抢地主
    connect(ui->buttonGroup,&ButtonGroup::pass,this,&GameMainWindow::onUserPass);//不要
    connect(ui->buttonGroup,&ButtonGroup::playHand,this,&GameMainWindow::onUserPlayHand); //出牌
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
        QPoint(cardsRect[1].right()-120,cardsRect[1].top()-10),//用户
        QPoint(cardsRect[2].right(),cardsRect[2].height()/2+20),//右侧机器人
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
    //设置游戏状态
    m_gameStatus=status;
    switch (status) {
    case GameControl::GameStatus::CallingLord:
        //设置底牌图片
        {   CardList last3Card=m_gameControl->initialCards().toCardList();//剩下三张底牌
            for(int i=0;i<last3Card.size();++i){
                QPixmap front=m_cardMap[last3Card[i]]->image();
                m_last3Cards[i]->setImage(front);
            }
            // 注意：不在此处调 startCallLord()！
            // startCallLord() 内部会 emit notifyGameStatusChanged → 再次触发本函数，导致无限递归。
            // startCallLord() 由 onCardArrived 直接调用即可。
        }
        break;
    case GameControl::GameStatus::DispatchCard:
        dispatchCards();//发牌结束后调用gameControl->starCallLoard
        break;

    case GameControl::GameStatus::PlayingHand:
        preparePlayingHand();

        break;
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

   //启动发牌动画，从当前玩家开始
   m_animator->dealToPlayer(m_gameControl->getCurrentPlayer());

   //音频：新一局开始（含再来一局）恢复常规背景乐 + 发牌音效
   BGMController::instance()->playBgm(BGMController::Scene::Normal);
   BGMController::instance()->playEffect(QStringLiteral("Special_Dispatch"));
}

void GameMainWindow::onCardArrived(Player* player)
{
    // DealAnimator 动画完成回调：执行发牌业务逻辑
    Cards& deck = m_gameControl->initialCards();
    // 只剩 3 张时停止发牌，作为底牌保留
    if (deck.cardCount() <= 3) {
        m_animator->stop();

        //隐藏base牌
        m_basePanel->hide();

        // 直接调 startCallLord()，它会 emit notifyGameStatusChanged(CallingLord)
        // → gameStatusProcess(CallingLord) 自动设置底牌图片
        m_gameControl->startCallLord();
        return;
    }
    // 从牌堆取一张发给当前玩家
    Card card = deck.takeRandomCard();
    player->addCard(card);
    //在主窗口中绘制玩家手中的牌
    dispatchCardHandle(player, player->cards());
    // 切换到下家，继续动画
    Player* next = player->nextPlayer();
    m_gameControl->setCurrentPlayer(next);
    m_animator->dealToPlayer(next);
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

    int stx=0;
    int sty=0;

    if(player==m_gameControl->getUserPlayer()){
        m_cardsRect=QRect();
        m_cardRectMap.clear();
    }


    Cards cards=player->cards();
    CardList list=cards.toCardList();
    //取出放牌区域
    QRect cardsRect=m_contextMap[player].cradRect;
    int cradSpace=20;
    for(int i=0;i<list.size();++i){//效率有点低
        CardPanel* panel =m_cardMap[list[i]];
        //手牌归属随显示同步（底牌进地主手牌、亮牌等场景下，面板owner可能滞后或为空）
        panel->setOwner(player);
        panel->setFrontSide(m_contextMap[player].isFront);
        //水平或垂直展示
        if(m_contextMap[player].align==CardAlign::horizontal)//水平
        {


            int leftx=cardsRect.left()+(cardsRect.width()-m_cardSize.width()-(list.size()-1)*cradSpace)/2;
            int y=cardsRect.top()+(cardsRect.height()-m_cardSize.height())/2;
            stx=leftx;
            sty=y;
            panel->move(leftx+i*cradSpace,y);
            if(i==list.size()-1){
                m_cardRectMap.insert(panel,QRect(leftx+i*cradSpace,y,m_cardSize.width(),m_cardSize.height()));
            }

            else{
                m_cardRectMap.insert(panel,QRect(leftx+i*cradSpace,y,cradSpace,m_cardSize.height()));
            }

        }

        else{//垂直
            int topy=cardsRect.top()+(cardsRect.height()-m_cardSize.height()-(list.size()-1)*cradSpace)/2;
            int x=cardsRect.left()+(cardsRect.width()-m_cardSize.width())/2;
            panel->move(x,topy+i*cradSpace);
        }

        panel->show();
        panel->raise();


    }
    if(m_contextMap[player].align==CardAlign::horizontal){
         m_cardsRect=QRect(stx,sty,(list.size()-1)*cradSpace+m_cardSize.width(),m_cardSize.height());
    }




}

void GameMainWindow::showAnimationWindow(AnimationType animationtype,int bet)
{
    switch (animationtype) {
    case AnimationType::BET:
        m_animationWindow->setFixedSize(160,98);
        m_animationWindow->move((width()-m_animationWindow->width())/2,(height()-m_animationWindow->height())/2-100);
        m_animationWindow->setBetImage(bet);

        break;
    case AnimationType::FEIJI:
        m_animationWindow->setFixedSize(800,75);
        m_animationWindow->move((width()-m_animationWindow->width())/2,(height()-m_animationWindow->height())/2-100);
        m_animationWindow->showPlane();
        break;

    case AnimationType::LIANDUI:
        m_animationWindow->setFixedSize(250,150);
        m_animationWindow->move((width()-m_animationWindow->width())/2,(height()-m_animationWindow->height())/2-100);
        m_animationWindow->showSequence(AnimationWindow::Pair);
        break;

    case AnimationType::SHUNZI:
        m_animationWindow->setFixedSize(250,150);
        m_animationWindow->move((width()-m_animationWindow->width())/2,(height()-m_animationWindow->height())/2-100);
        m_animationWindow->showSequence(AnimationWindow::Sequence);
        break;

    case AnimationType::WANGZHA:
        m_animationWindow->setFixedSize(250,200);
        m_animationWindow->move((width()-m_animationWindow->width())/2,(height()-m_animationWindow->height())/2-100);
        m_animationWindow->showJokerBomb();
        break;

    case AnimationType::ZHADAN:
        m_animationWindow->setFixedSize(180,200);
        m_animationWindow->move((width()-m_animationWindow->width())/2,(height()-m_animationWindow->height())/2-100);
        m_animationWindow->showBomb();
        break;
    default:
        break;
    }


    m_animationWindow->show();
}

void GameMainWindow::preparePlayingHand()
{
    //显示出人物头像
    for (auto it = m_contextMap.begin(); it != m_contextMap.end(); ++it)
    {
        //获取随机数
        int num = QRandomGenerator::global()->bounded(1, 3);
        QImage image;
        QPixmap pixmap;

        if(it.key()->role()==PlayerRole::Lord){
            //设置地主头像
            if(it.key()->sex()==PlayerSex::Female){
                 image.load(QString(":/images/lord_woman_%1.png").arg(num));

            }
            else{
                 image.load(QString(":/images/lord_man_%1.png").arg(num));
            }
        }

        else{
            //设置农民头像
            if(it.key()->sex()==PlayerSex::Female){
                  image.load(QString(":/images/farmer_woman_%1.png").arg(num));
            }
            else{
                  image.load(QString(":/images/farmer_man_%1.png").arg(num));
            }

        }

        //显示方向
        if(it.key()->direction()==PlayerDirection::Bottom){//脸朝左
            pixmap=QPixmap::fromImage(image.mirrored(true,false));
        }

        else if(it.key()->direction()==PlayerDirection::Left){//脸朝右
             pixmap=QPixmap::fromImage(image);
        }

        else if(it.key()->direction()==PlayerDirection::Right){//脸朝左
           pixmap=QPixmap::fromImage(image.mirrored(true,false));
        }
        it->roleImg->setPixmap(pixmap);

        //show
        it->roleImg->show();
    }

    //显示出3张底牌
    for(auto f:m_last3Cards){
        f->show();
    }

    //延迟一秒隐藏分数动画窗口
    QTimer::singleShot(1000,this,[=](){
        this->m_animationWindow->hide();
    });

    //延迟一秒隐藏叫地主信息
    QTimer::singleShot(1000,this,[=](){
        for (auto it = m_contextMap.begin(); it != m_contextMap.end(); ++it)
        {
            it.value().info->hide();
        }
    });



}

QString GameMainWindow::voicePrefix(Player *player) const
{
    //sex 由 GameControl 构造时随机设为 Male(1)/Female(2)；兜底 Male 防止未设置时拼出非法音效名
    if (player && player->sex() == PlayerSex::Female) {
        return QStringLiteral("Woman");
    }
    return QStringLiteral("Man");
}

void GameMainWindow::updateScorePanel()
{
    ui->scorePanel->setScore(m_gameControl->getUserPlayer(),m_gameControl->getUserPlayer()->score());
    ui->scorePanel->setScore(m_gameControl->getLeftRobot(),m_gameControl->getLeftRobot()->score());
    ui->scorePanel->setScore(m_gameControl->getRightRobot(),m_gameControl->getRightRobot()->score());
}

void GameMainWindow::onUserPlayHand()
{
    //判断游戏状态
    if(m_gameStatus!=GameControl::GameStatus::PlayingHand) return ;

    //没选牌
    if(m_selectCardPanels.size()==0) return ;

    Cards cards;

    //得到牌型，判断能不能打出
    for(QSet<CardPanel*>::Iterator it=m_selectCardPanels.begin();it!=m_selectCardPanels.end();++it){
        cards.add((*it)->card());
    }

    //不存在的牌型
    PlayHand playhand(cards);
    if(playhand.getHandType()==PlayHand::Hand_Unknown) return;

    //上一轮是用户自己，或者是第一轮出牌
    if(m_gameControl->getPendPlayer()==m_gameControl->getUserPlayer()||m_gameControl->getPendPlayer()==nullptr) {
        //无限制, //用户直接调槽函数了，应该可以优化成信号，先todo
         m_gameControl->playerPlayCards(m_gameControl->getUserPlayer(),cards);
    }

    else{
        Cards cs=m_gameControl->getPendCards();
        PlayHand p(cs);
        if(playhand.canBeat(p)){
            //用户直接调槽函数了，应该可以优化成信号，先todo
            m_gameControl->playerPlayCards(m_gameControl->getUserPlayer(),cards);
        }

        else{//压不过，打不了
            return ;
        }
    }

    //清空选择的牌
    m_selectCardPanels.clear();


    ui->buttonGroup->selectPage(ButtonGroup::Panel::Empty);


}

void GameMainWindow::onUserPass()
{
    //打出空牌
    Cards empty;
    m_gameControl->playerPass(m_gameControl->getUserPlayer());


    //用户可能选择了一些牌，但是点击了不要
    for(auto it=m_selectCardPanels.begin();it!=m_selectCardPanels.end();++it){
        (*it)->setSelected(false);
    }

    m_selectCardPanels.clear();
    updatePlayerCards(m_gameControl->getUserPlayer());



}

void GameMainWindow::showEndingPanel()
{

    bool isLord=m_gameControl->getUserPlayer()->role()==PlayerRole::Lord?true:false;
    bool isWin=m_gameControl->getUserPlayer()->isWin();

    //音频：按本局胜负切换结算音乐（点"继续游戏"后 dispatchCards 会恢复 Normal）
    BGMController::instance()->playBgm(isWin ? BGMController::Scene::Win
                                             : BGMController::Scene::Lose);

    EndPanel * panel=new EndPanel(isLord,isWin,this);
    panel->move((width()-panel->width())/2,-panel->height());
    panel->setPlayers(m_playerList[0],m_playerList[1],m_playerList[2]);
    panel->setScore(m_gameControl->getUserPlayer(),m_gameControl->getUserPlayer()->score());
    panel->setScore(m_gameControl->getLeftRobot(),m_gameControl->getLeftRobot()->score());
    panel->setScore(m_gameControl->getRightRobot(),m_gameControl->getRightRobot()->score());
    panel->show();

    QPropertyAnimation *animation = new QPropertyAnimation(panel, "geometry", this);
    // 动画持续的时间
    animation->setDuration(1500);   // 1.5s
    // 设置窗口的起始位置和终止位置
    animation->setStartValue(QRect(panel->x(), panel->y(), panel->width(), panel->height()));
    animation->setEndValue(QRect((width() - panel->width()) / 2, (height() - panel->height()) / 2,
                                 panel->width(), panel->height()));
    // 设置窗口的运动曲线
    animation->setEasingCurve(QEasingCurve(QEasingCurve::OutBounce));
    // 播放动画效果
    animation->start();

    // 处理窗口信号
    connect(panel, &EndPanel::continueGame, this, [=]()
            {
                panel->close();
                panel->deleteLater();
                animation->deleteLater();
                ui->buttonGroup->selectPage(ButtonGroup::Panel::Empty);

                gameStatusProcess(GameControl::DispatchCard);

            });


}

void GameMainWindow::initCountDown()
{

    m_counDown=new CountDown(this);
    //m_counDown->move((width()-m_counDown->width())/2,(height()-m_counDown->height())/2+120);
    connect(m_counDown,&CountDown::notMuchTimer,this,[=](){
        //播放提示音

    });
    connect(m_counDown,&CountDown::timeOut,this,[=](){
       //进入托管模式，但是托管模式还未实现，这里先强制玩家不要
        onUserPass();

    });


}

void GameMainWindow::onPlayerStatusChanged(Player *player, GameControl::PlayerStatus status)
{
    switch (status) {
    case GameControl::ThinkingForCallLord:
        if(player==m_gameControl->getUserPlayer()){
            ui->buttonGroup->selectPage(ButtonGroup::Panel::CallLord,m_gameControl->getPlayerMaxBet());//如果机器人抢1分，玩家只显示2分,3分,如果抢2分，玩家只显示3分
        }

        break;

    case GameControl::ThinkingForPlayHand:

        {
            //隐藏上一轮打出的牌
            auto itt =m_contextMap.find(player);
            if(!itt->lastCard.isEmpty()){
                QVector<Card> lastList=itt->lastCard.toCardList();
                for(auto f : lastList){
                    m_cardMap[f]->hide();
                }
            }

            else{
                itt->info->hide();//隐藏 “不要”
            }

            //移动闹钟到对应玩家的出牌区域
            QRect rect=m_contextMap[player].playHandRect;
            if(player==m_gameControl->getUserPlayer()){
                m_counDown->move(rect.left()+(rect.width()-m_counDown->width())/2,rect.top());
            }

            else{
                m_counDown->move(rect.left(),rect.top()+(rect.height()-m_counDown->height())/2);
            }

            //启动闹钟
            m_counDown->showCountDown();

        }


        //轮到用户出牌/接牌时，显示"出牌/不要"按钮组
        if(player==m_gameControl->getUserPlayer()){


            if(m_gameControl->getPendPlayer()==player||m_gameControl->getPendPlayer()==nullptr)
            {
                ui->buttonGroup->selectPage(ButtonGroup::Panel::PlayCard);
            }

            else{
                  ui->buttonGroup->selectPage(ButtonGroup::Panel::PassOrPlay);
            }

        }

        else{
            ui->buttonGroup->selectPage(ButtonGroup::Panel::Empty);
        }



        break;

    case GameControl::Winning:
        //所有玩家亮牌
        m_contextMap[m_gameControl->getLeftRobot()].isFront=true;
        m_contextMap[m_gameControl->getRightRobot()].isFront=true;
        updatePlayerCards(m_gameControl->getLeftRobot());
        updatePlayerCards(m_gameControl->getRightRobot());

        //更新分数面板得分
        updateScorePanel();
        //分数最高下一轮游戏优先叫地主
        m_gameControl->setCurrentPlayer(player);
        showEndingPanel();


        break;
    default:
        break;
    }
}

void GameMainWindow::onGrabLordBet(Player *bettor, int bet, bool isFirstCall)
{
    //显示抢地主的提示信息
    PlayerContext& context = m_contextMap[bettor];
    if(bet==0){//不抢
        context.info->setPixmap(QPixmap(":/images/buqinag.png"));
    }
    else if(isFirstCall){
        context.info->setPixmap(QPixmap(":/images/jiaodizhu.png"));
    }
    else{
        context.info->setPixmap(QPixmap(":/images/qiangdizhu.png"));
    }
    context.info->show();

    //显示叫地主的分数
    showAnimationWindow(AnimationType::BET,bet);

    //音频：按玩家性别播叫/抢/不抢语音
    BGMController* bgm = BGMController::instance();
    QString prefix = voicePrefix(bettor);
    if (bet == 0) {
        bgm->playEffect(prefix + QStringLiteral("_NoRob"));
    } else if (isFirstCall) {
        bgm->playEffect(prefix + QStringLiteral("_Order"));
    } else if(bet==1){
        bgm->playEffect(prefix + QStringLiteral("_Rob1"));
    } else if(bet ==2){
        bgm->playEffect(prefix + QStringLiteral("_Rob2"));
    } else if(bet==3){
        bgm->playEffect(prefix + QStringLiteral("_Rob3"));
    }
}

void GameMainWindow::onPlayCards(Player* player,  Cards& cards)
{


    //      在 player 的出牌区(playHandRect)绘制打出的牌
    //      可用 m_contextMap[player].playHandRect 获取位置，m_cardMap[card] 获取牌面板
    QRect rect=m_contextMap[player].playHandRect;
    QVector<Card> list=cards.toCardList();
    if(player==m_gameControl->getUserPlayer()){
        for(int i=0;i<list.size();++i){
            int l=(rect.width()-((list.size()-1)*25+m_cardSize.width()))/2;
            m_cardMap[list[i]]->move(rect.left()+l+i*25,rect.top());
            m_cardMap[list[i]]->setFrontSide(true);
            m_cardMap[list[i]]->show();
            m_cardMap[list[i]]->raise();
        }
    }

    else{
        for(int i=0;i<list.size();++i){
            int l=(rect.height()-((list.size()-1)*25+m_cardSize.height()))/2;
            m_cardMap[list[i]]->move(rect.left(),rect.top()+l+i*25);
            m_cardMap[list[i]]->setFrontSide(true);
            m_cardMap[list[i]]->show();
            m_cardMap[list[i]]->raise();
        }
    }

    // if(player!=m_gameControl->getUserPlayer()){

    //     {
    //         //隐藏上一轮打出的牌
    //         auto itt =m_contextMap.find(player);
    //         if(!itt->lastCard.isEmpty()){
    //             QVector<Card> lastList=itt->lastCard.toCardList();
    //             for(auto f : lastList){
    //                 m_cardMap[f]->hide();
    //             }
    //         }

    //         else{
    //             itt->info->hide();//隐藏 “不要”
    //         }

    //     }
    // }


    //记录cards
    auto it =m_contextMap.find(player);
    it->lastCard=cards;

    //根据牌型绘制特效
    PlayHand playhand(cards);
    PlayHand::HandType type=playhand.getHandType();
    //根据牌型播放音效
    QString prefix=voicePrefix(player);

    int random=QRandomGenerator::global()->bounded(1,4);

    switch (type) {
    case PlayHand::Hand_Bomb_Jokers://王炸
        showAnimationWindow(AnimationType::WANGZHA);
        BGMController::instance()->playEffect(prefix+"_wangzha");
        break;

    case PlayHand::Hand_Bomb://炸弹
        showAnimationWindow(AnimationType::ZHADAN);
        BGMController::instance()->playEffect(prefix+"_zhadan");
        break;

    case PlayHand::Hand_Plane://飞机
        if(player->pendPlayer()!=player&&player->pendPlayer()!=nullptr){
            BGMController::instance()->playEffect(prefix+"_dani"+QString("%1").arg(random));
        }
        else{
            BGMController::instance()->playEffect(prefix+"_feiji");
        }
        showAnimationWindow(AnimationType::FEIJI);

        break;
    case PlayHand::Hand_Plane_Two_Single://飞机
        if(player->pendPlayer()!=player&&player->pendPlayer()!=nullptr){
            BGMController::instance()->playEffect(prefix+"_dani"+QString("%1").arg(random));
        }
        else{
            BGMController::instance()->playEffect(prefix+"_feiji");
        }
        showAnimationWindow(AnimationType::FEIJI);
        break;

    case PlayHand::Hand_Plane_Two_Pair://飞机
        if(player->pendPlayer()!=player&&player->pendPlayer()!=nullptr){
            BGMController::instance()->playEffect(prefix+"_dani"+QString("%1").arg(random));
        }
        else{
            BGMController::instance()->playEffect(prefix+"_feiji");
        }
        showAnimationWindow(AnimationType::FEIJI);

        break;


    case PlayHand::Hand_Seq_Pair://连对
        showAnimationWindow(AnimationType::LIANDUI);
        if(player->pendPlayer()!=player&&player->pendPlayer()!=nullptr){
            BGMController::instance()->playEffect(prefix+"_dani"+QString("%1").arg(random));
        }
        else{
             BGMController::instance()->playEffect(prefix+"_liandui");
        }

        break;

    case PlayHand::Hand_Seq_Single://顺子
        showAnimationWindow(AnimationType::SHUNZI);
        if(player->pendPlayer()!=player&&player->pendPlayer()!=nullptr){
            BGMController::instance()->playEffect(prefix+"_dani"+QString("%1").arg(random));
        }
        else{
             BGMController::instance()->playEffect(prefix+"_shunzi");
        }

        break;
    case PlayHand::Hand_Single:
        switch(playhand.getCardPoint()){
        case CardPoint::Card_3:
            BGMController::instance()->playEffect(prefix+"_3");
            break;

        case CardPoint::Card_4:
            BGMController::instance()->playEffect(prefix+"_4");
            break;

        case CardPoint::Card_5:
            BGMController::instance()->playEffect(prefix+"_5");
            break;
        case CardPoint::Card_6:
            BGMController::instance()->playEffect(prefix+"_6");
            break;
        case CardPoint::Card_7:
            BGMController::instance()->playEffect(prefix+"_7");
            break;
        case CardPoint::Card_8:
            BGMController::instance()->playEffect(prefix+"_8");
            break;
        case CardPoint::Card_9:
            BGMController::instance()->playEffect(prefix+"_9");
            break;
        case CardPoint::Card_10:
            BGMController::instance()->playEffect(prefix+"_10");
            break;
        case CardPoint::Card_J:
            BGMController::instance()->playEffect(prefix+"_11");
            break;
        case CardPoint::Card_Q:
            BGMController::instance()->playEffect(prefix+"_12");
            break;
        case CardPoint::Card_K:
            BGMController::instance()->playEffect(prefix+"_13");
            break;
        case CardPoint::Card_A:
            BGMController::instance()->playEffect(prefix+"_1");
            break;
        case CardPoint::Card_2:
            BGMController::instance()->playEffect(prefix+"_2");
            break;
        case CardPoint::Card_SJ:
            BGMController::instance()->playEffect(prefix+"_14");
            break;
        case CardPoint::Card_BJ:
            BGMController::instance()->playEffect(prefix+"_15");
            break;
        default:
            break;
        }

        break;

    case PlayHand::Hand_Pair:
        switch(playhand.getCardPoint()){
        case CardPoint::Card_3:
            BGMController::instance()->playEffect(prefix+"_dui3");
            break;

        case CardPoint::Card_4:
            BGMController::instance()->playEffect(prefix+"_dui4");
            break;

        case CardPoint::Card_5:
            BGMController::instance()->playEffect(prefix+"_dui5");
            break;
        case CardPoint::Card_6:
            BGMController::instance()->playEffect(prefix+"_dui6");
            break;
        case CardPoint::Card_7:
            BGMController::instance()->playEffect(prefix+"_dui7");
            break;
        case CardPoint::Card_8:
            BGMController::instance()->playEffect(prefix+"_dui8");
            break;
        case CardPoint::Card_9:
            BGMController::instance()->playEffect(prefix+"_dui9");
            break;
        case CardPoint::Card_10:
            BGMController::instance()->playEffect(prefix+"_dui10");
            break;
        case CardPoint::Card_J:
            BGMController::instance()->playEffect(prefix+"_dui11");
            break;
        case CardPoint::Card_Q:
            BGMController::instance()->playEffect(prefix+"_dui12");
            break;
        case CardPoint::Card_K:
            BGMController::instance()->playEffect(prefix+"_dui13");
            break;
        case CardPoint::Card_A:
            BGMController::instance()->playEffect(prefix+"_dui1");
            break;
        case CardPoint::Card_2:
            BGMController::instance()->playEffect(prefix+"_dui2");
            break;
        default:
            break;
        }

        break;

        break;

    case PlayHand::Hand_Triple_Single:
        if(player->pendPlayer()!=player&&player->pendPlayer()!=nullptr){
            BGMController::instance()->playEffect(prefix+"_dani"+QString("%1").arg(random));
        }
        else{
             BGMController::instance()->playEffect(prefix+"_sandaiyi");
        }

        break;

    case PlayHand::Hand_Triple_Pair:
        if(player->pendPlayer()!=player&&player->pendPlayer()!=nullptr){
            BGMController::instance()->playEffect(prefix+"_dani"+QString("%1").arg(random));
        }
        else{
             BGMController::instance()->playEffect(prefix+"_sandaiyidui");
        }

    default:
        break;
    }





    //更新手牌显示
    updatePlayerCards(player);





    //隐藏闹钟
    m_counDown->stopCountDown();

}

void GameMainWindow::onPass(Player* player)
{

    //在 player 的出牌区显示"不要"提示
     auto it =m_contextMap.find(player);
     it->info->setPixmap(QPixmap(":/images/pass.png"));
     it->info->show();

     if(player!=m_gameControl->getUserPlayer()){

         {
             //隐藏上一轮打出的牌
             auto itt =m_contextMap.find(player);
             if(!itt->lastCard.isEmpty()){
                 QVector<Card> lastList=itt->lastCard.toCardList();
                 for(auto f : lastList){
                     m_cardMap[f]->hide();
                 }
             }

         }
     }

     //清空lastcard
     m_contextMap[player].lastCard.clear();
     //隐藏闹钟
     m_counDown->stopCountDown();

     //随机播一条"不要"语音（buyao1~4）
     int index = QRandomGenerator::global()->bounded(1, 5);
     BGMController::instance()->playEffect(voicePrefix(player) + QStringLiteral("_buyao") + QString::number(index));

}

void GameMainWindow::onLordConfirmed(Player* landlord)
{
    //: 刷新地主手牌显示(增加了3张)
    updatePlayerCards(landlord);

}

void GameMainWindow::onGameOver(Player* winner)
{
    Q_UNUSED(winner);
    //结算分数已在 GameControl::settleGame 中写入 Player 对象，这里同步到分数面板
    for (Player* player : m_playerList) {
        ui->scorePanel->setScore(player, player->score());
    }
}

void GameMainWindow::onCardClicked(CardPanel* panel)
{
    //只有用户自己的手牌、且处于出牌阶段才能选
    if (panel->owner() != m_gameControl->getUserPlayer()) {
        return;
    }
    if (m_gameStatus != GameControl::PlayingHand) {
        return;
    }

    //切换选中状态（paintEvent 根据选中态自动上浮/落回）
    panel->setSelected(!panel->selected());

    //保存选中的牌
    if(panel->selected()){
        m_selectCardPanels.insert(panel);
    }

    else{
        m_selectCardPanels.remove(panel);
    }

}


void GameMainWindow::mouseMoveEvent(QMouseEvent *event)
{
   
    if(event->buttons() & Qt::LeftButton)
    {
        QPoint pt = event->pos();
        QList<CardPanel*> list = m_cardRectMap.keys();
        static CardPanel*  current;
        for(int i=0; i<list.size(); ++i)
        {
            CardPanel* panel = list.at(i);

            if(m_cardRectMap[panel].contains(pt) &&current != panel)
            {
                    // 点击这张扑克牌
                    onCardClicked(panel);
                    current= panel;
            }
        }

    }
}


void GameMainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawPixmap(this->rect(),m_bkImage);

}
