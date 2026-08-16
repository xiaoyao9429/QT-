#include "cards.h"
#include <algorithm>
#include <QRandomGenerator>

Cards::Cards()
{
}

void Cards::add(const Card& card)
{
    m_cards.insert(card);
}

void Cards::add(const Cards& cards)
{
    m_cards.unite(cards.m_cards);
}

void Cards::add(const QVector<Card>& cards)
{
    for (const Card& card : cards) {
        m_cards.insert(card);
    }
}

void Cards::remove(const Card& card)
{
    m_cards.remove(card);
}

void Cards::remove(const Cards& cards)
{
    m_cards.subtract(cards.m_cards);
}

void Cards::remove(const QVector<Card>& cards)
{
    for (const Card& card : cards) {
        m_cards.remove(card);
    }
}

int Cards::cardCount() const
{
    return m_cards.size();
}

void Cards::clear()
{
    m_cards.clear();
}

bool Cards::isEmpty() const
{
    return m_cards.isEmpty();
}

CardPoint Cards::maxPoint() const
{
    CardPoint maxPoint = CardPoint::Card_Begin;
    for (const Card& card : m_cards) {
        if (card.point() > maxPoint) {
            maxPoint = card.point();
        }
    }
    return maxPoint;
}

CardPoint Cards::minPoint() const
{
    CardPoint minPoint = CardPoint::Card_End;
    for (const Card& card : m_cards) {
        if (card.point() < minPoint) {
            minPoint = card.point();
        }
    }
    return minPoint;
}

int Cards::pointCount(CardPoint point) const
{
    int count = 0;
    for (const Card& card : m_cards) {
        if (card.point() == point) {
            ++count;
        }
    }
    return count;
}

bool Cards::contains(const Card& card) const
{
    return m_cards.contains(card);
}

bool Cards::contains(const Cards& cards) const
{
    for (const Card& card : cards.m_cards) {
        if (!m_cards.contains(card)) {
            return false;
        }
    }
    return true;
}

CardList Cards::toCardList(bool desc) const
{
    CardList list = m_cards.values();
    if (desc) {
        // 降序：b < a 表示 a 排在 b 前面（从大到小）
        std::sort(list.begin(), list.end(),
                  [](const Card& a, const Card& b) { return b < a; });
    } else {
        // 升序：a < b 表示 a 排在 b 前面（从小到大）
        std::sort(list.begin(), list.end(),
                  [](const Card& a, const Card& b) { return a < b; });
    }
    return list;
}

const QSet<Card>& Cards::toSet() const
{
    return m_cards;
}

Cards& Cards::operator <<(const Card& card)
{
    add(card);
    return *this;
}

Cards& Cards::operator <<(const Cards& cards)
{
    add(cards);
    return *this;
}

bool Cards::operator ==(const Cards& cards) const
{
    return m_cards == cards.m_cards;
}

Card Cards::takeRandomCard()
{
    Card card;
    if (m_cards.isEmpty()) {
        return card;
    }

    // QSet 没有随机索引，先转为 QList 再按随机下标取出
    QList<Card> list = m_cards.values();
    int index = QRandomGenerator::global()->bounded(list.size());
    card = list.at(index);
    m_cards.remove(card);
    return card;
}

Cards Cards::takeRandomCards(int count)
{
    Cards result;
    if (count <= 0 || m_cards.isEmpty()) {
        return result;
    }

    // 实际可取数量不超过集合大小
    int actualCount = qMin(count, m_cards.size());
    for (int i = 0; i < actualCount; ++i) {
        result.add(takeRandomCard());
    }
    return result;
}
