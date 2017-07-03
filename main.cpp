#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QGamepadManager>
#include <QGamepad>
#include <QtEndian>
#include <QUdpSocket>
#include <QTimer>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialog>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QSettings>
#include <QComboBox>

#include <algorithm>
#include <cmath>

#define CPAD_BOUND          0x5d0
#define CPP_BOUND           0x7f

#define TOUCH_SCREEN_WIDTH  320
#define TOUCH_SCREEN_HEIGHT 240

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

double lx = 0.0, ly = 0.0;
double rx = 0.0, ry = 0.0;
QGamepadManager::GamepadButtons buttons = 0;
u32 interfaceButtons = 0;
QString ipAddress;
int yAxisMultiplier = 1;
bool abInverse = false;
bool xyInverse = false;

bool touchScreenPressed;
QPoint touchScreenPosition;

QSettings settings("TuxSH", "InputRedirectionClient-Qt");

QGamepadManager::GamepadButton homeButton = QGamepadManager::ButtonInvalid;
QGamepadManager::GamepadButton powerButton = QGamepadManager::ButtonInvalid;
QGamepadManager::GamepadButton powerLongButton = QGamepadManager::ButtonInvalid;

QGamepadManager::GamepadButton hidButtonsAB[] = {
    QGamepadManager::ButtonA,
    QGamepadManager::ButtonB,
};

QGamepadManager::GamepadButton hidButtonsMiddle[] = {
    QGamepadManager::ButtonSelect,
    QGamepadManager::ButtonStart,
    QGamepadManager::ButtonRight,
    QGamepadManager::ButtonLeft,
    QGamepadManager::ButtonUp,
    QGamepadManager::ButtonDown,
    QGamepadManager::ButtonR1,
    QGamepadManager::ButtonL1,
};

QGamepadManager::GamepadButton hidButtonsXY[] = {
    QGamepadManager::ButtonX,
    QGamepadManager::ButtonY,
};

QGamepadManager::GamepadButton irButtons[] = {
    QGamepadManager::ButtonR2,
    QGamepadManager::ButtonL2,
};

/*QGamepadManager::GamepadButton speButtons[] = {
    QGamepadManager::ButtonL3,
    QGamepadManager::ButtonR3,
    QGamepadManager::ButtonGuide,
};*/

void sendFrame(void)
{
    u32 hidPad = 0xfff;
    if(!abInverse)
    {
        for(u32 i = 0; i < 2; i++)
        {
            if(buttons & (1 << hidButtonsAB[i]))
                hidPad &= ~(1 << i);
        }
    }
    else
    {
        for(u32 i = 0; i < 2; i++)
        {
            if(buttons & (1 << hidButtonsAB[1-i]))
                hidPad &= ~(1 << i);
        }
    }

    for(u32 i = 2; i < 10; i++)
    {
        if(buttons & (1 << hidButtonsMiddle[i-2]))
            hidPad &= ~(1 << i);
    }

    if(!xyInverse)
    {
        for(u32 i = 10; i < 12; i++)
        {
            if(buttons & (1 << hidButtonsXY[i-10]))
                hidPad &= ~(1 << i);
        }
    }
    else
    {
        for(u32 i = 10; i < 12; i++)
        {
            if(buttons & (1 << hidButtonsXY[1-(i-10)]))
                hidPad &= ~(1 << i);
        }
    }

    u32 irButtonsState = 0;
    for(u32 i = 0; i < 2; i++)
    {
            if(buttons & (1 << irButtons[i]))
                irButtonsState |= 1 << (i + 1);
    }

    /*u32 specialButtonsState = 0;
    for(u32 i = 0; i < 3; i++)
    {

        if(buttons & (1 << speButtons[i]))
            specialButtonsState |= 1 << i;
    }
    specialButtonsState |= interfaceButtons;*/

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
        u32 x = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.x()), TOUCH_SCREEN_WIDTH)) / TOUCH_SCREEN_WIDTH;
        u32 y = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.y()), TOUCH_SCREEN_HEIGHT)) / TOUCH_SCREEN_HEIGHT;
        touchScreenState = (1 << 24) | (y << 12) | x;
    }

    QByteArray ba(20, 0);
    qToLittleEndian(hidPad, (uchar *)ba.data());
    qToLittleEndian(touchScreenState, (uchar *)ba.data() + 4);
    qToLittleEndian(circlePadState, (uchar *)ba.data() + 8);
    qToLittleEndian(cppState, (uchar *)ba.data() + 12);
    qToLittleEndian(interfaceButtons, (uchar *)ba.data() + 16);
    QUdpSocket().writeDatagram(ba, QHostAddress(ipAddress), 4950);
}

struct GamepadMonitor : public QObject {

    GamepadMonitor(QObject *parent = nullptr) : QObject(parent)
    {
        connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent, this,
            [](int deviceId, QGamepadManager::GamepadButton button, double value)
        {
            (void)deviceId;
            (void)value;
            buttons |= QGamepadManager::GamepadButtons(1 << button);

            if (button == homeButton)
            {
                interfaceButtons |= 1;
            }
            if (button == powerButton)
            {
                interfaceButtons |= 2;
            }
            if (button == powerLongButton)
            {
                interfaceButtons |= 4;
            }

            sendFrame();
        });

        connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,
            [](int deviceId, QGamepadManager::GamepadButton button)
        {
            (void)deviceId;
            buttons &= QGamepadManager::GamepadButtons(~(1 << button));

            if (button == homeButton)
            {
                interfaceButtons &= ~1;
            }
            if (button == powerButton)
            {
                interfaceButtons &= ~2;
            }
            if (button == powerLongButton)
            {
                interfaceButtons &= ~4;
            }

            sendFrame();
        });
        connect(QGamepadManager::instance(), &QGamepadManager::gamepadAxisEvent, this,
            [](int deviceId, QGamepadManager::GamepadAxis axis, double value)
        {
            (void)deviceId;
            (void)value;
            switch(axis)
            {
                case QGamepadManager::AxisLeftX:
                    lx = value;
                    break;
                case QGamepadManager::AxisLeftY:
                    ly = yAxisMultiplier * -value; // for some reason qt inverts this
                    break;

                case QGamepadManager::AxisRightX:
                    rx = value;
                    break;
                case QGamepadManager::AxisRightY:
                    ry = yAxisMultiplier * -value; // for some reason qt inverts this
                    break;
                default: break;
            }
            sendFrame();
        });
    }
};

struct TouchScreen : public QDialog {
    TouchScreen(QWidget *parent = nullptr) : QDialog(parent)
    {
        this->setFixedSize(TOUCH_SCREEN_WIDTH, TOUCH_SCREEN_HEIGHT);
        this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        this->setWindowTitle(tr("InputRedirectionClient-Qt - Touch screen"));
    }

    void mousePressEvent(QMouseEvent *ev)
    {
        if(ev->button() == Qt::LeftButton)
        {
            touchScreenPressed = true;
            touchScreenPosition = ev->pos();
            sendFrame();
        }
    }

    void mouseMoveEvent(QMouseEvent *ev)
    {
        if(touchScreenPressed && (ev->buttons() & Qt::LeftButton))
        {
            touchScreenPosition = ev->pos();
            sendFrame();
        }
    }

    void mouseReleaseEvent(QMouseEvent *ev)
    {
        if(ev->button() == Qt::LeftButton)
        {
            touchScreenPressed = false;
            sendFrame();
        }
    }

    void closeEvent(QCloseEvent *ev)
    {
        touchScreenPressed = false;
        sendFrame();
        ev->accept();
    }
};

struct FrameTimer : public QTimer {
    FrameTimer(QObject *parent = nullptr) : QTimer(parent)
    {
        connect(this, &QTimer::timeout, this,
                [](void)
        {
            sendFrame();
        });
    }
};

struct RemapConfig : public QDialog {
private:
    QVBoxLayout *layout;
    QFormLayout *formLayout;
    QComboBox *comboBoxA, *comboBoxB, *comboBoxX, *comboBoxY, *comboBoxL, *comboBoxR,
        *comboBoxUp, *comboBoxDown, *comboBoxLeft, *comboBoxRight, *comboBoxStart, *comboBoxSelect,
        *comboBoxZL, *comboBoxZR, *comboBoxHome, *comboBoxPower, *comboBoxPowerLong, *comboBoxTouchA, *comboBoxTouchB;
    QLineEdit *primTouchButtonXEdit, *primTouchButtonYEdit,
        *altTouchButtonXEdit, *altTouchButtonYEdit;
    QPushButton *saveButton, *closeButton;
    QComboBox* populateFields(QString button)
    {
        QComboBox *comboBox = new QComboBox();
        comboBox->addItem("A", QGamepadManager::ButtonA);
        comboBox->addItem("B", QGamepadManager::ButtonB);
        comboBox->addItem("X", QGamepadManager::ButtonX);
        comboBox->addItem("Y", QGamepadManager::ButtonY);
        comboBox->addItem("Right", QGamepadManager::ButtonRight);
        comboBox->addItem("Left", QGamepadManager::ButtonLeft);
        comboBox->addItem("Up", QGamepadManager::ButtonUp);
        comboBox->addItem("Down", QGamepadManager::ButtonDown);
        comboBox->addItem("RB", QGamepadManager::ButtonR1);
        comboBox->addItem("LB", QGamepadManager::ButtonL1);
        comboBox->addItem("Select", QGamepadManager::ButtonSelect);
        comboBox->addItem("Start", QGamepadManager::ButtonStart);
        comboBox->addItem("RT", QGamepadManager::ButtonR2);
        comboBox->addItem("LT", QGamepadManager::ButtonL2);
        comboBox->addItem("L3", QGamepadManager::ButtonL3);
        comboBox->addItem("R3", QGamepadManager::ButtonR3);
        comboBox->addItem("Guide", QGamepadManager::ButtonGuide);
        comboBox->addItem("None", QGamepadManager::ButtonInvalid);

        int index = comboBox->findText(button);
        comboBox->setCurrentIndex(index);

        return comboBox;
    }

public:
    RemapConfig(QWidget *parent = nullptr) : QDialog(parent)
    {
        this->setFixedSize(TOUCH_SCREEN_WIDTH, 550);
        this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        this->setWindowTitle(tr("InputRedirectionClient-Qt - Button Config"));

        layout = new QVBoxLayout(this);

        comboBoxA = populateFields("A");
        comboBoxB = populateFields("B");
        comboBoxX = populateFields("X");
        comboBoxY = populateFields("Y");
        comboBoxUp = populateFields("Up");
        comboBoxDown = populateFields("Down");
        comboBoxLeft = populateFields("Left");
        comboBoxRight = populateFields("Right");
        comboBoxL = populateFields("LB");
        comboBoxR = populateFields("RB");
        comboBoxSelect = populateFields("Select");
        comboBoxStart = populateFields("Start");
        comboBoxZL = populateFields("LT");
        comboBoxZR = populateFields("RT");
        comboBoxHome = populateFields("None");
        comboBoxPower = populateFields("None");
        comboBoxPowerLong = populateFields("None");

        formLayout = new QFormLayout;

        formLayout->addRow(tr("A Button"), comboBoxA);
        formLayout->addRow(tr("B Button"), comboBoxB);
        formLayout->addRow(tr("X Button"), comboBoxX);
        formLayout->addRow(tr("Y Button"), comboBoxY);
        formLayout->addRow(tr("DPad-Up"), comboBoxUp);
        formLayout->addRow(tr("DPad-Down"), comboBoxDown);
        formLayout->addRow(tr("DPad-Left"), comboBoxLeft);
        formLayout->addRow(tr("DPad-Right"), comboBoxRight);
        formLayout->addRow(tr("L Button"), comboBoxL);
        formLayout->addRow(tr("R Button"), comboBoxR);
        formLayout->addRow(tr("Select"), comboBoxSelect);
        formLayout->addRow(tr("Start"), comboBoxStart);
        formLayout->addRow(tr("Home"), comboBoxHome);
        formLayout->addRow(tr("Power"), comboBoxPower);
        formLayout->addRow(tr("Power-Long"), comboBoxPowerLong);
        formLayout->addRow(tr("ZL Button"), comboBoxZL);
        formLayout->addRow(tr("ZR Button"), comboBoxZR);

        saveButton = new QPushButton(tr("&SAVE"), this);
        closeButton = new QPushButton(tr("&CANCEL"), this);

        layout->addLayout(formLayout);
        layout->addWidget(saveButton);
        layout->addWidget(closeButton);

        connect(saveButton, &QPushButton::pressed, this,
                [this](void)
        {
            hidButtonsAB[0] = static_cast<QGamepadManager::GamepadButton>(comboBoxA->itemData(comboBoxA->currentIndex()).toInt());
            hidButtonsAB[1] = static_cast<QGamepadManager::GamepadButton>(comboBoxB->itemData(comboBoxB->currentIndex()).toInt());

            hidButtonsMiddle[0] = static_cast<QGamepadManager::GamepadButton>(comboBoxSelect->itemData(comboBoxSelect->currentIndex()).toInt());
            hidButtonsMiddle[1] = static_cast<QGamepadManager::GamepadButton>(comboBoxStart->itemData(comboBoxStart->currentIndex()).toInt());
            hidButtonsMiddle[2] = static_cast<QGamepadManager::GamepadButton>(comboBoxRight->itemData(comboBoxRight->currentIndex()).toInt());
            hidButtonsMiddle[3] = static_cast<QGamepadManager::GamepadButton>(comboBoxLeft->itemData(comboBoxLeft->currentIndex()).toInt());
            hidButtonsMiddle[4] = static_cast<QGamepadManager::GamepadButton>(comboBoxUp->itemData(comboBoxUp->currentIndex()).toInt());
            hidButtonsMiddle[5] = static_cast<QGamepadManager::GamepadButton>(comboBoxDown->itemData(comboBoxDown->currentIndex()).toInt());
            hidButtonsMiddle[6] = static_cast<QGamepadManager::GamepadButton>(comboBoxR->itemData(comboBoxR->currentIndex()).toInt());
            hidButtonsMiddle[7] = static_cast<QGamepadManager::GamepadButton>(comboBoxL->itemData(comboBoxL->currentIndex()).toInt());

            hidButtonsXY[0] = static_cast<QGamepadManager::GamepadButton>(comboBoxX->itemData(comboBoxX->currentIndex()).toInt());
            hidButtonsXY[1] = static_cast<QGamepadManager::GamepadButton>(comboBoxY->itemData(comboBoxY->currentIndex()).toInt());

            irButtons[0] = static_cast<QGamepadManager::GamepadButton>(comboBoxZR->itemData(comboBoxZR->currentIndex()).toInt());
            irButtons[1] = static_cast<QGamepadManager::GamepadButton>(comboBoxZL->itemData(comboBoxZL->currentIndex()).toInt());

            powerButton = static_cast<QGamepadManager::GamepadButton>(comboBoxPower->itemData(comboBoxPower->currentIndex()).toInt());
            powerLongButton = static_cast<QGamepadManager::GamepadButton>(comboBoxPowerLong->itemData(comboBoxPowerLong->currentIndex()).toInt());
            homeButton = static_cast<QGamepadManager::GamepadButton>(comboBoxHome->itemData(comboBoxHome->currentIndex()).toInt());
        });
        connect(closeButton, &QPushButton::pressed, this,
                [this](void)
        {
           this->hide();
        });
    }
};

class Widget : public QWidget
{
private:
    QVBoxLayout *layout;
    QFormLayout *formLayout;
    QLineEdit *addrLineEdit;
    QCheckBox *invertYCheckbox, *invertABCheckbox, *invertXYCheckbox;
    QPushButton *homeButton, *powerButton, *longPowerButton, *remapConfigButton;
    TouchScreen *touchScreen;
    RemapConfig *remapConfig;
public:
    Widget(QWidget *parent = nullptr) : QWidget(parent)
    {
        layout = new QVBoxLayout(this);

        addrLineEdit = new QLineEdit(this);
        addrLineEdit->setClearButtonEnabled(true);

        invertYCheckbox = new QCheckBox(this);
        invertABCheckbox = new QCheckBox(this);
        invertXYCheckbox = new QCheckBox(this);
        formLayout = new QFormLayout;

        formLayout->addRow(tr("IP &address"), addrLineEdit);
        formLayout->addRow(tr("&Invert Y axis"), invertYCheckbox);
        formLayout->addRow(tr("Invert A<->&B"), invertABCheckbox);
        formLayout->addRow(tr("Invert X<->&Y"), invertXYCheckbox);
        remapConfigButton = new QPushButton(tr("BUTTON &CONFIG"), this);

        homeButton = new QPushButton(tr("&HOME"), this);
        powerButton = new QPushButton(tr("&POWER"), this);
        longPowerButton = new QPushButton(tr("POWER (&long)"), this);

        layout->addLayout(formLayout);
        layout->addWidget(homeButton);
        layout->addWidget(powerButton);
        layout->addWidget(longPowerButton);
        layout->addWidget(remapConfigButton);

        connect(addrLineEdit, &QLineEdit::textChanged, this,
                [](const QString &text)
        {
            ipAddress = text;
            settings.setValue("ipAddress", text);
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

        connect(invertABCheckbox, &QCheckBox::stateChanged, this,
                [](int state)
        {
            switch(state)
            {
                case Qt::Unchecked:
                    abInverse = false;
                    settings.setValue("invertAB", false);
                    break;
                case Qt::Checked:
                    abInverse = true;
                    settings.setValue("invertAB", true);
                    break;
                default: break;
            }
        });

        connect(invertXYCheckbox, &QCheckBox::stateChanged, this,
                [](int state)
        {
            switch(state)
            {
                case Qt::Unchecked:
                    xyInverse = false;
                    settings.setValue("invertXY", false);
                    break;
                case Qt::Checked:
                    xyInverse = true;
                    settings.setValue("invertXY", true);
                    break;
                default: break;
            }
        });

        connect(homeButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 1;
           sendFrame();
        });

        connect(homeButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~1;
           sendFrame();
        });

        connect(powerButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 2;
           sendFrame();
        });

        connect(powerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~2;
           sendFrame();
        });

        connect(longPowerButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 4;
           sendFrame();
        });

        connect(longPowerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~4;
           sendFrame();
        });

        connect(remapConfigButton, &QPushButton::released, this,
                [this](void)
        {
           remapConfig->show();
        });

        touchScreen = new TouchScreen(nullptr);
        remapConfig = new RemapConfig(nullptr);
        this->setWindowTitle(tr("InputRedirectionClient-Qt"));

        addrLineEdit->setText(settings.value("ipAddress", "").toString());
        invertYCheckbox->setChecked(settings.value("invertY", false).toBool());
        invertABCheckbox->setChecked(settings.value("invertAB", false).toBool());
        invertXYCheckbox->setChecked(settings.value("invertXY", false).toBool());
    }

    void show(void)
    {
        QWidget::show();
        touchScreen->show();
        remapConfig->hide();
    }

    void closeEvent(QCloseEvent *ev)
    {
        touchScreen->close();
        remapConfig->close();
        ev->accept();
    }

    virtual ~Widget(void)
    {
        lx = ly = rx = ry = 0.0;
        buttons = 0;
        interfaceButtons = 0;
        touchScreenPressed = false;
        sendFrame();
        delete touchScreen;
        delete remapConfig;
    }

};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    GamepadMonitor m(&w);
    FrameTimer t(&w);
    TouchScreen ts;
    t.start(50);
    w.show();

    return a.exec();
}
