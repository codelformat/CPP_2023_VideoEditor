#pragma once
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <opencv2/core.hpp>
class VideoWidget: public QOpenGLWidget
{
	Q_OBJECT

	public:
	VideoWidget(QWidget* p);
	void paintEvent(QPaintEvent * e);
	virtual  ~VideoWidget();
	
	// ÐÅºÅ²Ûº¯Êý
public slots:
	void SetImage(cv::Mat mat);
protected:
	QImage img;

};

