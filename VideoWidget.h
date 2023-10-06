#pragma once
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtWidgets/QWidget>
#include <opencv2/core.hpp>
class VideoWidget: public QOpenGLWidget
{
	Q_OBJECT

	public:
	VideoWidget(QWidget* p);
	void paintEvent(QPaintEvent * e);
	virtual  ~VideoWidget();
	
	// 信号槽函数
public slots:
	void SetImage(cv::Mat mat);
protected:
	QImage img;

	void resizeEvent(QResizeEvent* event) override {
		// 确保宽度和高度都能被4整除
		int newWidth = width() - (width() % 4);
		int newHeight = height() - (height() % 4);

		// 设置新的窗口大小
		resize(newWidth, newHeight);

		// 调用父类的resizeEvent以确保正常的窗口调整
		QWidget::resizeEvent(event);
	}

};

