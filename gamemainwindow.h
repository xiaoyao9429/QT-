#ifndef GAMEMAINWINDOW_H
#define GAMEMAINWINDOW_H

#include <QMainWindow>
#include "gamecontrol.h"
#include "scorepanel.h"
#include "gamecontrol.h"
#include <QVector>
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
    void GameControlInit();
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::GameMainWindow * ui;
    GameControl* m_gameControl;
    QPixmap m_bkImage;
    QVector<Player*> m_playerList;

};
#endif // GAMEMAINWINDOW_H
