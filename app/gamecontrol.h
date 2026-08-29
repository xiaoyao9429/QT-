#ifndef GAMECONTROL_H
#define GAMECONTROL_H

#include <QObject>
#include <QMap>
#include "player.h"
#include "cards.h"
#include "userplayer.h"
#include "robotplayer.h"
struct BetRecord{//存储成为地主的玩家
    Player* player;
    int bet;
    int times;//第几个叫地主


    BetRecord(){
        reset();
    }

    void reset(){
        player=nullptr;
        bet=0;
        times=0;
    }
};

// 游戏控制类：管理玩家、发牌、出牌流程、胜负判定
class GameControl : public QObject
{
    Q_OBJECT
public:
    // 游戏状态
    enum GameStatus
    {
        Status_Begin,       // 起始标记
        DispatchCard,       // 发牌阶段
        CallingLord,        // 叫地主阶段
        PlayingHand,        // 出牌阶段
        Status_End          // 结束标记
    };

    // 玩家状态
    enum PlayerStatus
    {
        ThinkingForCallLord,
        ThinkingForPlayHand,
        Winning
    };

    explicit GameControl(QObject *parent = nullptr);
    ~GameControl() override;

    // 初始化玩家（创建三个玩家对象并建立上下家关系）
    void playerInit();

    //初始化牌组
    void initCards();

    // 获取三个玩家对象
    RobotPlayer* getLeftRobot() const;
    RobotPlayer* getRightRobot() const;
    UserPlayer* getUserPlayer() const;

    // 获取当前玩家（轮到出牌的玩家）
    Player* getCurrentPlayer() const;
    // 设置当前玩家
    void setCurrentPlayer(Player* player);

    // 获取游戏状态
    GameStatus gameStatus() const;

    // 开始游戏：发牌 → 叫地主 → 出牌
    void gameStart();

    // 获取底牌
    Cards bottomCards() const;

    // 开始叫地主
    void startCallLord();
    //成为地主
    void becomeLord(Player* player);

    // 玩家叫地主/抢地主的统一入口（bet: 0不抢, 1-3分）
    void playerBet(Player* bettor, int bet);

    // 玩家出牌
    void playerPlayCards(Player* player, const Cards& cards);
    // 玩家过牌（不要）
    void playerPass(Player* player);

    // 清空所有玩家手牌，重置游戏状态（用于新一局）
    void reset();

    //清空所有玩家得分
    void clearScores();

    //得到初始牌组
    Cards& initialCards();

    //得到初始牌数量
    int initCardsCount();

    // 得到玩家下注的最高分数
    int getPlayerMaxBet();





signals:
    // 通知：地主确定
    void notifyLordConfirmed(Player* landlord);
    // 通知：玩家出牌
    void notifyPlayCards(Player* player, const Cards& cards);
    // 通知：玩家过牌
    void notifyPass(Player* player);
    // 通知：游戏结束，胜者为 winner
    void notifyGameOver(Player* winner);
    // 通知：游戏状态变化
    void notifyGameStatusChanged(GameStatus status);
    //玩家状态发生变化
    void playerStatusChanged(Player* player,PlayerStatus status);
    // 通知 UI：某个玩家刚刚决定了叫地主几分
    //   bettor: 谁决定的；bet: 几分(0=不抢,1-3)；isFirstCall: 是不是全场第一个叫正分的
    void grabLordBetDecided(Player* bettor, int bet, bool isFirstCall);

private:
    // 传递出牌权给下家
    void passTurnToNext();
    // 检查游戏是否结束（有人手牌为空）
    bool checkGameOver();
    // 判定胜负并发出信号
    void settleGame(Player* winner);
    // 启动当前玩家叫地主决策 + 通知 UI 轮到他了
    void advanceBettor();

private:

    Cards m_initialCards;          // 初始牌组（54张）
    RobotPlayer* m_leftRobot;       // 左侧机器人
    RobotPlayer* m_rightRobot;      // 右侧机器人
    UserPlayer* m_userPlayer;       // 真人玩家

    Player* m_currentPlayer;        // 当前出牌玩家
    Player* m_pendPlayer;           // 打出待应对牌的玩家
    Cards m_pendCards;              // 待应对的牌

    Cards m_bottomCards;            // 底牌（3张）
    GameStatus m_status;            // 游戏状态
    BetRecord m_betRecord;
};

#endif // GAMECONTROL_H
