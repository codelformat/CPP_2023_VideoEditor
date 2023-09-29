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
    void SlidePress();
    void SlideRelease();

    // �������϶�
    void SetPos(int);
    // ���ù�����
    void Set();

    //������Ƶ
    void Export();

    //������Ƶ����
    void ExportEnd();

private:
    Ui::VideoUIClass ui;
};
