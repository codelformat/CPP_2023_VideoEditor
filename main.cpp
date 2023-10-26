#include "VideoUI.h"
#include "VideoTranscoder.h"
#include <QtWidgets/QApplication>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <exception>
#include <chrono>
#include <string>
#include <thread>

#include "MediaEncoder.h"
#include "Rtmp.h"

using namespace std;
/**
* ��������
* ����ImageProcess��ʵ���㷨
* ����VideoFilter������㷨����������õ��ú���
* �����VideoUI����Ӱ�ť���źŲ�
*
* ����ܹ�
* VideoUI�ǽ���
* VideoThread����Ƶ��д�߳�
* VideoFilter���˾�
* ImageProcess��ͼ����
*
*/
int main(int argc, char* argv[])
{
	if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
	QApplication a(argc, argv);
	//VideoUI w;
	//w.show();
	
	//return a.exec();

	//test

	const char* inUrl = "D:\\Cpp_2023_project\\bin\\test.mp4";
	const char* outUrl = "rtmp://localhost/live";

	VideoTranscoder* transcoder = VideoTranscoder::Get(inUrl, outUrl);

	if (!transcoder->open()) {
		return 1;
	}

	std::thread transcode_thread(&VideoTranscoder::transcode, transcoder);
	

	string cmd = "ffplay ";
	cmd += outUrl;
	system(cmd.c_str());

	delete transcoder;


	return a.exec();
}













// test
//// �����������ظ�ʽת������
//AVPacket packet;

//int sampleRate = 44100;
//int channels = 2;
//int sampleByte = 2;
//int ret = 0;

//QAudioFormat fmt;
//fmt.setSampleRate(sampleRate);
//fmt.setChannelCount(channels);
//fmt.setSampleFormat(QAudioFormat::Float);

//QAudioDevice info = QMediaDevices::defaultAudioInput();
//if (!info.isFormatSupported(fmt)) {
//    std::cout << "Audio format not supported" << std::endl;
//    //fmt = info.nearest
//}
//QAudioSource* input = new QAudioSource(fmt);

//////��ʼ¼����Ƶ
//QIODevice* io = input->start();

///////��Ƶ�ز���
//////��Ƶ��ʽΪFloat������aac���룬�����ز���

//MediaEncoder* me = MediaEncoder::Get();
//cout << "channels: " << (me->channels = channels) << endl;
//cout << "bit rate: " << (me->bit_rate) << endl;
//cout << "sampleRate: " << (me->sampleRate = sampleRate) << endl;
//cout << "nb_samples: " << (me->nb_samples = 1024) << endl;

///*me->inSampleFmt = inSampleFmt;
//me->outSampleFmt = outSampleFmt;*/
//if (!me->InitResample()) {
//    getchar();
//    return -1;
//}

/////4.��ʼ����Ƶ������
//if (!me->InitAudioCodec()) {
//    getchar();
//    return -1;
//}

/////5.�����װ������Ƶ������
//// a.���������װ��������
//Rtmp* rp = Rtmp::Get(0);
//if (!rp->Init(outUrl)) {
//    getchar();
//    return -1;
//}
////b.�����Ƶ��
//if (!rp->AddStream(me->ac)) {
//    getchar();
//    return -1;
//}

/////6.��rtmp���������IO
////д���װͷ
//if (!rp->SendHead()) {
//    getchar();
//    return -1;
//}

//// һ�ζ�ȡһ֡��Ƶ���ֽ���
//int readSize = me->nb_samples * channels * sampleByte;
//char* buf = new char[readSize];

//for (;;)
//{
//    if (input->bytesAvailable() < readSize)
//    {
//        Sleep(1);
//        continue;
//    }
//    int size = 0;
//    while (size != readSize) {
//        int len = io->read(buf + size, readSize - size);
//        if (len <= 0) {
//            std::cout << "read end" << std::endl;
//            break;
//        }
//        size += len;
//    }
//    if (size != readSize) continue;

//    // �ز���Դ����
//    AVFrame* pcm = me->Resample(buf);

//    // pts����
//    if (!pcm) {
//        cout << "Resample failed!" << endl;
//        continue;
//    }
//    AVPacket* pkt = me->EncodeAudio(pcm);
//    if (!pkt) continue;
//    ////����

//    //getchar();
//    if (!rp->SendFrame(pkt)) {
//        std::cout << "SendFrame Failed!" << std::endl;
//        continue;
//    }
//    std::cout << "#" << std::flush;

//}
//delete buf;

//getchar();
//return 0; // main program
//}

	// ---- test program ---- //

	//��ʼ�����з�װ��



   // const char* outUrl = "rtmp://localhost/live";
   // 
   // // �����������ظ�ʽת������
   // MediaEncoder *me = MediaEncoder::Get(0);

   // // ��װ����������
   // Rtmp* rtmp = Rtmp::Get(0);

   // cv::VideoCapture cam;


   // int ret = 0;
   // try
   // {
   //     // �������
   //     cam.open("D:\\Cpp_2023_project\\bin\\origin.mp4");

   //     cv::Mat frame;
   //     cv::namedWindow("video");
   //     int inWidth = cam.get(cv::CAP_PROP_FRAME_WIDTH);
   //     int inHeight = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
   //     int fps = cam.get(cv::CAP_PROP_FPS); // ����ͷ֡��

   //     ///2.��ʼ����ʽת��������
   //     ///3.��ʼ�����yuv���ݽṹ
   //     me->inWidth = inWidth;
   //     me->inHeight = inHeight;
   //     me->outWidth = inWidth;
   //     me->outHeight = inHeight;
   //     me->InitScale();
   //     
   //     ///4.��ʼ��������������
   //     //a.�ҵ�������
   //     if (!me->InitVideoCodec()) {
   //         throw std::exception("InitVideoCodec failed");
   //     }

   //     ///5 �����װ������Ƶ������
   //     rtmp->Init(outUrl);

   //     ///6.�����Ƶ��
   //     rtmp->AddStream(me->vc);
   //     rtmp->SendHead();

   //     ///h264����

   //     for (;;) {

   //         // ת��yuvΪrgb
   //         cam.read(frame);

   //         if (!cam.retrieve(frame)) {
   //             std::cout << "retrieve failed" << std::endl;
   //            continue;
   //         }
   //         cv::imshow("video", frame);
   //         cv::waitKey(1);

   //         //
   //         //rgb to yuv
   //         me->inPixSize = frame.elemSize();
   //         AVFrame* yuv = me->RGBToYUV((char*)frame.data);
   //         if (!yuv) {
   //             std::cout << "RGBToYUV failed" << std::endl;
   //             continue;
   //         }
   //         
   //         ///h264����
   //         AVPacket* pack = me->EncodeVideo(yuv);
   //         if (!pack) {
   //             std::cout << "EncodeVideo failed" << std::endl;
			//	continue;
			//}
   //         
   //         rtmp->SendFrame(pack);
   //         
			//// ���������
			//// ����������
			//// ���͵�rtmp������
   //     }
   // }
   // catch (std::exception e) {
   //     if (cam.isOpened())
   //         cam.release();
   //     std::cerr << e.what() << std::endl;
   // } 


   // std::cout << "file to rtmp" << std::endl;
   // getchar();
   //return 0;

//}
