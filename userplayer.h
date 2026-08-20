#ifndef USERPLAYER_H
#define USERPLAYER_H

#include "player.h"

// 真人玩家类，由用户通过UI交互决策
class UserPlayer : public Player
{
    Q_OBJECT
public:
    explicit UserPlayer(QObject *parent = nullptr);
    ~UserPlayer() override = default;

    // 重写：准备主动出牌（通知UI让用户选牌）
    void preparePlayCards() override;
    // 重写：准备接上家打出的牌（通知UI让用户选牌或过）
    void prepareTakeCards() override;

    // 重写：开始叫地主（通知UI显示叫地主按钮）
    void startCallLord() override;
};

#endif // USERPLAYER_H
