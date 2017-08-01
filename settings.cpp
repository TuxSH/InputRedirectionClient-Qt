#include "settings.h"

bool Settings::isShouldSwapStick()
{
    return shouldSwapStick;
}

bool Settings::isMonsterHunterCamera()
{
    return monsterHunterCamera;
}

bool Settings::isRightStickSmash()
{
    return rightStickSmash;
}

bool Settings::isSmashingV()
{
    return smashingV;
}

bool Settings::isSmashingH()
{
    return smashingH;
}

bool Settings::isRightStickFaceButtons()
{
    return rightStickFaceButtons;
}

bool Settings::isCStickDisabled()
{
    return cStickDisabled;
}

/***SETTERS***/
void Settings::setShouldSwapStick(bool b)
{
    shouldSwapStick = b;
}

void Settings::setMonsterHunterCamera(bool b)
{
    monsterHunterCamera = b;
}

void Settings::setRightStickSmash(bool b)
{
    rightStickSmash = b;
}

void Settings::setSmashingV(bool b)
{
    smashingV = b;
}

void Settings::setSmashingH(bool b)
{
    smashingH = b;
}

void Settings::setRightStickFaceButtons(bool b)
{
    rightStickFaceButtons = b;
}

void Settings::setCStickDisabled(bool b)
{
    cStickDisabled = b;
}
