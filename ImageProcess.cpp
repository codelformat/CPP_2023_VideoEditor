#include "ImageProcess.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include<unordered_map>
#include <vector>
#include <omp.h>

using namespace cv;
static CascadeClassifier object;
std::vector<cv::Scalar> colors = {
	cv::Scalar(0, 0, 255),    // Red
	cv::Scalar(0, 255, 0),    // Green
	cv::Scalar(255, 0, 0),    // Blue
	cv::Scalar(255, 255, 0),  // Yellow
	cv::Scalar(255, 0, 255)   // Magenta
};

std::vector<std::string> classNames = {
	"person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat",
	"traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat",
	"dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack",
	"umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball",
	"kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket",
	"bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
	"sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair",
	"sofa", "pottedplant", "bed", "diningtable", "toilet", "tvmonitor", "laptop", "mouse",
	"remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator",
	"book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush", "Footer"
};

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
	int start = 0. * height;
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

void ImageProcess::objectDectection() {
	Mat frame = src1;
	//int64 start = cv::getTickCount();
	// 图象预处理 - 格式化操作
	int w = frame.cols;
	int h = frame.rows;
	int _max = std::max(h, w);
	cv::Mat image = cv::Mat::zeros(cv::Size(_max, _max), CV_8UC3);
	cv::Rect roi(0, 0, w, h);
	frame.copyTo(image(roi));

	float x_factor = image.cols / 640.0f;
	float y_factor = image.rows / 640.0f;

	// 推理
	cv::Mat blob = cv::dnn::blobFromImage(image, 1 / 255.0, cv::Size(640, 640), cv::Scalar(0, 0, 0), true, false);
	net.setInput(blob);
	cv::Mat preds = net.forward();
	
	// 后处理, 1x25200x85
	cv::Mat det_output(preds.size[1], preds.size[2], CV_32F, preds.ptr<float>());
	float confidence_threshold = 0.5;
	std::vector<cv::Rect> boxes;
	std::vector<int> classIds;
	std::vector<float> confidences;
#pragma omp parallel for
	for (int i = 0; i < det_output.rows; i++) {
		float confidence = det_output.at<float>(i, 4);
		if (confidence < confidence_threshold) {
			continue;
		}
		cv::Mat classes_scores = det_output.row(i).colRange(5, preds.size[2]);
		cv::Point classIdPoint;
		double score;
		minMaxLoc(classes_scores, 0, &score, 0, &classIdPoint);

		// 置信度 0～1之间
		if (score > 0.25)
		{
			float cx = det_output.at<float>(i, 0);
			float cy = det_output.at<float>(i, 1);
			float ow = det_output.at<float>(i, 2);
			float oh = det_output.at<float>(i, 3);
			int x = static_cast<int>((cx - 0.5 * ow) * x_factor);
			int y = static_cast<int>((cy - 0.5 * oh) * y_factor);
			int width = static_cast<int>(ow * x_factor);
			int height = static_cast<int>(oh * y_factor);
			cv::Rect box;
			box.x = x;
			box.y = y;
			box.width = width;
			box.height = height;
#pragma omp critical
			{
				boxes.push_back(box);
				classIds.push_back(classIdPoint.x);
				confidences.push_back(score);
			}
		}
	}

	// NMS
	std::vector<int> indexes;
	cv::dnn::NMSBoxes(boxes, confidences, 0.25, 0.50, indexes);
	for (size_t i = 0; i < indexes.size(); i++) {
		int index = indexes[i];
		int idx = classIds[index];
		cv::rectangle(frame, boxes[index], colors[idx % 5], 2, 8);
		cv::rectangle(frame, cv::Point(boxes[index].tl().x, boxes[index].tl().y - 20),
			cv::Point(boxes[index].br().x, boxes[index].tl().y), cv::Scalar(255, 255, 255), -1);
		cv::putText(frame, classNames[idx], cv::Point(boxes[index].tl().x, boxes[index].tl().y - 10), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0),2);
	}
	des = frame;
}

ImageProcess::ImageProcess() {
	object.load(path_);
	net = cv::dnn::readNetFromONNX(onnx_path);

	net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
}

ImageProcess::~ImageProcess() {
}
