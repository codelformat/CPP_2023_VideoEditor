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
int main(int argc, char *argv[])
{
    if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication a(argc, argv);
    VideoUI w;
    w.show();
    return a.exec(); // main program

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
   //     cam.open(0);

   //     cv::Mat frame;
   //     cv::namedWindow("video");
   //     int inWidth = cam.get(cv::CAP_PROP_FRAME_WIDTH);
   //     int inHeight = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
   //     int fps = 25; // 摄像头帧率

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
   //         ///h264编码
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
    return 0;

}
