#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include "global.h"
#include "touchscreen.h"
#include <QComboBox>

class ConfigWindow : public QDialog
{
private:
    QGridLayout *layout;
    QComboBox   *comboBoxA, *comboBoxB, *comboBoxX,
                *comboBoxY, *comboBoxL, *comboBoxR,
                *comboBoxUp, *comboBoxDown, *comboBoxLeft,
                *comboBoxRight, *comboBoxStart, *comboBoxSelect,
                *comboBoxZL, *comboBoxZR, *comboBoxHome,
                *comboBoxPower, *comboBoxPowerLong,
                *comboBoxTouch1, *comboBoxTouch2, *comboBoxTouch3,
                *comboBoxTouch4;

    QLineEdit   *touchButton1XEdit, *touchButton1YEdit, *touchButton3XEdit,
                *touchButton3YEdit,*touchButton2XEdit, *touchButton2YEdit,
                *touchButton4XEdit, *touchButton4YEdit;

    QPushButton *saveButton, *closeButton;

    QCheckBox   *invertYCheckbox, *invertYCppCheckbox, *swapSticksCheckbox,
                *mhCameraCheckbox, *rsSmashCheckbox,
                *disableCStickCheckbox, *rsFaceButtonsCheckbox;

    QComboBox* populateItems(QGamepadManager::GamepadButton button);

    QVariant currentData(QComboBox *comboBox);

public:
    ConfigWindow(QWidget *parent = nullptr, TouchScreen *ts = nullptr);
};

#endif // CONFIGWINDOW_H
