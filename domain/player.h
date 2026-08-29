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

// 玩家类型
enum class PlayerType
{
    Type_Begin,         // 起始标记
    Human,              // 真人玩家
    Robot,              // 机器人
    Type_End            // 结束标记
};

// 玩家显示方位（在游戏窗口中的位置）
enum class PlayerDirection
{
    Direction_Begin,    // 起始标记
    Left,               // 左侧
    Right,              // 右侧
    Bottom,             // 底部（玩家自己）
    Direction_End       // 结束标记
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

    // 设置玩家类型（真人/机器人）
    void setType(PlayerType type);
    PlayerType type() const;

    // 设置显示方位
    void setDirection(PlayerDirection direction);
    PlayerDirection direction() const;

    // 设置是否在思考
    void setIsThinking(bool thinking);
    bool isThinking() const;

    // 设置是否获胜
    void setIsWin(bool isWin);
    bool isWin() const;

    // 设置上家玩家
    void setPrevPlayer(Player* player);
    Player* prevPlayer() const;
    // 设置下家玩家
    void setNextPlayer(Player* player);
    Player* nextPlayer() const;

    // 设置要应对的牌（需要管的牌）
    void setPendCards(const Cards& cards);
    Cards pendCards() const;
    // 设置打出该牌的玩家
    void setPendPlayer(Player* player);
    Player* pendPlayer() const;

    // 添加手牌
    void addCard(const Card& card);
    void addCards(const Cards& cards);

    // 移除手牌
    void removeCard(const Card& card);
    void removeCards(const Cards& cards);

    // 清空所有手牌
    void clearCards();
    // 获取手牌集合
    Cards cards() const;\

    // 设置当前得分
    void setScore(double score);
    // 获取当前得分
    double score() const;

   

    // 卡牌数量
    int cardCount() const;
    
    // 出牌：从手牌中移除指定卡牌
    void playCards(const Cards& cards);

    // 虚函数：准备主动出牌（自己起头，子类可重写，如机器人AI思考）
    virtual void preparePlayCards();
    // 虚函数：准备接上家打出的牌（被动跟牌，子类可重写）
    virtual void prepareTakeCards();

    // 虚函数：准备叫地主
    virtual void startCallLord();

    // 子类通过它向控制器提交自己的叫地主决定（bet: 0不抢, 1-3分）
    // 内部向控制器发出 callLordDecided 信号，不做任何业务判断
    void submitCallLord(int bet);

signals:
    // 通知将要出牌
    void notifyPlayCards(const Cards& cards);
    // 通知将要接牌
    void notifyTakeCards(const Cards& cards);
    // 通知不要
    void notifyPass();
    // 叫地主决策完成：player 是决策者自己，bet 是叫分(0-3)
    void callLordDecided(Player* bettor, int bet);

protected:
    QString m_name;             // 名字
    QPixmap m_avatar;           // 头像
    PlayerRole m_role;          // 角色（地主/农民）
    PlayerSex m_sex;            // 性别
    PlayerType m_type;          // 玩家类型（真人/机器人）
    PlayerDirection m_direction;// 显示方位
    bool m_isThinking;          // 是否在思考
    bool m_isWin;               // 是否获胜
    Player* m_prevPlayer;       // 上家玩家
    Player* m_nextPlayer;       // 下家玩家
    Cards m_pendCards;          // 要应对的牌（需要管的牌）
    Player* m_pendPlayer;       // 打出要应对牌的玩家
    Cards m_cards;              // 持有的卡牌
    double m_score;             // 当前得分
};

#endif // PLAYER_H
