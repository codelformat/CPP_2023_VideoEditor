#pragma once
// VideoTranscoder.h

#include <string>
#include <chrono>
#include <thread>
#include <QtCore\QThread>
#include <QtCore\QMutex>

extern "C" {
#include <libavformat/avformat.h>
}

class VideoTranscoder:public QThread
{
public:
    VideoTranscoder();
    static VideoTranscoder* Get();
    ~VideoTranscoder();

    bool open();
    void transcode();
    void setUrl(const std::string& input, const std::string& output);

    void stopTranscoding() {
        isRunning = false;
    }
    void close();

    //线程入口函数
    void run();

private:
    QMutex mutex;
    std::atomic<bool> isRunning = false;

    std::string inputUrl;
    std::string outputUrl;

    AVFormatContext* in_ctx = nullptr;
    AVFormatContext* out_ctx = nullptr;
    AVRational time_base;
    double frame_rate = 0.0;
    int64_t frame_delay = 0;

    std::chrono::high_resolution_clock::time_point start_time;

    bool openInput();
    bool openOutput();
    void copyStreamContext();
    void calculateFrameDelay();
   // bool readAndWriteFrame();
};

