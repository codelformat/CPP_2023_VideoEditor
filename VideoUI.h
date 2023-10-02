#pragma once

#include <QtWidgets/QWidget>
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



private:
    Ui::VideoUIClass ui;
};
