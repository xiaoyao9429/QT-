#ifndef CARDPANEL_H
#define CARDPANEL_H

#include <QWidget>
#include <QPixmap>
#include "card.h"

class Player;

// 卡牌窗口类，负责绘制和显示一张扑克牌
class CardPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CardPanel(QWidget *parent = nullptr);

    // 设置卡牌正面图片
    void setImage(const QPixmap& image);
    // 获取卡牌正面图片
    QPixmap image() const;

    // 设置卡牌背面图片
    void setBackImage(const QPixmap& image);
    // 获取卡牌背面图片
    QPixmap backImage() const;

    // 设置是否正面朝上
    void setFrontSide(bool frontSide);
    bool frontSide() const;

    // 设置选中状态（选中的牌会向上偏移显示）
    void setSelected(bool selected);
    bool selected() const;

    // 设置关联的卡牌
    void setCard(const Card& card);
    Card card() const;

    // 设置卡牌所有者
    void setOwner(Player* owner);
    Player* owner() const;

protected:
    // 绘制事件：根据正面/背面、选中状态绘制卡牌
    void paintEvent(QPaintEvent* event) override;
    // 鼠标按下事件：发出卡牌点击信号
    void mousePressEvent(QMouseEvent* event) override;

signals:
    // 卡牌被点击信号
    void cardClicked(CardPanel* panel);

private:
    QPixmap m_frontImage;        // 卡牌正面图片
    QPixmap m_backImage;    // 卡牌背面图片
    bool m_frontSide;       // 是否正面朝上
    bool m_selected;        // 是否被选中
    Card m_card;            // 关联的卡牌
    Player* m_owner;        // 卡牌所有者
};

#endif // CARDPANEL_H
