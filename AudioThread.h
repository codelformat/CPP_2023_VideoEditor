#pragma once
#include <string>
class AudioThread
{
public:
	static AudioThread* Get();

	// ������Ƶ�ļ�
	/**
	* @params string src Դ�ļ�
	* @params string out ����ļ�(MP3)
	* @params int beginMs ��ʼʱ��(ms)
	* @params int outMs ���ʱ��(ms)
	*/
	virtual bool ExportAudio(std::string src, std::string out, int beginMs=0, int outMs=0) = 0;

	// �ϲ���Ƶ�ļ�
	/**
	* @params v ��Ƶ�ļ�(avi)
	* @params a ��Ƶ�ļ�(mp3)
	* @params out ����ļ�(avi)
	*/
	virtual bool Merge(std::string v, std::string a, std::string out) = 0;
	AudioThread();
	virtual ~AudioThread();
};

