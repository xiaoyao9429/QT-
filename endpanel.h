#ifndef ENDPANEL_H
#define ENDPANEL_H

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include "scorepanel.h"
class EndPanel : public QWidget
{
    Q_OBJECT
public:
    explicit EndPanel(bool isLord,bool isWin,QWidget *parent = nullptr);
    void setPlayers(Player* left, Player* user, Player* right);
    void setScore(Player* player, int score);

protected:
    void paintEvent(QPaintEvent* event) override;

signals:
    void continueGame();

private:
    QPixmap m_backImage;
    QLabel* m_title;
    ScorePanel* m_scorePanel;
    QPushButton* m_continue;



};

#endif // ENDPANEL_H
