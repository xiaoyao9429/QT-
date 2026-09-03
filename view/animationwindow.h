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

    void setBetImage(int score);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::AnimationWindow *ui;
    QPixmap m_image;
};

#endif // ANIMATIONWINDOW_H
