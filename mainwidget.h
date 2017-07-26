#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "touchscreen.h"
#include "global.h"

QSettings settings("TuxSH", "InputRedirectionClient-Qt");

class Widget : public QWidget
{
private:

    QVBoxLayout *layout;
    QFormLayout *formLayout;
    QLineEdit *addrLineEdit;
    QCheckBox *invertYCheckbox, *invertYCppCheckbox, *swapSticksCheckbox;
    QPushButton *homeButton, *powerButton, *longPowerButton;
    QSlider *touchOpacitySlider;


public:
    TouchScreen *touchScreen;

    Widget(QWidget *parent = nullptr) : QWidget(parent)
    {
        layout = new QVBoxLayout(this);

        addrLineEdit = new QLineEdit(this);
        addrLineEdit->setClearButtonEnabled(true);

        invertYCheckbox = new QCheckBox(this);
        invertYCppCheckbox = new QCheckBox(this);
        swapSticksCheckbox = new QCheckBox(this);
        formLayout = new QFormLayout;

        formLayout->addRow(tr("IP &address"), addrLineEdit);
        formLayout->addRow(tr("&Invert Y axis"), invertYCheckbox);
        formLayout->addRow(tr("&Invert Cpp Y axis"), invertYCppCheckbox);
        formLayout->addRow(tr("&Swap Analog Sticks"), swapSticksCheckbox);

        touchOpacitySlider = new QSlider(Qt::Horizontal);
        touchOpacitySlider->setRange(1, 10);
        touchOpacitySlider->setValue(10);
        touchOpacitySlider->setTickInterval(1);
        formLayout->addRow(tr("TS &Opacity"), touchOpacitySlider);

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

        connect(touchOpacitySlider, &QSlider::valueChanged, this,
                [this](int value)
        {
            touchScreen->setWindowOpacity(value / 10.0);
            touchScreen->update();
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
        touchScreen->setTouchScreenPressed(false);
        sendFrame();
        delete touchScreen;
    }


};

#endif // MAINWIDGET_H
