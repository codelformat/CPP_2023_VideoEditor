#pragma once
#include <opencv2/core.hpp>
#include <vector>

enum TaskType {
	TASK_NONE,
	TASK_GAIN, //亮度对比度调整
	TASK_ROTATE90, //旋转90度
	TASK_ROTATE180, //旋转180度
	TASK_ROTATE270, //旋转270度
	TASK_FLIPX,
	TASK_FLIPY,
	TASK_FILPXY,
	TASK_RESIZE,
	TASK_PYDOWN,
	TASK_PYUP,
	TASK_CLIP,
	TASK_GRAY,
	TASK_MASK,
	TASK_MOSAIC,
	TASK_SKETCH,
	TASK_REMOVE_WATERMARK,
	TASK_OBJECT_DETECTION
};

struct Task {
	TaskType type;
	std::vector<double> para;
};
class VideoFilter
{
public:
	virtual cv::Mat Filter(cv::Mat mat1, cv::Mat mat2) = 0;
	virtual void Add(Task task) = 0;
	virtual void Clear() = 0;
	static VideoFilter* Get();
	virtual ~VideoFilter();
protected:
	VideoFilter();
};

