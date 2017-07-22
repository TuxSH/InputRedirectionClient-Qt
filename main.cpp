#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

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
#include <QSettings>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>

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

bool touchScreenPressed;
QPoint touchScreenPosition;

QSettings settings("TuxSH", "InputRedirectionClient-Qt");

QGamepadManager::GamepadButton getButton(QString dsBtn)
{
    QString gpPadBtn;
    QVariant val;
    QSettings settings("GamepadControls.ini", QSettings::IniFormat);

    val =  settings.value(dsBtn);
    gpPadBtn = val.toString();

    QGamepadManager::GamepadButton btnConv;

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
    else
    btnConv = QGamepadManager::ButtonGuide;

    return btnConv;
}

void sendFrame(void)
{
    static const QGamepadManager::GamepadButton hidButtonsAB[] = {
        getButton("Button/3DS_A"),
        getButton("Button/3DS_B"),
    };

    static const QGamepadManager::GamepadButton hidButtonsMiddle[] = {
        getButton("Button/3DS_SELECT"),
        getButton("Button/3DS_START"),
        getButton("Button/3DS_D_RIGHT"),
        getButton("Button/3DS_D_LEFT"),
        getButton("Button/3DS_D_UP"),
        getButton("Button/3DS_D_DOWN"),
        getButton("Button/3DS_R"),
        getButton("Button/3DS_L"),
    };

    static const QGamepadManager::GamepadButton hidButtonsXY[] = {
        getButton("Button/3DS_X"),
        getButton("Button/3DS_Y"),
    };

    static const QGamepadManager::GamepadButton irButtons[] = {
        getButton("Button/3DS_ZR"),
        getButton("Button/3DS_ZL"),
    };

    static const QGamepadManager::GamepadButton speButtons[] = {
        getButton("Button/3DS_HOME"),
        getButton("Button/3DS_POWER"),
        getButton("Button/3DS_LPOWER"),
    };

    u32 hidPad = 0xfff;
    for(u32 i = 0; i < 2; i++)
    {
         if(buttons & (1 << hidButtonsAB[i]))
                hidPad &= ~(1 << i);
    }

    for(u32 i = 2; i < 10; i++)
    {
        if(buttons & (1 << hidButtonsMiddle[i-2]))
            hidPad &= ~(1 << i);
    }

    for(u32 i = 10; i < 12; i++)
   {
            if(buttons & (1 << hidButtonsXY[i-10]))
                hidPad &= ~(1 << i);
   }

    u32 irButtonsState = 0;
    for(u32 i = 0; i < 2; i++)
    {
        if(buttons & (1 << irButtons[i]))
            irButtonsState |= 1 << (i + 1);
    }

    u32 specialButtonsState = 0;
    for(u32 i = 0; i < 3; i++)
    {

        if(buttons & (1 << speButtons[i]))
            specialButtonsState |= 1 << i;
    }
    specialButtonsState |= interfaceButtons;

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
    qToLittleEndian(specialButtonsState, (uchar *)ba.data() + 16);
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
            sendFrame();
        });

        connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,
            [](int deviceId, QGamepadManager::GamepadButton button)
        {
            (void)deviceId;
            buttons &= QGamepadManager::GamepadButtons(~(1 << button));
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
private:
    QLabel *bgLabel;
public:
    TouchScreen(QWidget *parent = nullptr) : QDialog(parent)
    {
        this->setFixedSize(TOUCH_SCREEN_WIDTH, TOUCH_SCREEN_HEIGHT);
        this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        this->setWindowTitle(tr("InputRedirectionClient-Qt - Touch screen"));

        QString curPath = qApp->QCoreApplication::applicationDirPath()+"/Touchscreen.jpg";
        QPixmap bkgnd(curPath);

        bgLabel = new QLabel(this);
        bgLabel->setFixedHeight(TOUCH_SCREEN_HEIGHT);
        bgLabel->setFixedWidth(TOUCH_SCREEN_WIDTH);
        bgLabel->setPixmap(bkgnd);
        bgLabel->setScaledContents(true);
        bgLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    }

    void mousePressEvent(QMouseEvent *ev)
    {
        if(ev->button() == Qt::LeftButton)
        {
            touchScreenPressed = true;
            touchScreenPosition = ev->pos();
            sendFrame();
        }

        if(ev->button() == Qt::RightButton)
        {

           QString strPic = QFileDialog::getOpenFileName(this,
                          tr("Open Touchscreen Image (320x240)"), "MyDocuments",
                          tr("Image Files (*.jpg *.jpeg *.png *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)"));

            if(!strPic.isNull())
            {
               QPixmap newPic(strPic);
               bgLabel->setPixmap(newPic);
            }
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

class Widget : public QWidget
{
private:
    QVBoxLayout *layout;
    QFormLayout *formLayout;
    QLineEdit *addrLineEdit;
    QCheckBox *invertYCheckbox;
    QPushButton *homeButton, *powerButton, *longPowerButton;
    TouchScreen *touchScreen;
public:
    Widget(QWidget *parent = nullptr) : QWidget(parent)
    {
        layout = new QVBoxLayout(this);

        addrLineEdit = new QLineEdit(this);
        addrLineEdit->setClearButtonEnabled(true);

        invertYCheckbox = new QCheckBox(this);
        formLayout = new QFormLayout;

        formLayout->addRow(tr("IP &address"), addrLineEdit);
        formLayout->addRow(tr("&Invert Y axis"), invertYCheckbox);

        homeButton = new QPushButton(tr("&HOME"), this);
        powerButton = new QPushButton(tr("&POWER"), this);
        longPowerButton = new QPushButton(tr("POWER (&long)"), this);

        layout->addLayout(formLayout);
        layout->addWidget(homeButton);
        layout->addWidget(powerButton);
        layout->addWidget(longPowerButton);

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


        touchScreen = new TouchScreen(nullptr);
        this->setWindowTitle(tr("InputRedirectionClient-Qt"));

        addrLineEdit->setText(settings.value("ipAddress", "").toString());
        invertYCheckbox->setChecked(settings.value("invertY", false).toBool());
    }

    void show(void)
    {
        QWidget::show();
        touchScreen->show();
    }

    void closeEvent(QCloseEvent *ev)
    {
        touchScreen->close();
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
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    GamepadMonitor m(&w);
    w.show();

    return a.exec();
}
