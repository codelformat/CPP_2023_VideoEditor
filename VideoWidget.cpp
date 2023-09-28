#include "VideoWidget.h"
#include <QtGui\QPainter>
#include <opencv2/opencv.hpp>
using namespace cv;

// 绘图事件
void VideoWidget::paintEvent(QPaintEvent* e) {
	QPainter p;
	p.begin(this);
	p.drawImage(QPoint(0, 0), img); //从左上角开始绘制
	p.end();
}

VideoWidget::VideoWidget(QWidget* p) : QOpenGLWidget(p) {
	/*setWindowFlags(Qt::FramelessWindowHint);*/
}

// 信号槽函数
void VideoWidget::SetImage(cv::Mat mat) {
	if (img.isNull()) {
		uchar* buf = new uchar[width() * height() * 3]; //灰度为1，彩色为3
		img = QImage(buf, width(), height(),QImage::Format_RGB888);
	}
	//给MAT缩放
	Mat des;
	cv::resize(mat, des, cv::Size(img.size().width(), img.size().height()));
	cv::cvtColor(des, des, COLOR_BGR2RGB);
	memcpy(img.bits(), des.data, des.cols * des.rows * des.elemSize());
	update();//界面刷新
}
VideoWidget::~VideoWidget() {

}