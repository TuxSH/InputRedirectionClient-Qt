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

#define TOUCH_SCREEN_WIDTH  320
#define TOUCH_SCREEN_HEIGHT 240

#define CPAD_BOUND          0x5d0
#define CPP_BOUND           0x7f

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

void sendFrame();

extern QGamepadManager::GamepadButtons buttons;
extern u32 interfaceButtons;
extern bool shouldSwapStick;
extern int yAxisMultiplier, yAxisMultiplierCpp;

extern double lx, ly;
extern double rx, ry;
extern QString ipAddress;
extern bool touchScreenPressed;
extern QSize touchScreenSize;
extern QPoint touchScreenPosition;

#endif // GLOBAL_H
