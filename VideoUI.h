#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/qmutex.h>
#include "ui_VideoUI.h"

class VideoUI : public QWidget
{
    Q_OBJECT

public:
    VideoUI(QWidget *parent = nullptr);
    void timerEvent(QTimerEvent* e);
    ~VideoUI();
public slots:
    void Open();
    void Play();
    void Pause();

    void SlidePress();
    void SlideRelease();

    // �������϶�
    void SetPos(int);
    void Left(int pos);
    void Right(int pos);

    // ���ù�����
    void Set();

    //������Ƶ
    void Export();

    //������Ƶ����
    void ExportEnd();

    // ���ˮӡ����
    void Mark();



private slots:
    void do_value_bright(int val);
    void do_value_contrast(int val);
    void do_slider_bright(int val);
    void do_slider_contrast(double val);
    void do_value_left();
    void do_value_right();
    void do_value_cur();
public slots:
    void do_des_clip(double xRatio,double yRatio,double widthRatio,double heightRatio);
private:
    Ui::VideoUIClass ui;
public:
    QString time_format(int time);
    QMutex mutex;
    void set_start_time();
    void set_end_time();

    std::string fileUrl;
};
