#include "AudioQThread.h"
#include <chrono>
using namespace std::chrono;
using namespace std;
AudioQThread::AudioQThread() {
	start();
};
AudioQThread* AudioQThread::Get() {
	static AudioQThread vt;
	return &vt;
}
void AudioQThread::run() {
	// Read audio frames from the input file
	
	AVPacket packet;
	
	for (;;)
	{
		mutex.lock();
		auto start_time = high_resolution_clock::now();
		if (!isPlay) {
			mutex.unlock();
			msleep(5);
			continue;
		}
		if(av_read_frame(audio_ctx, &packet) >= 0)
		{
			std::cout << "#" << std::flush;
			if (packet.stream_index == audio_stream_index)
			{
				AVFrame* frame = av_frame_alloc();
				
				int response = avcodec_send_packet(codec_ctx, &packet);
				if (response >= 0) {
					response = avcodec_receive_frame(codec_ctx, frame);

					auto current_time = std::chrono::high_resolution_clock::now();
					auto packet_time = start_time + std::chrono::microseconds(static_cast<int64_t>(packet.pts * av_q2d(time_base) * 1e6));
					if (current_time < packet_time) {
						std::this_thread::sleep_for(packet_time - current_time);
					}
					// Emit a signal with the frame data and timestamp
					emit audioFrameDecoded(frame->data[0], frame->nb_samples, packet.pts);
				}
				av_frame_free(&frame);
			}
			av_packet_unref(&packet);
			mutex.unlock();
		}
		else {
			mutex.unlock();
			msleep(5);
			continue;
		}
		
		
	}
	// Close the audio file and free up memory
	avcodec_close(codec_ctx);
	avcodec_free_context(&codec_ctx);
	avformat_close_input(&audio_ctx);
}
bool AudioQThread::Open(const std::string file) {
	audioUrl = file.c_str();
	std::cout << "AudioQThread open: " << file << std::endl;
	mutex.lock(); // 加锁，防止多线程同时打开视频文件
	if (avformat_open_input(&audio_ctx, audioUrl, nullptr, nullptr) < 0) {
		// handle error
		std::cout << "AudioQThread open audio file failed" << std::endl;
		return false;
	}
	for (int i = 0; i < audio_ctx->nb_streams; i++) {
		if (audio_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			time_base = audio_ctx->streams[i]->time_base;
			frame_rate = av_q2d(audio_ctx->streams[i]->avg_frame_rate);
			audio_stream_index = i;
			break;
		}
	}

	// Create an AVCodecContext for the audio codec
	codec_ctx = avcodec_alloc_context3(nullptr);
	avcodec_parameters_to_context(codec_ctx, audio_ctx->streams[audio_stream_index]->codecpar);

	// Open the audio codec
	codec = avcodec_find_decoder(codec_ctx->codec_id);
	avcodec_open2(codec_ctx, codec, nullptr);
	mutex.unlock();// 一定要解锁
	//start();
	return true;
}

int AudioQThread::getSampleRate() {
	if (codec_ctx) {
		return codec_ctx->sample_rate;
	}
	return -1;
}

AVSampleFormat AudioQThread::getSampleFormat() {
	if (codec_ctx) {
		return codec_ctx->sample_fmt;
	}
	return AV_SAMPLE_FMT_NONE;
}

int AudioQThread::getChannels() {
	if (codec_ctx) {
		return codec_ctx->channels;
	}
	return -1;
}

bool AudioQThread::isThreadOpen() {
	return codec_ctx != nullptr;
}

void AudioQThread::SetBegin(double p) {
	mutex.lock();
	if (audio_ctx && audio_ctx->streams[audio_stream_index]) {
		int64_t count = audio_ctx->streams[audio_stream_index]->nb_frames;
		int64_t frame = p * count;
		begin = frame;
	}
	mutex.unlock();
}

void AudioQThread::SetEnd(double p) {
	mutex.lock();
	if (audio_ctx && audio_ctx->streams[audio_stream_index]) {
		int64_t count = audio_ctx->streams[audio_stream_index]->nb_frames;
		int64_t frame = p * count;
		end = frame;
	}
	mutex.unlock();
}

bool AudioQThread::Seek(double pos) {
	if (audio_ctx && audio_ctx->streams[audio_stream_index]) {
		int64_t count = audio_ctx->streams[audio_stream_index]->nb_frames;
		int64_t frame = count * pos;
		return Seek(frame);
	}
	return false;
}

bool AudioQThread::Seek(int64_t frame) {
	mutex.lock();
	if (!audio_ctx) {
		mutex.unlock();
		return false;
	}

	// Calculate the target timestamp in the stream's timebase
	AVRational time_base = audio_ctx->streams[audio_stream_index]->time_base;
	int64_t target_ts = av_rescale(frame, time_base.den, time_base.num);

	// Seek to the target timestamp
	int ret = av_seek_frame(audio_ctx, audio_stream_index, target_ts, AVSEEK_FLAG_ANY);
	mutex.unlock();

	return ret >= 0;
}

void AudioQThread::Play() {
	mutex.lock();
	isPlay = true;
	mutex.unlock();
}

void AudioQThread::Pause() {
	mutex.lock();
	isPlay = false;
	mutex.unlock();
}

AudioQThread::~AudioQThread() {
	mutex.lock();
	if (codec_ctx) {
		avcodec_free_context(&codec_ctx);
		codec_ctx = nullptr;
	}
	if (audio_ctx) {
		avformat_close_input(&audio_ctx);
		audio_ctx = nullptr;
	}
	mutex.unlock();
}
