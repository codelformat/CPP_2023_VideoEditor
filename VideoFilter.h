#pragma once
#include <opencv2/core.hpp>
#include <vector>

enum TaskType {
	TASK_NONE,
	TASK_GAIN, //亮度对比度调整
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

