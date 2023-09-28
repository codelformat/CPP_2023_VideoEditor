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

    // 滑动条拖动
    void SetPos(int);
    // 设置过滤器
    void Set();

    //导出视频
    void Export();

private:
    Ui::VideoUIClass ui;
};
