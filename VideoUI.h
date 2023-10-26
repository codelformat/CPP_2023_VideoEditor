#pragma once

#include <QtWidgets/QWidget>
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

public slots:
    void do_des_clip(double xRatio,double yRatio,double widthRatio,double heightRatio);
private:
    Ui::VideoUIClass ui;
public:
    QString time_format(int time);
    void set_start_time();
    void set_end_time();
    //void set_menu_rotation();
    bool openFile();

    std::string fileUrl;
};
