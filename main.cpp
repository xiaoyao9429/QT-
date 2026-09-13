#include "gamemainwindow.h"
#include "scorepanel.h"
#include <QApplication>
#include "endpanel.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameMainWindow w;
    w.show();
  //  w.showEndingPanel();
    // EndPanel * panel=new EndPanel(true,true);
    // panel->show();
    // EndPanel * panel2=new EndPanel(true,false);
    // panel2->show();
    // EndPanel * panel3=new EndPanel(false,true);
    // panel3->show();
    // EndPanel * panel4=new EndPanel(false,false);
    // panel4->show();
    return a.exec();
}
