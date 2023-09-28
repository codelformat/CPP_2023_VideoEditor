#pragma once
#include <opencv2/core.hpp>
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

	ImageProcess();
	~ImageProcess();
private:
	cv::Mat src1, src2;

	// 目标图
	cv::Mat des;
};

