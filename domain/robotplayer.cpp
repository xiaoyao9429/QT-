#include "robotplayer.h"
#include "strategy.h"
#include <QTimer>
#include <QRandomGenerator>

RobotPlayer::RobotPlayer(QObject *parent)
    : Player(parent)
{
    // 机器人玩家默认设置 type 为 Robot
    setType(PlayerType::Robot);
    setName("robot");
}

void RobotPlayer::preparePlayCards()
{
    // 机器人主动出牌：基于手牌用 Strategy AI 决策
    setIsThinking(true);
    // 模拟思考 600~1500ms，避免出牌过快造成体验不真实
    int thinkMs = QRandomGenerator::global()->bounded(600, 1600);
    QTimer::singleShot(thinkMs, this, [this]() {
        Cards hand = this->cards();
        Strategy st(this, hand);
        Cards outCards = st.makeStrategy();
        setIsThinking(false);
        if (outCards.isEmpty()) {
            emit notifyPass();
        } else {
            emit notifyPlayCards(outCards);
        }
    });
}

void RobotPlayer::prepareTakeCards()
{
    // 机器人接牌：Strategy::makeStrategy() 会自动检测 pendPlayer 非自己，
    // 走"找更大牌 + whetherToBeat 判断"分支
    setIsThinking(true);
    int thinkMs = QRandomGenerator::global()->bounded(600, 1600);
    QTimer::singleShot(thinkMs, this, [this]() {
        Cards hand = this->cards();
        Strategy st(this, hand);
        Cards beatCards = st.makeStrategy();
        setIsThinking(false);
        if (beatCards.isEmpty()) {
            emit notifyPass();
        } else {
            emit notifyTakeCards(beatCards);
        }
    });
}

void RobotPlayer::startCallLord()
{
    setIsThinking(true);
    // 简单AI：延迟1秒后随机决定叫几分（0-3）
    // 后续可替换为基于手牌强度的评估
    QTimer::singleShot(2000, this, [this]() {
        int bet = QRandomGenerator::global()->bounded(0, 4);
        submitCallLord(bet);
        setIsThinking(false);
    });
}
