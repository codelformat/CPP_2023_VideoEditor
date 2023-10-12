#pragma once

class AVCodecContext;
class AVPacket;
class Rtmp
{
public:
	static Rtmp* Get(unsigned char index = 0);

	// 初始化封装器上下文
	virtual bool Init(const char* url) = 0;

	// 添加音视频流
	virtual bool AddStream(const AVCodecContext* c) = 0;

	// 打开rtmp网络IO 发送封装头
	virtual bool SendHead() = 0;

	// Rtmp帧推流
	virtual bool SendFrame(AVPacket* pkt) = 0;
	
	virtual ~Rtmp();
protected:
	Rtmp();
};

