#include "VideoThread.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "VideoFilter.h"
using namespace cv;
using namespace std;

//���ε������Կ��Ǵ���ڱ�
//ɾ����static�ؼ���
VideoCapture cap1;
static bool isExit = false;

// ������Ƶ
static VideoWriter vw;

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
	if (count > 0.001)
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
	width = cap1.get(CAP_PROP_FRAME_WIDTH);
	height = cap1.get(CAP_PROP_FRAME_HEIGHT);
	fps = cap1.get(CAP_PROP_FPS);
	if (fps <= 0) fps = 25;
	srcFile = file;
	cout << srcFile << endl;
	double count = cap1.get(CAP_PROP_FRAME_COUNT);

	// ���ý�βָ��λ��
	
	/*int frame = 0.999 * count;
	this->end = frame;*/

	totalMs = (count / (double)fps) * 1000;
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

		if (!isPlay) {
			mutex.unlock();
			msleep(5);
			continue;
		}

		int cur = cap1.get(CAP_PROP_POS_FRAMES);

		// ��ȡһ֡��Ƶ�����벢����ɫת������read�о�����
		if (cur >= end || !cap1.read(mat1) || mat1.empty())
		{
			

			mutex.unlock();//������ã������˳�
			// ��������βλ�ã� ֹͣ����
			if (isWrite)
			{
				StopSave();
				SaveEnd();
			}
			msleep(5);
			continue;
		}
		// ��ʾͼ��1
		if (!isWrite)
			ViewImage1(mat1);

		Mat mat2 = mark;
		Mat des = VideoFilter::Get()->Filter(mat1, mat2);

		// ��ʾ���ɺ�ͼ��
		if (!isWrite)
			ViewDes(des);
		int s = 0;
		s = 1000 / fps ;
		if (isWrite) {
			s = 1;
			vw.write(des);
		}
		// ��̫�� ����

		//msleep(40);
		// 

		mutex.unlock();
		if (!isWrite)
			msleep(s);
		// ���ͷ��ٵȴ�ˢ��

	}
}

bool VideoThread::isThreadOpen() {
	return cap1.isOpened();
}

void VideoThread::SetBegin(double p) {
	mutex.lock();
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	int frame = p * count;
	begin = frame;
	mutex.unlock();
}

void VideoThread::SetEnd(double p) {
	mutex.lock();
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	int frame = p * count;
	end = frame;
	mutex.unlock();
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

// ��ֻ֧��H264��ʽ
	// ��ʼ������Ƶ
bool VideoThread::StartSave(const std::string filename, int width, int height, bool isColor) {
	cout << "Start Exporting" << endl;
	Seek(begin);
	mutex.lock();
	if (!cap1.isOpened()) {
		mutex.unlock();
		return false;
	}
	if (width <= 0) {
		width = cap1.get(CAP_PROP_FRAME_WIDTH);
	}

	if (height <= 0) {
		height = cap1.get(CAP_PROP_FRAME_HEIGHT);
	}
	vw.open(filename,
		VideoWriter::fourcc('M', 'P', '4', '2'),
		this->fps,
		Size(width, height),
		isColor
	);
	if (!vw.isOpened()) {
		mutex.unlock();
		cout << "VideoWriter open failed." << endl;
		return false;
	}
	this->isWrite = true;
	desFile = filename;
	mutex.unlock();
	return true;
}

// ֹͣ������Ƶ��д����Ƶ֡������
bool VideoThread::StopSave() {
	cout << "Stop Exporting" << endl;
	mutex.lock();
	vw.release();
	isWrite = false;
	mutex.unlock();
	return true;
}

VideoThread::~VideoThread() {
	mutex.lock();
	isExit = true;
	mutex.unlock();
	wait();
}
