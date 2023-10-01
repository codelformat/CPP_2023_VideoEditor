#include "ImageProcess.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;

void ImageProcess::Set(cv::Mat mat1, cv::Mat mat2) {
	if (mat1.empty()) return;
	this->src1 = mat1;
	this->src2 = mat2;
	this->src1.copyTo(des);

	
}

void ImageProcess::Gain(double bright, double contrast) {
	if (des.empty()) return;
	des.convertTo(des, -1, contrast, bright);

}

void ImageProcess::Rotate90() {
	if (des.empty()) return;
	rotate(des, des, ROTATE_90_CLOCKWISE);

}

void ImageProcess::Rotate180() {
	if (des.empty()) return;
	rotate(des, des, ROTATE_180);
}

void ImageProcess::Rotate270() {
	if (des.empty()) return;
	rotate(des, des, ROTATE_90_COUNTERCLOCKWISE);
}

// ×óÓÒ¾µÏñ 0
void ImageProcess::FlipX() {
	if (des.empty()) return;
	flip(des, des, 0);
}

// ÉÏÏÂ¾µÏñ 1
void ImageProcess::FlipY() {
	if (des.empty()) return;
	flip(des, des, 1);
}

void ImageProcess::FlipXY() {
	if (des.empty()) return;
	flip(des, des, -1);
}

void ImageProcess::Resize(int width, int height) {
	if (des.empty()) return;
	resize(des, des, Size(width, height));
}

void ImageProcess::PyDown(int count) {
	if (des.empty()) return;
	for (int i = 0; i < count; i++) {
		pyrDown(des, des);
	}
}

void ImageProcess::PyUp(int count) {
	if (des.empty()) return;
	for (int i = 0; i < count; i++) {
		pyrUp(des, des);
	}
}

void ImageProcess::Clip(int x, int y, int w, int h) {
	if (des.empty()) return;
	if (x < 0 || y < 0 || w <= 0 || h <= 0) return;
	if (x > des.cols || y > des.rows) return;

	des = des(Rect(x, y, w, h));
}

void ImageProcess::Gray() {
	if (des.empty()) return;
	cvtColor(des, des, COLOR_BGR2GRAY);
}

ImageProcess::ImageProcess() {
}

ImageProcess::~ImageProcess() {
}
