#ifndef GAMECONTROL_H
#define GAMECONTROL_H

#include <QObject>
#include <QMap>
#include "player.h"
#include "cards.h"

class UserPlayer;
class RobotPlayer;

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

    // 发牌：给三个玩家各发 17 张，剩余 3 张作为底牌
    void dispatchCards();
    // 获取底牌
    Cards bottomCards() const;

    // 开始叫地主
    void startCallLord();
    //成为地主
    void becomeLord(Player* player);
    // 玩家叫地主（score: 0~3 分，0 表示不叫）
    void playerCallLord(Player* player, int score);

    // 开始出牌
    void startPlayCards();
    // 玩家出牌
    void playerPlayCards(Player* player, const Cards& cards);
    // 玩家过牌（不要）
    void playerPass(Player* player);

    // 清空所有玩家手牌，重置游戏状态（用于新一局）
    void reset();

    //清空所有玩家得分
    void clearScores();


signals:
    // 通知：发牌完成（参数为玩家指针，让 UI 刷新该玩家手牌）
    void notifyDispatchCards(Player* player);
    // 通知：玩家叫地主结果（玩家 + 叫分）
    void notifyCallLord(Player* player, int score);
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

private:
    // 传递出牌权给下家
    void passTurnToNext();
    // 检查游戏是否结束（有人手牌为空）
    bool checkGameOver();
    // 判定胜负并发出信号
    void settleGame(Player* winner);

private:

     Cards m_initialCards;          // 初始牌组（52张）
    RobotPlayer* m_leftRobot;       // 左侧机器人
    RobotPlayer* m_rightRobot;      // 右侧机器人
    UserPlayer* m_userPlayer;       // 真人玩家

    Player* m_currentPlayer;        // 当前出牌玩家
    Player* m_pendPlayer;           // 打出待应对牌的玩家
    Cards m_pendCards;              // 待应对的牌

    Cards m_bottomCards;            // 底牌（3张）
    int m_lordScore;                // 地主叫分（1/2/3）
    GameStatus m_status;            // 游戏状态
};

#endif // GAMECONTROL_H
