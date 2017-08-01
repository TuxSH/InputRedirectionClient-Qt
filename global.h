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
#include <QMetaObject>
#include "gpconfigurator.h"
#include "settings.h"
#include "shortcut.h"

#define TOUCH_SCREEN_WIDTH  320
#define TOUCH_SCREEN_HEIGHT 240

extern int CPAD_BOUND;
extern int CPP_BOUND;

extern Settings btnSettings;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

int appScreenTo3dsX(int);
int appScreenTo3dsY(int);

struct TouchButton
{
    int x, y;
};


extern std::vector<ShortCut> listShortcuts;
QGamepadManager::GamepadButton variantToButton(QVariant variant);
ShortCut variantToShortCut(QVariant variant);

extern int id, fid;

extern QSettings settings;

extern QGamepadManager::GamepadButtons buttons;
extern u32 interfaceButtons;

extern int yAxisMultiplier, yAxisMultiplierCpp;

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

extern TouchButton tbOne, tbTwo, tbThree, tbFour;

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
    ~Worker()
    {

    }

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
