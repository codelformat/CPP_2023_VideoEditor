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

// ��ȡCPU��������
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
		///4.��ʼ������������
		//a.�ҵ�������
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
		//b.���ñ���������
		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //ȫ�ֲ���
		vc->codec_id = codec->id;
		vc->thread_count = 16; //�����߳�����

		vc->bit_rate = 50 * 1024 * 8; //ѹ����ÿ����Ƶ��bitλ��С 50kB
		vc->width = outWidth;
		vc->height = outHeight;
		vc->time_base = { 1,fps }; //1���� ֡�� 25fps
		vc->framerate = { fps, 1 };

		//������Ĵ�С��50֡һ���ؼ�֡��
		vc->gop_size = 50;
		vc->max_b_frames = 0; //��ҪB֡
		vc->pix_fmt = AV_PIX_FMT_YUV420P; //���ظ�ʽ

		//c.�򿪱�����������
		int ret = avcodec_open2(vc, 0, 0); // ������ʱ���Ѿ�ָ��codec
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
		///h264����
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
		// 2,��ʼ����ʽת��������
		vsc = sws_getCachedContext(vsc,
			// Դ��Ⱥ͸߶ȣ���ʽ
			inWidth, inHeight, AV_PIX_FMT_BGR24,
			// Ŀ���Ⱥ͸߶�,��ʽ
			inWidth, inHeight, AV_PIX_FMT_YUV420P,
			// ��ֵ�㷨
			SWS_BICUBIC, // �ߴ�仯ʱʹ�ø��㷨
			0, 0, 0 // ������,��ѡ��
		);

		if (!vsc) {
			cout << "sws_getCachedContext failed" << endl;
			return false;
		}

		//��������ݽṹ
		// ����yuv����Ŀռ�
		yuv = av_frame_alloc();
		yuv->format = AV_PIX_FMT_YUV420P;
		yuv->width = inWidth;
		yuv->height = inHeight;
		yuv->pts = 0;
		// ����yuv�ռ�
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
		//��������ݸ�ʽ
		uint8_t* inData[AV_NUM_DATA_POINTERS] = { 0 };
		// bgrbgrbgr
		// plane bbbbbb gggggg rrrrr  indata[0] = bbbbbb
		inData[0] = (uint8_t*)rgb;
		int insize[AV_NUM_DATA_POINTERS] = { 0 };
		//һ�У������ݵ��ֽ���
		insize[0] = inWidth * inPixSize;

		int h = sws_scale(vsc, inData, insize, 0, inHeight, // ����Դ����
			  yuv->data, yuv->linesize);
		if (h <= 0) {
			return NULL;
		}
		return yuv;
	}

	CMediaEncoder() {}
	~CMediaEncoder(){}
private:
	SwsContext* vsc = NULL; // ���ظ�ʽת��������
	AVFrame* yuv = NULL; // ��������ݽṹyuv

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