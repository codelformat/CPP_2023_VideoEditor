#pragma once

/// <summary>
/// ����Ƶ����ӿ���
/// </summary>

struct AVFrame;
struct AVPacket;
class AVCodecContext;

class MediaEncoder
{
public:

	///�������
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;

	///�������
	int outWidth = 1280;
	int outHeight = 720;
	int bit_rate = 40000000; //ѹ����ÿ����Ƶ��bitλ��С 50kB
	int fps = 25; // ����ͷ֡��

	//��������������
	static MediaEncoder* Get(unsigned char index = 0);

	virtual void Close() = 0;

	//��ʼ�����ظ�ʽת����������
	virtual bool InitScale() = 0;

	virtual AVFrame* RGBToYUV(char* rgb) = 0;

	//��Ƶ��������ʼ��
	virtual bool InitVideoCodec() = 0;

	//��Ƶ����
	virtual AVPacket* EncodeVideo(AVFrame* frame) = 0;
	
	virtual ~MediaEncoder();
	AVCodecContext* vc = 0;	//������������

protected:
	MediaEncoder();
};

