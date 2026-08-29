#include "card.h"

Card::Card()
    : m_point(CardPoint::Card_Begin)
    , m_suit(CardSuit::Suit_Begin)
{
}

Card::Card(CardPoint point, CardSuit suit)
    : m_point(point)
    , m_suit(suit)
{
}

void Card::setPoint(CardPoint point)
{
    m_point = point;
}

void Card::setSuit(CardSuit suit)
{
    m_suit = suit;
}

CardPoint Card::point() const
{
    return m_point;
}

CardSuit Card::suit() const
{
    return m_suit;
}

bool Card::operator <(const Card& card) const
{
    // 先比较点数，点数相同再比较花色
    if (m_point == card.m_point) {
        return m_suit < card.m_suit;
    }
    return m_point < card.m_point;
}

bool Card::operator ==(const Card& card) const
{
    return (m_point == card.m_point) && (m_suit == card.m_suit);
}
