#include "VideoUI.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QMenu>
#include <iostream>
#include <string>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <QtCore/QFile>
#include "VideoThread.h"
#include "VideoFilter.h"
#include "VideoTranscoder.h"
#include "AudioThread.h"
#include "DrawingWidget.h"
#include<QToolButton>
#include<QInputDialog>
#include <QFormLayout>
#include"MyDialog.h"
#include"BasicDialog.h"
using namespace std;
using namespace cv;
static bool pressSlider = false;
static bool isExport = false;
static bool isColor = true;
static bool isMark = false;
extern  VideoCapture cap1;
bool isok=false;
bool isClip=false;
MyDialog*dialog_size=nullptr;
//QPushButton* btn_size_ok;
//QPushButton* btn_size_cancel;
int Index_{0};
//QSpinBox* spinBoxWidth;
//QSpinBox* spinBoxHeight;
//QDialog*dialog;
VideoUI::VideoUI(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    //setWindowFlags(Qt::FramelessWindowHint);
    //set_menu_rotation();
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
    QObject::connect(ui.horizontalSlider_bright,SIGNAL(valueChanged(int)),this,SLOT(do_value_bright(int)));
    QObject::connect(ui.horizontalSlider_contrast,SIGNAL(valueChanged(int)),this,SLOT(do_value_contrast(int)));
    QObject::connect(ui.bright,SIGNAL(valueChanged(int)),this,SLOT(do_slider_bright(int)));
    QObject::connect(ui.contrast,SIGNAL(valueChanged(double)),this,SLOT(do_slider_contrast(double)));
    QObject::connect(ui.right,SIGNAL(valueChanged(int)),this,SLOT(do_value_right()));
    QObject::connect(ui.left,SIGNAL(valueChanged(int)),this,SLOT(do_value_left()));
    QObject::connect(ui.playSlider,SIGNAL(valueChanged(int)),this,SLOT(do_value_cur()));

    ui.pauseButton->hide();
    //组件的位置可能要调整下
//    ui.color->hide();
//    ui.ma->hide();
//    ui.mx->hide();
//    ui.my->hide();
//    ui.width->hide();
//    ui.pydown->hide();
//    ui.flip->hide();
//    ui.rotate->hide();
//    ui.height->hide();
//    ui.mosaic->hide();


    ui.stackedLayout->QStackedLayout::setStackingMode(QStackedLayout::StackAll);
    connect(ui.drawRect,SIGNAL(clipSignal(double,double,double,double)),this,SLOT(do_des_clip(double,double,double,double)));

    startTimer(40); // 可根据fps设置定时器的时间
}

void VideoUI::timerEvent(QTimerEvent* e) {
    if (pressSlider) return;
    double pos = VideoThread::Get()->GetPos();
    ui.playSlider->setValue(pos*1000);
}



void VideoUI::Open()
{
    //QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择视频文件"));
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("select file name"));
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
bool VideoUI::openFile(){
    return false;
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
    //isClip = false;//暂时改为了全局变量；
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

    //图像打码
    if (ui.mosaic->currentIndex() == 1) {
        VideoFilter::Get()->Add(Task{ TASK_MOSAIC});
    }
    else if (ui.mosaic->currentIndex() == 2) {
        VideoFilter::Get()->Add(Task{ TASK_SKETCH });
    }
    else if (ui.mosaic->currentIndex() == 3) {
        /*const char* outUrl = "rtmp://localhost/live";
        auto vt = VideoTranscoder::Get(fileUrl, outUrl);
        vt->open();
        mutex.lock();
        std::thread transcode_thread(&VideoTranscoder::transcode, vt);
        string cmd = "ffplay ";
        cmd += outUrl;
        system(cmd.c_str());*/

       // mutex.unlock();
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
       ui.action_export->setText("Export");
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
        ui.action_export->setText("Stop");
        return;
    }
}

void VideoUI::ExportEnd() {
    isExport = false;
    //QString name = "Export";
    ui.action_export->setText("Export");
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
//连接滚动条和数值
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
//void VideoUI::set_menu_rotation(){
//      // ui.action->setPopupMode(QToolButton::InstantPopup);
//       QMenu*menu=new QMenu();
//       menu->addAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical),tr("90"));
//       menu->addAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion),tr("180"));
//       menu->addAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion),tr("270"));
//       ui.action->setMenu(menu);
//}

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
       //此处有一问题暂未处理。即矩形为从右下往左上拖动，此时widthRaito和heightRaito为负，应更换顶点位置。
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

//可以将原来ui页面的同样功能的装置隐藏
//消息对话框里要改一下样式表  把蓝色的按钮换成别的
void VideoUI::on_action_open_triggered()
{
       this->Open();
}


void VideoUI::on_action_set_triggered()
{
       this->Set();
}


void VideoUI::on_action_export_triggered()
{
       this->Export();
}



void VideoUI::on_action_triggered()
{      QStringList items;
       items<<"0"<<"90°"<<"180°"<<"270°";
       QString dlg="选择角度";
       QString textlabel="选择角度";
       int curIndex=Index_;
       bool editable=false;
       bool ok=false;
       QString text=QInputDialog::getItem(this,dlg,textlabel,items,curIndex,editable,&ok);
       if (text==(QString("90°"))) {
        ui.rotate->setCurrentIndex(1);
       }
       else if (text==QString("180°")) {
        ui.rotate->setCurrentIndex(2);
       }
       else if (text==QString("270°")) {
        ui.rotate->setCurrentIndex(3) ;}
       if(ok){
       this->Set();
        Index_=ui.rotate->currentIndex();}
}

//可以尝试对话框里改成中文
void VideoUI::on_action_mirror_triggered()
{
       QStringList items;

       items<<"no"<<"Vertical flip"<<"Horizontal flip"<<"Bidirectional flip";
       QString dlg="set flip";
       QString textlabel="choose flip";
       int curIndex=Index_;
       bool editable=false;
       bool ok=false;
       QString text=QInputDialog::getItem(this,dlg,textlabel,items,curIndex,editable,&ok);

       if (text==(QString("Vertical flip"))) {
        ui.flip->setCurrentIndex(1);

}
        else if (text==QString("Horizontal flip")) {
            ui.flip->setCurrentIndex(2);
        }
        else if (text==QString("Bidirectional flip")) {
            ui.rotate->setCurrentIndex(3) ;}

        if(ok){
            this->Set();
            Index_=ui.flip->currentIndex();}
    }



void VideoUI::on_action_greyscale_triggered()
{

    QString dlgTitle="消息框";
    QString strInfo="是否使用灰度图模式";
    QMessageBox::StandardButton  defaultBtn=QMessageBox::NoButton; //缺省按钮
    QMessageBox::StandardButton result;//返回选择的按钮
    result=QMessageBox::question(this, dlgTitle, strInfo,
                               QMessageBox::Yes|QMessageBox::No |QMessageBox::Cancel,
                               defaultBtn);
    if (result==QMessageBox::Yes)
       ui.color->setCurrentIndex(1);
    else if(result==QMessageBox::No){}

    else if(result==QMessageBox::Cancel){}

    this->Set();
}


void VideoUI::on_action_RGB_triggered()
{
    QString dlgTitle="消息框";
    QString strInfo="是否使用RGB模式";
    QMessageBox::StandardButton  defaultBtn=QMessageBox::NoButton; //缺省按钮
    QMessageBox::StandardButton result;//返回选择的按钮
    result=QMessageBox::question(this, dlgTitle, strInfo,
                               QMessageBox::Yes|QMessageBox::No |QMessageBox::Cancel,
                               defaultBtn);
    if (result==QMessageBox::Yes)
       ui.color->setCurrentIndex(0);
    else if(result==QMessageBox::No){}

    else if(result==QMessageBox::Cancel){}

    this->Set();
}
void VideoUI::do_size_ok_cliked(){
    ui.width->setValue(dialog_size->spinBoxWidth->value());
    ui.height->setValue(dialog_size->spinBoxHeight->value());
    this->Set();
}
void VideoUI::do_pyramid_ok_clicked(){
    ui.pydown->setValue(dialog_size->spinBoxWidth->value());
    ui.pyup->setValue(dialog_size->spinBoxHeight->value());
    this->Set();
}
void VideoUI::do_watermark_ok_clicked(){
    ui.mx->setValue(dialog_size->spinBoxWidth->value());
    ui.my->setValue(dialog_size->spinBoxHeight->value());
    ui.ma->setValue(dialog_size->spinBoxVertical->value());
    this->Set();
}
void VideoUI::on_action_size_triggered()
{
    dialog_size=new MyDialog();
    dialog_size->setWindowTitle("请设置尺寸大小");
    dialog_size->setFixedHeight(200);
    dialog_size->setFixedWidth(300);
// 创建布局
    QVBoxLayout layout(dialog_size);
    dialog_size->spinBoxWidth = new QSpinBox(dialog_size);
    dialog_size->spinBoxHeight = new QSpinBox(dialog_size);
    QFormLayout form=QFormLayout();
    form.addRow("宽度：",dialog_size->spinBoxWidth);
    form.addRow("高度",dialog_size->spinBoxHeight);
    layout.addLayout(&form);
    dialog_size->spinBoxWidth->setMaximum(5000);
    dialog_size->spinBoxWidth->setMinimum(0);
    dialog_size->spinBoxHeight->setMaximum(5000);
    dialog_size->spinBoxHeight->setMinimum(0);
    dialog_size->btn_size_ok= new QPushButton("OK",dialog_size);
    dialog_size->spinBoxWidth->setValue(ui.width->value());
    dialog_size->spinBoxHeight->setValue(ui.height->value());
    dialog_size->btn_size_cancel= new QPushButton("cancel", dialog_size);
    QHBoxLayout buttons;
    buttons.addWidget(dialog_size->btn_size_ok);
    buttons.addWidget(dialog_size->btn_size_cancel);
    layout.addLayout(&buttons);
   // QObject::connect(dialog_size->btn_size_ok,SIGNAL(clicked()),this,SLOT(do_size_ok_cliked()));
    QObject::connect(dialog_size->btn_size_ok,SIGNAL(clicked()),this,SLOT(do_size_ok_cliked()));
    QObject::connect(dialog_size->btn_size_ok,SIGNAL(clicked(bool)),dialog_size,SLOT(accept()));
    QObject::connect(dialog_size->btn_size_cancel,SIGNAL(clicked(bool)),dialog_size,SLOT(close()));
    dialog_size->exec();

}


void VideoUI::on_action_pyramid_triggered()
{   //存在问题 无法改动值

    BasicDialog* pyramid=new BasicDialog(ui.pydown,ui.pyup);
    pyramid->setWindowTitle("设置图像金字塔");
    pyramid->label1->setText("PYDOWN: ");
    pyramid->label2->setText("PYUP: ");
    //pyramid->setModal(true);
    //pyramid->show();
    pyramid->exec();
    this->Set();

    /*
    dialog_size=new MyDialog();
    dialog_size->setWindowTitle("请设置金字塔参数");
    dialog_size->setFixedHeight(200);
    dialog_size->setFixedWidth(300);
// 创建布局
    QVBoxLayout layout(dialog_size);
    dialog_size->spinBoxWidth = new QSpinBox(dialog_size);
    dialog_size->spinBoxHeight = new QSpinBox(dialog_size);
    QFormLayout form=QFormLayout();
    form.addRow("PYDOWN：",dialog_size->spinBoxWidth);
    form.addRow("PYUP",dialog_size->spinBoxHeight);
    layout.addLayout(&form);
    dialog_size->spinBoxWidth->setMaximum(100);
    dialog_size->spinBoxWidth->setMinimum(0);
    dialog_size->spinBoxHeight->setMaximum(100);
    dialog_size->spinBoxHeight->setMinimum(0);
    dialog_size->btn_size_ok= new QPushButton("确认",dialog_size);
    dialog_size->spinBoxWidth->setValue(ui.pydown->value());
    dialog_size->spinBoxHeight->setValue(ui.pyup->value());
    dialog_size->btn_size_cancel= new QPushButton("取消", dialog_size);
    QHBoxLayout buttons;
    buttons.addWidget(dialog_size->btn_size_ok);
    buttons.addWidget(dialog_size->btn_size_cancel);
    layout.addLayout(&buttons);
    QObject::connect(dialog_size->btn_size_ok,SIGNAL(clicked()),this,SLOT(do_pyramid_ok_cliked()));
    QObject::connect(dialog_size->btn_size_ok,SIGNAL(clicked()),dialog_size,SLOT(accept()));
    QObject::connect(dialog_size->btn_size_cancel,SIGNAL(clicked(bool)),dialog_size,SLOT(close()));
    dialog_size->exec();
*/


}



void VideoUI::on_action_watermark_triggered()

{   //存在问题 在关掉open窗口后 还会弹出坐标对话框
    //this->Open();
    //this->Mark();
    isMark = false;
    QString name = QFileDialog::getOpenFileName(this, "Select Image：");
    if (name.isEmpty()) {
            return;
    }
    std::string file = name.toLocal8Bit().data();
    cv::Mat mark = cv::imread(file);
    if (mark.empty()) return;



    dialog_size=new MyDialog();
    dialog_size->setWindowTitle("请设置水印参数");
    dialog_size->setFixedHeight(200);
    dialog_size->setFixedWidth(300);
// 创建布局
    QVBoxLayout layout(dialog_size);
    dialog_size->spinBoxWidth = new QSpinBox(dialog_size);
    dialog_size->spinBoxHeight = new QSpinBox(dialog_size);
    dialog_size->spinBoxVertical = new QDoubleSpinBox(dialog_size);
    QFormLayout form=QFormLayout();
    form.addRow("坐标X",dialog_size->spinBoxWidth);
    form.addRow("坐标Y",dialog_size->spinBoxHeight);
    form.addRow("透明度",dialog_size->spinBoxVertical);
    layout.addLayout(&form);
    dialog_size->spinBoxWidth->setMaximum(6000);
    dialog_size->spinBoxWidth->setMinimum(0);
    dialog_size->spinBoxHeight->setMaximum(6000);
    dialog_size->spinBoxHeight->setMinimum(0);
    dialog_size->spinBoxVertical->setMaximum(1.0);
    dialog_size->spinBoxVertical->setMinimum(0);
    dialog_size->spinBoxVertical->setSingleStep(0.01);
    dialog_size->btn_size_ok= new QPushButton("确认",dialog_size);
    dialog_size->spinBoxWidth->setValue(ui.mx->value());
    dialog_size->spinBoxHeight->setValue(ui.my->value());
    dialog_size->spinBoxVertical->setValue(ui.ma->value());

    dialog_size->btn_size_cancel= new QPushButton("取消", dialog_size);
    QHBoxLayout buttons;
    buttons.addWidget(dialog_size->btn_size_ok);
    buttons.addWidget(dialog_size->btn_size_cancel);
    layout.addLayout(&buttons);
    QObject::connect(dialog_size->btn_size_ok,SIGNAL(clicked()),this,SLOT(do_watermark_ok_clicked()));
    QObject::connect(dialog_size->btn_size_ok,SIGNAL(clicked(bool)),dialog_size,SLOT(accept()));
    QObject::connect(dialog_size->btn_size_cancel,SIGNAL(clicked(bool)),dialog_size,SLOT(close()));
    dialog_size->exec();
    VideoThread::Get()->SetMark(mark);
    isMark = true;
    this->Set();
}

//这两个没有测试
void VideoUI::on_action_mosaic_triggered()
{
    QString dlgTitle="消息框";
    QString strInfo="是否使用人脸打码模式";
    QMessageBox::StandardButton  defaultBtn=QMessageBox::NoButton; //缺省按钮
    QMessageBox::StandardButton result;//返回选择的按钮
    result=QMessageBox::question(this, dlgTitle, strInfo,
                                   QMessageBox::Yes|QMessageBox::No |QMessageBox::Cancel,
                                   defaultBtn);
    if (result==QMessageBox::Yes)
       ui.mosaic->setCurrentIndex(1);
    else if(result==QMessageBox::No){}

    else if(result==QMessageBox::Cancel){}

    this->Set();
}

//这两个没有测试
void VideoUI::on_action_sketch_triggered()
{
    QString dlgTitle="消息框";
    QString strInfo="是否使用素描图模式";
    QMessageBox::StandardButton  defaultBtn=QMessageBox::NoButton; //缺省按钮
    QMessageBox::StandardButton result;//返回选择的按钮
    result=QMessageBox::question(this, dlgTitle, strInfo,
                                   QMessageBox::Yes|QMessageBox::No |QMessageBox::Cancel,
                                   defaultBtn);
    if (result==QMessageBox::Yes)
       ui.mosaic->setCurrentIndex(2);
    else if(result==QMessageBox::No){}

    else if(result==QMessageBox::Cancel){}

    this->Set();
}

