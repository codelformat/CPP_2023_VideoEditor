#include "VideoWidget.h"
#include <QtGui\QPainter>
#include <opencv2/opencv.hpp>
using namespace cv;

// ��ͼ�¼�
void VideoWidget::paintEvent(QPaintEvent* e) {
	QPainter p;
	p.begin(this);
	p.drawImage(QPoint(0, 0), img); //�����Ͻǿ�ʼ����
	p.end();
}

VideoWidget::VideoWidget(QWidget* p) : QOpenGLWidget(p) {
    /*setWindowFlags(Qt::FramelessWindowHint);*/
}

// �źŲۺ���
void VideoWidget::SetImage(cv::Mat mat) {
	QImage::Format fmt = QImage::Format_RGB888;
	int pixSize = 3;
	 //�Ҷ�ͼ��ʽCV_8UC2
	if (mat.type() == CV_8UC2) {
		fmt = QImage::Format_Grayscale16;
		pixSize = 2; // �Ҷ�ͼ���ش�С
	}
	if (img.isNull() || img.format()!=fmt) {
		delete img.bits();
		int realWidth = width(); //TODO
		int realHeight = height(); // TODO
		uchar* buf = new uchar[realWidth * realHeight * pixSize]; //�Ҷ�Ϊ2����ɫΪ3
		img = QImage(buf, realWidth, realHeight, fmt);
		std::cout << realWidth << " " << realHeight << std::endl;
	}
	//��MAT����
	Mat des;
	cv::resize(mat, des, cv::Size(img.size().width(), img.size().height()));
	if(pixSize == 3)
		cv::cvtColor(des, des, COLOR_BGR2RGB); //ΪRGBʱ����BGRתRGB
	memcpy(img.bits(), des.data, des.cols * des.rows * des.elemSize());
	update();//����ˢ��
}
VideoWidget::~VideoWidget() {

}
