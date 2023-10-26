#pragma once

extern "C"
{
//#include <libswresample/swresample.h>

}
/// <summary>
/// 音视频编码接口类
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

	///输入参数
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;
	int channels = 2;
	int sampleRate = 44100;
	int inSampleFmt = SAMPLE_FMT_S16; //输入音频采样格式
	

	///输出参数
	int outWidth = 1280;
	int outHeight = 720;
	int bit_rate = 40000; //压缩后每秒视频的bit位大小 50kB
	int fps = 25; // 摄像头帧率
	int nb_samples = 1024; //音频一帧的采样数量
	int outSampleFmt = SAMPLE_FMT_FLTP; //输出音频采样格式

	//工厂的生产方法
	static MediaEncoder* Get(unsigned char index = 0);

	virtual void Close() = 0;

	//初始化像素格式转换的上下文
	virtual bool InitScale() = 0;

	//音频重采样上下文初始化
	virtual bool InitResample() = 0;

	//返回值无需调用者清理
	virtual AVFrame* Resample(char* data) = 0;
	virtual AVFrame* Resample(uint8_t* data) = 0;

	//返回值无需调用者清理
	virtual AVFrame* RGBToYUV(char* rgb) = 0;

	//视频编码器初始化
	virtual bool InitVideoCodec() = 0;

	//音频编码器初始化
	virtual bool InitAudioCodec() = 0;

	//视频编码 返回值无需调用者清理
	virtual AVPacket* EncodeVideo(AVFrame* frame) = 0;

	//音频编码 返回值无需调用者清理
	virtual AVPacket* EncodeAudio(AVFrame* frame) = 0;
	
	virtual ~MediaEncoder();
	AVCodecContext* vc = 0;	//编码器上下文
	AVCodecContext* ac = 0;	//音频编码器上下文

protected:
	MediaEncoder();
};

