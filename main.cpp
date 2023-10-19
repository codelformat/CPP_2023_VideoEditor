#include "VideoUI.h"
#include <QtWidgets/QApplication>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <exception>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioDevice>
#include <QtMultimedia/qmediadevices.h>
#include <QtMultimedia/qaudiosource.h>

extern "C"
{
#include <libavformat/avformat.h> // test
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#include "MediaEncoder.h"
#include "Rtmp.h"

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
    VideoUI w;
    w.show();
	return a.exec();

 //   const char* outUrl = "rtmp://localhost/live";
 //   const char* fileUrl = "D:\\Cpp_2023_project\\bin\\origin.mp4";

 //   int sampleRate = 44100;
 //   int channels = 2;
 //   int sampleByte = 2;
 //   int ret;

 //   QAudioFormat fmt;
 //   fmt.setSampleRate(sampleRate);
 //   fmt.setChannelCount(channels);
 //   fmt.setSampleFormat(QAudioFormat::Float);

 //   QAudioDevice info = QMediaDevices::defaultAudioInput();
 //   if (!info.isFormatSupported(fmt)) {
 //       std::cout << "Audio format not supported" << std::endl;
 //       //fmt = info.nearest
 //   }
 //   QAudioSource* input = new QAudioSource(fmt);

 //   ////��ʼ¼����Ƶ
 //   QIODevice* io = input->start();

 //   /////��Ƶ�ز���
 //   ////��Ƶ��ʽΪFloat������aac���룬�����ز���
 //   AVFormatContext* fileCtx = NULL;
 //   if (avformat_open_input(&fileCtx, fileUrl, NULL, NULL)) {
 //       std::cout << "open failed";
 //       return -1;
 //   }
 //   std::cout << "open " << fileUrl << " success" <<std::endl;
 //   if (avformat_find_stream_info(fileCtx, nullptr) < 0) {
 //       std::cout << "find failed";
 //       getchar();
 //       return -1;
 //   }

 //   AVCodecParameters* codecParameters = fileCtx->streams[1]->codecpar;

 //   MediaEncoder* me = MediaEncoder::Get();
 //   me->channels = codecParameters->channels;
 //   me->bit_rate = codecParameters->bit_rate;
 //   me->sampleRate = codecParameters->sample_rate;
 //   me->nb_samples = codecParameters->frame_size;
 //   
 //   /*me->inSampleFmt = inSampleFmt;
 //   me->outSampleFmt = outSampleFmt;*/
 //   if (!me->InitResample()) {
 //       getchar();
 //       return -1;
 //   }

 //   ///4.��ʼ����Ƶ������
 //   if (!me->InitAudioCodec()) {
	//	getchar();
	//	return -1;
	//}

 //   ///5.�����װ������Ƶ������
 //   // a.���������װ��������
 //   Rtmp *rp = Rtmp::Get(0);
 //   if (!rp->Init(outUrl)) {
 //       getchar();
 //       return -1;
 //   }
 //   //b.�����Ƶ��
 //   if (!rp->AddStream(me->ac)) {
 //       getchar();
 //       return -1;
 //   }

 //   ///6.��rtmp���������IO
 //   //д���װͷ
 //   if (!rp->SendHead()) {
	//	getchar();
	//	return -1;
	//}

 //   // һ�ζ�ȡһ֡��Ƶ���ֽ���
 //   int readSize = me->nb_samples * channels * sampleByte;
 //   char* buf = new char[readSize];


 //   AVPacket packet;
 //   for (;;)
 //   {       
 //  //     if (input->bytesAvailable() <  readSize)
 //  //     {
 //  //         Sleep(1);
 //  //         continue;
 //  //     }
 //  //     int size = 0;
 //  //     while (size != readSize) {
 //  //         int len = io->read(buf + size, readSize - size);
 //  //         if (len <= 0) {
	//		//	std::cout << "read end" << std::endl;
	//		//	break;
	//		//}
 //  //         size += len;
 //  //     }
 //  //     if (size != readSize) continue;

 //  //     // �ز���Դ����
 //  //     AVFrame *pcm = me->Resample(buf);
 //       
 //       // pts����
 //       
 //       if (av_read_frame(fileCtx, &packet) != 0) {
 //           std::cout << "$" << std::flush;
 //           continue;
 //       }
 //       AVFrame* pcm = me->Resample(packet.data);
 //       AVPacket * pkt = me->EncodeAudio(pcm);
 //       if(!pkt) continue;
	//	////����
	//
 //       if (!rp->SendFrame(pkt)) {
	//		std::cout << "SendFrame Failed!" << std::endl;
	//		continue;
	//	}
	//	std::cout << "#" << std::flush;
 //       av_packet_unref(pkt);
 //   }
 //   delete buf;

 //   return a.exec(); // main program




























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
   // return 0;

}
