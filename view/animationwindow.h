#ifndef ANIMATIONWINDOW_H
#define ANIMATIONWINDOW_H

#include <QWidget>
#include<QPixmap>
namespace Ui {
class AnimationWindow;
}

class AnimationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AnimationWindow(QWidget *parent = nullptr);
    ~AnimationWindow();
    enum Type{Sequence, Pair};
    void setBetImage(int score);
    // 显示顺子和连对
    void showSequence(Type type);
    // 显示王炸
    void showJokerBomb();
    // 显示炸弹
    void showBomb();
    // 显示飞机
    void showPlane();

protected:
    void paintEvent(QPaintEvent *event) override;

private:

    Ui::AnimationWindow *ui;
    QPixmap m_image;
    int m_index = 0;
    int m_x = 0;

};

#endif // ANIMATIONWINDOW_H
