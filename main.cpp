#include "VideoUI.h"
#include <QtWidgets/QApplication>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <exception>

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
int main(int argc, char *argv[])
{
    if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication a(argc, argv);
    VideoUI w;
    w.show();
    return a.exec(); // main program

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
   //     cam.open(0);

   //     cv::Mat frame;
   //     cv::namedWindow("video");
   //     int inWidth = cam.get(cv::CAP_PROP_FRAME_WIDTH);
   //     int inHeight = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
   //     int fps = 25; // ����ͷ֡��

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
   //         if (!cam.retrieve(frame)) {
   //             continue;
   //         }
   //         cv::imshow("video", frame);
   //         cv::waitKey(1);

   //         //
   //         //rgb to yuv
   //         me->inPixSize = frame.elemSize();
   //         AVFrame* yuv = me->RGBToYUV((char*)frame.data);
   //         if (!yuv) {
   //             continue;
   //         }
   //         
   //         ///h264����
   //         if (!me->EncodeVideo(yuv))
   //         {
			//	continue;
			//}
   //         AVPacket* pack = me->EncodeVideo(yuv);
   //         if (!pack) {
			//	continue;
			//}
   //         
   //         rtmp->SendFrame(pack);



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
    return 0;

}
