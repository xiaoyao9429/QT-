#include "robotplayer.h"

RobotPlayer::RobotPlayer(QObject *parent)
    : Player(parent)
{
    // 机器人玩家默认设置 type 为 Robot
    setType(PlayerType::Robot);
    setName("robot");
}

void RobotPlayer::preparePlayCards()
{
    // 机器人主动出牌：AI 决策选牌后通过信号通知
    // 待牌型判断模块实现后，在此处补全选牌逻辑
    setIsThinking(true);
    // TODO: AI 选牌 → 调用 playCards → emit notifyPlayCards
    // 临时占位：直接"不要"，等后续接入AI
    emit notifyPass();
    setIsThinking(false);
}

void RobotPlayer::prepareTakeCards()
{
    // 机器人接牌：判断能否管上 pendCards
    // 待牌型判断模块实现后，在此处补全决策逻辑
    setIsThinking(true);
    // TODO: AI 判断管牌 → 要么管牌要么过
    // 临时占位：直接"不要"，等后续接入AI
    emit notifyPass();
    setIsThinking(false);
}

void RobotPlayer::startCallLord()
{
    // 机器人叫地主：根据手牌强弱决定是否叫
    // 待牌型判断模块实现后，在此处补全决策逻辑
    setIsThinking(true);
    // TODO: AI 叫地主决策
    // 临时占位：默认不叫
    setIsThinking(false);
}
