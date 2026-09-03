#ifndef ROBOTPLAYER_H
#define ROBOTPLAYER_H

#include "player.h"

// 机器人玩家类，由AI自动决策
class RobotPlayer : public Player
{
    Q_OBJECT
public:
    explicit RobotPlayer(QObject *parent = nullptr);
    ~RobotPlayer() override = default;

    // 重写：准备主动出牌（AI思考并选择出牌）
    void preparePlayCards() override;
    // 重写：准备接上次出牌的玩家打出的牌（AI思考并选择跟牌或过）
    void prepareTakeCards() override;

    // 重写：开始叫地主（AI判断是否叫地主）
    void startCallLord() override;
};

#endif // ROBOTPLAYER_H
