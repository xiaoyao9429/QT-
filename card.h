#ifndef CARD_H
#define CARD_H

#include <QString>

// 牌的花色
enum class CardSuit
{
    Suit_Begin,         // 起始标记
    Diamond,            // 方块
    Club,               // 梅花
    Heart,              // 红桃
    Spade,              // 黑桃
    Suit_End            // 结束标记
};

// 牌的点数
enum class CardPoint
{
    Card_Begin,         // 起始标记
    Card_3,             // 3
    Card_4,             // 4
    Card_5,             // 5
    Card_6,             // 6
    Card_7,             // 7
    Card_8,             // 8
    Card_9,             // 9
    Card_10,            // 10
    Card_J,             // J
    Card_Q,             // Q
    Card_K,             // K
    Card_A,             // A
    Card_2,             // 2
    Card_SJ,            // 小王
    Card_BJ,            // 大王
    Card_End            // 结束标记
};

// 扑克牌类
class Card
{
public:
    Card();
    Card(CardPoint point, CardSuit suit);

    // 设置点数
    void setPoint(CardPoint point);
    // 设置花色
    void setSuit(CardSuit suit);
    // 获取点数
    CardPoint point() const;
    // 获取花色
    CardSuit suit() const;

    // 重载小于运算符，用于排序
    bool operator <(const Card& card) const;
    // 重载等于运算符
    bool operator ==(const Card& card) const;

private:
    CardPoint m_point;  // 牌的点数
    CardSuit m_suit;    // 牌的花色
};

#endif // CARD_H
