#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QWidget>

namespace Ui {
class ButtonGroup;
}

class ButtonGroup : public QWidget
{
    Q_OBJECT

public:
    explicit ButtonGroup(QWidget *parent = nullptr);
    ~ButtonGroup();
    enum class Panel{Start,PlayCard,PassOrPlay,CallLord,Empty};

    //初始化按钮组
    void initButtons();
    //page页切换
    void selectPage(Panel type,int curBet=0);

signals:
    //开始游戏
    void startGame();
    //出牌
    void playHand();
    //不出
    void pass();
    //抢地主,0分就是不抢
    void betPoint(int bet);

private:
    Ui::ButtonGroup *ui;
};

#endif // BUTTONGROUP_H
