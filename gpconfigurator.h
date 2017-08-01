#ifndef GPCONFIGURATOR_H
#define GPCONFIGURATOR_H

#include "global.h"

struct GamepadConfigurator : public QWidget {

private:
    QLabel* lblCurButton;
    QFormLayout *formLayout;
    QVBoxLayout* layout;
    int devId;
    int count;

public:
   QPushButton* skipButton, *resetConfigButton;

    GamepadConfigurator(QWidget *parent = 0) : QWidget(parent)
    {
        this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
        this->setWindowTitle(tr("Gamepad Configurator"));


        layout = new QVBoxLayout(this);
        formLayout = new QFormLayout(this);
        lblCurButton = new QLabel(this);
        skipButton = new QPushButton(tr("&Skip"), this);
        resetConfigButton = new QPushButton(tr("&Reset Configuration"), this);

       count = 0;

       resetConfigButton->setEnabled(false);
       skipButton->setEnabled(false);

       formLayout->addRow(lblCurButton);
       lblCurButton->setText(QString("Press any button on the controller to continue..."));
       layout->addLayout(formLayout);
       layout->addWidget(skipButton);
       layout->addWidget(resetConfigButton);

       setMinimumSize(300, 75);

    }

    void setCurDeviceId(int id)
    {
        devId = id;
    }

    int getCurDeviceId()
    {
        return devId;
    }

    int getCount()
    {
       return count;
    }

    void next()
    {

        getInput(devId, showDirections(QGamepadManager::ButtonInvalid));
        repaint();
    }

    QGamepadManager::GamepadButton showDirections(QGamepadManager::GamepadButton btn)
    {
        QGamepadManager::GamepadButton btnToMap =  QGamepadManager::ButtonInvalid;
        btnToMap = btn;


         switch(count)
         {
            case 0:
             lblCurButton->setText("Press Any Button To Continue...");
             btnToMap = QGamepadManager::ButtonA;

             return btnToMap;
             break;

            case 1:
               resetConfigButton->setEnabled(true);
               skipButton->setEnabled(true);

               lblCurButton->setText("BUTTON A");
               btnToMap = QGamepadManager::ButtonB;
             break;

            case 2:
               lblCurButton->setText("BUTTON B");
               btnToMap = QGamepadManager::ButtonX;
             break;

            case 3:
               lblCurButton->setText("BUTTON X");
               btnToMap = QGamepadManager::ButtonY;
             break;

            case 4:
               lblCurButton->setText("BUTTON Y");
               btnToMap = QGamepadManager::ButtonR1;
             break;

            case 5:
               lblCurButton->setText("R1");
               btnToMap = QGamepadManager::ButtonR2;
              break;

            case 6:
               lblCurButton->setText("R2");
               btnToMap = QGamepadManager::ButtonR3;
             break;

            case 7:
              lblCurButton->setText("R3");
              btnToMap = QGamepadManager::ButtonL1;
             break;

         case 8:
            lblCurButton->setText("L1");
            btnToMap = QGamepadManager::ButtonL2;
          break;

         case 9:
            lblCurButton->setText("L2");
            btnToMap = QGamepadManager::ButtonL3;
          break;

         case 10:
            lblCurButton->setText("L3");
            btnToMap = QGamepadManager::ButtonUp;
          break;

         case 11:
            lblCurButton->setText("D Up");
            btnToMap = QGamepadManager::ButtonDown;
          break;

         case 12:
            lblCurButton->setText("D Down");
            btnToMap = QGamepadManager::ButtonLeft;
          break;

         case 13:
            lblCurButton->setText("D Left");
            btnToMap = QGamepadManager::ButtonRight;
           break;

         case 14:
            lblCurButton->setText("D Right");
            btnToMap = QGamepadManager::ButtonSelect;
           break;

          case 15:
             lblCurButton->setText("SELECT");
             btnToMap = QGamepadManager::ButtonStart;
             break;

          case 16:
             lblCurButton->setText("Start");
             btnToMap = QGamepadManager::ButtonGuide;
             break;


           case 17:
             lblCurButton->setText("Guide Button");
             btnToMap = QGamepadManager::ButtonGuide;
             break;

         case 18:
             lblCurButton->setText("Done!!");
             break;

           default:
             resetConfigButton->setEnabled(false);
             skipButton->setEnabled(false);

             this->close();
             count = 0;
             break;
         }

         this->repaint();

         return btnToMap;
    }

    void getInput(int deviceId, QGamepadManager::GamepadButton btn)
    {
        QGamepadManager::GamepadButton btnToMap = QGamepadManager::ButtonInvalid;
        btnToMap = showDirections(btn);

        devId = deviceId;

        while(!QGamepadManager::instance()->configureButton(devId, btnToMap));

       count++;

       btnToMap = showDirections(btn);
    }

    void showGui()
    {
       if(!this->isVisible())
       {
          this->setVisible(true);
          showDirections(QGamepadManager::ButtonInvalid);
       }
    }

    virtual ~GamepadConfigurator(void)
   {

   }
};

#endif // GPCONFIGURATOR_H
