#include "shortcut.h"
#include <QBuffer>
#include <QGamepadManager>

qint8 getButtonId(QGamepadManager::GamepadButton btn)
{
    qint8 button = 0;
    if(btn == QGamepadManager::ButtonInvalid)
    {
        button = 0;
    }
    if(btn == QGamepadManager::ButtonA)
    {
        button = 1;
    }

    if(btn == QGamepadManager::ButtonB)
    {
        button = 2;
    }
    if(btn == QGamepadManager::ButtonX)
    {
        button = 3;
    }
    if(btn == QGamepadManager::ButtonY)
    {
        button = 4;
    }
    if(btn == QGamepadManager::ButtonL1)
    {
        button = 5;
    }
    if(btn == QGamepadManager::ButtonL2)
    {
        button = 6;
    }

    if(btn == QGamepadManager::ButtonL3)
    {
        button = 7;
    }
    if(btn == QGamepadManager::ButtonR1)
    {
        button = 8;
    }
    if(btn == QGamepadManager::ButtonR2)
    {
        button = 9;
    }
    if(btn == QGamepadManager::ButtonR3)
    {
        button = 10;
    }
    if(btn == QGamepadManager::ButtonUp)
    {
        button = 11;
    }
    if(btn == QGamepadManager::ButtonDown)
    {
        button = 12;
    }
    if(btn == QGamepadManager::ButtonLeft)
    {
        button = 13;
    }
    if(btn == QGamepadManager::ButtonRight)
    {
        button = 14;
    }
    if(btn == QGamepadManager::ButtonSelect)
    {
        button = 15;
    }
    if(btn == QGamepadManager::ButtonStart)
    {
        button = 16;
    }
    if(btn == QGamepadManager::ButtonGuide)
    {
        button = 17;
    }

    return button;
}

QGamepadManager::GamepadButton getIdButton(int btn)
{
    QGamepadManager::GamepadButton button;
    if(btn == 0)
    {
        button = QGamepadManager::ButtonInvalid;
    }
    if(btn == 1)
    {
        button = QGamepadManager::ButtonA;
    }

    if(btn == 2)
    {
        button = QGamepadManager::ButtonB;
    }
    if(btn == 3)
    {
        button = QGamepadManager::ButtonX;
    }
    if(btn == 4)
    {
        button = QGamepadManager::ButtonY;
    }

    if(btn == 5)
    {
        button = QGamepadManager::ButtonL1;
    }
    if(btn == 6)
    {
        button = QGamepadManager::ButtonL2;
    }

    if(btn == 7)
    {
        button = QGamepadManager::ButtonL3;
    }
    if(btn == 8)
    {
        button = QGamepadManager::ButtonR1;
    }
    if(btn == 9)
    {
        button = QGamepadManager::ButtonR2;
    }
    if(btn == 10)
    {
        button = QGamepadManager::ButtonR3;
    }
    if(btn == 11)
    {
        button = QGamepadManager::ButtonUp;
    }
    if(btn == 12)
    {
        button = QGamepadManager::ButtonDown;
    }
    if(btn == 13)
    {
        button = QGamepadManager::ButtonLeft;
    }
    if(btn == 14)
    {
        button = QGamepadManager::ButtonRight;
    }
    if(btn == 15)
    {
        button = QGamepadManager::ButtonSelect;
    }
    if(btn == 16)
    {
        button = QGamepadManager::ButtonStart;
    }
    if(btn == 17)
    {
        button = QGamepadManager::ButtonGuide;
    }

    return button;
}

QDataStream &operator<<(QDataStream &out, const ShortCut &obj)
{
    out << obj.name << obj.pos << obj.color << getIdButton(obj.button);
    return out;
}

QDataStream &operator>>(QDataStream &in, ShortCut &obj)
{
    qint8 id = getButtonId(obj.button);
    in >> obj.name >> obj.pos >> obj.color >> id;
    return in;
}
