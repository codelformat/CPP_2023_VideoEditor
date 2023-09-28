#pragma once
#include <QtCore\QThread>
#include <QtCore\QMutex>
#include <opencv2\core.hpp>
class VideoThread:public QThread
{

	Q_OBJECT
    
public:
	int fps = 0;

	//单件模式 获取对象
	static VideoThread* Get() {
		static VideoThread vt;
		return &vt;
	}
	//打开一号视频源文件
	bool Open(const std::string file);
	// 返回当前播放的位置
	double GetPos();

	// 跳转视频
	/**
	 * @params frame int 帧位置
	 */
	bool Seek(int frame);
	bool Seek(double pos);

	~VideoThread();

	// 线程入口函数
	void run();

	// 信号函数
signals:
	void ViewImage1(cv::Mat mat);

	// 显示生成后图像
	void ViewDes(cv::Mat mat);
protected:
	QMutex mutex;
	VideoThread();
};

