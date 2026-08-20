#include "userplayer.h"

UserPlayer::UserPlayer(QObject *parent)
    : Player(parent)
{
    // 真人玩家默认设置 type 为 Human
    setType(PlayerType::Human);
    setName("user");
}

void UserPlayer::preparePlayCards()
{
    // 真人玩家：轮到自己主动出牌
    // 此处只做状态标记，由外部UI响应后处理
    setIsThinking(true);
}

void UserPlayer::prepareTakeCards()
{
    // 真人玩家：需要接上家打出的牌
    // 此处只做状态标记，由外部UI响应后处理
    setIsThinking(true);
}

void UserPlayer::startCallLord()
{
    // 真人玩家：轮到叫地主
    // 此处只做状态标记，由外部UI显示叫/不叫按钮
    setIsThinking(true);
}
