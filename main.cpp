#include "gamemainwindow.h"
#include "scorepanel.h"
#include "Loading.h"
#include <QApplication>
#include "endpanel.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //GameMainWindow w;
    // w.show();
    Loading loading ;
    loading.show();
    return a.exec();
}
