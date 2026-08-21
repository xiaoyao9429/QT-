#include "gamemainwindow.h"
#include "./ui_gamemainwindow.h"
#include "buttongroup.h"
GameMainWindow::GameMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameMainWindow)
{

    ui->setupUi(this);
    m_gameControl=new GameControl(this);
    resize(1200,800);
    ui->buttonGroup->selectPage(ButtonGroup::Panel::CallLord);
}

GameMainWindow::~GameMainWindow()
{
    delete ui;
}
