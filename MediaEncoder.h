#pragma once

extern "C"
{
//#include <libswresample/swresample.h>

}
/// <summary>
/// ����Ƶ����ӿ���
/// </summary>

struct AVFrame;
struct AVPacket;
class AVCodecContext;

enum SampleFMT {
	SAMPLE_FMT_NONE = -1,
	SAMPLE_FMT_U8,          ///< unsigned 8 bits
	SAMPLE_FMT_S16,         ///< signed 16 bits
	SAMPLE_FMT_S32,         ///< signed 32 bits
	SAMPLE_FMT_FLT,         ///< float
	SAMPLE_FMT_DBL,         ///< double

	SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
	SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
	SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
	SAMPLE_FMT_FLTP,        ///< float, planar
	SAMPLE_FMT_DBLP,        ///< double, planar
	SAMPLE_FMT_S64,         ///< signed 64 bits
	SAMPLE_FMT_S64P,        ///< signed 64 bits, planar

	SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};

class MediaEncoder
{
public:

	///�������
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;
	int channels = 2;
	int sampleRate = 44100;
	int inSampleFmt = SAMPLE_FMT_S16; //������Ƶ������ʽ
	

	///�������
	int outWidth = 1280;
	int outHeight = 720;
	int bit_rate = 40000; //ѹ����ÿ����Ƶ��bitλ��С 50kB
	int fps = 25; // ����ͷ֡��
	int nb_samples = 1024; //��Ƶһ֡�Ĳ�������
	int outSampleFmt = SAMPLE_FMT_FLTP; //�����Ƶ������ʽ

	//��������������
	static MediaEncoder* Get(unsigned char index = 0);

	virtual void Close() = 0;

	//��ʼ�����ظ�ʽת����������
	virtual bool InitScale() = 0;

	//��Ƶ�ز��������ĳ�ʼ��
	virtual bool InitResample() = 0;

	//����ֵ�������������
	virtual AVFrame* Resample(char* data) = 0;
	virtual AVFrame* Resample(uint8_t* data) = 0;

	//����ֵ�������������
	virtual AVFrame* RGBToYUV(char* rgb) = 0;

	//��Ƶ��������ʼ��
	virtual bool InitVideoCodec() = 0;

	//��Ƶ��������ʼ��
	virtual bool InitAudioCodec() = 0;

	//��Ƶ���� ����ֵ�������������
	virtual AVPacket* EncodeVideo(AVFrame* frame) = 0;

	//��Ƶ���� ����ֵ�������������
	virtual AVPacket* EncodeAudio(AVFrame* frame) = 0;
	
	virtual ~MediaEncoder();
	AVCodecContext* vc = 0;	//������������
	AVCodecContext* ac = 0;	//��Ƶ������������

protected:
	MediaEncoder();
};

