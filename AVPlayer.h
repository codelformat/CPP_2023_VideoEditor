#ifndef AVPLAYER_H
#define AVPLAYER_H

#include <QtCore/QObject>
#include <queue>
#include <utility>
#include <stdint.h>
#include <SDL.h>
#include <iostream>
//#include <QtMultimedia/QAudioOutput>
//#include <QtMultimedia/QAudioFormat>
//#include <QtMultimedia/QAudioDevice>
#include "VideoThread.h"
#include "AudioQThread.h"
class AVPlayer :
  public QObject
{
    Q_OBJECT

public:
    static AVPlayer* Get(VideoThread* videoThread, AudioQThread* audioThread);

    AVPlayer(VideoThread* videoThread, AudioQThread* audioThread);

    bool Open(const std::string& file);

    bool Seek(double pos);

    ~AVPlayer();

public slots:
    void onVideoFrameDecoded(cv::Mat frame, int64_t pts) {
        // Display the video frame
        //ViewDes(frame);
        // If there is audio data in the buffer with a timestamp <= pts,
        // send it to the audio device
        std::cout << "Video frame decoded with timestamp: " << pts << "\n";
        if (audioBuffer.empty()) {
            std::cout << "Audio buffer is empty.\n";
        }
        else {
            std::cout << "Audio buffer front timestamp: " << audioBuffer.front().first << "\n";
        }
        //std::cout << "$";
        while (!audioBuffer.empty() && audioBuffer.front().first <= pts) {
            int ret = SDL_QueueAudio(1, audioBuffer.front().second.first, audioBuffer.front().second.second);
            if (ret) {
                std::cout << "!SDLF!";
            }
            else {
				std::cout << "SDLF";
            }
            //av_freep((audioBuffer.front().second.first)); 
            
            audioBuffer.pop();
        }
    }

    void onAudioFrameDecoded(uint8_t* data, int nb_samples, int64_t pts) {
        std::cout << "Audio frame decoded with timestamp: " << pts << "\n";
        // Store the audio data in the buffer with its timestamp
        audioBuffer.push(std::make_pair(pts, std::make_pair(data, nb_samples * 2 /* assuming 16-bit stereo audio */)));
    }


protected:
    VideoThread* videoThread;
    AudioQThread* audioThread;
    std::queue<std::pair<int64_t, std::pair<uint8_t*, int>>> audioBuffer;
    SDL_AudioDeviceID audioDeviceID;

    SDL_AudioFormat convertAudioFormat(AVSampleFormat avFormat)
    {
        switch (avFormat)
        {
        case AV_SAMPLE_FMT_U8:
        case AV_SAMPLE_FMT_U8P:
            return AUDIO_U8;
        case AV_SAMPLE_FMT_S16:
        case AV_SAMPLE_FMT_S16P:
            return AUDIO_S16SYS;
        case AV_SAMPLE_FMT_S32:
        case AV_SAMPLE_FMT_S32P:
            return AUDIO_S32SYS;
        case AV_SAMPLE_FMT_FLT:
        case AV_SAMPLE_FMT_FLTP:
            return AUDIO_F32SYS;
        default:
            return AUDIO_S16SYS; // default to signed 16-bit PCM
        }
    }
};

#endif