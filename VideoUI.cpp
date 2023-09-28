#include "VideoUI.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <string>
#include "VideoThread.h"
#include "VideoFilter.h"
using namespace std;
static bool pressSlider = false;

VideoUI::VideoUI(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    // ע��������
    qRegisterMetaType<cv::Mat>("cv::Mat");
    // ������Ƶ
    // �źŲ۹���
    QObject::connect(VideoThread::Get(),//�ź�Դ
        SIGNAL(ViewImage1(cv::Mat)),//�ź�
        ui.src1,
        SLOT(SetImage(cv::Mat))//�ۺ���
    );

    // �����Ƶ
    //�źŲ۹���
    QObject::connect(VideoThread::Get(),//�ź�Դ
        SIGNAL(ViewDes(cv::Mat)),//�ź�
        ui.des,
        SLOT(SetImage(cv::Mat))//�ۺ���
    );


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

    string file = name.toLocal8Bit().data();
    if (!VideoThread::Get()->Open(file))
    {
        QMessageBox::information(this, "error", name+" open failed");
        return;
    }
    // QMessageBox::information(this, "", name);

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

void VideoUI::Set()
{
    VideoFilter::Get()->Clear();
	// ���öԱȶȺ�����
    if (ui.bright->value() > 0 || 
        ui.contrast->value() > 1) {
        VideoFilter::Get()->Add(Task{ TASK_GAIN, 
            {(double)ui.bright->value(), ui.contrast->value()} });
	}
}

VideoUI::~VideoUI()
{}
