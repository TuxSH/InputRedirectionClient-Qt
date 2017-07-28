#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMessageBox>
#include "global.h"

#include "touchscreen.h"
#include "gpconfigurator.h"

QSettings settings("TuxSH", "InputRedirectionClient-Qt");

class Widget : public QWidget
{
private:

    QVBoxLayout *layout;
    QFormLayout *formLayout;
    QLineEdit *addrLineEdit;
    QCheckBox *invertYCheckbox, *invertYCppCheckbox, *swapSticksCheckbox;
    QPushButton *homeButton, *powerButton, *longPowerButton, *configButton;
    QSlider *touchOpacitySlider;

    TouchScreen *touchScreen;

public:

    Widget(QWidget *parent = nullptr) : QWidget(parent)
    {
        layout = new QVBoxLayout(this);

        addrLineEdit = new QLineEdit(this);
        formLayout = new QFormLayout(this);

        addrLineEdit->setClearButtonEnabled(true);

        invertYCheckbox = new QCheckBox(this);
        invertYCppCheckbox = new QCheckBox(this);
        swapSticksCheckbox = new QCheckBox(this);

        formLayout->addRow(tr("IP &address"), addrLineEdit);
        formLayout->addRow(tr("&Invert Y axis"), invertYCheckbox);
        formLayout->addRow(tr("&Invert Cpp Y axis"), invertYCppCheckbox);
        formLayout->addRow(tr("&Swap Analog Sticks"), swapSticksCheckbox);

        touchOpacitySlider = new QSlider(Qt::Horizontal);
        touchOpacitySlider->setRange(0, 10);
        touchOpacitySlider->setValue(10);
        touchOpacitySlider->setTickInterval(1);
        formLayout->addRow(tr("TS &Opacity"), touchOpacitySlider);

        homeButton = new QPushButton(tr("&HOME"), this);
        powerButton = new QPushButton(tr("&POWER"), this);
        longPowerButton = new QPushButton(tr("POWER (&long)"), this);
        configButton = new QPushButton(tr("&Configure Controller"), this);

        layout->addLayout(formLayout);
        layout->addWidget(homeButton);
        layout->addWidget(powerButton);
        layout->addWidget(longPowerButton);
        layout->addWidget(configButton);

        touchScreen = new TouchScreen(nullptr);
        gpConfigurator = new GamepadConfigurator();

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

        connect(invertYCppCheckbox, &QCheckBox::stateChanged, this,
                [](int state)
        {
            switch(state)
            {
                case Qt::Unchecked:
                    yAxisMultiplierCpp = 1;
                    settings.setValue("invertYCpp", false);
                    break;
                case Qt::Checked:
                    yAxisMultiplierCpp = -1;
                    settings.setValue("invertYCpp", true);
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
                    settings.setValue("shouldSwapStick", false);
                    break;
                case Qt::Checked:
                    shouldSwapStick = true;
                    settings.setValue("shouldSwapStick", true);
                    break;
                default: break;
            }

        });

        connect(homeButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons |= 1;
           //sendFrame();
        });

        connect(homeButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~1;
          // sendFrame();
        });

        connect(powerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons |= 2;
          // sendFrame();
        });

        connect(powerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~2;
          // sendFrame();
        });

        connect(longPowerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons |= 4;
          // sendFrame();
        });

        connect(longPowerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~4;
           //sendFrame();
        });

        connect(configButton, &QPushButton::released, this,
                [](void)
        {
           gpConfigurator->showGui();
        });

        connect(touchOpacitySlider, &QSlider::valueChanged, this,
                [this](int value)
        {
            touchScreen->setWindowOpacity(value / 10.0);
            touchScreen->update();
        });

        connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,
            [](int deviceId, QGamepadManager::GamepadButton button)
        {
            (void)deviceId;

            gpConfigurator->setCurDeviceId(deviceId);

            if(gpConfigurator->isVisible())
            {
               gpConfigurator->getInput(deviceId, button);
                return;
            }

                buttons &= QGamepadManager::GamepadButtons(~(1 << button));
               // sendFrame();

        });

        connect(gpConfigurator->skipButton, &QPushButton::released, this,
                [](void)
        {
               gpConfigurator->next();
        });


        connect(gpConfigurator->resetConfigButton, &QPushButton::released, this,
                [](void)
        {
               QMessageBox *msgBox = new QMessageBox(0);
               QGamepadManager::instance()->resetConfiguration(gpConfigurator->getCurDeviceId());

               msgBox->setText("Reset");
               msgBox->setInformativeText("Please restart the program for changes to take affect.");
               msgBox->show();

        });

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
        touchScreen->setTouchScreenPressed(false);
        delete touchScreen;
        delete gpConfigurator;
    }
};

#endif // MAINWIDGET_H
