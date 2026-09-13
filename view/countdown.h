#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QTimer>
class CountDown : public QWidget
{
    Q_OBJECT
public:
    explicit CountDown(QWidget *parent = nullptr);
    void showCountDown();
    void stopCountDown();

signals:
    //给ui提示时间不多了
    void notMuchTimer();

    //时间耗尽了，强制用户不要，下家出牌
    void timeOut();
protected:
    void paintEvent(QPaintEvent* event) override;


private:
    QPixmap m_clock;
    QPixmap m_number;
    QTimer * m_timer;
    int m_count;
};

#endif // COUNTDOWN_H
