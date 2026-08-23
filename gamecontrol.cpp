#include "gamecontrol.h"
#include "userplayer.h"
#include "robotplayer.h"
#include <QRandomGenerator>
#include <QTimer>
GameControl::GameControl(QObject *parent)
    : QObject(parent)
    , m_leftRobot(nullptr)
    , m_rightRobot(nullptr)
    , m_userPlayer(nullptr)
    , m_currentPlayer(nullptr)
    , m_pendPlayer(nullptr)
    , m_lordScore(0)
    , m_status(Status_Begin)
{
    playerInit();
    initCards();
}

GameControl::~GameControl()
{
}

void GameControl::playerInit()
{
    // 创建三个玩家对象，this 作为父对象自动管理生命周期
    m_leftRobot = new RobotPlayer(this);
    m_leftRobot->setName("左边的机器人");
    m_rightRobot = new RobotPlayer(this);
    m_rightRobot->setName("右边的机器人");
    m_userPlayer = new UserPlayer(this);
    m_userPlayer->setName("张志文");

    //随机性别,1男,2女
    m_leftRobot->setSex((PlayerSex)QRandomGenerator::global()->bounded(1, 3));
    m_rightRobot->setSex((PlayerSex)QRandomGenerator::global()->bounded(1, 3));
    m_userPlayer->setSex((PlayerSex)QRandomGenerator::global()->bounded(1, 3));


    // 设置玩家类型与显示方位
    m_leftRobot->setType(PlayerType::Robot);
    m_leftRobot->setDirection(PlayerDirection::Left);

    m_rightRobot->setType(PlayerType::Robot);
    m_rightRobot->setDirection(PlayerDirection::Right);

    m_userPlayer->setType(PlayerType::Human);
    m_userPlayer->setDirection(PlayerDirection::Bottom);

    //默认当前玩家为用户(自己)
    m_currentPlayer = m_userPlayer;

    // 建立环形出牌链：左机器人 → 真人 → 右机器人 → 左机器人
    m_leftRobot->setPrevPlayer(m_rightRobot);
    m_leftRobot->setNextPlayer(m_userPlayer);

    m_userPlayer->setPrevPlayer(m_leftRobot);
    m_userPlayer->setNextPlayer(m_rightRobot);

    m_rightRobot->setPrevPlayer(m_userPlayer);
    m_rightRobot->setNextPlayer(m_leftRobot);

    // 连接玩家信号到本控制类（显式限定 QObject::connect 避免命名遮蔽）
    QObject::connect(m_leftRobot, &Player::notifyPlayCards,
                     this, [this](const Cards& cards){ playerPlayCards(m_leftRobot, cards); });
    QObject::connect(m_rightRobot, &Player::notifyPlayCards,
                     this, [this](const Cards& cards){ playerPlayCards(m_rightRobot, cards); });
    QObject::connect(m_userPlayer, &Player::notifyPlayCards,
                     this, [this](const Cards& cards){ playerPlayCards(m_userPlayer, cards); });

    QObject::connect(m_leftRobot, &Player::notifyPass, this, [this](){ playerPass(m_leftRobot); });
    QObject::connect(m_rightRobot, &Player::notifyPass, this, [this](){ playerPass(m_rightRobot); });
    QObject::connect(m_userPlayer, &Player::notifyPass, this, [this](){ playerPass(m_userPlayer); });

    QObject::connect(m_leftRobot, &Player::notifyTakeCards,
                     this, [this](const Cards& cards){ playerPlayCards(m_leftRobot, cards); });
    QObject::connect(m_rightRobot, &Player::notifyTakeCards,
                     this, [this](const Cards& cards){ playerPlayCards(m_rightRobot, cards); });
    QObject::connect(m_userPlayer, &Player::notifyTakeCards,
                     this, [this](const Cards& cards){ playerPlayCards(m_userPlayer, cards); });

    connect(m_userPlayer,&Player::notifyGravLordBet,this,&GameControl::onGraBet);
    connect(m_leftRobot,&Player::notifyGravLordBet,this,&GameControl::onGraBet);
    connect(m_rightRobot,&Player::notifyGravLordBet,this,&GameControl::onGraBet);
}

void GameControl::initCards()
{
    m_initialCards.clear();
    for (int suit = static_cast<int>(CardSuit::Diamond);
         suit <= static_cast<int>(CardSuit::Spade); ++suit) {
        for (int point = static_cast<int>(CardPoint::Card_3);
             point <= static_cast<int>(CardPoint::Card_2); ++point) {
            m_initialCards << Card(static_cast<CardPoint>(point),
                                   static_cast<CardSuit>(suit));
        }
    }
    m_initialCards << Card(CardPoint::Card_SJ, CardSuit::Suit_Begin);
    m_initialCards << Card(CardPoint::Card_BJ, CardSuit::Suit_Begin);
}

RobotPlayer* GameControl::getLeftRobot() const
{
    return m_leftRobot;
}

RobotPlayer* GameControl::getRightRobot() const
{
    return m_rightRobot;
}

UserPlayer* GameControl::getUserPlayer() const
{
    return m_userPlayer;
}

Player* GameControl::getCurrentPlayer() const
{
    return m_currentPlayer;
}

void GameControl::setCurrentPlayer(Player* player)
{
    m_currentPlayer = player;
}

GameControl::GameStatus GameControl::gameStatus() const
{
    return m_status;
}

void GameControl::gameStart()
{
    // 整局流程：发牌 → 叫地主 → 出牌
    dispatchCards();
    // 发完牌后由 UI 触发叫地主流程，或此处直接调用 startCallLord()
}

void GameControl::dispatchCards()
{
    m_status = DispatchCard;
    emit notifyGameStatusChanged(m_status);

    // 1. 生成一副完整 54 张牌
    initCards();

    // 2. 给三个玩家各发 17 张
    m_userPlayer->addCards(m_initialCards.takeRandomCards(17));
    emit notifyDispatchCards(m_userPlayer);

    m_leftRobot->addCards(m_initialCards.takeRandomCards(17));
    emit notifyDispatchCards(m_leftRobot);

    m_rightRobot->addCards(m_initialCards.takeRandomCards(17));
    emit notifyDispatchCards(m_rightRobot);

    // 3. 剩余 3 张作为底牌
    m_bottomCards = m_initialCards;
}

Cards GameControl::bottomCards() const
{
    return m_bottomCards;
}

void GameControl::startCallLord()
{
    // m_status = CallingLord;
    // emit notifyGameStatusChanged(m_status);
     // 从当前玩家(默认为用户)开始叫地主
    m_currentPlayer->startCallLord();

    emit playerStatusChanged(m_currentPlayer,ThinkingForCallLord);
}

void GameControl::becomeLord(Player *player)
{
    player->setRole(PlayerRole::Lord);
    // 地主获得底牌
    player->addCards(m_bottomCards);
    //设置农民
    player->nextPlayer()->setRole(PlayerRole::Farmer);
    player->prevPlayer()->setRole(PlayerRole::Farmer);

    //设置当前玩家(从地主开始出牌)
    m_currentPlayer = player;

    emit notifyLordConfirmed(player);

    // 进入出牌阶段
    startPlayCards();


    QTimer::singleShot(1000,this,[=](){
        emit notifyGameStatusChanged(PlayingHand);//通知主窗口游戏状态改变
        emit playerStatusChanged(m_currentPlayer,ThinkingForPlayHand);//通知主窗口玩家状态改变
        //准备出牌
        m_currentPlayer->preparePlayCards();
    });

}

void GameControl::playerCallLord(Player* player, int score)
{
    emit notifyCallLord(player, score);

    if (score > m_lordScore) {
        m_lordScore = score;
        m_pendPlayer = player;   // 暂存当前最高叫分玩家
    }

    // 三家叫完或有人叫 3 分直接确定地主
    // 此处为简化实现：先按叫分最高者定地主
    if (score == 3) {
        // 直接定地主
        m_pendPlayer->setRole(PlayerRole::Lord);
        m_pendPlayer->addCards(m_bottomCards);
        emit notifyLordConfirmed(m_pendPlayer);
        startPlayCards();
    } else {
        // 继续传递到下家叫
        Player* next = player->nextPlayer();
        if (next != m_currentPlayer) {
            next->startCallLord();
        } else {
            // 一轮结束，确定地主
            if (m_pendPlayer && m_lordScore > 0) {
                m_pendPlayer->setRole(PlayerRole::Lord);
                m_pendPlayer->addCards(m_bottomCards);
                emit notifyLordConfirmed(m_pendPlayer);
                startPlayCards();
            }
            // 如果没人叫，则重新发牌（此处简化处理）
        }
    }
}

void GameControl::startPlayCards()
{
    m_status = PlayingHand;
    emit notifyGameStatusChanged(m_status);

    // 地主先出牌
    m_currentPlayer = m_pendPlayer;
    m_pendPlayer = nullptr;
    m_pendCards.clear();
    m_currentPlayer->preparePlayCards();
}

void GameControl::playerPlayCards(Player* player, const Cards& cards)
{
    // 真正执行出牌：从手牌移除
    player->playCards(cards);

    // 更新待应对局面
    m_pendPlayer = player;
    m_pendCards = cards;

    // 通知 UI 显示出牌
    emit notifyPlayCards(player, cards);

    // 检查是否游戏结束
    if (checkGameOver()) {
        settleGame(player);
        return;
    }

    // 传递出牌权给下家，下家需要接牌
    passTurnToNext();
}

void GameControl::playerPass(Player* player)
{
    emit notifyPass(player);

    // 检查是否所有人都过了（即回到打出待应对牌的玩家）
    Player* next = player->nextPlayer();
    if (next == m_pendPlayer) {
        // 一轮过完，m_pendPlayer 重新主动出牌
        m_currentPlayer = m_pendPlayer;
        m_pendCards.clear();
        m_pendPlayer = nullptr;
        m_currentPlayer->preparePlayCards();
    } else {
        // 继续给下家
        m_currentPlayer = next;
        // 把待应对信息同步给下家
        m_currentPlayer->setPendCards(m_pendCards);
        m_currentPlayer->setPendPlayer(m_pendPlayer);
        m_currentPlayer->prepareTakeCards();
    }
}

void GameControl::passTurnToNext()
{
    m_currentPlayer = m_currentPlayer->nextPlayer();
    m_currentPlayer->setPendCards(m_pendCards);
    m_currentPlayer->setPendPlayer(m_pendPlayer);
    m_currentPlayer->prepareTakeCards();
}

bool GameControl::checkGameOver()
{
    if (m_userPlayer->cardCount() == 0) {
        return true;
    }
    if (m_leftRobot->cardCount() == 0) {
        return true;
    }
    if (m_rightRobot->cardCount() == 0) {
        return true;
    }
    return false;
}

void GameControl::settleGame(Player* winner)
{
    winner->setIsWin(true);

    // 根据角色判定其他玩家胜负
    if (winner->role() == PlayerRole::Lord) {
        // 地主赢，农民全输
        m_leftRobot->setIsWin(false);
        m_rightRobot->setIsWin(false);
    } else {
        // 农民赢，地主输
        // 找出地主
        Player* landlord = nullptr;
        if (m_userPlayer->role() == PlayerRole::Lord) {
            landlord = m_userPlayer;
            m_leftRobot->setIsWin(true);
            m_rightRobot->setIsWin(true);
        } else if (m_leftRobot->role() == PlayerRole::Lord) {
            landlord = m_leftRobot;
            m_userPlayer->setIsWin(true);
            m_rightRobot->setIsWin(true);
        } else {
            landlord = m_rightRobot;
            m_userPlayer->setIsWin(true);
            m_leftRobot->setIsWin(true);
        }
        landlord->setIsWin(false);
    }

    emit notifyGameOver(winner);
}

void GameControl::reset()
{
    m_userPlayer->clearCards();
    m_leftRobot->clearCards();
    m_rightRobot->clearCards();

    m_userPlayer->setRole(PlayerRole::Farmer);
    m_leftRobot->setRole(PlayerRole::Farmer);
    m_rightRobot->setRole(PlayerRole::Farmer);

    m_userPlayer->setIsWin(false);
    m_leftRobot->setIsWin(false);
    m_rightRobot->setIsWin(false);

    m_bottomCards.clear();
    m_pendCards.clear();
    m_pendPlayer = nullptr;
    m_currentPlayer = m_userPlayer;  // 恢复默认当前玩家，不能置空，否则发牌阶段解引用崩溃
    m_lordScore = 0;
    m_status = Status_Begin;
    clearScores();
    initCards();  // 重新初始化 54 张牌堆
}

void GameControl::clearScores()
{
    m_userPlayer->setScore(0);
    m_leftRobot->setScore(0);
    m_rightRobot->setScore(0);
}

Cards& GameControl::initialCards()
{
    return m_initialCards;
}

int GameControl::initCardsCount()
{
    return m_initialCards.cardCount();
}

void GameControl::onGraBet(Player *player, int bet)
{
    //通知主界面
    if(bet>0&&m_betRecord.bet==0)//第一个叫地主的玩家
    {
         emit notifyGrabLordBet(player,bet,true);
    }

    else{
         emit notifyGrabLordBet(player,bet,false);
    }

    //如果是3分则直接成为地主
    if(bet==3){
        m_betRecord.reset();
        becomeLord(player);
        m_betRecord.reset();
        return;
    }

    if(m_betRecord.bet<3){
        m_betRecord.bet=bet,
        m_betRecord.player=player;
    }
    m_betRecord.times++;
    if(m_betRecord.times==3){
        if(m_betRecord.bet==0){//都没抢，重新发牌
            emit notifyGameStatusChanged(DispatchCard);
        }

        else{
            becomeLord(m_betRecord.player);
        }
        m_betRecord.reset();
        return;
    }
     //如果不是则下家继续抢
    //切到下家
    m_currentPlayer=m_currentPlayer->nextPlayer();
    //通知主界面
    playerStatusChanged(m_currentPlayer, ThinkingForCallLord);
    m_currentPlayer->startCallLord();

}
