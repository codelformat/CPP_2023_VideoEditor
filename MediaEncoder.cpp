#include "MediaEncoder.h"
extern "C"
{
#include <libavformat/avformat.h> // test
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
		if (yuv)
		{
			av_frame_free(&yuv);
		}
		if (vc) {
			avcodec_free_context(&vc);
		}
		vpts = 0;
		av_packet_unref(&pack);
	}

	bool InitVideoCodec() {
		///4.初始化编码上下文
		//a.找到编码器
		const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec) {
			cout << "avcodec_find_encoder failed!";
			return false;
		}
		vc = avcodec_alloc_context3(codec);
		if (!vc) {
			cout << "avcodec_find_encoder failed!" << endl;
			return false;
		}
		//b.配置编码器参数
		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //全局参数
		vc->codec_id = codec->id;
		vc->thread_count = 16; //编码线程数量

		vc->bit_rate = 50 * 1024 * 8; //压缩后每秒视频的bit位大小 50kB
		vc->width = outWidth;
		vc->height = outHeight;
		vc->time_base = { 1,fps }; //1秒钟 帧率 25fps
		vc->framerate = { fps, 1 };

		//画面组的大小（50帧一个关键帧）
		vc->gop_size = 50;
		vc->max_b_frames = 0; //不要B帧
		vc->pix_fmt = AV_PIX_FMT_YUV420P; //像素格式

		//c.打开编码器上下文
		int ret = avcodec_open2(vc, 0, 0); // 创建的时候已经指定codec
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << buf << endl;
			return false;
		}
		std::cout << "avcodec_open2 success!" << std::endl;
		return true;
	}

	AVPacket* EncodeVideo(AVFrame* frame) {
		av_packet_unref(&pack);
		///h264编码
		frame->pts = vpts++;
		int ret = avcodec_send_frame(vc, frame);
		if (ret != 0) {
			std::cout << "avcodec_send_frame failed!" << std::endl;
			return NULL;
		}

		ret = avcodec_receive_packet(vc, &pack);
		if (ret != 0 || pack.size <= 0) {
			std::cout << "avcodec_receive_packet failed!" << std::endl;
			return NULL;
		}

		return &pack;
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

	CMediaEncoder() {}
	~CMediaEncoder(){}
private:
	SwsContext* vsc = NULL; // 像素格式转换上下文
	AVFrame* yuv = NULL; // 输出的数据结构yuv

	AVPacket pack = { 0 }; 
	int vpts = 0;
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