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
	QImage::Format fmt = QImage::Format_RGB888;
	int pixSize = 3;
	 //灰度图格式CV_8UC2
	if (mat.type() == CV_8UC2) {
		fmt = QImage::Format_Grayscale16;
		pixSize = 2; // 灰度图像素大小
	}
	if (img.isNull() || img.format()!=fmt) {
		delete img.bits();
		uchar* buf = new uchar[width() * height() * pixSize]; //灰度为2，彩色为3
		img = QImage(buf, width(), height(), fmt);
		std::cout << width() << " " << height() << std::endl;
	}
	//给MAT缩放
	Mat des;
	cv::resize(mat, des, cv::Size(img.size().width(), img.size().height()));
	if(pixSize == 3)
		cv::cvtColor(des, des, COLOR_BGR2RGB); //为RGB时，做BGR转RGB
	memcpy(img.bits(), des.data, des.cols * des.rows * des.elemSize());
	update();//界面刷新
}
VideoWidget::~VideoWidget() {

}