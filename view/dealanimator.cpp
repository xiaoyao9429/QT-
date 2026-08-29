#include "dealanimator.h"
#include "cardpanel.h"
#include "player.h"

DealAnimator::DealAnimator(QObject* parent)
    : QObject(parent)
    , m_moveCard(nullptr)
    , m_currentTarget(nullptr)
    , m_curMovePos(0)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DealAnimator::onTimeout);
}

void DealAnimator::setMoveCard(CardPanel* card)
{
    m_moveCard = card;
}

void DealAnimator::setBasePos(QPoint pos)
{
    m_basePos = pos;
}

void DealAnimator::setTargetPos(Player* player, QPoint pos)
{
    m_targetPositions[player] = pos;
}

void DealAnimator::dealToPlayer(Player* player)
{
    m_currentTarget = player;
    m_curMovePos = 0;
    if (m_moveCard) {
        m_moveCard->move(m_basePos);
        m_moveCard->show();
    }
    m_timer->start(15);
}

void DealAnimator::stop()
{
    m_timer->stop();
    if (m_moveCard) {
        m_moveCard->hide();
    }
}

void DealAnimator::onTimeout()
{
    if (!m_currentTarget || !m_moveCard) {
        m_timer->stop();
        return;
    }

    m_curMovePos += 15;

    // 线性插值：从起点到目标位置
    QPoint target = m_targetPositions.value(m_currentTarget, m_basePos);
    // 先乘后除，减少整数除法误差
    int x = m_basePos.x() + m_curMovePos * (target.x() - m_basePos.x()) / 100;
    int y = m_basePos.y() + m_curMovePos * (target.y() - m_basePos.y()) / 100;
    m_moveCard->move(x, y);

    if (m_curMovePos >= 100) {
        m_moveCard->hide();
        emit cardArrived(m_currentTarget);
    }
}
