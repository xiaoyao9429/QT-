#ifndef SCOREPANEL_H
#define SCOREPANEL_H

#include <QWidget>
#include <QLabel>
#include <QVector>
#include "player.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ScorePanel;
}
QT_END_NAMESPACE

// 分数面板：显示三个玩家的名字与得分
class ScorePanel : public QWidget
{
    Q_OBJECT
public:
    explicit ScorePanel(QWidget *parent = nullptr);
    ~ScorePanel() override;

    // 设置玩家（顺序：左机器人、真人、右机器人）
    void setPlayers(Player* left, Player* user, Player* right);

    // 设置玩家得分
    void setScore(Player* player, int score);

    // 增加玩家得分
    void addScore(Player* player, int delta);

    // 重置所有得分为 0
    void reset();

private:
    // 单个玩家的显示项
    struct PlayerItem
    {
        QLabel* nameLabel;      // 名字标签
        QLabel* scoreLabel;     // 得分标签
        Player* player;         // 关联的玩家指针
        int score;              // 当前得分
    };

    // 初始化显示项映射（把 ui 中的标签绑定到 PlayerItem）
    void initItems();
    // 刷新某个玩家的显示
    void refreshItem(int index);

private:
    Ui::ScorePanel* ui;
    QVector<PlayerItem> m_items;   // 三个玩家显示项
};

#endif // SCOREPANEL_H
