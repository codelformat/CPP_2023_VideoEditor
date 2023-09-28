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
	if (img.isNull()) {
		uchar* buf = new uchar[width() * height() * 3]; //�Ҷ�Ϊ1����ɫΪ3
		img = QImage(buf, width(), height(),QImage::Format_RGB888);
	}
	//��MAT����
	Mat des;
	cv::resize(mat, des, cv::Size(img.size().width(), img.size().height()));
	cv::cvtColor(des, des, COLOR_BGR2RGB);
	memcpy(img.bits(), des.data, des.cols * des.rows * des.elemSize());
	update();//����ˢ��
}
VideoWidget::~VideoWidget() {

}