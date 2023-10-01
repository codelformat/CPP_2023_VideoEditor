#include "VideoUI.h"
#include <QtWidgets/QApplication>

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
    return a.exec();
}
