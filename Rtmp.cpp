#include "Rtmp.h"
#include <iostream>
#include <string>
extern "C"
{
#include <libavformat/avformat.h> // test
#include <libavcodec/avcodec.h>
}
using namespace std;

class CRtmp: public Rtmp{
public:

	void Close() {
		if (ic) {
			avformat_close_input(&ic);
			vs = NULL;
		}
		vc = NULL;
		url = "";

	}

	bool Init(const char* outUrl) {
		//a.创建输出封装器上下文
		int ret = avformat_alloc_output_context2(&ic, 0, "flv", outUrl);
		this->url = outUrl;
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << "avformat_alloc_output_context2 failed:" << buf << endl;
			return false;
		}

		return true;
	}

	bool AddStream(const AVCodecContext* c) {
		if(!c) return false;
		if (c->codec_type == AVMEDIA_TYPE_VIDEO) {
			vc = c;
		}
		//b.添加视频流
		AVStream* st = avformat_new_stream(ic, NULL); // 0表示视频流
		if (!st) {
			cout << "avformat_new_stream failed" ;
			return false;
		}
		st->codecpar->codec_tag = 0; // 标签
		avcodec_parameters_from_context(st->codecpar, c); // 从编码器复制参数
		av_dump_format(ic, 0, url.c_str(), 1); // 打印封装格式信息

		if (c->codec_type == AVMEDIA_TYPE_VIDEO) {
			vc = c;
			vs = st;
		}

		return true;
	}

	bool SendHead() {
		///打开rtmp的网络输出IO
		int ret = avio_open(&ic->pb, url.c_str(), AVIO_FLAG_WRITE); // 打开IO
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << "avio_open failed:" << buf << endl;
			return false;
		}
		//写入封装头
		ret = avformat_write_header(ic, NULL); // ！！该过程会把vs的timebase改动
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << "avformat_write_header failed:" << buf << endl;
			return false;
		}
		return true;
	}

	bool SendFrame(AVPacket* pack) {
		//推流
		pack->pts = av_rescale_q(pack->pts, vc->time_base, vs->time_base);
		pack->dts = av_rescale_q(pack->dts, vc->time_base, vs->time_base);
		int ret = av_interleaved_write_frame(ic, pack);
		if (ret == 0) {
			std::cout << "#" << std::flush;
		}
		else {
			std::cout << "$" << std::flush;
		}
		return true;
	}
	CRtmp(){}
	~CRtmp(){}
private:
	//rtmp flv 封装器
	AVFormatContext* ic = NULL;
	//视频编码器
	const AVCodecContext* vc = NULL;

	AVStream* vs = NULL;
	string url = "";
};

Rtmp* Rtmp::Get(unsigned char index)
{
	static CRtmp cxr[255];
	static bool isFirst = true;
	if (isFirst) {
		avformat_network_init();
		isFirst = false;
	}
	return &cxr[index];
}

Rtmp::Rtmp()
{

}

Rtmp::~Rtmp()
{
}