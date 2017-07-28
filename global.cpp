#include "global.h"
#include "gpmanager.h"

QSettings settings("TuxSH", "InputRedirectionClient-Qt");

QGamepadManager::GamepadButtons buttons = 0;
u32 interfaceButtons = 0;
int yAxisMultiplier = 1, yAxisMultiplierCpp = 1;
bool shouldSwapStick = false;
bool monsterHunterCamera = false;
bool rightStickSmash = false;
bool isSmashingH = false;
bool isSmashingV = false;
bool rightStickFaceButtons = false;
bool cStickDisabled = false;

double lx = 0.0, ly = 0.0;
double rx = 0.0, ry = 0.0;
double previousLX = lx, previousLY = ly;

GamepadConfigurator *gpConfigurator;

QString ipAddress;
bool timerEnabled = false;

bool touchScreenPressed;
QSize touchScreenSize;
QPoint touchScreenPosition;
double tsRatio = 1;

QGamepadManager::GamepadButton homeButton = variantToButton(settings.value("ButtonHome", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton powerButton = variantToButton(settings.value("ButtonPower", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton powerLongButton = variantToButton(settings.value("ButtonPowerLong", QGamepadManager::ButtonInvalid));

QGamepadManager::GamepadButton touchButton1 = variantToButton(settings.value("ButtonT1", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton touchButton2 = variantToButton(settings.value("ButtonT2", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton touchButton3 = variantToButton(settings.value("ButtonT3", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton touchButton4 = variantToButton(settings.value("ButtonT4", QGamepadManager::ButtonInvalid));
int touchButton1X = settings.value("touchButton1X", 0).toInt();
int touchButton1Y = settings.value("touchButton1Y", 0).toInt();
int touchButton2X = settings.value("touchButton2X", 0).toInt();
int touchButton2Y = settings.value("touchButton2Y", 0).toInt();
int touchButton3X = settings.value("touchButton3X", 0).toInt();
int touchButton3Y = settings.value("touchButton3Y", 0).toInt();
int touchButton4X = settings.value("touchButton4X", 0).toInt();
int touchButton4Y = settings.value("touchButton4Y", 0).toInt();

QGamepadManager::GamepadButton hidButtonsAB[2]={
variantToButton(settings.value("ButtonA", QGamepadManager::ButtonA)),
variantToButton(settings.value("ButtonB", QGamepadManager::ButtonB))};

QGamepadManager::GamepadButton hidButtonsMiddle[8] ={
variantToButton(settings.value("ButtonSelect", QGamepadManager::ButtonSelect)),
variantToButton(settings.value("ButtonStart", QGamepadManager::ButtonStart)),
variantToButton(settings.value("ButtonRight", QGamepadManager::ButtonRight)),
variantToButton(settings.value("ButtonLeft", QGamepadManager::ButtonLeft)),
variantToButton(settings.value("ButtonUp", QGamepadManager::ButtonUp)),
variantToButton(settings.value("ButtonDown", QGamepadManager::ButtonDown)),
variantToButton(settings.value("ButtonR", QGamepadManager::ButtonR1)),
variantToButton(settings.value("ButtonL", QGamepadManager::ButtonL1))};

QGamepadManager::GamepadButton hidButtonsXY[2] = {
    variantToButton(settings.value("ButtonX", QGamepadManager::ButtonX)),
    variantToButton(settings.value("ButtonY", QGamepadManager::ButtonY))};

QGamepadManager::GamepadButton irButtons[2] = {
    variantToButton(settings.value("ButtonZR", QGamepadManager::ButtonR2)),
    variantToButton(settings.value("ButtonZL", QGamepadManager::ButtonL2))};


void sendFrame(void)
{
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

    u32 touchScreenState = 0x2000000;
    u32 circlePadState = 0x7ff7ff;
    u32 cppState = 0x80800081;

    if(touchScreenPressed)
    {
        u32 x = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.x()),
                                       touchScreenSize.width())) / touchScreenSize.width();
        u32 y = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.y()),
                                       touchScreenSize.height())) / touchScreenSize.height();

        touchScreenState = (1 << 24) | (y << 12) | x;
    }

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

      QByteArray ba(20, 0);
      qToLittleEndian(hidPad, (uchar *)ba.data());
      qToLittleEndian(touchScreenState, (uchar *)ba.data() + 4);
      qToLittleEndian(circlePadState, (uchar *)ba.data() + 8);
      qToLittleEndian(cppState, (uchar *)ba.data() + 12);
      qToLittleEndian(interfaceButtons, (uchar *)ba.data() + 16);
      QUdpSocket().writeDatagram(ba, QHostAddress(ipAddress), 4950);
}

QGamepadManager::GamepadButton variantToButton(QVariant variant)
{
    QGamepadManager::GamepadButton button;

    button = static_cast<QGamepadManager::GamepadButton>(variant.toInt());

    return button;
}
