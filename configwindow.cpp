#include "configwindow.h"
#include "gpmanager.h"

ConfigWindow::ConfigWindow(QWidget *parent, TouchScreen *ts) : QDialog(parent)
{
    this->setFixedSize(TOUCH_SCREEN_WIDTH, 700);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    this->setWindowTitle(tr("InputRedirectionClient-Qt - Button Config"));

    comboBoxA = populateItems(variantToButton(settings.value("ButtonA", QGamepadManager::ButtonA)));
    comboBoxB = populateItems(variantToButton(settings.value("ButtonB", QGamepadManager::ButtonB)));
    comboBoxX = populateItems(variantToButton(settings.value("ButtonX", QGamepadManager::ButtonX)));
    comboBoxY = populateItems(variantToButton(settings.value("ButtonY", QGamepadManager::ButtonY)));
    comboBoxUp = populateItems(variantToButton(settings.value("ButtonUp", QGamepadManager::ButtonUp)));
    comboBoxDown = populateItems(variantToButton(settings.value("ButtonDown", QGamepadManager::ButtonDown)));
    comboBoxLeft = populateItems(variantToButton(settings.value("ButtonLeft", QGamepadManager::ButtonLeft)));
    comboBoxRight = populateItems(variantToButton(settings.value("ButtonRight", QGamepadManager::ButtonRight)));
    comboBoxL = populateItems(variantToButton(settings.value("ButtonL", QGamepadManager::ButtonL1)));
    comboBoxR = populateItems(variantToButton(settings.value("ButtonR", QGamepadManager::ButtonR1)));
    comboBoxSelect = populateItems(variantToButton(settings.value("ButtonSelect", QGamepadManager::ButtonSelect)));
    comboBoxStart = populateItems(variantToButton(settings.value("ButtonStart", QGamepadManager::ButtonStart)));
    comboBoxZL = populateItems(variantToButton(settings.value("ButtonZL", QGamepadManager::ButtonL2)));
    comboBoxZR = populateItems(variantToButton(settings.value("ButtonZR", QGamepadManager::ButtonR2)));
    comboBoxHome = populateItems(variantToButton(settings.value("ButtonHome", QGamepadManager::ButtonInvalid)));
    comboBoxPower = populateItems(variantToButton(settings.value("ButtonPower", QGamepadManager::ButtonInvalid)));
    comboBoxPowerLong = populateItems(variantToButton(settings.value("ButtonPowerLong", QGamepadManager::ButtonInvalid)));
    comboBoxTouch1 = populateItems(variantToButton(settings.value("ButtonT1", QGamepadManager::ButtonInvalid)));
    comboBoxTouch2 = populateItems(variantToButton(settings.value("ButtonT2", QGamepadManager::ButtonInvalid)));
    comboBoxTouch3 = populateItems(variantToButton(settings.value("ButtonT3", QGamepadManager::ButtonInvalid)));
    comboBoxTouch4 = populateItems(variantToButton(settings.value("ButtonT4", QGamepadManager::ButtonInvalid)));

    touchButton1XEdit = new QLineEdit(this);
    touchButton1XEdit->setClearButtonEnabled(true);
    touchButton1XEdit->setText(settings.value("touchButton1X", "0").toString());
    touchButton1YEdit = new QLineEdit(this);
    touchButton1YEdit->setClearButtonEnabled(true);
    touchButton1YEdit->setText(settings.value("touchButton1Y", "0").toString());

    touchButton2XEdit = new QLineEdit(this);
    touchButton2XEdit->setClearButtonEnabled(true);
    touchButton2XEdit->setText(settings.value("touchButton2X", "0").toString());
    touchButton2YEdit = new QLineEdit(this);
    touchButton2YEdit->setClearButtonEnabled(true);
    touchButton2YEdit->setText(settings.value("touchButton2Y", "0").toString());

    touchButton3XEdit = new QLineEdit(this);
    touchButton3XEdit->setClearButtonEnabled(true);
    touchButton3XEdit->setText(settings.value("touchButton3X", "0").toString());
    touchButton3YEdit = new QLineEdit(this);
    touchButton3YEdit->setClearButtonEnabled(true);
    touchButton3YEdit->setText(settings.value("touchButton3Y", "0").toString());

    touchButton4XEdit = new QLineEdit(this);
    touchButton4XEdit->setClearButtonEnabled(true);
    touchButton4XEdit->setText(settings.value("touchButton4X", "0").toString());
    touchButton4YEdit = new QLineEdit(this);
    touchButton4YEdit->setClearButtonEnabled(true);
    touchButton4YEdit->setText(settings.value("touchButton4Y", "0").toString());

    invertYCheckbox = new QCheckBox(this);
    invertYCppCheckbox = new QCheckBox(this);
    swapSticksCheckbox = new QCheckBox(this);
    mhCameraCheckbox = new QCheckBox(this);
    rsSmashCheckbox = new QCheckBox(this);
    rsFaceButtonsCheckbox = new QCheckBox();
    disableCStickCheckbox = new QCheckBox();

    saveButton = new QPushButton(tr("&SAVE"), this);
    closeButton = new QPushButton(tr("&CANCEL"), this);

    layout = new QGridLayout(this);

    layout->addWidget(new QLabel("Y Button"), 0, 0);
    layout->addWidget(comboBoxY, 0, 1);
    layout->addWidget(new QLabel("X Button"), 0, 2);
    layout->addWidget(comboBoxX, 0, 3);
    layout->addWidget(new QLabel("B Button"), 1, 0);
    layout->addWidget(comboBoxB, 1, 1);
    layout->addWidget(new QLabel("A Button"), 1, 2);
    layout->addWidget(comboBoxA, 1, 3);

    layout->addWidget(new QLabel("DPad-Down"), 2, 0);
    layout->addWidget(comboBoxDown, 2, 1);
    layout->addWidget(new QLabel("DPad-Up"), 2, 2);
    layout->addWidget(comboBoxUp, 2, 3);
    layout->addWidget(new QLabel("DPad-Left"), 3, 0);
    layout->addWidget(comboBoxLeft, 3, 1);
    layout->addWidget(new QLabel("DPad-Right"), 3, 2);
    layout->addWidget(comboBoxRight, 3, 3);

    layout->addWidget(new QLabel("L Button"), 4, 0);
    layout->addWidget(comboBoxL, 4, 1);
    layout->addWidget(new QLabel("R Button"), 4, 2);
    layout->addWidget(comboBoxR, 4, 3);
    layout->addWidget(new QLabel("ZL Button"), 5, 0);
    layout->addWidget(comboBoxZL, 5, 1);
    layout->addWidget(new QLabel("ZR Button"), 5, 2);
    layout->addWidget(comboBoxZR, 5, 3);

    layout->addWidget(new QLabel("Select"), 6, 0);
    layout->addWidget(comboBoxSelect, 6, 1);
    layout->addWidget(new QLabel("Start"), 6, 2);
    layout->addWidget(comboBoxStart, 6, 3);

    layout->addWidget(new QLabel("Power Button"), 7, 0);
    layout->addWidget(comboBoxPower, 7, 1);
    layout->addWidget(new QLabel("Power-Long"), 7, 2);
    layout->addWidget(comboBoxPowerLong, 7, 3);
    layout->addWidget(new QLabel("Home Button"), 8, 0, 1, 2);
    layout->addWidget(comboBoxHome, 8, 1, 1, 2);

    layout->addWidget(new QLabel("Invert Y axis"), 17, 0);
    layout->addWidget(invertYCheckbox, 17, 1);
    layout->addWidget(new QLabel("Invert CPP Y"), 17, 2);
    layout->addWidget(invertYCppCheckbox, 17, 3);
    layout->addWidget(new QLabel("Swap CPads"), 18, 2);
    layout->addWidget(swapSticksCheckbox, 18, 3);
    layout->addWidget(new QLabel("Disable C"), 18, 0);
    layout->addWidget(disableCStickCheckbox, 18, 1);

    layout->addWidget(new QLabel("RS as DPad"), 19, 0);
    layout->addWidget(mhCameraCheckbox, 19, 1);
    layout->addWidget(new QLabel("RS as Smash"), 20, 0);
    layout->addWidget(rsSmashCheckbox, 20, 1);
    layout->addWidget(new QLabel("RS as ABXY"), 19, 2);
    layout->addWidget(rsFaceButtonsCheckbox, 19, 3);

    layout->addWidget(new QLabel("Touch R"), 9, 0, 1, 2);
    layout->addWidget(comboBoxTouch1, 9, 1, 1, 2);
    layout->addWidget(new QLabel("X"), 10, 0);
    layout->addWidget(touchButton1XEdit, 10, 1);
    layout->addWidget(new QLabel("Y"), 10, 2);
    layout->addWidget(touchButton1YEdit, 10, 3);
    layout->addWidget(new QLabel("Touch B"), 11, 0, 1, 2);
    layout->addWidget(comboBoxTouch2, 11, 1, 1, 2);
    layout->addWidget(new QLabel("X"), 12, 0);
    layout->addWidget(touchButton2XEdit, 12, 1);
    layout->addWidget(new QLabel("Y"), 12, 2);
    layout->addWidget(touchButton2YEdit, 12, 3);
    layout->addWidget(new QLabel("Touch G"), 13, 0, 1, 2);
    layout->addWidget(comboBoxTouch3, 13, 1, 1, 2);
    layout->addWidget(new QLabel("X"), 14, 0);
    layout->addWidget(touchButton3XEdit, 14, 1);
    layout->addWidget(new QLabel("Y"), 14, 2);
    layout->addWidget(touchButton3YEdit, 14, 3);
    layout->addWidget(new QLabel("Touch Y"), 15, 0, 1, 2);
    layout->addWidget(comboBoxTouch4, 15, 1, 1, 2);
    layout->addWidget(new QLabel("X"), 16, 0);
    layout->addWidget(touchButton4XEdit, 16, 1);
    layout->addWidget(new QLabel("Y"), 16, 2);
    layout->addWidget(touchButton4YEdit, 16, 3);

    layout->addWidget(saveButton, 21, 0, 1, 2);
    layout->addWidget(closeButton, 21, 2, 1, 2);

    connect(touchButton1XEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton1X = text.toUInt();
        settings.setValue("touchButton1X", text);
    });
    connect(touchButton1YEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton1Y = text.toUInt();
        settings.setValue("touchButton1Y", text);
    });
    connect(touchButton2XEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton2X = text.toUInt();
        settings.setValue("touchButton2X", text);
    });
    connect(touchButton2YEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton2Y = text.toUInt();
        settings.setValue("touchButton2Y", text);
    });
    connect(touchButton3XEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton3X = text.toUInt();
        settings.setValue("touchButton3X", text);
    });
    connect(touchButton3YEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton3Y = text.toUInt();
        settings.setValue("touchButton3Y", text);
    });
    connect(touchButton4XEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton4X = text.toUInt();
        settings.setValue("touchButton4X", text);
    });
    connect(touchButton4YEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton4Y = text.toUInt();
        settings.setValue("touchButton4Y", text);
    });

    connect(invertYCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                yAxisMultiplier = 1;
                settings.setValue("invertY", false);
                break;
            case Qt::Checked:
                yAxisMultiplier = -1;
                settings.setValue("invertY", true);
                break;
            default: break;
        }
    });

    connect(invertYCppCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                yAxisMultiplierCpp = 1;
                settings.setValue("invertCPPY", false);
                break;
            case Qt::Checked:
                yAxisMultiplierCpp = -1;
                settings.setValue("invertCPPY", true);
                break;
            default: break;
        }
    });

    connect(swapSticksCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                shouldSwapStick = false;
                settings.setValue("swapSticks", false);
                break;
            case Qt::Checked:
                shouldSwapStick = true;
                settings.setValue("swapSticks", true);
                break;
            default: break;
        }

    });

    connect(rsSmashCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
         switch(state)
         {
             case Qt::Unchecked:
                 rightStickSmash = false;
                 settings.setValue("rightStickSmash", false);
                 break;
             case Qt::Checked:
                 rightStickSmash = true;
                 settings.setValue("rightStickSmash", true);
                 break;
             default: break;
         }

     });

    connect(mhCameraCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                monsterHunterCamera = false;
                settings.setValue("monsterHunterCamera", false);
                break;
            case Qt::Checked:
                monsterHunterCamera = true;
                settings.setValue("monsterHunterCamera", true);
                break;
            default: break;
        }
    });
    connect(disableCStickCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                cStickDisabled = false;
                settings.setValue("cStickDisable", false);
                break;
            case Qt::Checked:
                cStickDisabled = true;
                settings.setValue("cStickDisable", true);
                break;
            default: break;
        }
    });
    connect(rsFaceButtonsCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                rightStickFaceButtons = false;
                settings.setValue("rightStickABXY", false);
                break;
            case Qt::Checked:
                rightStickFaceButtons = true;
                settings.setValue("rightStickABXY", true);
                break;
            default: break;
        }
    });

    connect(saveButton, &QPushButton::pressed, this,
            [this, ts](void)
    {
        QGamepadManager::GamepadButton a = variantToButton(currentData(comboBoxA));
        hidButtonsAB[0] = a;
        settings.setValue("ButtonA", a);
        QGamepadManager::GamepadButton b = variantToButton(currentData(comboBoxB));
        hidButtonsAB[1] = b;
        settings.setValue("ButtonB", b);

        QGamepadManager::GamepadButton select = variantToButton(currentData(comboBoxSelect));
        hidButtonsMiddle[0] = select;
        settings.setValue("ButtonSelect", select);
        QGamepadManager::GamepadButton start = variantToButton(currentData(comboBoxStart));
        hidButtonsMiddle[1] = start;
        settings.setValue("ButtonStart", start);
        QGamepadManager::GamepadButton right = variantToButton(currentData(comboBoxRight));
        hidButtonsMiddle[2] = right;
        settings.setValue("ButtonRight", right);
        QGamepadManager::GamepadButton left = variantToButton(currentData(comboBoxLeft));
        hidButtonsMiddle[3] = left;
        settings.setValue("ButtonLeft", left);
        QGamepadManager::GamepadButton up = variantToButton(currentData(comboBoxUp));
        hidButtonsMiddle[4] = up;
        settings.setValue("ButtonUp", up);
        QGamepadManager::GamepadButton down = variantToButton(currentData(comboBoxDown));
        hidButtonsMiddle[5] = down;
        settings.setValue("ButtonDown", down);
        QGamepadManager::GamepadButton r = variantToButton(currentData(comboBoxR));
        hidButtonsMiddle[6] = r;
        settings.setValue("ButtonR", r);
        QGamepadManager::GamepadButton l = variantToButton(currentData(comboBoxL));
        hidButtonsMiddle[7] = l;
        settings.setValue("ButtonL", l);

        QGamepadManager::GamepadButton x = variantToButton(currentData(comboBoxX));
        hidButtonsXY[0] = x;
        settings.setValue("ButtonX", x);
        QGamepadManager::GamepadButton y = variantToButton(currentData(comboBoxY));
        hidButtonsXY[1] = y;
        settings.setValue("ButtonY", y);

        QGamepadManager::GamepadButton zr = variantToButton(currentData(comboBoxZR));
        irButtons[0] = zr;
        settings.setValue("ButtonZR", zr);
        QGamepadManager::GamepadButton zl = variantToButton(currentData(comboBoxZL));
        irButtons[1] = zl;
        settings.setValue("ButtonZL", zl);

        QGamepadManager::GamepadButton power = variantToButton(currentData(comboBoxPower));
        powerButton = power;
        settings.setValue("ButtonPower", power);
        QGamepadManager::GamepadButton powerLong = variantToButton(currentData(comboBoxPowerLong));
        powerLongButton = powerLong;
        settings.setValue("ButtonPowerLong", powerLong);
        QGamepadManager::GamepadButton home = variantToButton(currentData(comboBoxHome));
        homeButton = home;
        settings.setValue("ButtonHome", home);

        QGamepadManager::GamepadButton t1 = variantToButton(currentData(comboBoxTouch1));
        touchButton1 = t1;
        settings.setValue("ButtonT1", t1);
        QGamepadManager::GamepadButton t2 = variantToButton(currentData(comboBoxTouch2));
        touchButton2 = t2;
        settings.setValue("ButtonT2", t2);
        QGamepadManager::GamepadButton t3 = variantToButton(currentData(comboBoxTouch3));
        touchButton3 = t3;
        settings.setValue("ButtonT3", t3);
        QGamepadManager::GamepadButton t4 = variantToButton(currentData(comboBoxTouch4));
        touchButton4 = t4;
        settings.setValue("ButtonT4", t4);

        ts->updatePixmap();

    });
    connect(closeButton, &QPushButton::pressed, this,
            [this](void)
    {
       this->hide();
    });

    invertYCheckbox->setChecked(settings.value("invertY", false).toBool());
    invertYCppCheckbox->setChecked(settings.value("invertCPPY", false).toBool());
    swapSticksCheckbox->setChecked(settings.value("swapSticks", false).toBool());
    mhCameraCheckbox->setChecked(settings.value("monsterHunterCamera", false).toBool());
    rsSmashCheckbox->setChecked(settings.value("rightStickSmash", false).toBool());
    disableCStickCheckbox->setChecked(settings.value("cStickDisable", false).toBool());
    rsFaceButtonsCheckbox->setChecked(settings.value("rightStickABXY", false).toBool());
}

QComboBox* ConfigWindow::populateItems(QGamepadManager::GamepadButton button)
{
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("A", QGamepadManager::ButtonA);
    comboBox->addItem("B", QGamepadManager::ButtonB);
    comboBox->addItem("X", QGamepadManager::ButtonX);
    comboBox->addItem("Y", QGamepadManager::ButtonY);
    comboBox->addItem("Up", QGamepadManager::ButtonUp);
    comboBox->addItem("Down", QGamepadManager::ButtonDown);
    comboBox->addItem("Right", QGamepadManager::ButtonRight);
    comboBox->addItem("Left", QGamepadManager::ButtonLeft);
    comboBox->addItem("LB", QGamepadManager::ButtonL1);
    comboBox->addItem("RB", QGamepadManager::ButtonR1);
    comboBox->addItem("LT", QGamepadManager::ButtonL2);
    comboBox->addItem("RT", QGamepadManager::ButtonR2);
    comboBox->addItem("Start", QGamepadManager::ButtonStart);
    comboBox->addItem("Back", QGamepadManager::ButtonSelect);
    comboBox->addItem("L3", QGamepadManager::ButtonL3);
    comboBox->addItem("R3", QGamepadManager::ButtonR3);
    comboBox->addItem("Guide", QGamepadManager::ButtonGuide);
    comboBox->addItem("None", QGamepadManager::ButtonInvalid);

    int index = comboBox->findData(button);
    comboBox->setCurrentIndex(index);

    return comboBox;
}

QVariant ConfigWindow::currentData(QComboBox *comboBox)
{
    QVariant variant;

    variant = comboBox->itemData(comboBox->currentIndex());

    return variant;
}
