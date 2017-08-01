#include "global.h"
#include "gpmanager.h"

QSettings settings("TuxSH", "InputRedirectionClient-Qt");

Worker worker;
Settings btnSettings;
double tsRatio;
std::vector<ShortCut> listShortcuts;

QGamepadManager::GamepadButtons buttons = 0;
u32 interfaceButtons = 0;
int yAxisMultiplier = 1, yAxisMultiplierCpp = 1;
bool shouldSwapStick = false;
int CPAD_BOUND = (settings.contains("StickBound") ? settings.value("StickBound").toInt() : 1488);
int CPP_BOUND = (settings.contains("CppBound") ? settings.value("CppBound").toInt() : 127);

GamepadConfigurator *gpConfigurator;

QString ipAddress;

bool touchScreenPressed;
QSize touchScreenSize = QSize(TOUCH_SCREEN_WIDTH, TOUCH_SCREEN_HEIGHT);
QPoint touchScreenPosition;

QGamepadManager::GamepadButton homeButton = variantToButton(settings.value("ButtonHome", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton powerButton = variantToButton(settings.value("ButtonPower", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton powerLongButton = variantToButton(settings.value("ButtonPowerLong", QGamepadManager::ButtonInvalid));

QGamepadManager::GamepadButton touchButton1 = variantToButton(settings.value("ButtonT1", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton touchButton2 = variantToButton(settings.value("ButtonT2", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton touchButton3 = variantToButton(settings.value("ButtonT3", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton touchButton4 = variantToButton(settings.value("ButtonT4", QGamepadManager::ButtonInvalid));

TouchButton tbOne={.x=settings.value("touchButton1X").toInt(),
                   .y=settings.value("touchButton1Y").toInt()};

TouchButton tbTwo={.x=settings.value("touchButton2X").toInt(),
                   .y=settings.value("touchButton2Y").toInt()};

TouchButton tbThree={.x=settings.value("touchButton3X").toInt(),
                     .y=settings.value("touchButton3Y").toInt()};

TouchButton tbFour={.x=settings.value("touchButton4X").toInt(),
                    .y=settings.value("touchButton4Y").toInt()};

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


void Worker::setLeftAxis(double x, double y)
{
    leftAxis.x = x;
    leftAxis.y = y;
}

void Worker::setRightAxis(double x, double y)
{
    rightAxis.x = x;
    rightAxis.y = y;
}

void Worker::setPreviousLAxis(double x, double y)
{
    previousLeftAxis.x = x;
    previousLeftAxis.y = y;
}

MyAxis Worker::getLeftAxis()
{
    return leftAxis;
}

MyAxis Worker::getRightAxis()
{
    return rightAxis;
}

MyAxis Worker::getPreviousLAxis()
{
    return previousLeftAxis;
}

void Worker::sendFrame(void)
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
                                       TOUCH_SCREEN_WIDTH*touchScreenPosition.x())) / touchScreenSize.width();
        u32 y = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.y()),
                                       TOUCH_SCREEN_HEIGHT*touchScreenPosition.y())) / touchScreenSize.height();

        touchScreenState = (1 << 24) | (y << 12) | x;
    }

    if(leftAxis.x != 0.0 || leftAxis.y != 0.0)
      {
          u32 x = (u32)(leftAxis.x * CPAD_BOUND + 0x800);
          u32 y = (u32)(leftAxis.y * CPAD_BOUND + 0x800);
          x = x >= 0xfff ? (leftAxis.x < 0.0 ? 0x000 : 0xfff) : x;
          y = y >= 0xfff ? (leftAxis.y < 0.0 ? 0x000 : 0xfff) : y;

          circlePadState = (y << 12) | x;
      }

      if(rightAxis.x != 0.0 || rightAxis.y != 0.0 || irButtonsState != 0)
      {
          // We have to rotate the c-stick position 45°. Thanks, Nintendo.
          u32 x = (u32)(M_SQRT1_2 * (rightAxis.x + rightAxis.y) * CPP_BOUND + 0x80);
          u32 y = (u32)(M_SQRT1_2 * (rightAxis.y - rightAxis.x) * CPP_BOUND + 0x80);
          x = x >= 0xff ? (rightAxis.x < 0.0 ? 0x00 : 0xff) : x;
          y = y >= 0xff ? (rightAxis.y < 0.0 ? 0x00 : 0xff) : y;

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

int appScreenTo3dsX(int posX)
{
    qDebug() << "PosX: " << posX;
    return TOUCH_SCREEN_WIDTH*((touchScreenSize.height()*posX)/TOUCH_SCREEN_HEIGHT)/touchScreenSize.width();
}

int appScreenTo3dsY(int posY)
{
    qDebug() << "PosX: " << posY;
    return TOUCH_SCREEN_HEIGHT*((touchScreenSize.width()*posY)/TOUCH_SCREEN_WIDTH)/touchScreenSize.height();
}
