#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <QApplication>
#include <QGamepadManager>
#include <QGamepad>
#include <QtEndian>
#include <QUdpSocket>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialog>
#include <QSettings>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QFileDialog>
#include <QLabel>
#include <QSlider>
#include <QThread>
#include "gpconfigurator.h"

#define TOUCH_SCREEN_WIDTH  320
#define TOUCH_SCREEN_HEIGHT 240

#define CPAD_BOUND          0x5d0
#define CPP_BOUND           0x7f

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

//void sendFrame(void);
QGamepadManager::GamepadButton variantToButton(QVariant variant);

extern QSettings settings;

extern QGamepadManager::GamepadButtons buttons;
extern u32 interfaceButtons;

extern int yAxisMultiplier, yAxisMultiplierCpp;
extern bool shouldSwapStick;
extern bool monsterHunterCamera;
extern bool rightStickSmash;
extern bool isSmashingV;
extern bool isSmashingH;
extern bool rightStickFaceButtons;
extern bool cStickDisabled;

extern QString ipAddress;
extern bool timerEnabled;

extern GamepadConfigurator *gpConfigurator;

extern bool touchScreenPressed;
extern QSize touchScreenSize;
extern QPoint touchScreenPosition;
extern double tsRatio;

extern QGamepadManager::GamepadButton homeButton;
extern QGamepadManager::GamepadButton powerButton;
extern QGamepadManager::GamepadButton powerLongButton;

extern QGamepadManager::GamepadButton touchButton1;
extern QGamepadManager::GamepadButton touchButton2;
extern QGamepadManager::GamepadButton touchButton3;
extern QGamepadManager::GamepadButton touchButton4;
extern int touchButton1X, touchButton1Y, touchButton2X, touchButton2Y;
extern int touchButton3X, touchButton3Y, touchButton4X, touchButton4Y;

extern QGamepadManager::GamepadButton hidButtonsAB[2];
extern QGamepadManager::GamepadButton hidButtonsMiddle[8];
extern QGamepadManager::GamepadButton hidButtonsXY[2];
extern QGamepadManager::GamepadButton irButtons[2];

struct MyAxis
{
    double x, y;
};

class Worker : public QObject {
    Q_OBJECT
 public:

    MyAxis getLeftAxis();
    MyAxis getRightAxis();
    MyAxis getPreviousLAxis();

    void setLeftAxis(double x, double y);
    void setRightAxis(double x, double y);
    void setPreviousLAxis(double x, double y);

    explicit Worker(QObject *parent = 0) : QObject(parent)
    {
        previousLeftAxis.x = leftAxis.x;
        previousLeftAxis.y = leftAxis.y;
    }
    ~Worker(){}

 public slots:
    void sendFrame();

 signals:
    void finished();
    void error(QString err);

 private:
    MyAxis leftAxis;
    MyAxis rightAxis;
    MyAxis previousLeftAxis;

};

extern Worker worker;

#endif // GLOBAL_H
