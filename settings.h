#ifndef SETTINGS_H
#define SETTINGS_H

class Settings
{
 private:
    bool shouldSwapStick;
    bool monsterHunterCamera;
    bool rightStickSmash;
    bool smashingV;
    bool smashingH;
    bool rightStickFaceButtons;
    bool cStickDisabled;

 public:
    Settings()
    {
        shouldSwapStick = false;
    }

    bool isShouldSwapStick();
    bool isMonsterHunterCamera();
    bool isRightStickSmash();
    bool isSmashingV();
    bool isSmashingH();
    bool isRightStickFaceButtons();
    bool isCStickDisabled();


    void setShouldSwapStick(bool);
    void setMonsterHunterCamera(bool);
    void setRightStickSmash(bool);
    void setSmashingV(bool);
    void setSmashingH(bool);
    void setRightStickFaceButtons(bool);
    void setCStickDisabled(bool);
};

#endif // SETTINGS_H
