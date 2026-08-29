#include "player.h"

Player::Player(QObject *parent)
    : QObject(parent)
    , m_role(PlayerRole::Role_Begin)
    , m_sex(PlayerSex::Sex_Begin)
    , m_type(PlayerType::Type_Begin)
    , m_direction(PlayerDirection::Direction_Begin)
    , m_isThinking(false)
    , m_isWin(false)
    , m_prevPlayer(nullptr)
    , m_nextPlayer(nullptr)
    , m_pendPlayer(nullptr)
    , m_name("player")
{
}

void Player::setName(const QString& name)
{
    m_name = name;
}

QString Player::name() const
{
    return m_name;
}

void Player::setAvatar(const QPixmap& avatar)
{
    m_avatar = avatar;
}

QPixmap Player::avatar() const
{
    return m_avatar;
}

void Player::setRole(PlayerRole role)
{
    m_role = role;
}

PlayerRole Player::role() const
{
    return m_role;
}

void Player::setSex(PlayerSex sex)
{
    m_sex = sex;
}

PlayerSex Player::sex() const
{
    return m_sex;
}

void Player::setType(PlayerType type)
{
    m_type = type;
}

PlayerType Player::type() const
{
    return m_type;
}

void Player::setDirection(PlayerDirection direction)
{
    m_direction = direction;
}

PlayerDirection Player::direction() const
{
    return m_direction;
}

void Player::setIsThinking(bool thinking)
{
    m_isThinking = thinking;
}

bool Player::isThinking() const
{
    return m_isThinking;
}

void Player::setIsWin(bool isWin)
{
    m_isWin = isWin;
}

bool Player::isWin() const
{
    return m_isWin;
}

void Player::setPrevPlayer(Player* player)
{
    m_prevPlayer = player;
}

Player* Player::prevPlayer() const
{
    return m_prevPlayer;
}

void Player::setNextPlayer(Player* player)
{
    m_nextPlayer = player;
}

Player* Player::nextPlayer() const
{
    return m_nextPlayer;
}

void Player::setPendCards(const Cards& cards)
{
    m_pendCards = cards;
}

Cards Player::pendCards() const
{
    return m_pendCards;
}

void Player::setPendPlayer(Player* player)
{
    m_pendPlayer = player;
}

Player* Player::pendPlayer() const
{
    return m_pendPlayer;
}

void Player::addCard(const Card& card)
{
    m_cards.add(card);
}

void Player::addCards(const Cards& cards)
{
    m_cards.add(cards);
}

void Player::removeCard(const Card& card)
{
    m_cards.remove(card);
}

void Player::removeCards(const Cards& cards)
{
    m_cards.remove(cards);
}

void Player::clearCards()
{
    m_cards.clear();
}

Cards Player::cards() const
{
    return m_cards;
}

void Player::setScore(double score)
{
    m_score = score;
}

double Player::score() const
{
    return m_score;
}

void Player::playCards(const Cards& cards)
{
    m_cards.remove(cards);
}

int Player::cardCount() const
{
    return m_cards.cardCount();
}

void Player::preparePlayCards()
{
    // 基类默认空实现，由子类（玩家/机器人）重写
}

void Player::prepareTakeCards()
{
    // 基类默认空实现，由子类（玩家/机器人）重写
}

void Player::startCallLord()
{
    // 基类默认空实现，由子类（玩家/机器人）重写
}

void Player::submitCallLord(int bet)
{
    // 只负责把决策抛给控制器，不做流程判断
    emit callLordDecided(this, bet);
}
