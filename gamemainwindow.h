#ifndef GAMEMAINWINDOW_H
#define GAMEMAINWINDOW_H

#include <QMainWindow>
#include "gamecontrol.h"
#include "scorepanel.h"
#include "gamecontrol.h"
#include <QVector>
#include <QMap>
#include "cardpanel.h"
#include <QSize>
#include <QLabel>
#include <QTimer>
QT_BEGIN_NAMESPACE
namespace Ui {
class GameMainWindow;
}
QT_END_NAMESPACE

class GameMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameMainWindow(QWidget *parent = nullptr);
    ~GameMainWindow();
    //实例化游戏控制类
    void GameControlInit();
    //初始化牌组
    void initCardMap();
    //连接按钮组信号
    void connectButtonGroup();
    //玩家窗口上下文
    void initPlayerContext();
    //初始化游戏场景
    void initGameScene();
    //处理游戏状态
    void gameStatusProcess(GameControl::GameStatus status);
    //发牌
    void dispatchCards();
    //定时器槽函数,绘制发牌动画
    void onDispatchCard();
    //绘制发牌动画
    void drawDispatchCard(Player* player,int curPos);
    //拿到发牌对应的CardPanel（发牌发的是card）
    void dispatchCardHandle(Player* player, const Cards & cards);
    //更新扑克牌在窗口的显示
    void updatePlayerCards(Player* player);

    public slots:
    void onPlayerStatusChanged(Player* player ,GameControl::PlayerStatus status);
        void onGrabLordBet(Player* player,int bet,bool isFrist);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::GameMainWindow * ui;
    GameControl* m_gameControl;
    QPixmap m_bkImage;
    QVector<Player*> m_playerList;
    QMap<Card,CardPanel*> m_cardMap;
    QSize m_cardSize;
    QPixmap m_cardBackImage;

    enum class CardAlign{horizontal,vertical};
    struct PlayerContext
    {
        //出牌区域
        QRect playHandRect;
        //放牌区域
        QRect cradRect;
        //扑克牌对齐方式
        CardAlign align;
        //扑克牌显示正面还是背面
        bool isFront;
        //提示信息
        QLabel * info;
        //玩家头像
        QLabel* roleImg;
        //玩家刚出的牌
        Cards lastCard;

    };

    QMap<Player*,PlayerContext> m_contextMap;

    QVector<CardPanel*> m_last3Cards;
    CardPanel * m_basePanel;
    CardPanel* m_moveCard;
    QPoint m_baseCardPos;
    GameControl::GameStatus m_gameStatus;
    QTimer* m_timer;
    int m_curMovePos;  // 发牌动画当前位移格子数

};
#endif // GAMEMAINWINDOW_H
