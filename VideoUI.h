#pragma once

#include <QtWidgets/QWidget>

#include <QtCore/qmutex.h>

#include <QtWidgets/QMainWindow>

#include "ui_VideoUI.h"

class VideoUI : public QMainWindow
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

    // 滑动条拖动
    void SetPos(int);
    void Left(int pos);
    void Right(int pos);

    // 设置过滤器
    void Set();

    //导出视频
    void Export();

    //导出视频结束
    void ExportEnd();

    // 添加水印按下
    void Mark();

    void PlayOrPause(bool status);

private slots:
    void do_value_bright(int val);
    void do_value_contrast(int val);
    void do_slider_bright(int val);
    void do_slider_contrast(double val);
    void do_value_left();
    void do_value_right();
    void do_value_cur();
    void on_action_open_triggered();

    void on_action_set_triggered();

    void on_action_export_triggered();

    void on_action_triggered();

    void on_action_mirror_triggered();

    void on_action_greyscale_triggered();

    void on_action_RGB_triggered();

    void on_action_size_triggered();
    void do_size_ok_cliked();

    void on_action_pyramid_triggered();
    void do_pyramid_ok_clicked();
    void do_watermark_ok_clicked();
    void on_action_watermark_triggered();

    void on_action_mosaic_triggered();

    void on_action_sketch_triggered();

    void on_action_dewatermark_triggered();

    void on_action_stream_triggered();



    void on_double_video_triggered(bool checked);

    void on_action_open_2_triggered();

    void on_action_export_2_triggered();

    void on_action_time_clip_triggered(bool checked);

public slots:
    void do_des_clip(double xRatio,double yRatio,double widthRatio,double heightRatio);
private:
    Ui::VideoUIClass ui;
public:
    QString time_format(int time);
    QMutex mutex;
    void set_start_time();
    void set_end_time();
    //void set_menu_rotation();
    bool openFile();
    void hideLayout(QLayout* layout);
    void enableLayout(QLayout* layout);
    void setLayoutVisible(QLayout* layout, bool enable);
    std::string fileUrl;

    void resetAllIcons(QGridLayout* layout);
    TabButton* findTabButton( int index,QGridLayout* layout);
public:
    static const int dockWidth;
private:
    QLabel* readyInfo=new QLabel("就绪",this);
    QLabel* playInfo=new QLabel("正在播放",this);
    QLabel* pauseInfo=new QLabel("暂停",this);
    QLabel* exportInfo=new QLabel("正在导出",this);
public:
    void changeStatus(QStatusBar* statusBar,QLabel* status=nullptr);
    //QHBoxLayout* statusLayout=new QHBoxLayout();
    //QWidget* statusWidget=new QWidget();

public slots:
    void choosePage(int index);
};
