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
                *comboBoxPower, *comboBoxPowerLong;

    QPushButton *saveButton;

    QCheckBox   *invertYCheckbox, *invertYCppCheckbox, *swapSticksCheckbox,
                *mhCameraCheckbox, *rsSmashCheckbox,
                *disableCStickCheckbox, *rsFaceButtonsCheckbox;

    QComboBox* populateItems(QGamepadManager::GamepadButton button);

    QLineEdit *txtStickVal, *txtCppVal;
    QValidator *validator;

    QVariant currentData(QComboBox *comboBox);

public:
    ConfigWindow(QWidget *parent = nullptr, TouchScreen *ts = nullptr);
};

#endif // CONFIGWINDOW_H
