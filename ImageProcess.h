#pragma once
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
class ImageProcess
{
public:
	// 设置原图，会清理之前的处理结果
	void Set(cv::Mat mat1, cv::Mat mat2);
	// 获取处理后的结果
	cv::Mat Get() { return des; }
	
	//设置亮度和对比度
	/** 
	* @params bright double 亮度 0~100
	* @params contrast double 对比度 1.0~3.0
	*/
	void Gain(double bright, double contrast);

	void Rotate90();
	void Rotate180();
	void Rotate270();

	void FlipX();
	void FlipY();
	void FlipXY();

	//图像尺寸
	void Resize(int width, int height);

	//图像金字塔
	void PyDown(int count);
	void PyUp(int count);

	//视频画面裁剪
	void Clip(int x, int y, int w, int h);

	// 转灰度图
	void Gray();

	// 加水印
	void Mark(int x, int y, double a);

	//人脸打马赛克
	void Mosaic();

	//素描化
	void Sketch();

	// 去水印
	void removeWatermark();

	ImageProcess();
	~ImageProcess();
private:
	cv::Mat src1, src2;
	std::string path_ = "D:\\Cpp_2023_project\\bin\\haarcascade_frontalface_default.xml";
	

	// 目标图
	cv::Mat des;
};

