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
		else {
			cout << "avformat_alloc_output_context2 success" << endl;
		}

		return true;
	}

	bool AddStream(const AVCodecContext* c) {
		if(!c) return false;
		
		//b.�������Ƶ��
		AVStream* st = avformat_new_stream(ic, NULL); // 0��ʾ��Ƶ��
		if (!st) {
			cout << "avformat_new_stream failed" ;
			return false;
		}
		else {
			cout << "avformat_new_stream success" << endl;
		}
		st->codecpar->codec_tag = 0; // ��ǩ
		avcodec_parameters_from_context(st->codecpar, c); // �ӱ��������Ʋ���
		av_dump_format(ic, 0, url.c_str(), 1); // ��ӡ��װ��ʽ��Ϣ

		if (c->codec_type == AVMEDIA_TYPE_VIDEO) {
			vc = c;
			vs = st;
		}
		else if (c->codec_type == AVMEDIA_TYPE_AUDIO) {
			ac = c;
			as = st;
		}
		else {
			return false;
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
		else {
			cout << "avio_open success" << endl;
		}
		//д���װͷ
		ret = avformat_write_header(ic, NULL); // �����ù��̻��vs��timebase�Ķ�
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << "avformat_write_header failed:" << buf << endl;
			return false;
		}
		else {
			cout << "avformat_write_header success" << endl;
		}
		return true;
	}

	bool SendFrame(AVPacket* pack) {
		if(!pack || pack->size <= 0 || !pack->data) return false;
		AVRational stime;
		AVRational dtime;
		//�ж�����Ƶ������Ƶ
		if (vs && vc && pack->stream_index == vs->index) {
			stime = vc->time_base;
			dtime = vs->time_base;
		}
		else if (as && ac && pack->stream_index == as->index) {
			stime = ac->time_base;
			dtime = as->time_base;
		}
		else {
			std::cout << "no video or audio!" << std::endl;
			return false;
		}
		//����
		pack->pts = av_rescale_q(pack->pts, stime, dtime);
		pack->dts = av_rescale_q(pack->dts, stime, dtime);
		pack->duration = av_rescale_q(pack->duration, stime, dtime);
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

	//��Ƶ������
	const AVCodecContext* ac = NULL;

	//��Ƶ��
	AVStream* vs = NULL;

	//��Ƶ��
	AVStream* as = NULL;

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