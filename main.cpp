#include "VideoUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VideoUI w;
    w.show();
    return a.exec();
}
