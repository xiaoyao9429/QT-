#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPixmap>
#include <QVector>
#include "card.h"
#include "cards.h"

// 玩家角色
enum class PlayerRole
{
    Role_Begin,         // 起始标记
    Lord,               // 地主
    Farmer,             // 农民
    Role_End            // 结束标记
};

// 玩家性别
enum class PlayerSex
{
    Sex_Begin,          // 起始标记
    Male,               // 男
    Female,             // 女
    Sex_End             // 结束标记
};

// 玩家基类，可派生出玩家类和机器人类
class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
    virtual ~Player() = default;

    // 设置玩家名字
    void setName(const QString& name);
    QString name() const;

    // 设置头像
    void setAvatar(const QPixmap& avatar);
    QPixmap avatar() const;

    // 设置角色
    void setRole(PlayerRole role);
    PlayerRole role() const;

    // 设置性别
    void setSex(PlayerSex sex);
    PlayerSex sex() const;

    // 设置是否在思考
    void setIsThinking(bool thinking);
    bool isThinking() const;

    // 添加卡牌
    void addCard(const Card& card);
    void addCards(const Cards& cards);

    // 移除卡牌
    void removeCard(const Card& card);
    void removeCards(const Cards& cards);

    // 清空卡牌
    void clearCards();
    // 获取卡牌集合
    Cards cards() const;

    // 卡牌数量
    int cardCount() const;

    // 虚函数：准备出牌（子类可重写，如机器人AI思考）
    virtual void preparePlayCards();
    // 虚函数：准备接牌（子类可重写）
    virtual void prepareTakeCards();

    // 虚函数：开始叫地主
    virtual void startCallLord();
    // 虚函数：开始出牌
    virtual void startPlayCards();

signals:
    // 通知将要出牌
    void notifyPlayCards(const Cards& cards);
    // 通知将要接牌
    void notifyTakeCards(const Cards& cards);
    // 通知不要
    void notifyPass();

protected:
    QString m_name;             // 名字
    QPixmap m_avatar;           // 头像
    PlayerRole m_role;          // 角色（地主/农民）
    PlayerSex m_sex;            // 性别
    bool m_isThinking;          // 是否在思考
    Cards m_cards;              // 持有的卡牌
};

#endif // PLAYER_H
