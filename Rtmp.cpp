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
		//a.���������װ��������
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
		//b.�����Ƶ��
		AVStream* st = avformat_new_stream(ic, NULL); // 0��ʾ��Ƶ��
		if (!st) {
			cout << "avformat_new_stream failed" ;
			return false;
		}
		st->codecpar->codec_tag = 0; // ��ǩ
		avcodec_parameters_from_context(st->codecpar, c); // �ӱ��������Ʋ���
		av_dump_format(ic, 0, url.c_str(), 1); // ��ӡ��װ��ʽ��Ϣ

		if (c->codec_type == AVMEDIA_TYPE_VIDEO) {
			vc = c;
			vs = st;
		}

		return true;
	}

	bool SendHead() {
		///��rtmp���������IO
		int ret = avio_open(&ic->pb, url.c_str(), AVIO_FLAG_WRITE); // ��IO
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << "avio_open failed:" << buf << endl;
			return false;
		}
		//д���װͷ
		ret = avformat_write_header(ic, NULL); // �����ù��̻��vs��timebase�Ķ�
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << "avformat_write_header failed:" << buf << endl;
			return false;
		}
		return true;
	}

	bool SendFrame(AVPacket* pack) {
		//����
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
	//rtmp flv ��װ��
	AVFormatContext* ic = NULL;
	//��Ƶ������
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