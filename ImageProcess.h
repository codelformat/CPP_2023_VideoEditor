#pragma once
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
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

	void Rotate90();
	void Rotate180();
	void Rotate270();

	void FlipX();
	void FlipY();
	void FlipXY();

	//ͼ��ߴ�
	void Resize(int width, int height);

	//ͼ�������
	void PyDown(int count);
	void PyUp(int count);

	//��Ƶ����ü�
	void Clip(int x, int y, int w, int h);

	// ת�Ҷ�ͼ
	void Gray();

	// ��ˮӡ
	void Mark(int x, int y, double a);

	//������������
	void Mosaic();

	//���軯
	void Sketch();

	// ȥˮӡ
	void removeWatermark();

	ImageProcess();
	~ImageProcess();
private:
	cv::Mat src1, src2;
	std::string path_ = "D:\\Cpp_2023_project\\bin\\haarcascade_frontalface_default.xml";
	

	// Ŀ��ͼ
	cv::Mat des;
};

