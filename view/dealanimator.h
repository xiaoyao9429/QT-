#ifndef DEALANIMATOR_H
#define DEALANIMATOR_H

#include <QObject>
#include <QMap>
#include <QPoint>
#include <QTimer>

class CardPanel;
class Player;

// 发牌动画控制器：负责牌从发牌区移动到玩家位置的动画
// GameMainWindow 只需调用 dealToPlayer() 并监听 cardArrived 信号
class DealAnimator : public QObject
{
    Q_OBJECT
public:
    explicit DealAnimator(QObject* parent = nullptr);

    // 设置动画用的移动牌面板（从 GameMainWindow 传入）
    void setMoveCard(CardPanel* card);
    // 设置发牌起点（牌堆中央位置）
    void setBasePos(QPoint pos);
    // 设置某玩家的发牌目标位置
    void setTargetPos(Player* player, QPoint pos);

    // 开始发牌动画到指定玩家
    void dealToPlayer(Player* player);
    // 停止动画
    void stop();

signals:
    // 一张牌到达目标玩家（动画完成，可以执行业务逻辑了）
    void cardArrived(Player* player);

private slots:
    void onTimeout();

private:
    QTimer* m_timer;
    CardPanel* m_moveCard;
    QPoint m_basePos;
    QMap<Player*, QPoint> m_targetPositions;
    Player* m_currentTarget;
    int m_curMovePos;
};

#endif // DEALANIMATOR_H
