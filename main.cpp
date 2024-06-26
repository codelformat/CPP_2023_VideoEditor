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

using namespace std;
/**
* 开发步骤
* 先在ImageProcess中实现算法
* 再在VideoFilter中添加算法的任务和设置调用函数
* 最后在VideoUI中添加按钮和信号槽
*
* 整体架构
* VideoUI是界面
* VideoThread是视频读写线程
* VideoFilter是滤镜
* ImageProcess是图像处理
*
*/
int main(int argc, char* argv[])
{
	if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
	QApplication a(argc, argv);
	VideoUI w;
	w.show();
	
	//return a.exec();

	//test

	/*const char* inUrl = "D:\\Cpp_2023_project\\bin\\test.mp4";
	const char* outUrl = "rtmp://localhost/live";

	VideoTranscoder* transcoder = VideoTranscoder::Get(inUrl, outUrl);

	if (!transcoder->open()) {
		return 1;
	}

	std::thread transcode_thread(&VideoTranscoder::transcode, transcoder);
	

	string cmd = "ffplay ";
	cmd += outUrl;
	system(cmd.c_str());

	delete transcoder;*/


	return a.exec();
}













// test
//// 编码器和像素格式转换对象
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

//////开始录制音频
//QIODevice* io = input->start();

///////音频重采样
//////视频格式为Float，符合aac编码，不用重采样

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

/////4.初始化音频编码器
//if (!me->InitAudioCodec()) {
//    getchar();
//    return -1;
//}

/////5.输出封装器和音频流配置
//// a.创建输出封装器上下文
//Rtmp* rp = Rtmp::Get(0);
//if (!rp->Init(outUrl)) {
//    getchar();
//    return -1;
//}
////b.添加音频流
//if (!rp->AddStream(me->ac)) {
//    getchar();
//    return -1;
//}

/////6.打开rtmp的网络输出IO
////写入封装头
//if (!rp->SendHead()) {
//    getchar();
//    return -1;
//}

//// 一次读取一帧音频的字节数
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

//    // 重采样源数据
//    AVFrame* pcm = me->Resample(buf);

//    // pts运算
//    if (!pcm) {
//        cout << "Resample failed!" << endl;
//        continue;
//    }
//    AVPacket* pkt = me->EncodeAudio(pcm);
//    if (!pkt) continue;
//    ////推流

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

	//初始化所有封装器



   // const char* outUrl = "rtmp://localhost/live";
   // 
   // // 编码器和像素格式转换对象
   // MediaEncoder *me = MediaEncoder::Get(0);

   // // 封装和推流对象
   // Rtmp* rtmp = Rtmp::Get(0);

   // cv::VideoCapture cam;


   // int ret = 0;
   // try
   // {
   //     // 打开摄像机
   //     cam.open("D:\\Cpp_2023_project\\bin\\origin.mp4");

   //     cv::Mat frame;
   //     cv::namedWindow("video");
   //     int inWidth = cam.get(cv::CAP_PROP_FRAME_WIDTH);
   //     int inHeight = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
   //     int fps = cam.get(cv::CAP_PROP_FPS); // 摄像头帧率

   //     ///2.初始化格式转换上下文
   //     ///3.初始化输出yuv数据结构
   //     me->inWidth = inWidth;
   //     me->inHeight = inHeight;
   //     me->outWidth = inWidth;
   //     me->outHeight = inHeight;
   //     me->InitScale();
   //     
   //     ///4.初始化编码器上下文
   //     //a.找到编码器
   //     if (!me->InitVideoCodec()) {
   //         throw std::exception("InitVideoCodec failed");
   //     }

   //     ///5 输出封装器和视频流配置
   //     rtmp->Init(outUrl);

   //     ///6.添加视频流
   //     rtmp->AddStream(me->vc);
   //     rtmp->SendHead();

   //     ///h264编码

   //     for (;;) {

   //         // 转换yuv为rgb
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
   //         ///h264编码
   //         AVPacket* pack = me->EncodeVideo(yuv);
   //         if (!pack) {
   //             std::cout << "EncodeVideo failed" << std::endl;
			//	continue;
			//}
   //         
   //         rtmp->SendFrame(pack);
   //         
			//// 送入编码器
			//// 编码器编码
			//// 发送到rtmp服务器
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
