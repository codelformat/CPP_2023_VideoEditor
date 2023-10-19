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
		///4.��ʼ����Ƶ������
		if (!CreateCodec(AV_CODEC_ID_AAC)) {
			return false;
		}
		ac->bit_rate = 317 * 1000; // ������
		ac->sample_rate = sampleRate; // ������
		ac->sample_fmt = (AVSampleFormat)outSampleFmt; // ������ʽ
		ac->channels = channels; // ��������
		ac->channel_layout = av_get_default_channel_layout(channels); // ��������
		return OpenCodec(&ac);
	}

	bool InitVideoCodec() {
		///4.��ʼ������������
		//a.�ҵ�������
		if (!CreateCodec(AV_CODEC_ID_H264)) {
			return false;
		}
		vc->bit_rate = 50 * 1024 * 8; //ѹ����ÿ����Ƶ��bitλ��С 50kB
		vc->width = outWidth;
		vc->height = outHeight;
		vc->time_base = { 1,fps }; //1���� ֡�� 25fps
		vc->framerate = { fps, 1 };
		//������Ĵ�С��50֡һ���ؼ�֡��
		vc->gop_size = 50;
		vc->max_b_frames = 0; //��ҪB֡
		vc->pix_fmt = AV_PIX_FMT_YUV420P; //���ظ�ʽ
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
		///h264����
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

	bool InitResample() override {
		/////��Ƶ�ز���
	////��Ƶ��ʽΪFloat������aac���룬�����ز���
		asc = swr_alloc_set_opts(NULL,
			av_get_default_channel_layout(channels), (AVSampleFormat)outSampleFmt, sampleRate,//�����ʽ
			av_get_default_channel_layout(channels), (AVSampleFormat)inSampleFmt, sampleRate, 0, 0);//�����ʽ
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
		std::cout << "��Ƶ�ز��� �����ĳ�ʼ���ɹ���" << std::endl;

		/////��Ƶ�ز����ռ����
		pcm = av_frame_alloc();
		pcm->format = outSampleFmt;
		pcm->channels = channels;
		pcm->channel_layout = av_get_default_channel_layout(channels);
		pcm->nb_samples = nb_samples;  //һ֡��Ƶ�Ĳ�������
		ret = av_frame_get_buffer(pcm, 0);   //��pcm����洢�ռ�
		if (ret != 0) {
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			std::cout << err << std::endl;
			return false;
		}
		return true;
	}

	AVFrame* Resample(char* data) {
		//�ز���
		
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
		//�ز���

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
		//����Ƶ������
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
		// ��Ƶ������������
		ac = avcodec_alloc_context3(codec);
		if (!ac) {
			std::cout << "avcodec_alloc_context3 failed!" << std::endl;
			return false;
		}
		std::cout << "avcodec_alloc_context3 success!" << std::endl;

		ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; // ȫ�ֲ�������Ƶ��Ϣ��Э��ͷ
		ac->thread_count = getCPUNum(); // �����߳�����
		return true;
	}
	SwsContext* vsc = NULL; // ���ظ�ʽת��������
	SwrContext* asc = NULL; //��Ƶ�ز���������
	AVFrame* yuv = NULL; // ��������ݽṹyuv
	AVFrame* pcm = NULL; // ��������ݽṹpcm
	AVPacket vpack = { 0 };  // ��Ƶ֡����
	AVPacket apack = { 0 };  // ��Ƶ֡����
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