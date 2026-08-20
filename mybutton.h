#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QObject>
#include <QPushButton>

class MyButton : public QPushButton
{
    Q_OBJECT
public:
    MyButton(QWidget *parent = nullptr);
    void setImage( QString normal, QString hover, QString pressed);
protected:
    //鼠标按下
    void mousePressEvent(QMouseEvent* ev) override;
    //鼠标释放
    void mouseReleaseEvent(QMouseEvent * ev) override;
    //鼠标进入
    void enterEvent(QEnterEvent *event) override;
    //鼠标离开
    void leaveEvent(QEvent *ev) override;
    //绘图
    void paintEvent(QPaintEvent* ev) override;

private:
    QString m_normal;
    QString m_hover;
    QString m_pressed;
    QPixmap m_pixmap;

};

#endif // MYBUTTON_H
