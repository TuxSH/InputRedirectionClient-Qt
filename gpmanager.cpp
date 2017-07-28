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

        if (button == touchButton1)
        {
            touchScreenPressed = true;
            touchScreenPosition = QPoint(touchButton1X, touchButton1Y)*tsRatio;
        }
        if (button == touchButton2)
        {
            touchScreenPressed = true;
            touchScreenPosition = QPoint(touchButton2X, touchButton2Y)*tsRatio;
        }
        if (button == touchButton3)
        {
            touchScreenPressed = true;
            touchScreenPosition = QPoint(touchButton3X, touchButton3Y)*tsRatio;
        }
        if (button == touchButton4)
        {
            touchScreenPressed = true;
            touchScreenPosition = QPoint(touchButton4X, touchButton4Y)*tsRatio;
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

        if ((button == touchButton1) || (button == touchButton2)
                || (button == touchButton3) || (button == touchButton4))
        {
            touchScreenPressed = false;
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

        if(shouldSwapStick)
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
            if (!cStickDisabled) worker.setRightAxis(value, worker.getRightAxis().y);

            if (monsterHunterCamera)
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
            if (rightStickFaceButtons)
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
            if (rightStickSmash)
            {
                if (value > -1.2 && value < -0.5) // RS tilted left
                {
                    isSmashingH = true;
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                    worker.setLeftAxis(-1.2, worker.getLeftAxis().y);
                } else if (value > 0.5 && value < 1.2) // RS tilted right
                {
                    isSmashingH = true;
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                    worker.setLeftAxis(1.2, worker.getLeftAxis().y);
                } else { // RS neutral, release buttons
                    if (isSmashingH)
                    {
                        if (!isSmashingV)
                            buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // Release A
                        worker.setLeftAxis(worker.getPreviousLAxis().x, worker.getRightAxis().y);
                        isSmashingH = false;
                    }
                }
            }
        }
        else
        if(axis==axRightY)
        {
            worker.setRightAxis(worker.getRightAxis().x, yAxisMultiplierCpp * -value);

            if (monsterHunterCamera)
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
            if (rightStickFaceButtons)
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
            if (rightStickSmash)
            {
                if (worker.getRightAxis().y > -1.2 && worker.getRightAxis().y < -0.5) // RS tilted down
                {
                    isSmashingV = true;
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                    worker.setLeftAxis(worker.getLeftAxis().x, -1.2);
                } else if (worker.getRightAxis().y  > 0.5 && worker.getRightAxis().y  < 1.2) // RS tilted up
                {
                    isSmashingV = true;
                    buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                    worker.setLeftAxis(worker.getLeftAxis().x, 1.2);
                } else { // RS neutral, release button A
                    if (isSmashingV)
                    {
                        if (!isSmashingH)
                            buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // Release A
                        worker.setLeftAxis(worker.getLeftAxis().x, worker.getPreviousLAxis().y);
                        isSmashingV = false;
                    }
                }
            }
            if (cStickDisabled)
            {
                worker.setRightAxis(0.0, 0.0);
            }
        }
    });
}
