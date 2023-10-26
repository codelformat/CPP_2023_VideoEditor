// VideoTranscoder.cpp
#include "VideoTranscoder.h"
#include <iostream>

using namespace std::chrono;
using namespace std;

VideoTranscoder* VideoTranscoder::Get(const std::string& input, const std::string& output) {
	static VideoTranscoder transcoder(input, output);
	return &transcoder;
}

VideoTranscoder::VideoTranscoder(const std::string& input, const std::string& output)
    : inputUrl(input), outputUrl(output) {
    avformat_network_init();
}

VideoTranscoder::~VideoTranscoder() {
    close();
}

bool VideoTranscoder::open() {
    if (!openInput()) {
        return false;
    }

    if (!openOutput()) {
        return false;
    }

    copyStreamContext();
    calculateFrameDelay();
    return true;
}

bool VideoTranscoder::openInput() {
    if (avformat_open_input(&in_ctx, inputUrl.c_str(), nullptr, nullptr) < 0) {
        // handle error
        cout << "open input file failed" << endl;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(in_ctx, nullptr) < 0) {
        // handle error
        cout << "find stream info failed" << endl;
    }

    return true;
}

bool VideoTranscoder::openOutput() {
    // Open output file
    const AVOutputFormat* out_fmt = av_guess_format("flv", nullptr, nullptr);
    if (!out_fmt) {
        // handle error
        cout << "guess format failed" << endl;
    }

    avformat_alloc_output_context2(&out_ctx, out_fmt, nullptr, outputUrl.c_str());
    if (!out_ctx) {
        // handle error
        cout << "alloc output context failed" << endl;
    }

    return true;
}

void VideoTranscoder::copyStreamContext() {
    // Copy stream contexts
    for (int i = 0; i < in_ctx->nb_streams; i++) {
        AVStream* in_stream = in_ctx->streams[i];
        AVStream* out_stream = avformat_new_stream(out_ctx, nullptr);
        avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
        out_stream->codecpar->codec_tag = 0;
    }

    // Open output file
    if (!(out_ctx->flags & AVFMT_NOFILE)) {
        if (avio_open(&out_ctx->pb, outputUrl.c_str(), AVIO_FLAG_WRITE) < 0) {
            cout << "open output file failed" << endl;
            // handle error
        }
    }

    // Write file header
    if (avformat_write_header(out_ctx, nullptr) < 0) {
        // handle error
        cout << "write header failed" << endl;
    }
}

void VideoTranscoder::calculateFrameDelay() {
    // Get the time base and frame rate of the first video stream
    time_base = { 1, 1 };
    frame_rate = 0.0;
    for (int i = 0; i < in_ctx->nb_streams; i++) {
        if (in_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            time_base = in_ctx->streams[i]->time_base;
            frame_rate = av_q2d(in_ctx->streams[i]->avg_frame_rate);
            break;
        }
    }

    // Calculate the delay between frames in microseconds
    frame_delay = frame_rate > 0 ? static_cast<int64_t>(1e6 / frame_rate) : 0;
}

void VideoTranscoder::transcode() {
    isRunning = true;
    start_time = high_resolution_clock::now();
    //while (readAndWriteFrame()) {
    //    // Loop until end of file or error
    //}
    // Read packets and write them to the output file
    AVPacket packet;
    while (av_read_frame(in_ctx, &packet) >= 0) {
        AVStream* in_stream = in_ctx->streams[packet.stream_index];
        AVStream* out_stream = out_ctx->streams[packet.stream_index];

        // Add delay to maintain the original frame rate
        auto current_time = std::chrono::high_resolution_clock::now();
        auto packet_time = start_time + std::chrono::microseconds(static_cast<int64_t>(packet.pts * av_q2d(time_base) * 1e6));
        if (current_time < packet_time) {
            std::this_thread::sleep_for(packet_time - current_time);
        }

        // Convert PTS/DTS
        packet.pts = av_rescale_q_rnd(packet.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        packet.dts = av_rescale_q_rnd(packet.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        packet.duration = av_rescale_q(packet.duration, in_stream->time_base, out_stream->time_base);
        packet.pos = -1;

        // Write the packet
        if (av_interleaved_write_frame(out_ctx, &packet) < 0) {
            // handle error
            cout << "$" << flush;
        }
        cout << "#" << flush;

        av_packet_unref(&packet);

        if (!isRunning)
            break;
    }

    this->close();
}

void VideoTranscoder::close() {
    if (out_ctx) {
        av_write_trailer(out_ctx);
        if (!(out_ctx->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&out_ctx->pb);
        }
        avformat_free_context(out_ctx);
        out_ctx = nullptr;
    }

    if (in_ctx) {
        avformat_close_input(&in_ctx);
        in_ctx = nullptr;
    }
}

