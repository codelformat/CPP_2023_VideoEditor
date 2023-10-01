#include "VideoUI.h"
#include <QtWidgets/QApplication>

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
    return a.exec();
}
