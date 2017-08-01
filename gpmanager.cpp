#include "gpmanager.h"

GamepadMonitor::GamepadMonitor(QObject *parent) : QObject(parent)
{
    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent, this,
        [this](int deviceId, QGamepadManager::GamepadButton button, double value)
    {
        (void)deviceId;
        (void)value;
        buttons |= QGamepadManager::GamepadButtons(1 << button);

        if (button == homeButton)
        {
            interfaceButtons |= 1;
        }
        if (button == powerButton)
        {
            interfaceButtons |= 2;
        }
        if (button == powerLongButton)
        {
            interfaceButtons |= 4;
        }
        for (auto it = listShortcuts.begin(); it != listShortcuts.end(); ++it) {
            int index = std::distance(listShortcuts.begin(), it);
            ShortCut curShort = listShortcuts.at(index);

            if(curShort.button == button)
            {
                touchScreenPressed = true;
                touchScreenPosition = curShort.pos*tsRatio;
            }
        }

    });

    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,
        [this](int deviceId, QGamepadManager::GamepadButton button)
    {
        (void)deviceId;
        buttons &= QGamepadManager::GamepadButtons(~(1 << button));

        if (button == homeButton)
        {
            interfaceButtons &= ~1;
        }
        if (button == powerButton)
        {
            interfaceButtons &= ~2;
        }
        if (button == powerLongButton)
        {
            interfaceButtons &= ~4;
        }


        for (auto it = listShortcuts.begin(); it != listShortcuts.end(); ++it) {
            int index = std::distance(listShortcuts.begin(), it);
            ShortCut curShort = listShortcuts.at(index);

            if(curShort.button == button)
            {
                touchScreenPressed = false;
                return;
            }
        }

    });

    connect(QGamepadManager::instance(), &QGamepadManager::gamepadAxisEvent, this,
        [this](int deviceId, QGamepadManager::GamepadAxis axis, double value)
    {
        (void)deviceId;
        (void)value;
        QGamepadManager::GamepadAxis axLeftX = QGamepadManager::AxisLeftX,
                                     axLeftY= QGamepadManager::AxisLeftY,
                                     axRightX= QGamepadManager::AxisRightX,
                                     axRightY= QGamepadManager::AxisRightY;

        if(btnSettings.isShouldSwapStick())
        {
            axLeftX = QGamepadManager::AxisRightX;
            axLeftY = QGamepadManager::AxisRightY;

            axRightX = QGamepadManager::AxisLeftX;
            axRightY = QGamepadManager::AxisLeftY;
        }

        if(axis==axLeftX)
        {
           worker.setLeftAxis(value, worker.getLeftAxis().y);
           worker.setPreviousLAxis(worker.getLeftAxis().x, worker.getPreviousLAxis().y);
        }
        else
        if(axis==axLeftY)
        {
            worker.setLeftAxis(worker.getLeftAxis().x, yAxisMultiplier * -value); // for some reason qt inverts this
            worker.setPreviousLAxis(worker.getPreviousLAxis().x, worker.getLeftAxis().y);
        }
        else
        if(axis==axRightX)
        {
            if (!btnSettings.isCStickDisabled()) worker.setRightAxis(value, worker.getRightAxis().y);

            if (btnSettings.isMonsterHunterCamera())
            {
                if (value > -1.2 && value < -0.5) // RS tilted left
                {
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[3]); // press Left
                } else if (value > 0.5 && value < 1.2) // RS tilted right
                {
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[2]); // press Right
                } else { // RS neutral, release buttons
                    buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[3])); // Release Left
                    buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[2])); // release Right
                }
            }
            if (btnSettings.isRightStickFaceButtons())
            {
                if (value > -1.2 && value < -0.5) // RS tilted left
                {
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsXY[1]); // press Y
                } else if (value > 0.5 && value < 1.2) // RS tilted right
                {
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                } else { // RS neutral, release buttons
                    buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsXY[1])); // Release Y
                    buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // release A
                }
            }
            if (btnSettings.isRightStickSmash())
            {
                if (value > -1.2 && value < -0.5) // RS tilted left
                {
                    btnSettings.setSmashingH(true);
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                    worker.setLeftAxis(-1.2, worker.getLeftAxis().y);
                } else if (value > 0.5 && value < 1.2) // RS tilted right
                {
                    btnSettings.setSmashingH(true);
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                    worker.setLeftAxis(1.2, worker.getLeftAxis().y);
                } else { // RS neutral, release buttons
                    if (btnSettings.isSmashingH())
                    {
                        if (!btnSettings.isSmashingV())
                            buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // Release A
                        worker.setLeftAxis(worker.getPreviousLAxis().x, worker.getRightAxis().y);
                        btnSettings.setSmashingH(false);
                    }
                }
            }
        }
        else
        if(axis==axRightY)
        {
            worker.setRightAxis(worker.getRightAxis().x, yAxisMultiplierCpp * -value);

            if (btnSettings.isMonsterHunterCamera())
            {
                if (worker.getRightAxis().y > -1.2 && worker.getRightAxis().y  < -0.5) // RS tilted down
                {
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[5]); // press Down
                } else if (worker.getRightAxis().y  > 0.5 && worker.getRightAxis().y  < 1.2) // RS tilted up
                {
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[4]); // press Up
                } else { // RS neutral, release buttons
                    buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[5])); // release Down
                    buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[4])); // Release Up
                }
            }
            if (btnSettings.isRightStickFaceButtons())
            {
                if (worker.getRightAxis().y > -1.2 && worker.getRightAxis().y < -0.5) // RS tilted down
                {
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[1]); // press B
                } else if (worker.getRightAxis().y  > 0.5 && worker.getRightAxis().y < 1.2) // RS tilted up
                {
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsXY[0]); // press X
                } else { // RS neutral, release buttons
                    buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[1])); // release B
                    buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsXY[0])); // Release X
                }
            }
            if (btnSettings.isRightStickSmash())
            {
                if (worker.getRightAxis().y > -1.2 && worker.getRightAxis().y < -0.5) // RS tilted down
                {
                    btnSettings.setSmashingV(true);
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                    worker.setLeftAxis(worker.getLeftAxis().x, -1.2);
                } else if (worker.getRightAxis().y  > 0.5 && worker.getRightAxis().y  < 1.2) // RS tilted up
                {
                    btnSettings.setSmashingV(true);
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                    worker.setLeftAxis(worker.getLeftAxis().x, 1.2);
                } else { // RS neutral, release button A
                    if (btnSettings.isSmashingV())
                    {
                        if (!btnSettings.isSmashingH())
                            buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // Release A
                        worker.setLeftAxis(worker.getLeftAxis().x, worker.getPreviousLAxis().y);
                        btnSettings.setSmashingV(false);
                    }
                }
            }
            if (btnSettings.isCStickDisabled())
            {
                worker.setRightAxis(0.0, 0.0);
            }
        }
    });
}


