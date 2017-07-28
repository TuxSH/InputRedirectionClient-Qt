#include "global.h"
#include <QThread>

#include "gpmanager.h"

QGamepadManager::GamepadButtons buttons = 0;
u32 interfaceButtons = 0;
bool shouldSwapStick = false;
int yAxisMultiplier = 1, yAxisMultiplierCpp = 1;

double lx = 0.0, ly = 0.0;
double rx = 0.0, ry = 0.0;

QString ipAddress;
bool touchScreenPressed;

QSize touchScreenSize;
QPoint touchScreenPosition;

GamepadConfigurator *gpConfigurator;

void SendFrameClass::run()
{
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerHit()), Qt::DirectConnection);
    timer.setInterval(20);
    timer.start();   // puts one event in the threads event queue
    exec();
    timer.stop();
}

void SendFrameClass::timerHit()
{
    sendFrame();
}

void SendFrameClass::sendFrame()
{
    static const QGamepadManager::GamepadButton hidButtonsAB[] = {
        getButton("Button/3DS_A"),
        getButton("Button/3DS_B"),
    };

    static const QGamepadManager::GamepadButton hidButtonsMiddle[] = {
        getButton("Button/3DS_SELECT"),
        getButton("Button/3DS_START"),
        getButton("Button/3DS_D_RIGHT"),
        getButton("Button/3DS_D_LEFT"),
        getButton("Button/3DS_D_UP"),
        getButton("Button/3DS_D_DOWN"),
        getButton("Button/3DS_R"),
        getButton("Button/3DS_L"),
    };

    static const QGamepadManager::GamepadButton hidButtonsXY[] = {
        getButton("Button/3DS_X"),
        getButton("Button/3DS_Y"),
    };

    static const QGamepadManager::GamepadButton irButtons[] = {
        getButton("Button/3DS_ZR"),
        getButton("Button/3DS_ZL"),
    };

    static const QGamepadManager::GamepadButton speButtons[] = {
        getButton("Button/3DS_HOME"),
        getButton("Button/3DS_POWER"),
        getButton("Button/3DS_LPOWER"),
    };

    qDebug() << "Sending";

    u32 hidPad = 0xfff;
    for(u32 i = 0; i < 2; i++)
    {
         if(buttons & (1 << hidButtonsAB[i]))
                hidPad &= ~(1 << i);
    }

    for(u32 i = 2; i < 10; i++)
    {
        if(buttons & (1 << hidButtonsMiddle[i-2]))
            hidPad &= ~(1 << i);
    }

    for(u32 i = 10; i < 12; i++)
   {
            if(buttons & (1 << hidButtonsXY[i-10]))
                hidPad &= ~(1 << i);
   }

    u32 irButtonsState = 0;
    for(u32 i = 0; i < 2; i++)
    {
        if(buttons & (1 << irButtons[i]))
            irButtonsState |= 1 << (i + 1);
    }

    u32 specialButtonsState = 0;
    for(u32 i = 0; i < 3; i++)
    {

        if(buttons & (1 << speButtons[i]))
            specialButtonsState |= 1 << i;
    }
    specialButtonsState |= interfaceButtons;

    u32 touchScreenState = 0x2000000;
    u32 circlePadState = 0x7ff7ff;
    u32 cppState = 0x80800081;

    if(lx != 0.0 || ly != 0.0)
      {
          u32 x = (u32)(lx * CPAD_BOUND + 0x800);
          u32 y = (u32)(ly * CPAD_BOUND + 0x800);
          x = x >= 0xfff ? (lx < 0.0 ? 0x000 : 0xfff) : x;
          y = y >= 0xfff ? (ly < 0.0 ? 0x000 : 0xfff) : y;

          circlePadState = (y << 12) | x;
      }

      if(rx != 0.0 || ry != 0.0 || irButtonsState != 0)
      {
          // We have to rotate the c-stick position 45°. Thanks, Nintendo.
          u32 x = (u32)(M_SQRT1_2 * (rx + ry) * CPP_BOUND + 0x80);
          u32 y = (u32)(M_SQRT1_2 * (ry - rx) * CPP_BOUND + 0x80);
          x = x >= 0xff ? (rx < 0.0 ? 0x00 : 0xff) : x;
          y = y >= 0xff ? (ry < 0.0 ? 0x00 : 0xff) : y;

          cppState = (y << 24) | (x << 16) | (irButtonsState << 8) | 0x81;
      }

      if(touchScreenPressed)
      {
          u32 x = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.x()),
                                         touchScreenSize.width())) / touchScreenSize.width();
          u32 y = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.y()),
                                         touchScreenSize.height())) / touchScreenSize.height();

          touchScreenState = (1 << 24) | (y << 12) | x;
      }

      QByteArray ba(20, 0);
      qToLittleEndian(hidPad, (uchar *)ba.data());
      qToLittleEndian(touchScreenState, (uchar *)ba.data() + 4);
      qToLittleEndian(circlePadState, (uchar *)ba.data() + 8);
      qToLittleEndian(cppState, (uchar *)ba.data() + 12);
      qToLittleEndian(specialButtonsState, (uchar *)ba.data() + 16);
      QUdpSocket().writeDatagram(ba, QHostAddress(ipAddress), 4950);
}
