#ifndef GPMANAGER_H
#define GPMANAGER_H

#include <QGamepadManager>
#include <QGamepad>

#include "global.h"

struct GamepadMonitor : public QObject {
    GamepadMonitor(QObject *parent);
};

#endif // GPMANAGER_H
