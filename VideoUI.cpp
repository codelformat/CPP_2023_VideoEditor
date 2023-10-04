#include "VideoUI.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <iostream>
#include <string>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <QtCore/QFile>
#include "VideoThread.h"
#include "VideoFilter.h"
#include "AudioThread.h"
using namespace std;
static bool pressSlider = false;
static bool isExport = false;
static bool isColor = true;
static bool isMark = false;

VideoUI::VideoUI(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    //setWindowFlags(Qt::FramelessWindowHint);

    // 注册新类型
    qRegisterMetaType<cv::Mat>("cv::Mat");
    // 输入视频
    // 信号槽关联
    QObject::connect(VideoThread::Get(),//信号源
        SIGNAL(ViewImage1(cv::Mat)),//信号
        ui.src1,
        SLOT(SetImage(cv::Mat))//槽函数
    );

    // 导出视频结束
    //信号槽关联
    QObject::connect(VideoThread::Get(),//信号源
        SIGNAL(ViewDes(cv::Mat)),//信号
        ui.des,
        SLOT(SetImage(cv::Mat))//槽函数
    );

    QObject::connect(VideoThread::Get(),//信号源
        SIGNAL(SaveEnd()),//信号
        this,
        SLOT(ExportEnd())//槽函数
        );
    ui.pauseButton->hide();

    startTimer(40); // 可根据fps设置定时器的时间
}

void VideoUI::timerEvent(QTimerEvent* e) {
    if (pressSlider) return;
    double pos = VideoThread::Get()->GetPos();
    ui.playSlider->setValue(pos*1000);
}



void VideoUI::Open()
{
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择视频文件"));
    if (name.isEmpty()) {
        return;
    }

    string file = name.toLocal8Bit().data();
    if (!VideoThread::Get()->Open(file))
    {
        QMessageBox::information(this, "error", name+" open failed");
        return;
    }
    else {
        Play();
    }

}

void VideoUI::Play() {
    if(VideoThread::Get()->isThreadOpen())
    {
        ui.pauseButton->show();
        ui.pauseButton->setGeometry(ui.playButton->geometry());
        this->Right(999);
        this->Left(0);
        VideoThread::Get()->Play();
        ui.playButton->hide();
    }
}

void VideoUI::Pause() {
    ui.playButton->show();
    VideoThread::Get()->Pause();
    ui.pauseButton->hide();
}

void VideoUI::SlidePress()
{
    pressSlider = true;
}

void VideoUI::SlideRelease()
{
    pressSlider = false;
}

void VideoUI::SetPos(int pos)
{
	VideoThread::Get()->Seek((double)pos / 1000.);
}

void VideoUI::Left(int pos) {
    VideoThread::Get()->SetBegin((double)pos / 1000.);
    SetPos(pos);

}
void VideoUI::Right(int pos) {
    cout << "Set end!" << " " << (double)pos / 1000. << endl;
    VideoThread::Get()->SetEnd((double)pos / 1000.);
}

void VideoUI::Set()
{
    isColor = true;
    VideoFilter::Get()->Clear();
    // 视频图像裁剪
    bool isClip = false;
    int cx = ui.cx->value();
    int cy = ui.cy->value();
    int cw = ui.cw->value();
    int ch = ui.ch->value();
    if (cx + cy + cw + ch > 0)
    {
        isClip = true;

        VideoFilter::Get()->Add(Task{ TASK_CLIP, {(double)cx, (double)cy, (double)cw, (double)ch} });

        double w = VideoThread::Get()->width;
        double h = VideoThread::Get()->height;
        VideoFilter::Get()->Add(Task{ TASK_RESIZE, {w, h} });
    }
    

    // 图像金字塔
    bool isPy = false;
    int down = ui.pydown->value();
    int up = ui.pyup->value();
    if (down > 0) {
        isPy = true;
		VideoFilter::Get()->Add(Task{ TASK_PYDOWN, {(double)down, (double)up} });
        int w = VideoThread::Get()->width;
        int h = VideoThread::Get()->height;
        for (int i = 0; i < down; i++)
        {
			w /= 2;
            h /= 2;
		}
        ui.width->setValue(w);
		ui.height->setValue(h);
    }
    if (up > 0) {
        isPy = true;
        VideoFilter::Get()->Add(Task{ TASK_PYUP, {(double)down, (double)up} });
        int w = VideoThread::Get()->width;
        int h = VideoThread::Get()->height;
        for (int i = 0; i < up; i++)
        {
			w *= 2;
			h *= 2;
		}
        ui.width->setValue(w);
        ui.height->setValue(h);
    }


    // 调整视频尺寸
    double w = ui.width->value();
    double h = ui.height->value();
    if (!isClip && !isPy && w > 0 && h > 0) {
        VideoFilter::Get()->Add(Task{ TASK_RESIZE, {w, h}});
    }


	// 设置对比度和亮度
    if (ui.bright->value() > 0 || 
        ui.contrast->value() > 1) {
        VideoFilter::Get()->Add(Task{ TASK_GAIN, 
            {(double)ui.bright->value(), ui.contrast->value()} });
	}

    // 灰度图
    if (ui.color->currentIndex() == 1) {
        VideoFilter::Get()->Add(Task{ TASK_GRAY });
        isColor = false;
    }

    //图像旋转 1:90,2:180,3:270
    if (ui.rotate->currentIndex() == 1) {
        VideoFilter::Get()->Add(Task{ TASK_ROTATE90 });
    }
    else if (ui.rotate->currentIndex() == 2) {
		VideoFilter::Get()->Add(Task{ TASK_ROTATE180 });
    }
    else if (ui.rotate->currentIndex() == 3) {
		VideoFilter::Get()->Add(Task{ TASK_ROTATE270 });
    }

    //图像镜像 1:x,2:y,3:xy
    if (ui.flip->currentIndex() == 1) {
        VideoFilter::Get()->Add(Task{ TASK_FLIPX });
    }
    else if (ui.flip->currentIndex() == 2) {
        VideoFilter::Get()->Add(Task{ TASK_FLIPY });
    }
    else if (ui.flip->currentIndex() == 3) {
        VideoFilter::Get()->Add(Task{ TASK_FILPXY });
    }

    // 添加水印
    if (isMark) {
        double x = ui.mx->value();
        double y = ui.my->value();
        double a = ui.ma->value();
        VideoFilter::Get()->Add(Task{ TASK_MASK, {x, y, a} });
    }

}

void VideoUI::Export() {
    if (isExport) {
        // 停止导出
        VideoThread::Get()->StopSave();
        isExport = false;
        ui.exportButton->setText("Export");
        return;
    }
    // 开始导出
    QString name = QFileDialog::getSaveFileName(this, "Save", "out.avi"); // 先不考虑格式
    if (name.isEmpty()) return;
    std::string filename = name.toLocal8Bit().data();
    int w = ui.width->value();
    int h = ui.height->value();

    if (VideoThread::Get()->StartSave(filename,w,h, isColor))
    {
        isExport = true;
        ui.exportButton->setText("Stop");
        return;
    }
}

void VideoUI::ExportEnd() {
    isExport = false;
    //QString name = "Export";
    ui.exportButton->setText("Export");
    string src = VideoThread::Get()->srcFile;
    string des = VideoThread::Get()->desFile;
    int ss = 0;
    int t = 0;
    ss = VideoThread::Get()->totalMs * ((double)ui.left->value() / 1000.);
    t = VideoThread::Get()->totalMs * ((double)ui.right->value() / 1000.) - ss;

    // 处理音频
    AudioThread::Get()->ExportAudio(src, src + ".mp3", ss, t);
    string tmp = des + ".avi";

    QFile::remove(tmp.c_str());
    QFile::rename(des.c_str(), tmp.c_str());
    AudioThread::Get()->Merge(tmp, src+".mp3", des);

}

void VideoUI::Mark() {
    isMark = false;
    QString name = QFileDialog::getOpenFileName(this, "Select Image：");
    if (name.isEmpty()) {
        return;
    }
    std::string file = name.toLocal8Bit().data();
    cv::Mat mark = cv::imread(file);
    if (mark.empty()) return;

    VideoThread::Get()->SetMark(mark);
    isMark = true;
}

VideoUI::~VideoUI()
{}
