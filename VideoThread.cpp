#include "VideoThread.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "VideoFilter.h"
using namespace cv;
using namespace std;

//���ε������Կ��Ǵ���ڱ�
static VideoCapture cap1;
static bool isExit = false;

// ���ص�ǰ���ŵ�λ��
double VideoThread::GetPos() {
	double pos = 0;
	mutex.lock();
	if (!cap1.isOpened()) {
		mutex.unlock();
		return pos;
	}
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	double cur = cap1.get(CAP_PROP_POS_FRAMES);
	if(count > 0.001)
		pos = cur / count;
	mutex.unlock();
	return pos;
}
//

//��һ����ƵԴ�ļ�
bool VideoThread::Open(const std::string file) {
	cout << "open: " << file << endl;
	mutex.lock(); // ��������ֹ���߳�ͬʱ����Ƶ�ļ�
	bool re = cap1.open(file);
	mutex.unlock();// һ��Ҫ����
	cout << re << endl;
	if (!re) {
		return re;
	}
	fps = cap1.get(CAP_PROP_FPS);
	if (fps <= 0) fps = 25;
	return re;
}
void VideoThread::run()
{
	Mat mat1;
	for (;;) {
		mutex.lock();
		if (isExit)
		{
			mutex.unlock();
			break;
		}

		// �ж���Ƶ�Ƿ��
		if (!cap1.isOpened()) {
			mutex.unlock();
			msleep(5);
			continue;
		}
		// ��ȡһ֡��Ƶ�����벢����ɫת������read�о�����
		if (!cap1.read(mat1) || mat1.empty())
		{
			mutex.unlock();//������ã������˳�
			msleep(5);
			continue;
		}
		// ��ʾͼ��1
		ViewImage1(mat1);

		Mat des = VideoFilter::Get()->Filter(mat1, Mat());

		// ��ʾ���ɺ�ͼ��
		ViewDes(des);
		// ��̫�� ����
		mutex.unlock();
		//msleep(40);
		// 
		int s = 0;
		s = 1000 / fps;
		msleep(s);
		// ���ͷ��ٵȴ�ˢ��
	}
}

bool VideoThread::Seek(int frame) {
	mutex.lock();
	if (!cap1.isOpened()) {
		mutex.unlock();
		return false;
	}
	bool re = cap1.set(CAP_PROP_POS_FRAMES, frame);
	mutex.unlock();
	return re;
}

bool VideoThread::Seek(double pos) {
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	int frame = count * pos;
	return Seek(frame);
}

VideoThread::VideoThread()
{
	start();
}

VideoThread::~VideoThread() {
	mutex.lock();
	isExit = true;
	mutex.unlock();
	wait();
}