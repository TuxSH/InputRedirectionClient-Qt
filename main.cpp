#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include "global.h"

#include "mainwidget.h"
#include "gpmanager.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    GamepadMonitor m(&w);
    SendFrameClass sfc;

    sfc.start();
    w.show();
    a.exec();

    return 0;
}
