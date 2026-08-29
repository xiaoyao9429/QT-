#ifndef CARDS_H
#define CARDS_H

#include <QSet>
#include <QVector>
#include "card.h"

// 卡牌列表类型别名（QVector 顺序容器，用于排序后的有序输出）
using CardList = QVector<Card>;

// 卡牌集合类，封装 QSet<Card>，用于玩家手牌管理
class Cards
{
public:
    Cards();

    // 添加单张卡牌
    void add(const Card& card);
    // 添加一个卡牌集合
    void add(const Cards& cards);
    // 添加一组卡牌
    void add(const QVector<Card>& cards);
    void add(const QVector <Cards> & cards);

    // 移除单张卡牌
    void remove(const Card& card);
    // 移除一个卡牌集合
    void remove(const Cards& cards);
    // 移除一组卡牌
    void remove(const QVector<Card>& cards);
    void remove(const QVector<Cards>& cards);

    // 卡牌数量
    int cardCount() const;
    // 清空所有卡牌
    void clear();
    // 是否为空
    bool isEmpty() const;

    // 获取最大点数
    CardPoint maxPoint() const;
    // 获取最小点数
    CardPoint minPoint() const;
    // 统计某点数出现的次数（用于判断对子、三张、炸弹等）
    int pointCount(CardPoint point) const;

    // 是否包含某张卡牌
    bool contains(const Card& card) const;
    // 是否包含整个卡牌集合
    bool contains(const Cards& cards) const;

    // 转为有序列表（默认降序）
    CardList toCardList(bool desc = true) const;

    // 获取内部 QSet 引用
    const QSet<Card>& toSet() const;

    // 重载 << 运算符，便于链式添加
    Cards& operator <<(const Card& card);
    Cards& operator <<(const Cards& cards);

    // 随机取出一张卡牌（从集合中移除并返回）
    Card takeRandomCard();
    // 随机取出 count 张卡牌（从集合中移除并返回）
    Cards takeRandomCards(int count);

    // 重载等于运算符
    bool operator ==(const Cards& cards) const;

private:
    QSet<Card> m_cards;     // 卡牌集合（无序、去重）
};

#endif // CARDS_H
