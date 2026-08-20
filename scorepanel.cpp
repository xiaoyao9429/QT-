#include "scorepanel.h"
#include "ui_scorepanel.h"

ScorePanel::ScorePanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScorePanel)
{
    ui->setupUi(this);
    initItems();
}

ScorePanel::~ScorePanel()
{
    delete ui;
}

void ScorePanel::initItems()
{
    // 把 ui 中创建的标签按顺序绑定到 PlayerItem
    m_items.resize(3);
    m_items[0].nameLabel = ui->nameLabel1;
    m_items[0].scoreLabel = ui->scoreLabel1;
    m_items[1].nameLabel = ui->nameLabel2;
    m_items[1].scoreLabel = ui->scoreLabel2;
    m_items[2].nameLabel = ui->nameLabel3;
    m_items[2].scoreLabel = ui->scoreLabel3;

    for (int i = 0; i < 3; ++i) {
        m_items[i].player = nullptr;
        m_items[i].score = 0;
    }
}

void ScorePanel::setPlayers(Player* left, Player* user, Player* right)
{
    // 顺序：左机器人 → 真人 → 右机器人
    m_items[0].player = left;
    m_items[1].player = user;
    m_items[2].player = right;

    for (int i = 0; i < 3; ++i) {
        if (m_items[i].player) {
            m_items[i].nameLabel->setText(m_items[i].player->name());
        }
    }
}

void ScorePanel::setScore(Player* player, int score)
{
    if (!player) {
        return;
    }
    for (int i = 0; i < 3; ++i) {
        if (m_items[i].player == player) {
            m_items[i].score = score;
            refreshItem(i);
            return;
        }
    }
}

void ScorePanel::addScore(Player* player, int delta)
{
    if (!player) {
        return;
    }
    for (int i = 0; i < 3; ++i) {
        if (m_items[i].player == player) {
            m_items[i].score += delta;
            refreshItem(i);
            return;
        }
    }
}

void ScorePanel::reset()
{
    for (int i = 0; i < 3; ++i) {
        m_items[i].score = 0;
        refreshItem(i);
    }
}

void ScorePanel::refreshItem(int index)
{
    if (index < 0 || index >= m_items.size()) {
        return;
    }
    m_items[index].scoreLabel->setText(QString::number(m_items[index].score));
}
