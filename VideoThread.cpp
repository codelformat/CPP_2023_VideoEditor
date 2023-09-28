#include "VideoThread.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "VideoFilter.h"
using namespace cv;
using namespace std;

//二次迭代可以考虑存放在别处
static VideoCapture cap1;
static bool isExit = false;

// 返回当前播放的位置
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

//打开一号视频源文件
bool VideoThread::Open(const std::string file) {
	cout << "open: " << file << endl;
	mutex.lock(); // 加锁，防止多线程同时打开视频文件
	bool re = cap1.open(file);
	mutex.unlock();// 一定要解锁
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

		// 判断视频是否打开
		if (!cap1.isOpened()) {
			mutex.unlock();
			msleep(5);
			continue;
		}
		// 读取一帧视频，解码并作颜色转换，在read中均做了
		if (!cap1.read(mat1) || mat1.empty())
		{
			mutex.unlock();//尽晚调用，尽早退出
			msleep(5);
			continue;
		}
		// 显示图像1
		ViewImage1(mat1);

		Mat des = VideoFilter::Get()->Filter(mat1, Mat());

		// 显示生成后图像
		ViewDes(des);
		// 发太快 卡死
		mutex.unlock();
		//msleep(40);
		// 
		int s = 0;
		s = 1000 / fps;
		msleep(s);
		// 先释放再等待刷新
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