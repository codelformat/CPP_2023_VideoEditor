#pragma once
// VideoTranscoder.h

#include <string>
#include <chrono>
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
}

class VideoTranscoder
{
public:
    VideoTranscoder(const std::string& input, const std::string& output);
    static VideoTranscoder* Get(const std::string& input, const std::string& output);
    ~VideoTranscoder();

    bool open();
    void transcode();

    void stopTranscoding() {
        isRunning = false;
    }
    void close();

private:
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

