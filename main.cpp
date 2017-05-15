#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QGamepadManager>
#include <QGamepad>
#include <QtEndian>
#include <QUdpSocket>
#include <QTimer>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialog>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

double lx = 0.0, ly = 0.0;
double rx = 0.0, ry = 0.0;
QGamepadManager::GamepadButtons buttons = 0;
QString hostname;
int yAxisMultiplicator = 1;

void sendFrame(void)
{
    static const QGamepadManager::GamepadButton hidButtons[] = {
        QGamepadManager::ButtonA,
        QGamepadManager::ButtonB,
        QGamepadManager::ButtonSelect,
        QGamepadManager::ButtonStart,
        QGamepadManager::ButtonRight,
        QGamepadManager::ButtonLeft,
        QGamepadManager::ButtonUp,
        QGamepadManager::ButtonDown,
        QGamepadManager::ButtonR1,
        QGamepadManager::ButtonL1,
        QGamepadManager::ButtonX,
        QGamepadManager::ButtonY,
    };

    u32 hidPad = 0xfff;
    for(u32 i = 0; i < 12; i++)
    {
        if(buttons & (1 << hidButtons[i]))
            hidPad &= ~(1 << i);
    }

    u32 touchState = 0x2000000;
    u32 circleState = 0x7ff7ff;
    u16 x = (u16)(lx * 0x5d0 + 0x800);
    u16 y = (u16)(ly * 0x5d0 + 0x800);

    if(lx != 0.0 || ly != 0.0)
        circleState = ((u32) y << 12) | (u32)x;

    QByteArray ba(12, 0);
    qToLittleEndian(hidPad, (uchar *)ba.data());
    qToLittleEndian(touchState, (uchar *)ba.data() + 4);
    qToLittleEndian(circleState, (uchar *)ba.data() + 8);

    QUdpSocket().writeDatagram(ba, QHostAddress("192.168.1.12"), 4950);
}

struct GamepadMonitor : public QObject {

    GamepadMonitor(QObject *parent = Q_NULLPTR) : QObject(parent)
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
            [](int deviceId, QGamepadManager::GamepadAxis axis, double  value)
        {
            (void)deviceId;
            (void)value;
            switch(axis)
            {
                case QGamepadManager::AxisLeftX:
                    lx = value;
                    break;
                case QGamepadManager::AxisLeftY:
                    ly = -value; // for some reason qt inverts this
                    break;
                default: break;
            }
            sendFrame();
        });
    }
};

struct TouchScreen : public QDialog {
    TouchScreen(QWidget *parent = Q_NULLPTR) : QDialog(parent)
    {
        this->setFixedSize(320, 240);
    }
};

struct FrameTimer : public QTimer {
    FrameTimer(QObject *parent = Q_NULLPTR) : QTimer(parent)
    {
        connect(this, &QTimer::timeout, this,
                [](void)
        {
            sendFrame();
        });
    }
};


class Widget : public QWidget
{
private:
    QFormLayout *layout;
    QLineEdit *addrLineEdit;
    QCheckBox *invertYCheckbox;
    TouchScreen *touchScreen;
public:
    Widget(QWidget *parent = Q_NULLPTR) : QWidget(parent)
    {
        addrLineEdit = new QLineEdit(this);
        invertYCheckbox = new QCheckBox(this);
        layout = new QFormLayout(this);

        layout->addRow(tr("Hostname or IP &address"), addrLineEdit);
        layout->addRow(tr("&Invert Y axis"), invertYCheckbox);

        this->setLayout(layout);

        connect(addrLineEdit, &QLineEdit::textChanged, this,
                [](const QString &text)
        {
            hostname = text;
        });

        connect(invertYCheckbox, &QCheckBox::stateChanged, this,
                [](int state)
        {
            switch(state)
            {
                case Qt::Unchecked:
                    yAxisMultiplicator = 1;
                    break;
                case Qt::Checked:
                    yAxisMultiplicator = -1;
                    break;
                default: break;
            }
        });

        touchScreen = new TouchScreen(this);
    }

    /*virtual void show(void)
    {
        QWidget::show();
        touchScreen->show();
    }*/
};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    GamepadMonitor m(&w);
    FrameTimer t(&w);
    TouchScreen ts;
    t.start(10);
    w.show();

    return a.exec();
}
