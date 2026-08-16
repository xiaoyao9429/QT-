#include "player.h"

Player::Player(QObject *parent)
    : QObject(parent)
    , m_role(PlayerRole::Role_Begin)
    , m_sex(PlayerSex::Sex_Begin)
    , m_isThinking(false)
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

void Player::setIsThinking(bool thinking)
{
    m_isThinking = thinking;
}

bool Player::isThinking() const
{
    return m_isThinking;
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

void Player::startPlayCards()
{
    // 基类默认空实现，由子类（玩家/机器人）重写
}
