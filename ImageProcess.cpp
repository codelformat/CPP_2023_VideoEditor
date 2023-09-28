#include "ImageProcess.h"

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

ImageProcess::ImageProcess() {
}

ImageProcess::~ImageProcess() {
}
