#ifndef GPMANAGER_H
#define GPMANAGER_H

#include <QGamepadManager>
#include <QGamepad>

#include "global.h"

extern QGamepadManager::GamepadButton getButton(QString dsBtn);

struct GamepadMonitor : public QObject {

    GamepadMonitor(QObject *parent) : QObject(parent)
    {
        connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent, this,
            [](int deviceId, QGamepadManager::GamepadButton button, double value)
        {
            (void)deviceId;
            (void)value;
            buttons |= QGamepadManager::GamepadButtons(1 << button);

            sendFrame();
        });

        connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,
            [](int deviceId, QGamepadManager::GamepadButton button)
        {
            (void)deviceId;
            buttons &= QGamepadManager::GamepadButtons(~(1 << button));
           sendFrame();
        });
        connect(QGamepadManager::instance(), &QGamepadManager::gamepadAxisEvent, this,
            [](int deviceId, QGamepadManager::GamepadAxis axis, double value)
        {
            (void)deviceId;
            (void)value;
            QGamepadManager::GamepadAxis axLeftX = QGamepadManager::AxisLeftX,
                                         axLeftY= QGamepadManager::AxisLeftY,
                                         axRightX= QGamepadManager::AxisRightX,
                                         axRightY= QGamepadManager::AxisRightY;

            if(axis==axLeftX)
            {

                lx = value;
            }

             if(axis==axLeftY)
             {
                 ly = yAxisMultiplier*-value; // for some reason qt inverts this
             }

             if(axis==axRightX)
             {
                 rx = value;
             }

             if(axis==axRightY)
             {
                 ry = yAxisMultiplier*-value;
             }

            sendFrame();
        });
    }
};

#endif // GPMANAGER_H
