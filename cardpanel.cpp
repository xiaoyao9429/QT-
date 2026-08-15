#include "cardpanel.h"
#include <QPainter>
#include <QMouseEvent>

CardPanel::CardPanel(QWidget *parent)
    : QWidget(parent)
    , m_frontSide(true)
    , m_selected(false)
    , m_owner(nullptr)
{
}

void CardPanel::setImage(const QPixmap& image)
{
   m_frontImage = image;
   setFixedSize(image.size());
    // 图片变化后触发重绘
    update();
}

QPixmap CardPanel::image() const
{
    return m_frontImage;
}

void CardPanel::setBackImage(const QPixmap& image)
{
    m_backImage = image;
    update();
}

QPixmap CardPanel::backImage() const
{
    return m_backImage;
}

void CardPanel::setFrontSide(bool frontSide)
{
    m_frontSide = frontSide;
    update();
}

bool CardPanel::frontSide() const
{
    return m_frontSide;
}

void CardPanel::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

bool CardPanel::selected() const
{
    return m_selected;
}

void CardPanel::setCard(const Card& card)
{
    m_card = card;
}

Card CardPanel::card() const
{
    return m_card;
}

void CardPanel::setOwner(Player* owner)
{
    m_owner = owner;
}

Player* CardPanel::owner() const
{
    return m_owner;
}

void CardPanel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 根据正面/背面状态选择对应图片
    QPixmap pixmap = m_frontSide ? m_frontImage : m_backImage;
    if (pixmap.isNull()) {
        return;
    }

    // 选中的牌向上偏移10个像素
    int yOffset = m_selected ? -10 : 0;

    // 绘制卡牌图片，自适应窗口大小
    QRect drawRect = rect();
    drawRect.translate(0, yOffset);
    painter.drawPixmap(drawRect, pixmap);
}

void CardPanel::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // 发出卡牌点击信号，由外部处理选中逻辑
    emit cardClicked(this);
}
