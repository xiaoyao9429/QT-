#include "gamecontrol.h"
#include "userplayer.h"
#include "robotplayer.h"
#include <QRandomGenerator>
#include <QTimer>
#include "playhand.h"
GameControl::GameControl(QObject *parent)
    : QObject(parent)
    , m_leftRobot(nullptr)
    , m_rightRobot(nullptr)
    , m_userPlayer(nullptr)
    , m_currentPlayer(nullptr)
    , m_pendPlayer(nullptr)
    , m_status(Status_Begin)
    , m_gameScore(500)
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

    // 玩家出牌/不要/接牌信号直连到控制入口（信号自带 player 参数，无需 lambda 中转）
    QObject::connect(m_leftRobot, &Player::notifyPlayCards, this, &GameControl::playerPlayCards);
    QObject::connect(m_rightRobot, &Player::notifyPlayCards, this, &GameControl::playerPlayCards);
    QObject::connect(m_userPlayer, &Player::notifyPlayCards, this, &GameControl::playerPlayCards);

    QObject::connect(m_leftRobot, &Player::notifyPass, this, &GameControl::playerPass);
    QObject::connect(m_rightRobot, &Player::notifyPass, this, &GameControl::playerPass);
    QObject::connect(m_userPlayer, &Player::notifyPass, this, &GameControl::playerPass);

    QObject::connect(m_leftRobot, &Player::notifyTakeCards, this, &GameControl::playerPlayCards);
    QObject::connect(m_rightRobot, &Player::notifyTakeCards, this, &GameControl::playerPlayCards);
    QObject::connect(m_userPlayer, &Player::notifyTakeCards, this, &GameControl::playerPlayCards);

    // 机器人叫地主决策：各自 AI 思考后通过 callLordDecided 进入 playerBet 入口
    // 注：人类玩家不需要这个 connect —— 用户的按钮直接在 MainWindow 调 playerBet()
    connect(m_leftRobot,   &Player::callLordDecided, this, &GameControl::playerBet);
    connect(m_rightRobot,  &Player::callLordDecided, this, &GameControl::playerBet);
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

Player* GameControl::getPendPlayer() const
{
    return m_pendPlayer;
}

Cards GameControl::getPendCards() const
{
    return m_pendCards;
}

GameControl::GameStatus GameControl::gameStatus() const
{
    return m_status;
}

void GameControl::gameStart()
{
    // 发牌由 UI 端一张张带动画完成，发完 51 张后 UI 调 startCallLord()
}

Cards GameControl::bottomCards() const
{
    return m_bottomCards;
}

void GameControl::startCallLord()
{
    // 牌堆中剩余的3张即为底牌（只复制不移除：UI 显示顶部底牌仍读 initialCards()）
    m_bottomCards.clear();
    m_bottomCards.add(m_initialCards);

    m_status = CallingLord;
    emit notifyGameStatusChanged(m_status);
    // 从当前玩家(默认为用户)开始叫地主
    advanceBettor();
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
    m_pendPlayer = nullptr;
    m_pendCards.clear();
    m_status = PlayingHand;

    emit notifyLordConfirmed(player);

    // 延迟1秒后进入出牌阶段，给UI时间显示地主确认
    QTimer::singleShot(1000, this, [=](){
        emit notifyGameStatusChanged(PlayingHand);
        emit playerStatusChanged(m_currentPlayer, ThinkingForPlayHand);
        m_currentPlayer->preparePlayCards();
    });
}

void GameControl::playerPlayCards(Player* player,Cards& cards)
{
    // 真正执行出牌：从手牌移除
    player->playCards(cards);

    // 更新待应对局面
    m_pendPlayer = player;
    m_pendCards = cards;

    // 出牌者自己已无需应对任何牌，清掉它 Player 层的待应对状态，
    // 否则一轮结束重新领出时 Strategy 会读到旧的 pendPlayer 误走跟牌分支
    player->setPendCards(Cards());
    player->setPendPlayer(nullptr);

    // 通知 UI 显示出牌
    emit notifyPlayCards(player, cards);

    //cards是炸弹,游戏分数翻倍
    PlayHand playhand(cards);
    PlayHand::HandType type=playhand.getHandType();
    if(type==PlayHand::Hand_Bomb||type==PlayHand::Hand_Bomb_Jokers){
        m_gameScore*=2;
    }

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
    // 自由出牌（开局/一轮结束重新领出）时桌面上没有待压的牌，不存在"不要"，忽略非法调用
    if (m_pendPlayer == nullptr) {
        return;
    }

    //通知ui
    emit notifyPass(player);

    // 检查是否所有人都过了（即回到打出待应对牌的玩家）
    Player* next = player->nextPlayer();
    if (next == m_pendPlayer) {
        // 一轮过完，m_pendPlayer 重新主动出牌
        m_currentPlayer = m_pendPlayer;


        m_pendCards.clear();//不出->打出的是空牌
        m_pendPlayer = nullptr;

        // 同步清空领出者 Player 层的待应对状态，保证机器人走主动出牌分支
        m_currentPlayer->setPendCards(Cards());
        m_currentPlayer->setPendPlayer(nullptr);

        emit playerStatusChanged(m_currentPlayer, ThinkingForPlayHand);
        m_currentPlayer->preparePlayCards();
    } else {
        // 继续给下家
        passTurnToNext();
    }
}

void GameControl::passTurnToNext()
{
    m_currentPlayer = m_currentPlayer->nextPlayer();
    m_currentPlayer->setPendCards(m_pendCards);
    m_currentPlayer->setPendPlayer(m_pendPlayer);
    // 通知 UI 轮到谁出牌/接牌了（用户回合要显示出牌按钮组）
    emit playerStatusChanged(m_currentPlayer, ThinkingForPlayHand);
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
    Player* lord = nullptr;      // 地主
    Player* farmer1 = nullptr;   // 农民1
    Player* farmer2 = nullptr;   // 农民2

    if (winner->role() == PlayerRole::Lord) {
        // 出完牌的是地主：地主赢，上下家两个农民输
        lord = winner;
        farmer1 = winner->prevPlayer();
        farmer2 = winner->nextPlayer();

        lord->setIsWin(true);
        farmer1->setIsWin(false);
        farmer2->setIsWin(false);
    } else {
        // 出完牌的是农民：在它上下家里找地主，地主输，两个农民赢
        farmer1 = winner;
        if (winner->prevPlayer()->role() == PlayerRole::Lord) {
            lord = winner->prevPlayer();
            farmer2 = winner->nextPlayer();
        } else {
            lord = winner->nextPlayer();
            farmer2 = winner->prevPlayer();
        }

        lord->setIsWin(false);
        farmer1->setIsWin(true);
        farmer2->setIsWin(true);
    }

    // 计算分数：地主赢→地主+2倍、农民各-1倍；地主输→地主-2倍、农民各+1倍
    bool lordWins = lord->isWin();
    lord->setScore(lord->score() + (lordWins ? 2 * m_gameScore : -2 * m_gameScore));
    farmer1->setScore(farmer1->score() + (lordWins ? -m_gameScore : m_gameScore));
    farmer2->setScore(farmer2->score() + (lordWins ? -m_gameScore : m_gameScore));

    // 通知 UI（放在算分之后：Winning 处理里会弹结算窗口、刷新分数面板，
    // 必须保证执行时 isWin 和 score 都已写入）
    emit playerStatusChanged(winner, Winning);

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
    m_status = Status_Begin;
    m_gameScore = 500;               // 重置本局游戏分数（炸弹翻倍不累积到下一局）

    // 清空每个玩家 Player 层的待应对状态，防止上一局残留影响新一局领出判断
    m_userPlayer->setPendCards(Cards());
    m_userPlayer->setPendPlayer(nullptr);
    m_leftRobot->setPendCards(Cards());
    m_leftRobot->setPendPlayer(nullptr);
    m_rightRobot->setPendCards(Cards());
    m_rightRobot->setPendPlayer(nullptr);
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

int GameControl::getPlayerMaxBet()
{
    return m_betRecord.bet;
}

void GameControl::playerBet(Player *bettor, int bet)
{
    // 只有当前叫地主轮到的人才能叫，忽略其他玩家的乱序调用
    if (m_status != CallingLord || bettor != m_currentPlayer) {
        return;
    }

    // 叫分合法性：0-3
    if (bet < 0 || bet > 3) {
        return;
    }

    // 新分必须高于当前最高叫分（否则就是"不抢"，只能传0）
    if (bet > 0 && bet <= m_betRecord.bet) {
        bet = 0;
    }

    // 判断是否是全场第一个叫正分的（决定 UI 显示"叫地主"还是"抢地主"）
    bool isFirstCall = (bet > 0 && m_betRecord.bet == 0);
    emit grabLordBetDecided(bettor, bet, isFirstCall);

    // 叫3分直接成为地主
    if (bet == 3) {
        m_betRecord.reset();
        becomeLord(bettor);
        return;
    }

    // 记录最高叫分者（只在bet>0时更新，过牌不覆盖）
    if (bet > 0) {
        m_betRecord.bet = bet;
        m_betRecord.player = bettor;
    }

    m_betRecord.times++;
    if (m_betRecord.times == 3) {
        if (m_betRecord.bet == 0) {
            // 都没叫，重新发牌
            emit notifyGameStatusChanged(DispatchCard);
        } else {
            becomeLord(m_betRecord.player);
        }
        m_betRecord.reset();
        return;
    }

    // 切到下家继续抢
    m_currentPlayer = m_currentPlayer->nextPlayer();
    advanceBettor();
}

void GameControl::advanceBettor()
{
    // 启动当前玩家的叫地主决策，并通知 UI 切换到"轮到他了"的界面
    // —— startCallLord() 和 playerStatusChanged 收敛到此，避免两处重复
    m_currentPlayer->startCallLord();
    emit playerStatusChanged(m_currentPlayer, ThinkingForCallLord);
}
