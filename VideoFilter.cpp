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
		auto p = ImageProcess::Get();
		p->Set(mat1, mat2);
		for (int i = 0; i < tasks.size(); i++) {
			switch (tasks[i].type) {
			case TASK_GAIN:
				// 亮度对比度调整
				p->Gain(tasks[i].para[0], tasks[i].para[1]);
				break;
			case TASK_ROTATE90:
				p->Rotate90();
				break;
			case TASK_ROTATE180:
				p->Rotate180();
				break;
			case TASK_ROTATE270:
				p->Rotate270();
				break;
			case TASK_FLIPX:
				p->FlipX();
				break;
			case TASK_FLIPY:
				p->FlipY();
				break;
			case TASK_FILPXY:
				p->FlipXY();
				break;
			case TASK_RESIZE:
				p->Resize(tasks[i].para[0], tasks[i].para[1]);
				break;
			case TASK_PYDOWN:
				p->PyDown(tasks[i].para[0]);
				break;
			case TASK_PYUP:
				p->PyUp(tasks[i].para[0]);
				break;
			case TASK_CLIP:
				p->Clip(tasks[i].para[0], tasks[i].para[1], tasks[i].para[2], tasks[i].para[3]);
				break;
			case TASK_GRAY:
				p->Gray();
				break;
			case TASK_MASK:
				p->Mark(tasks[i].para[0], tasks[i].para[1], tasks[i].para[2]);
				break;
			case TASK_MOSAIC:
				p->Mosaic();
				break;
			case TASK_SKETCH:
				p->Sketch();
				break;
			case TASK_REMOVE_WATERMARK:
				p->removeWatermark();
				break;
			case TASK_OBJECT_DETECTION:
				p->objectDectection();
				break;
			default:
				break;
			}
		}

		cv::Mat re = p->getMat();
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

