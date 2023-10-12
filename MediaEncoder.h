#pragma once

/// <summary>
/// 音视频编码接口类
/// </summary>

struct AVFrame;
struct AVPacket;
class AVCodecContext;

class MediaEncoder
{
public:

	///输入参数
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;

	///输出参数
	int outWidth = 1280;
	int outHeight = 720;
	int bit_rate = 40000000; //压缩后每秒视频的bit位大小 50kB
	int fps = 25; // 摄像头帧率

	//工厂的生产方法
	static MediaEncoder* Get(unsigned char index = 0);

	virtual void Close() = 0;

	//初始化像素格式转换的上下文
	virtual bool InitScale() = 0;

	virtual AVFrame* RGBToYUV(char* rgb) = 0;

	//视频编码器初始化
	virtual bool InitVideoCodec() = 0;

	//视频编码
	virtual AVPacket* EncodeVideo(AVFrame* frame) = 0;
	
	virtual ~MediaEncoder();
	AVCodecContext* vc = 0;	//编码器上下文

protected:
	MediaEncoder();
};

