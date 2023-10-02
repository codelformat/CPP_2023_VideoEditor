#include "AudioThread.h"
#include <iostream>
#include <QtCore\QMutex>
using namespace std;
struct Time {
	Time(int tms) {
		h = (tms / 1000) / 3600;
		m = ((tms / 1000) % 3600) / 60;
		s = ((tms / 1000) % 3600) % 60;
		ms = tms % 1000;
	}
	std::string ToString() {
		char buf[16] = { 0 };
		sprintf_s(buf, "%d:%d:%d.%d", h, m, s, ms);
		return buf;
	}
	int h = 0;
	int m = 0;
	int s = 0;
	int ms = 0;
};

class CAudioThread : public AudioThread {
public:
	QMutex mutex;
	bool ExportAudio(std::string src, std::string out, int beginMs, int outMs) {
		// 先使用工具不使用代码
		// ffmpeg -i test.mp4 -ss 0:1:10.112 -t 0:1:10.100 -vn -y test.mp3
		string cmd = "ffmpeg ";
		cmd += " -i ";
		cmd += src;
		cmd += " ";
		if (beginMs > 0) {
			cmd += " -ss ";
			Time t(beginMs);
			cmd += t.ToString();
		}

		if (outMs > 0) {
			cmd += " -t ";
			Time t(outMs);
			cmd += t.ToString();
		}

		cmd += " -vn -y ";
		cmd += out;
		cout << cmd << endl;
		mutex.lock();
		system(cmd.c_str());
		mutex.unlock();
		return true;
	}

	bool Merge(std::string v, std::string a, std::string out) {
		// ffmpeg -i test.avi -i test.mp3 -c copy out.avi
		string cmd = "ffmpeg ";
		cmd += " -i ";
		cmd += v;
		cmd += " -i ";
		cmd += a;
		cmd += " -c copy ";
		cmd += out;

		mutex.lock();
		system(cmd.c_str());
		mutex.unlock();
		return true;
	}
};

AudioThread* AudioThread::Get() {
	static CAudioThread ca;
	return &ca;
}


AudioThread::AudioThread() {}
AudioThread::~AudioThread() {}
