#pragma once
#include <QtCore\QThread>
#include <QtCore\QMutex>
#include <opencv2\core.hpp>
class VideoThread:public QThread
{

	Q_OBJECT
    
public:
	int fps = 0;
	int width = 0;
	int height = 0;

	//单件模式 获取对象
	static VideoThread* Get() {
		static VideoThread vt;
		return &vt;
	}
	//打开一号视频源文件
	bool Open(const std::string file);
	void Play() { 
		mutex.lock();
		isPlay = true; 
		mutex.unlock();
	};
	void Pause() { 
		mutex.lock();
		isPlay = false;
		mutex.unlock();
	};

	bool isThreadOpen();
	// 返回当前播放的位置
	double GetPos();

	// 跳转视频
	/**
	 * @params frame int 帧位置
	 */
	bool Seek(int frame);
	bool Seek(double pos);

	// 暂只支持H264格式
	// 开始保存视频
	bool StartSave(const std::string filename, int width = 0, int height = 0, bool isColor = true);

	// 停止保存视频，写入视频帧的索引
	bool StopSave();

	~VideoThread();

	// 线程入口函数
	void run();

	// 信号函数
signals:
	void ViewImage1(cv::Mat mat);

	// 显示生成后图像
	void ViewDes(cv::Mat mat);
	void SaveEnd();
protected:
	QMutex mutex;
	
	//是否开始写视频
	bool isWrite = false;
	bool isPlay = false;
	VideoThread();
};

