#include "MediaEncoder.h"
extern "C"
{
#include <libavformat/avformat.h> // test
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#include <iostream>
#if defined WIN32 || defined _WIN32
#include <Windows.h>
#endif
using namespace std;

// 获取CPU核心数量
static int getCPUNum() {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	return (int)sysinfo.dwNumberOfProcessors;
}

class CMediaEncoder : public MediaEncoder {
public:
	void Close() {
		if (vsc)
		{
			sws_freeContext(vsc);
			vsc = NULL;
		}
		if (asc) 
		{
			swr_free(&asc);
		}
		if (yuv)
		{
			av_frame_free(&yuv);
		}
		if (vc) {
			avcodec_free_context(&vc);
		}
		if (pcm)
		{
			av_frame_free(&pcm);
		}
		
		vpts = 0;
		av_packet_unref(&vpack);
		apts = 0;
		av_packet_unref(&apack);
	}



	bool InitAudioCodec() override {
		///4.初始化音频编码器
		if (!CreateCodec(AV_CODEC_ID_AAC)) {
			return false;
		}
		ac->bit_rate = 317 * 1000; // 比特率
		ac->sample_rate = sampleRate; // 采样率
		ac->sample_fmt = (AVSampleFormat)outSampleFmt; // 采样格式
		ac->channels = channels; // 声道数量
		ac->channel_layout = av_get_default_channel_layout(channels); // 声道布局
		return OpenCodec(&ac);
	}

	bool InitVideoCodec() {
		///4.初始化编码上下文
		//a.找到编码器
		if (!CreateCodec(AV_CODEC_ID_H264)) {
			return false;
		}
		vc->bit_rate = 50 * 1024 * 8; //压缩后每秒视频的bit位大小 50kB
		vc->width = outWidth;
		vc->height = outHeight;
		vc->time_base = { 1,fps }; //1秒钟 帧率 25fps
		vc->framerate = { fps, 1 };
		//画面组的大小（50帧一个关键帧）
		vc->gop_size = 50;
		vc->max_b_frames = 0; //不要B帧
		vc->pix_fmt = AV_PIX_FMT_YUV420P; //像素格式
		return OpenCodec(&vc);
	}

	AVPacket* EncodeAudio(AVFrame* frame) {
		pcm->pts = apts;
		apts += av_rescale_q(pcm->nb_samples, { 1, sampleRate }, ac->time_base);

		int ret = avcodec_send_frame(ac, pcm);
		if (ret != 0) {
			std::cout << "avcodec_send_frame failed!" << std::endl;
			return NULL;
		}

		av_packet_unref(&apack);
		ret = avcodec_receive_packet(ac, &apack);
		if(ret != 0) {
			std::cout << "$" << std::flush;
			return NULL;
		}

		return &apack;
	}

	AVPacket* EncodeVideo(AVFrame* frame) {
		av_packet_unref(&vpack);
		///h264编码
		frame->pts = vpts++;
		int ret = avcodec_send_frame(vc, frame);
		if (ret != 0) {
			std::cout << "avcodec_send_frame failed!" << std::endl;
			return NULL;
		}

		ret = avcodec_receive_packet(vc, &vpack);
		if (ret != 0 || vpack.size <= 0) {
			std::cout << "avcodec_receive_packet failed!" << std::endl;
			return NULL;
		}

		return &vpack;
	}

	bool InitScale() override{
		// 2,初始化格式转换上下文
		vsc = sws_getCachedContext(vsc,
			// 源宽度和高度，格式
			inWidth, inHeight, AV_PIX_FMT_BGR24,
			// 目标宽度和高度,格式
			inWidth, inHeight, AV_PIX_FMT_YUV420P,
			// 插值算法
			SWS_BICUBIC, // 尺寸变化时使用该算法
			0, 0, 0 // 过滤器,可选项
		);

		if (!vsc) {
			cout << "sws_getCachedContext failed" << endl;
			return false;
		}

		//输出的数据结构
		// 分配yuv对象的空间
		yuv = av_frame_alloc();
		yuv->format = AV_PIX_FMT_YUV420P;
		yuv->width = inWidth;
		yuv->height = inHeight;
		yuv->pts = 0;
		// 分配yuv空间
		int ret = av_frame_get_buffer(yuv, 32);
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw std::exception(buf);
		}

		return true;
	}

	AVFrame* RGBToYUV(char* rgb) override{
		//rgb to yuv
		//输入的数据格式
		uint8_t* inData[AV_NUM_DATA_POINTERS] = { 0 };
		// bgrbgrbgr
		// plane bbbbbb gggggg rrrrr  indata[0] = bbbbbb
		inData[0] = (uint8_t*)rgb;
		int insize[AV_NUM_DATA_POINTERS] = { 0 };
		//一行（宽）数据的字节数
		insize[0] = inWidth * inPixSize;

		int h = sws_scale(vsc, inData, insize, 0, inHeight, // 输入源数据
			  yuv->data, yuv->linesize);
		if (h <= 0) {
			return NULL;
		}
		return yuv;
	}

	bool InitResample() override {
		/////音频重采样
	////视频格式为Float，符合aac编码，不用重采样
		asc = swr_alloc_set_opts(NULL,
			av_get_default_channel_layout(channels), (AVSampleFormat)outSampleFmt, sampleRate,//输出格式
			av_get_default_channel_layout(channels), (AVSampleFormat)inSampleFmt, sampleRate, 0, 0);//输入格式
		if (!asc)
		{
			std::cout << "swr_alloc_set_opts failed" << std::endl;
			return false;
		}
		int ret = swr_init(asc);
		if (ret != 0) {
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			std::cout << err << std::endl;
			return false;
		}
		std::cout << "音频重采样 上下文初始化成功！" << std::endl;

		/////音频重采样空间分配
		pcm = av_frame_alloc();
		pcm->format = outSampleFmt;
		pcm->channels = channels;
		pcm->channel_layout = av_get_default_channel_layout(channels);
		pcm->nb_samples = nb_samples;  //一帧音频的采样数量
		ret = av_frame_get_buffer(pcm, 0);   //给pcm分配存储空间
		if (ret != 0) {
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			std::cout << err << std::endl;
			return false;
		}
		return true;
	}

	AVFrame* Resample(char* data) {
		//重采样
		
        const uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
        indata[0] = (uint8_t*)data;
        int len = swr_convert(asc, pcm->data, pcm->nb_samples,
            indata, pcm->nb_samples
        );
		if (len <= 0)
		{
			return NULL;
		}

		return pcm;
	}


	//UNDO
	AVFrame* Resample(uint8_t* data) {
		//重采样

		const uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = data;
		int len = swr_convert(asc, pcm->data, pcm->nb_samples,
			indata, pcm->nb_samples
		);
		if (len <= 0)
		{
			std::cout << "Convert failed!" << std::endl;
			return NULL;
		}

		return pcm;
	}

	CMediaEncoder() {}
	~CMediaEncoder(){}
private:
	bool OpenCodec(AVCodecContext** c) {
		//打开音频编码器
		int ret = avcodec_open2(*c, 0, 0);
		if (ret != 0) {
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			std::cout << err << std::endl;
			avcodec_free_context(c);
			return false;
		}
		std::cout << "avcodec_open2 success!" << std::endl;
		return true;
	}
	bool CreateCodec(AVCodecID cid) {
		const AVCodec* codec = avcodec_find_encoder(cid);
		if (!codec) {
			std::cout << "avcodec_find_encoder " << cid << " failed!" << std::endl;
			return false;
		}
		// 音频编码器上下文
		ac = avcodec_alloc_context3(codec);
		if (!ac) {
			std::cout << "avcodec_alloc_context3 failed!" << std::endl;
			return false;
		}
		std::cout << "avcodec_alloc_context3 success!" << std::endl;

		ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; // 全局参数，音频信息的协议头
		ac->thread_count = getCPUNum(); // 编码线程数量
		return true;
	}
	SwsContext* vsc = NULL; // 像素格式转换上下文
	SwrContext* asc = NULL; //音频重采样上下文
	AVFrame* yuv = NULL; // 输出的数据结构yuv
	AVFrame* pcm = NULL; // 输出的数据结构pcm
	AVPacket vpack = { 0 };  // 视频帧数据
	AVPacket apack = { 0 };  // 音频帧数据
	int vpts = 0;
	int apts = 0;
};
 

MediaEncoder* MediaEncoder::Get(unsigned char index) {
	static bool isFirst = true;

	static CMediaEncoder cxm[255];
	return &cxm[index];
}

MediaEncoder::MediaEncoder() {

}

MediaEncoder::~MediaEncoder() {

}