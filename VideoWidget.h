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
	
	// �źŲۺ���
public slots:
	void SetImage(cv::Mat mat);
protected:
	QImage img;

	void resizeEvent(QResizeEvent* event) override {
		// ȷ����Ⱥ͸߶ȶ��ܱ�4����
		int newWidth = width() - (width() % 4);
		int newHeight = height() - (height() % 4);

		// �����µĴ��ڴ�С
		resize(newWidth, newHeight);

		// ���ø����resizeEvent��ȷ�������Ĵ��ڵ���
		QWidget::resizeEvent(event);
	}

};

