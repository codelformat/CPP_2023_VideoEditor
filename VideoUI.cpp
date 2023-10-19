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
#include<QStackedLayout>
using namespace std;
using namespace cv;
static bool pressSlider = false;
static bool isExport = false;
static bool isColor = true;
static bool isMark = false;
extern  VideoCapture cap1;
bool isClip=false;
VideoUI::VideoUI(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    //setWindowFlags(Qt::FramelessWindowHint);

    // ע��������
    qRegisterMetaType<cv::Mat>("cv::Mat");
    // ������Ƶ
    // �źŲ۹���
    QObject::connect(VideoThread::Get(),//�ź�Դ
        SIGNAL(ViewImage1(cv::Mat)),//�ź�
        ui.src1,
        SLOT(SetImage(cv::Mat))//�ۺ���
    );

    // ������Ƶ����
    //�źŲ۹���
    QObject::connect(VideoThread::Get(),//�ź�Դ
        SIGNAL(ViewDes(cv::Mat)),//�ź�
        ui.des,
        SLOT(SetImage(cv::Mat))//�ۺ���
    );

    QObject::connect(VideoThread::Get(),//�ź�Դ
        SIGNAL(SaveEnd()),//�ź�
        this,
        SLOT(ExportEnd())//�ۺ���
        );
    QObject::connect(ui.horizontalSlider_bright,SIGNAL(valueChanged(int)),this,SLOT(do_value_bright(int)));
    QObject::connect(ui.horizontalSlider_contrast,SIGNAL(valueChanged(int)),this,SLOT(do_value_contrast(int)));
    QObject::connect(ui.bright,SIGNAL(valueChanged(int)),this,SLOT(do_slider_bright(int)));
    QObject::connect(ui.contrast,SIGNAL(valueChanged(double)),this,SLOT(do_slider_contrast(double)));
    QObject::connect(ui.right,SIGNAL(valueChanged(int)),this,SLOT(do_value_right()));
    QObject::connect(ui.left,SIGNAL(valueChanged(int)),this,SLOT(do_value_left()));
    QObject::connect(ui.playSlider,SIGNAL(valueChanged(int)),this,SLOT(do_value_cur()));
    ui.pauseButton->hide();

    ui.stackedLayout->setStackingMode(QStackedLayout::StackAll);
    connect(ui.drawRect,SIGNAL(clipSignal(double,double,double,double)),this,SLOT(do_des_clip(double,double,double,double)));

    startTimer(40); // �ɸ���fps���ö�ʱ����ʱ��
}

void VideoUI::timerEvent(QTimerEvent* e) {
    if (pressSlider) return;
    double pos = VideoThread::Get()->GetPos();
    ui.playSlider->setValue(pos*1000);
}



void VideoUI::Open()
{
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("��ѡ����Ƶ�ļ�"));
    if (name.isEmpty()) {
        return;
    }

    fileUrl = name.toLocal8Bit().data();
    if (!VideoThread::Get()->Open(fileUrl))
    {
        QMessageBox::information(this, "error", name+" open failed");
        return;
    }
    else {
        set_start_time();
        set_end_time();
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
    // ��Ƶͼ��ü�
    //isClip = false;//��ʱ��Ϊ��ȫ�ֱ�����
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
    

    // ͼ�������
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


    // ������Ƶ�ߴ�
    double w = ui.width->value();
    double h = ui.height->value();
    if (!isClip && !isPy && w > 0 && h > 0) {
        VideoFilter::Get()->Add(Task{ TASK_RESIZE, {w, h}});
    }


	// ���öԱȶȺ�����
    if (ui.bright->value() > 0 || 
        ui.contrast->value() > 1) {
        VideoFilter::Get()->Add(Task{ TASK_GAIN, 
            {(double)ui.bright->value(), ui.contrast->value()} });
	}

    // �Ҷ�ͼ
    if (ui.color->currentIndex() == 1) {
        VideoFilter::Get()->Add(Task{ TASK_GRAY });
        isColor = false;
    }

    //ͼ�����
    if (ui.mosaic->currentIndex() == 1) {
        VideoFilter::Get()->Add(Task{ TASK_MOSAIC});
    }
    else if (ui.mosaic->currentIndex() == 2) {
        VideoFilter::Get()->Add(Task{ TASK_SKETCH });
    }

    //ͼ����ת 1:90,2:180,3:270
    if (ui.rotate->currentIndex() == 1) {
        VideoFilter::Get()->Add(Task{ TASK_ROTATE90 });
    }
    else if (ui.rotate->currentIndex() == 2) {
		VideoFilter::Get()->Add(Task{ TASK_ROTATE180 });
    }
    else if (ui.rotate->currentIndex() == 3) {
		VideoFilter::Get()->Add(Task{ TASK_ROTATE270 });
    }

    //ͼ���� 1:x,2:y,3:xy
    if (ui.flip->currentIndex() == 1) {
        VideoFilter::Get()->Add(Task{ TASK_FLIPX });
    }
    else if (ui.flip->currentIndex() == 2) {
        VideoFilter::Get()->Add(Task{ TASK_FLIPY });
    }
    else if (ui.flip->currentIndex() == 3) {
        VideoFilter::Get()->Add(Task{ TASK_FILPXY });
    }

    // ���ˮӡ
    if (isMark) {
        double x = ui.mx->value();
        double y = ui.my->value();
        double a = ui.ma->value();
        VideoFilter::Get()->Add(Task{ TASK_MASK, {x, y, a} });
    }

}

void VideoUI::Export() {
    if (isExport) {
        // ֹͣ����
        VideoThread::Get()->StopSave();
        isExport = false;
        ui.exportButton->setText("Export");
        return;
    }
    // ��ʼ����
    QString name = QFileDialog::getSaveFileName(this, "Save", "out.avi"); // �Ȳ����Ǹ�ʽ
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

    // ������Ƶ
    AudioThread::Get()->ExportAudio(src, src + ".mp3", ss, t);
    string tmp = des + ".avi";

    QFile::remove(tmp.c_str());
    QFile::rename(des.c_str(), tmp.c_str());
    AudioThread::Get()->Merge(tmp, src+".mp3", des);

}

void VideoUI::Mark() {
    isMark = false;
    QString name = QFileDialog::getOpenFileName(this, "Select Image��");
    if (name.isEmpty()) {
        return;
    }
    std::string file = name.toLocal8Bit().data();
    cv::Mat mark = cv::imread(file);
    if (mark.empty()) return;

    VideoThread::Get()->SetMark(mark);
    isMark = true;
}
//���ӹ���������ֵ
void VideoUI::do_value_bright(int val){
       ui.bright->setValue(val);
}

void VideoUI::do_value_contrast(int val){
       double val_contrast=val/100.00;
       ui.contrast->setValue(val_contrast);
}
void VideoUI::do_slider_bright(int val){
       ui.horizontalSlider_bright->setValue(val);
}
void VideoUI::do_slider_contrast(double val){
       ui.horizontalSlider_contrast->setValue((int)(val*100));
}
void VideoUI::do_value_right(){

        double count = cap1.get(CAP_PROP_FRAME_COUNT);
        int fps = cap1.get(CAP_PROP_FPS);
        if (fps <= 0) fps = 25;
        int val=ui.right->value();
        int total=ui.right->maximum()-ui.right->minimum()+1;
        double rate=val*1.0/total;
        double total_time=count/fps;
        double end_time=total_time*rate;
        QString str=time_format(end_time);
        str+='/';
        str.append(time_format(total_time));
        ui.label_end_time->setText(str);}
void VideoUI::do_value_left(){



        double count = cap1.get(CAP_PROP_FRAME_COUNT);
        int fps = cap1.get(CAP_PROP_FPS);
        if (fps <= 0) fps = 25;
        int val=ui.left->value();
        int total=ui.left->maximum()-ui.left->minimum()+1;
        double rate=val*1.0/total;
        double total_time=count/fps;
        double end_time=total_time*rate;
        QString str=time_format(end_time);
        str+='/';
        str.append(time_format(total_time));
        ui.label_start_time->setText(str);


}
QString VideoUI::time_format(int time){
       QString str="";
       if(time>3600){
        int temp=time/3600;
        if(temp<10){
            str+='0';
        }
        str.append(QString::number(temp));
        str.append(':');
        time%=3600;
       }
       else{
        //str.append("00:");
       }
       if(time>60){
        int temp=time/60;
        if(temp<10){
            str.append('0');
        }
        str.append(QString::number(time/60));
        str.append(':');
        time%=60;
       }
       else{
        str.append("00:");
       }
       if(time<10){
        str+='0';
       }
       str.append(QString::number(time));
       return str;
}
void VideoUI::do_value_cur(){
       double count = cap1.get(CAP_PROP_FRAME_COUNT);
       int fps = cap1.get(CAP_PROP_FPS);
       if (fps <= 0) fps = 25;
       int val=ui.playSlider->value();
       int total=ui.playSlider->maximum()-ui.playSlider->minimum()+1;
       double rate=val*1.0/total;
       double total_time=count/fps;
       double end_time=total_time*rate;
       QString str=time_format(end_time);
       str+='/';
       str.append(time_format(total_time));
       ui.label_cur_time->setText(str);
}
void VideoUI::set_start_time(){
       double count = cap1.get(CAP_PROP_FRAME_COUNT);
       int fps = cap1.get(CAP_PROP_FPS);
       if (fps <= 0) fps = 25;
       int val=ui.left->value();
       int total=ui.left->maximum()-ui.left->minimum()+1;
       double rate=val*1.0/total;
       double total_time=count/fps;
       double end_time=total_time*rate;
       QString str=time_format(end_time);
       str+='/';
       str.append(time_format(total_time));
       ui.label_start_time->setText(str);

}
void VideoUI::set_end_time(){
       double count = cap1.get(CAP_PROP_FRAME_COUNT);
       int fps = cap1.get(CAP_PROP_FPS);
       if (fps <= 0) fps = 25;
       int val=ui.right->value();
       int total=ui.right->maximum()-ui.right->minimum()+1;
       double rate=val*1.0/total;
       double total_time=count/fps;
       double end_time=total_time*rate;
       QString str=time_format(end_time);
       str+='/';
       str.append(time_format(total_time));
       ui.label_end_time->setText(str);
}

void VideoUI::do_des_clip(double xRatio,double yRatio,double widthRatio,double heightRatio){
       VideoFilter::Get()->Clear();
       int width = cap1.get(CAP_PROP_FRAME_WIDTH);
       int height = cap1.get(CAP_PROP_FRAME_HEIGHT);
       int cx=xRatio*width;
       int cy=yRatio*height;
       int cw=widthRatio*width;
       int ch=heightRatio*height;
       if(cw<0){
        cx=cx+cw;
        cw=-cw;
       }
       if(ch<0){
        cy=cy+ch;
        ch=-ch;
       }
       //�˴���һ������δ����������Ϊ�������������϶�����ʱwidthRaito��heightRaitoΪ����Ӧ��������λ�á�
       ui.cx->setValue(cx);
       ui.cy->setValue(cy);
       ui.cw->setValue(cw);
       ui.ch->setValue(ch);
       /*if ((cx+cw)<width&&(cy+ch)<height&&cx>0&&cy>0)
       {
        isClip = true;

        VideoFilter::Get()->Add(Task{ TASK_CLIP, {(double)cx, (double)cy, (double)cw, (double)ch} });

        double w = VideoThread::Get()->width;
        double h = VideoThread::Get()->height;
        VideoFilter::Get()->Add(Task{ TASK_RESIZE, {w, h} });
       }*/
       this->Set();
}

VideoUI::~VideoUI()
{}
