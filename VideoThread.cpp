#include "VideoThread.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "VideoFilter.h"
using namespace cv;
using namespace std;

//二次迭代可以考虑存放在别处
//删除了static关键字
VideoCapture cap1;
static bool isExit = false;

// 保存视频
static VideoWriter vw;

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
	if (count > 0.001)
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
	width = cap1.get(CAP_PROP_FRAME_WIDTH);
	height = cap1.get(CAP_PROP_FRAME_HEIGHT);
	fps = cap1.get(CAP_PROP_FPS);
	if (fps <= 0) fps = 25;
	srcFile = file;
	cout << srcFile << endl;
	double count = cap1.get(CAP_PROP_FRAME_COUNT);

	// 设置结尾指针位置
	
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

		// 判断视频是否打开
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

		// 读取一帧视频，解码并作颜色转换，在read中均做了
		if (cur >= end || !cap1.read(mat1) || mat1.empty())
		{
			

			mutex.unlock();//尽晚调用，尽早退出
			// 导出到结尾位置， 停止导出
			if (isWrite)
			{
				StopSave();
				SaveEnd();
			}
			msleep(5);
			continue;
		}
		// 显示图像1
		if (!isWrite)
			ViewImage1(mat1);

		Mat mat2 = mark;
		Mat des = VideoFilter::Get()->Filter(mat1, mat2);

		// 显示生成后图像
		if (!isWrite)
			ViewDes(des);
		int s = 0;
		s = 1000 / fps ;
		if (isWrite) {
			s = 1;
			vw.write(des);
		}
		// 发太快 卡死

		//msleep(40);
		// 

		mutex.unlock();
		if (!isWrite)
			msleep(s);
		// 先释放再等待刷新

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

// 暂只支持H264格式
	// 开始保存视频
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

// 停止保存视频，写入视频帧的索引
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
