#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include "mainwidget.h"
#include "gpmanager.h"
#include <QTimer>

struct FrameTimer : public QTimer {
    FrameTimer(QObject *parent = nullptr) : QTimer(parent)
    {
        connect(this, &QTimer::timeout, this,
                [](void)
        {
            if (timerEnabled)
            sendFrame();
        });
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    GamepadMonitor m(&w);
    FrameTimer t(&w);
    t.start(50);
    w.show();

    return a.exec();
}
