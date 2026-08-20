#include "gamemainwindow.h"
#include "scorepanel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameMainWindow w;
    w.show();

    // ScorePanel sp1;
    // sp1.show();
    return a.exec();
}
