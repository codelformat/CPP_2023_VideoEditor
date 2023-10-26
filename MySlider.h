#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QtCore/QObject>
#include <QtWidgets/QSlider>

class MySlider : public QSlider
{
public:
    MySlider(QWidget *parent = nullptr);
    ~MySlider();
    void mousePressEvent(QMouseEvent *ev);
};

#endif // MYSLIDER_H
