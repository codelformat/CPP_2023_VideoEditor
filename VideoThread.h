#pragma once
#include <QtCore\QThread>
#include <QtCore\QMutex>
#include <opencv2\core.hpp>
class VideoThread:public QThread
{

	Q_OBJECT
    
public:
	int fps = 0;
	int width = 0;
	int height = 0;

	//����ģʽ ��ȡ����
	static VideoThread* Get() {
		static VideoThread vt;
		return &vt;
	}
	//��һ����ƵԴ�ļ�
	bool Open(const std::string file);
	void Play() { 
		mutex.lock();
		isPlay = true; 
		mutex.unlock();
	};
	void Pause() { 
		mutex.lock();
		isPlay = false;
		mutex.unlock();
	};

	bool isThreadOpen();
	// ���ص�ǰ���ŵ�λ��
	double GetPos();

	// ��ת��Ƶ
	/**
	 * @params frame int ֡λ��
	 */
	bool Seek(int frame);
	bool Seek(double pos);

	// ��ֻ֧��H264��ʽ
	// ��ʼ������Ƶ
	bool StartSave(const std::string filename, int width = 0, int height = 0, bool isColor = true);

	// ֹͣ������Ƶ��д����Ƶ֡������
	bool StopSave();

	~VideoThread();

	// �߳���ں���
	void run();

	// �źź���
signals:
	void ViewImage1(cv::Mat mat);

	// ��ʾ���ɺ�ͼ��
	void ViewDes(cv::Mat mat);
	void SaveEnd();
protected:
	QMutex mutex;
	
	//�Ƿ�ʼд��Ƶ
	bool isWrite = false;
	bool isPlay = false;
	VideoThread();
};

