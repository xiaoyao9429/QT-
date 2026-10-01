#include "gamemainwindow.h"
#include "scorepanel.h"
#include "Loading.h"
#include <QApplication>
#include <QIcon>
#include "endpanel.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //全局图标：任务栏 + 各窗口标题栏左上角
    a.setWindowIcon(QIcon(":/images/logo.ico"));
    //GameMainWindow w;
    // w.show();
    Loading loading ;
    loading.show();
    return a.exec();
}
