#include "gpmanager.h"
QGamepadManager::GamepadButton getButton(QString dsBtn)
{
    QString gpPadBtn;
    QVariant val;
    QSettings settings("GamepadControls.ini", QSettings::IniFormat);

    val =  settings.value(dsBtn);
    gpPadBtn = val.toString();

    QGamepadManager::GamepadButton btnConv = QGamepadManager::ButtonGuide;

    if(gpPadBtn == "360_A") btnConv = QGamepadManager::ButtonA;
    else
    if(gpPadBtn == "360_B") btnConv = QGamepadManager::ButtonB;
    else
    if(gpPadBtn == "360_X") btnConv = QGamepadManager::ButtonX;
    else
    if(gpPadBtn == "360_Y") btnConv = QGamepadManager::ButtonY;
    else
    if(gpPadBtn == "360_BACK") btnConv = QGamepadManager::ButtonSelect;
    else
    if(gpPadBtn == "360_START") btnConv =  QGamepadManager::ButtonStart;
    else
    if(gpPadBtn == "360_D_UP") btnConv =  QGamepadManager::ButtonUp;
    else
    if(gpPadBtn == "360_D_DOWN") btnConv =  QGamepadManager::ButtonDown;
    else
    if(gpPadBtn == "360_D_LEFT") btnConv = QGamepadManager::ButtonLeft;
    else
    if(gpPadBtn == "360_D_RIGHT") btnConv = QGamepadManager::ButtonRight;
    else
    if(gpPadBtn == "360_R1") btnConv = QGamepadManager::ButtonR1;
    else
    if(gpPadBtn == "360_R2") btnConv = QGamepadManager::ButtonR2;
    else
    if(gpPadBtn == "360_R3") btnConv = QGamepadManager::ButtonR3;
    else
    if(gpPadBtn == "360_L1") btnConv = QGamepadManager::ButtonL1;
    else
    if(gpPadBtn == "360_L2") btnConv = QGamepadManager::ButtonL2;
    else
    if(gpPadBtn == "360_L3") btnConv = QGamepadManager::ButtonL3;

    return btnConv;
}


