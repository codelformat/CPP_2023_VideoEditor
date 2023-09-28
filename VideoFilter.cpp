#include "VideoFilter.h"
#include <QtCore\QThread>
#include <QtCore\QMutex>
#include "ImageProcess.h"

class CVideoFilter :public VideoFilter {
public:
	std::vector<Task> tasks;
	QMutex mutex;
	virtual cv::Mat Filter(cv::Mat mat1, cv::Mat mat2) {
		mutex.lock();
		ImageProcess p;
		p.Set(mat1, mat2);
		for (int i = 0; i < tasks.size(); i++) {
			switch (tasks[i].type) {
			case TASK_GAIN:
				// 亮度对比度调整
				p.Gain(tasks[i].para[0], tasks[i].para[1]);
				break;
			default:
				break;
			}
		}

		cv::Mat re = p.Get();
		mutex.unlock();
		return re;
	}

	void Add(Task task) {
		mutex.lock();
		tasks.push_back(task);
		mutex.unlock();
	}

	void Clear() {
		mutex.lock();
		tasks.clear();
		mutex.unlock();
	}
};

VideoFilter* VideoFilter::Get() {
	static CVideoFilter cvf;
	return &cvf;
}

VideoFilter::VideoFilter() {
}

VideoFilter::~VideoFilter() {
}

