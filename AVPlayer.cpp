#include "AVPlayer.h"
AVPlayer* AVPlayer::Get(VideoThread* videoThread, AudioQThread* audioThread) {
    static AVPlayer avplayer(videoThread, audioThread);
    return &avplayer;
}

AVPlayer::AVPlayer(VideoThread* videoThread, AudioQThread* audioThread)
    : videoThread(videoThread), audioThread(audioThread)
{
    /*connect(videoThread, &VideoThread::videoFrameDecoded, this, &AVPlayer::onVideoFrameDecoded);
    connect(audioThread, &AudioQThread::audioFrameDecoded, this, &AVPlayer::onAudioFrameDecoded);*/
    QObject::connect(VideoThread::Get(), SIGNAL(videoFrameDecoded(cv::Mat, int64_t)), this, SLOT(onVideoFrameDecoded(cv::Mat, int64_t)));
    QObject::connect(AudioQThread::Get(), SIGNAL(audioFrameDecoded(uint8_t*, int, int64_t)), this, SLOT(onAudioFrameDecoded(uint8_t*, int, int64_t)));

    // Initialize SDL2 audio subsystem
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        // handle error
        std::cout << "SDL init failed" << std::endl;
    }
}

bool AVPlayer::Open(const std::string& file) {
    if (!AudioQThread::Get()->Open(file)) {
        std::cout << "Failed to open audio: " << file << std::endl;
        return false;
    }

    if (!VideoThread::Get()->Open(file)) {
        std::cout << "Failed to open video: " << file << std::endl;
        return false;
    }

    // Set up the SDL_AudioSpec structure
    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = audioThread->getSampleRate();
    desiredSpec.format = convertAudioFormat(audioThread->getSampleFormat()); // you might need to convert this to an SDL_AudioFormat value
    desiredSpec.channels = audioThread->getChannels();
    desiredSpec.samples = 4096; // this value depends on your application
    desiredSpec.callback = nullptr;

    audioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, nullptr, 0);
    // Open the audio device
    if (audioDeviceID == 0) {
        // handle error
        std::cout << "SDL open audio failed" << std::endl;
    }

    SDL_PauseAudioDevice(audioDeviceID, 0);

    return true;
}

bool AVPlayer::Seek(double pos) {
    if (!VideoThread::Get()->Seek(pos)) {
        std::cout << "AV Video Seek failed" << std::endl;
        return false;
    }
    if (!AudioQThread::Get()->Seek(pos))
    {
        std::cout << "AV Audio Seek failed" << std::endl;
        return false;
    }

    return true;
}

AVPlayer::~AVPlayer() {
    SDL_CloseAudio();
    SDL_Quit();
}