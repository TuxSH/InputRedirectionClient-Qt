#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "global.h"

#include <QMessageBox>

#include "touchscreen.h"
#include "configwindow.h"
#include "gpconfigurator.h"


class Widget : public QWidget
{
private:
    QVBoxLayout  *layout;
    QFormLayout  *formLayout;
    QPushButton  *homeButton, *powerButton, *longPowerButton, *remapConfigButton,
                 *clearImageButton, *configGamepadButton;
    QLineEdit    *addrLineEdit;
    QSlider      *touchOpacitySlider;
    ConfigWindow *remapConfig;

public:
    TouchScreen *touchScreen;

    Widget(QWidget *parent = nullptr) : QWidget(parent)
    {
        layout = new QVBoxLayout(this);

        addrLineEdit = new QLineEdit(this);
        addrLineEdit->setClearButtonEnabled(true);

        formLayout = new QFormLayout;
        formLayout->addRow(tr("IP &address"), addrLineEdit);

        touchOpacitySlider = new QSlider(Qt::Horizontal);
        touchOpacitySlider->setRange(1, 10);
        touchOpacitySlider->setValue(10);
        touchOpacitySlider->setTickInterval(1);
        formLayout->addRow(tr("TS &Opacity"), touchOpacitySlider);

        homeButton = new QPushButton(tr("&Home"), this);
        powerButton = new QPushButton(tr("&Power"), this);
        longPowerButton = new QPushButton(tr("Power (&long)"), this);
        remapConfigButton = new QPushButton(tr("&Settings"), this);
        clearImageButton = new QPushButton(tr("&Clear Image"), this);
        configGamepadButton = new QPushButton(tr("&Configure Custom Gamepad"));

        // Disable/hide the configurator button if running windows since it's not supported
         if (QSysInfo::productType() == "windows")
         {
             configGamepadButton->setEnabled(false);
             configGamepadButton->setVisible(false);
         }

        layout->addLayout(formLayout);
        layout->addWidget(homeButton);
        layout->addWidget(powerButton);
        layout->addWidget(longPowerButton);
        layout->addWidget(configGamepadButton);
        layout->addWidget(remapConfigButton);
        layout->addWidget(clearImageButton);

        gpConfigurator = new GamepadConfigurator();

        connect(addrLineEdit, &QLineEdit::textChanged, this,
                [](const QString &text)
        {
            ipAddress = text;
            settings.setValue("ipAddress", text);
        });

        connect(configGamepadButton, &QPushButton::released, this,
                [](void)
        {
           gpConfigurator->showGui();
         });

        connect(homeButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 1;
        });

        connect(homeButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~1;
        });

        connect(powerButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 2;
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

        connect(powerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~2;
           //sendFrame();
        });

        connect(longPowerButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 4;
           //sendFrame();
        });

        connect(longPowerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~4;
           //sendFrame();
        });

        connect(remapConfigButton, &QPushButton::released, this,
                [this](void)
        {
            if (!remapConfig->isVisible())
            {
                remapConfig->move(this->x() - remapConfig->width() - 5,this->y());
                remapConfig->show();
            } else remapConfig->hide();
        });

        connect(clearImageButton, &QPushButton::released, this,
                [this](void)
        {
           touchScreen->clearImage();
        });

        connect(touchOpacitySlider, &QSlider::valueChanged, this,
                [this](int value)
        {
            touchScreen->setWindowOpacity(value / 10.0);
            touchScreen->update();
        });


        touchScreen = new TouchScreen(nullptr);
        remapConfig = new ConfigWindow(nullptr, touchScreen);
        this->setWindowTitle(tr("InputRedirectionClient-Qt"));

        addrLineEdit->setText(settings.value("ipAddress", "").toString());
    }

    void show(void)
    {
        QWidget::show();
        touchScreen->move(this->x() + this->width() + 5,this->y());
        touchScreen->show();
        remapConfig->hide();
    }

    void closeEvent(QCloseEvent *ev)
    {
        touchScreen->close();
        remapConfig->close();
        ev->accept();
    }

    void moveEvent(QMoveEvent *event)
    {
        touchScreen->move(touchScreen->pos() + (event->pos() - event->oldPos()));
    }

    virtual ~Widget(void)
    {
        worker.setLeftAxis(0.0, 0.0);
        worker.setRightAxis(0.0, 0.0);

        buttons = 0;
        interfaceButtons = 0;
        touchScreen->setTouchScreenPressed(false);
        //sendFrame();
        delete touchScreen;
        delete remapConfig;
    }
};

#endif // MAINWIDGET_H
