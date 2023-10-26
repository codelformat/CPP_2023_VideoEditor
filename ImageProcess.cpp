#include "ImageProcess.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include<unordered_map>
#include <omp.h>

using namespace cv;
static CascadeClassifier object;

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

// 左右镜像 0
void ImageProcess::FlipX() {
	if (des.empty()) return;
	flip(des, des, 0);
}

// 上下镜像 1
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

// 水印
void ImageProcess::Mark(int x, int y, double a) {
	if(des.empty()) return;
	if (src2.empty()) return;
	if (x < 0 || y < 0) return;

	/*
	// 还未考虑图片大小大于视频分辨率的情况
	*/
	Mat roi = des(Rect(x, y, src2.cols, src2.rows));
	addWeighted(src2, a, roi, 1 - a, 0, roi);

}

//void ImageProcess::Mosaic() {
//	//加载文件
//	
//	std::vector<Rect> face;
//	// CascadeClassifier object;
//	
//	object.detectMultiScale(des, face, 1.2, 5);
//	//判断有没有人脸
//	if (face.empty()) return;
//	int step = 10;
//	for (int t = 0; t < face.size(); t++) {
//		int x = face[t].tl().x;
//		int y = face[t].tl().y;
//		int width = face[t].width;
//		int height = face[t].height;
//
//		for (int i = y; i < y + height; i += step) {
//			for (int j = x; j < x + width; j += step) {
//				//逐像素处理
//				for (int k = i; k < step + i; k++) {
//					for (int m = j; m < step + j; m++) {
//						for (int c = 0; c < 3; c++) {
//							//颜色替换
//							des.at<Vec3b>(k, m)[c] = des.at<Vec3b>(i, j)[2 - c];
//						}
//					}
//				}
//			}
//		}
//		/*for (int i = y; i < y + height; i++) {
//			for (int j = x; j < x + width; j++) {
//				des.at<Vec3b>(i, j)[0] = 255;
//				des.at<Vec3b>(i, j)[1] = 255;
//				des.at<Vec3b>(i, j)[2] = 255;
//			}
//		}*/
//	}
//	return;
//}

void ImageProcess::Mosaic() {
	std::vector<Rect> face;
	object.detectMultiScale(des, face, 1.2, 5);
	if (face.empty()) return;

	int step = 20; // increase the step size to make the mosaic effect deeper

	cv::parallel_for_(cv::Range(0, face.size()), [&](const cv::Range& range) {
		for (int t = range.start; t < range.end; t++) {
			int x = face[t].tl().x;
			int y = face[t].tl().y;
			int width = face[t].width;
			int height = face[t].height;

#pragma omp parallel for collapse(2)
			for (int i = y; i < y + height; i += step) {
				for (int j = x; j < x + width; j += step) {
					// read the pixel once
					Vec3b pixel = des.at<Vec3b>(i, j);

					// update the pixel in a local variable
					std::swap(pixel[0], pixel[2]);

					for (int k = i; k < std::min(i + step, y + height); k++) {
						for (int m = j; m < std::min(j + step, x + width); m++) {
							// write the pixel back to the image once
							des.at<Vec3b>(k, m) = pixel;
						}
					}
				}
			}
		}
		});
}

//素描化：
void ImageProcess::Sketch() {
	std::unordered_map<std::string, Mat> result;
	cvtColor(des, result["Gray"], COLOR_BGR2GRAY);
	bitwise_not(result["Gray"], result["Nagative"]);
	GaussianBlur(result["Nagative"], result["Blur"], Size(21, 21), 0, 0);
	divide(result["Gray"], 255 - result["Blur"], result["Light"], 256);
	divide(255 - result["Light"], 255 - result["Blur"], result["Deepen"], 256);
	des = 255 - result["Deepen"];
}

//去水印
void ImageProcess::removeWatermark() {
	Mat gray;
	cvtColor(des, gray, COLOR_BGR2GRAY);

	//图像二值化，筛选出白色区域部分
	Mat thresh;
	threshold(gray, thresh, 220, 255, THRESH_BINARY);

	//提取图片下方的水印，制作掩模图像
	Mat mask = Mat::zeros(des.size(), CV_8U);
	int height = des.rows;
	int width = des.cols;
	int start = 0.9 * height;
	//遍历图像像素，提取出水印部分像素，制作掩模图像
#pragma omp parallel for
	for (int i = start; i < height; i++)
	{
		uchar* data = thresh.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			if (data[j] == 255)
			{
				mask.at<uchar>(i, j) = 255;
			}
		}
	}

	//将掩模进行膨胀，使其能够覆盖图像更大区域
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(mask, mask, kernel);

	//使用inpaint进行图像修复
	inpaint(des, mask, des, 1, INPAINT_NS);
}

ImageProcess::ImageProcess() {
	object.load(path_);
}

ImageProcess::~ImageProcess() {
}
