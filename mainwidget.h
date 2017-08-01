#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "global.h"

#include <QMessageBox>
#include <QVariant>

#include "touchscreen.h"
#include "configwindow.h"
#include "gpconfigurator.h"


class Widget : public QWidget
{

private slots:
    void ShowContextMenu(const QPoint &pos);
private:
    QVBoxLayout  *layout;
    QFormLayout  *formLayout;
    QPushButton  *homeButton, *powerButton, *longPowerButton, *settingsConfigButton,
                 *clearImageButton, *configGamepadButton;
    QLineEdit    *addrLineEdit;
    QSlider      *touchOpacitySlider;
    ConfigWindow *settingsConfig;

public:
    TouchScreen *touchScreen;

    Widget(QWidget *parent = nullptr) : QWidget(parent)
    {
        layout = new QVBoxLayout(this);

        addrLineEdit = new QLineEdit(this);
        addrLineEdit->setClearButtonEnabled(true);

        formLayout = new QFormLayout();
        formLayout->addRow(tr("IP &address"), addrLineEdit);

        touchOpacitySlider = new QSlider(Qt::Horizontal);
        touchOpacitySlider->setRange(0, 10);
        touchOpacitySlider->setValue(10);
        touchOpacitySlider->setTickInterval(1);
        formLayout->addRow(tr("TS &Opacity"), touchOpacitySlider);

        homeButton = new QPushButton(tr("&Home"), this);
        powerButton = new QPushButton(tr("&Power"), this);
        longPowerButton = new QPushButton(tr("Power (&long)"), this);
        settingsConfigButton = new QPushButton(tr("&Settings"), this);
        clearImageButton = new QPushButton(tr("&Clear Image"), this);
        configGamepadButton = new QPushButton(tr("&Configure Custom Gamepad"));

        setContextMenuPolicy(Qt::CustomContextMenu);

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
        layout->addWidget(settingsConfigButton);
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
        });

        connect(longPowerButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 4;
        });

        connect(longPowerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~4;
        });

        connect(settingsConfigButton, &QPushButton::released, this,
                [this](void)
        {
            if (!settingsConfig->isVisible())
            {
                settingsConfig->move(this->x() - settingsConfig->width() - 5,this->y());
                settingsConfig->show();
            } else settingsConfig->hide();
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
        settingsConfig = new ConfigWindow(nullptr, touchScreen);
        this->setWindowTitle(tr("InputRedirectionClient-Qt"));

        addrLineEdit->setText(settings.value("ipAddress", "").toString());
    }


    void show(void)
    {
        QWidget::show();
        touchScreen->move(this->x() + this->width() + 5,this->y());
        touchScreen->show();
        settingsConfig->hide();
    }

    //When closing, save shortcuts
    void closeEvent(QCloseEvent *ev)
    {
        //Save shortcuts
        unsigned int i = 0;
        for (i=0; i<listShortcuts.size(); i++)
         {
            QString valName = tr("tsShortcut%1").arg(i);
             settings.setValue(valName, qVariantFromValue(listShortcuts[i]));
         }

        //Remove leftover settings
        for(; settings.contains(tr("tsShortcut%1").arg(i)); i++)
         {
             settings.remove(tr("tsShortcut%1").arg(i));
         }
        touchScreen->close();
        settingsConfig->close();
        ev->accept();
    }

    //Move touchscreen window with main window if moved
    void moveEvent(QMoveEvent *event)
    {
        touchScreen->move(touchScreen->pos() + (event->pos() - event->oldPos()));
    }

    //When main window is opened, load shortcut settings
    void showEvent(QShowEvent* event)
    {
        int id = qRegisterMetaType<ShortCut>("ShortCut");
        qRegisterMetaTypeStreamOperators<ShortCut>("ShortCut");

    QString valName = "tsShortcut0";
    for(int i = 0; settings.contains(valName); i++)
     {
        valName = tr("tsShortcut%1").arg(i);
        QVariant variant = settings.value(valName);

        ShortCut curShort = variant.value<ShortCut>();
         if(variant.isValid() && curShort.name != "")
         {
             if(curShort.name != "")
             {
              listShortcuts.push_back(curShort);
             }

         }
         else
         {
             settings.remove("tsShortcut"+i);
         }

     }
    }

    virtual ~Widget(void)
    {
        worker.setLeftAxis(0.0, 0.0);
        worker.setRightAxis(0.0, 0.0);

        buttons = 0;
        interfaceButtons = 0;
        touchScreen->setTouchScreenPressed(false);
        delete touchScreen;
        delete settingsConfig;
    }
};

#endif // MAINWIDGET_H
