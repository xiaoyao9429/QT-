#ifndef GAMEMAINWINDOW_H
#define GAMEMAINWINDOW_H

#include <QMainWindow>
#include "gamecontrol.h"
#include "scorepanel.h"
#include <QVector>
#include <QMap>
#include "cardpanel.h"
#include "dealanimator.h"
#include <QSize>
#include <QLabel>
#include <animationwindow.h>
#include "countdown.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class GameMainWindow;
}
QT_END_NAMESPACE

enum class AnimationType{
    SHUNZI,//顺子
    ZHADAN,//炸弹
    WANGZHA,//王炸
    FEIJI,//飞机
    LIANDUI,//连对
    BET//抢地主分数
};

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
    //拿到发牌对应的CardPanel（发牌发的是card）
    void dispatchCardHandle(Player* player, const Cards & cards);
    //更新扑克牌在窗口的显示
    void updatePlayerCards(Player* player);
    //显示各种动画
    void showAnimationWindow(AnimationType animationtype,int bet=0);
    //出牌前的一些ui动作
    void preparePlayingHand();
    //根据玩家性别返回语音前缀（"Man"/"Woman"），用于拼接 playEffect 的音效名
    QString voicePrefix(Player* player) const;
    //更新分数面板
    void updateScorePanel();
    //处理玩家出牌
    void onUserPlayHand();
    //处理玩家不要
    void onUserPass();
    //显示结算窗口
    void showEndingPanel();
    //初始化倒计时闹钟
    void initCountDown();

public slots:

    //轮到某人行动了
    void onPlayerStatusChanged(Player* player, GameControl::PlayerStatus status);
    //玩家抢地主后的提示信息
    void onGrabLordBet(Player* bettor, int bet, bool isFirstCall);
    // DealAnimator 动画完成回调：一张牌到达玩家位置
    void onCardArrived(Player* player);
    //玩家出牌：在出牌区显示打出的牌
    void onPlayCards(Player* player, Cards& cards);
    //玩家不要：在出牌区显示"不要"
    void onPass(Player* player);
    //地主确定：显示地主角色标识、翻开底牌等
    void onLordConfirmed(Player* landlord);
    //游戏结束：显示胜负结算
    void onGameOver(Player* winner);
    //点击手牌：切换选中状态
    void onCardClicked(CardPanel* panel);

protected:
    void paintEvent(QPaintEvent *event) override;
    //批量框选
    void mouseMoveEvent(QMouseEvent *event) override;
private:
    Ui::GameMainWindow * ui;
    GameControl* m_gameControl;
    QPixmap m_bkImage;
    QVector<Player*> m_playerList;
    QMap<Card,CardPanel*> m_cardMap;
    QSize m_cardSize;
    QPixmap m_cardBackImage;
    AnimationWindow* m_animationWindow;

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
    DealAnimator* m_animator;
    QSet<CardPanel*> m_selectCardPanels;

    //用户剩余手牌所占的rect
    QRect m_cardsRect;
    QHash<CardPanel*,QRect> m_cardRectMap;

    CountDown * m_counDown;



};

#endif // GAMEMAINWINDOW_H
