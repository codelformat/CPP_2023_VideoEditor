#ifndef AUDIOQTHREAD_H
#define AUDIOQTHREAD_H

#include <QtCore\QThread>
#include <QtCore\QObject>
#include <QtCore\QMutex>
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class AudioQThread:public QThread
{
	Q_OBJECT

public:
	int begin = 0;
	int end = 999;

	AudioQThread();
	static AudioQThread* Get();
	void run();
	bool Open(const std::string file);

	int getSampleRate();

	AVSampleFormat getSampleFormat();

	int getChannels();

	bool isThreadOpen();

	void SetBegin(double p);

	void SetEnd(double p);

	bool Seek(double pos);

	bool Seek(int64_t frame); 

	void Play();

	void Pause();
	~AudioQThread();

signals:
	void audioFrameDecoded(uint8_t* data, int nb_samples, int64_t pts);

protected:
	QMutex mutex;
	const char* audioUrl;

	AVFormatContext* audio_ctx = nullptr;
	AVCodecContext* codec_ctx = nullptr;
	const AVCodec* codec = nullptr;
	int audio_stream_index = -1;
	bool isPlay = false;
	AVRational time_base;
	double frame_rate;
};


#endif // AUDIOQTHREAD_H

