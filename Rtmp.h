#pragma once

class AVCodecContext;
class AVPacket;
class Rtmp
{
public:
	static Rtmp* Get(unsigned char index = 0);

	// ��ʼ����װ��������
	virtual bool Init(const char* url) = 0;

	// �������Ƶ��
	virtual bool AddStream(const AVCodecContext* c) = 0;

	// ��rtmp����IO ���ͷ�װͷ
	virtual bool SendHead() = 0;

	// Rtmp֡����
	virtual bool SendFrame(AVPacket* pkt) = 0;
	
	virtual ~Rtmp();
protected:
	Rtmp();
};

