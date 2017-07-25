#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include "main.h"
#include "gpmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    GamepadMonitor m(&w);
    w.show();

    return a.exec();
}
