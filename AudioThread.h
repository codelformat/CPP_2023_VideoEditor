#pragma once
#include <string>
class AudioThread
{
public:
	static AudioThread* Get();

	// 导出音频文件
	/**
	* @params string src 源文件
	* @params string out 输出文件(MP3)
	* @params int beginMs 开始时间(ms)
	* @params int outMs 输出时长(ms)
	*/
	virtual bool ExportAudio(std::string src, std::string out, int beginMs=0, int outMs=0) = 0;

	// 合并音频文件
	/**
	* @params v 视频文件(avi)
	* @params a 音频文件(mp3)
	* @params out 输出文件(avi)
	*/
	virtual bool Merge(std::string v, std::string a, std::string out) = 0;
	AudioThread();
	virtual ~AudioThread();
};

