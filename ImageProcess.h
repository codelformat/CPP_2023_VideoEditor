#pragma once
#include <opencv2/core.hpp>
class ImageProcess
{
public:
	// ����ԭͼ��������֮ǰ�Ĵ�����
	void Set(cv::Mat mat1, cv::Mat mat2);
	// ��ȡ�����Ľ��
	cv::Mat Get() { return des; }
	
	//�������ȺͶԱȶ�
	/** 
	* @params bright double ���� 0~100
	* @params contrast double �Աȶ� 1.0~3.0
	*/
	void Gain(double bright, double contrast);

	ImageProcess();
	~ImageProcess();
private:
	cv::Mat src1, src2;

	// Ŀ��ͼ
	cv::Mat des;
};

