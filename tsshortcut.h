#ifndef TSSHORTCUT_H
#define TSSHORTCUT_H
#include "global.h"
#include <QListView>
#include <QList>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QComboBox>
#include <QColorDialog>

class TsShortcut : public QWidget
{
private:
    QString wTitle;
    QPushButton  *acceptButton;
    QVBoxLayout  *layout;
    QFormLayout  *formLayout;
    QListWidget    *lstWidget;
    QGamepadManager::GamepadButton btnToMap;
    QLabel       *lblDirections;
    QPushButton  *btnColorDialog, *btnCreateShort,
                 *btnDelShort, *btnHelp, *btnPressNow;
    QLineEdit* txtShortName;
    QPoint curPos;
    QComboBox* cboxBtns;
    QColor curColor;


public:
    TsShortcut(QWidget *parent = nullptr) : QWidget(parent)
    {
        layout = new QVBoxLayout(this);
        formLayout= new QFormLayout(this);

        lblDirections = new QLabel(this);
        lstWidget = new QListWidget(this);
        btnColorDialog = new QPushButton(this);
        btnCreateShort = new QPushButton(this);
        btnPressNow = new QPushButton(this);
        btnDelShort = new QPushButton(this);
        btnHelp = new QPushButton(this);
        txtShortName = new QLineEdit(this);
        cboxBtns = populateItems();

        lblDirections->setText("Select a shortcut, then press a button to map it.");
        btnColorDialog->setText("Choose &Color");
        btnPressNow->setText("Press Selected &Shortcut");
        btnCreateShort->setText("&Create");
        btnDelShort->setText("&Delete");
        btnHelp->setText("&Help");

        layout->addWidget(lblDirections);
        layout->addWidget(lstWidget);
        layout->addWidget(txtShortName);
        layout->addWidget(cboxBtns);

        layout->addWidget(btnColorDialog);
        layout->addWidget(btnCreateShort);
        layout->addSpacing(10);
        layout->addWidget(btnPressNow);
        layout->addWidget(btnDelShort);
        layout->addWidget(btnHelp);

        this->setMinimumSize(300, 200);
        this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
        this->setVisible(false);

        connect(btnColorDialog, &QPushButton::released, this,
                 [this](void)
        {
            QColor color = QColorDialog::getColor(Qt::yellow, this );
                if( color.isValid() )
                {
                    curColor = color;
                    QString qss = QString("background-color: %1").arg(color.name());
                    btnColorDialog->setStyleSheet(qss);

                }
         });

        connect(btnCreateShort, &QPushButton::released, this,
                 [this](void)
        {
            ShortCut newShortCut;
            QString newShortName = txtShortName->text();

            if(newShortName != "")
            {
                newShortCut.name = newShortName;

                newShortCut.button =variantToButton(cboxBtns->currentData());
                newShortCut.pos = curPos;
                newShortCut.color = curColor;

                if(!curColor.isValid())
                {
                    newShortCut.color = Qt::blue;
                }

                lstWidget->addItem(newShortName);
                listShortcuts.push_back(newShortCut);
                txtShortName->clear();
                btnColorDialog->setStyleSheet("");
                curColor = nullptr;
             }
            else
            {
                QMessageBox *msgBox = new QMessageBox(0);
                msgBox->setInformativeText(tr("Cannot create new shortcut without a name."));
                msgBox->show();
            }
         });

        connect(btnDelShort, &QPushButton::released, this,
                 [this](void)
        {
            if(lstWidget->selectedItems().size() != 0)
            {
              listShortcuts.erase(listShortcuts.begin()+(lstWidget->currentRow()));
              qDeleteAll(lstWidget->selectedItems());
            }
         });

        connect(btnPressNow, &QPushButton::pressed, this,
                 [this](void)
        {
            if(lstWidget->selectedItems().size() != 0)
            {
               touchScreenPressed = true;
               touchScreenPosition = listShortcuts[lstWidget->currentRow()].pos*tsRatio;
            }

         });

         connect(btnPressNow, &QPushButton::released, this,
                  [this](void)
         {
                touchScreenPressed = false;
          });


        connect(btnHelp, &QPushButton::released, this,
                 [this](void)
        {
            QMessageBox *msgBox = new QMessageBox(0);

            msgBox->setText("Map Touchpad to Button");
            msgBox->setInformativeText(tr("1. Right-click touchpad in the position you want then open this menu.\n\
                                           2. Type a name for your shortcut in the textbox.\n\
                                           3. Choose a button on the gamepad to map this point to.\n\
                                           4. Choose a color for your shortcut, (this will the circle's color\
                                              on the touchpad window\
                                           5. Press create, then close this window"));
            msgBox->show();

         });


    }

    void setCurrentPos(QPoint pos)
    {
        curPos = pos;
    }

    void updateTitleText()
    {
        wTitle = QString("Current X: %1 Y: %2").arg(QString::number(curPos.x())).arg(QString::number(curPos.y()));
        this->setWindowTitle(wTitle);
    }

    void showEvent(QShowEvent * event)
    {
        lstWidget->clear();
        for (unsigned int i=0; i<listShortcuts.size(); i++)
        {
            QString curName = listShortcuts[i].name;
            lstWidget->addItem(curName);
        }

        updateTitleText();
        event->accept();
    }


    QComboBox* populateItems()
    {
        QComboBox *comboBox = new QComboBox();
        comboBox->addItem("None", QGamepadManager::ButtonInvalid);
        comboBox->addItem("A", QGamepadManager::ButtonA);
        comboBox->addItem("B", QGamepadManager::ButtonB);
        comboBox->addItem("X", QGamepadManager::ButtonX);
        comboBox->addItem("Y", QGamepadManager::ButtonY);
        comboBox->addItem("Up", QGamepadManager::ButtonUp);
        comboBox->addItem("Down", QGamepadManager::ButtonDown);
        comboBox->addItem("Right", QGamepadManager::ButtonRight);
        comboBox->addItem("Left", QGamepadManager::ButtonLeft);
        comboBox->addItem("LB", QGamepadManager::ButtonL1);
        comboBox->addItem("RB", QGamepadManager::ButtonR1);
        comboBox->addItem("LT", QGamepadManager::ButtonL2);
        comboBox->addItem("RT", QGamepadManager::ButtonR2);
        comboBox->addItem("Start", QGamepadManager::ButtonStart);
        comboBox->addItem("Back", QGamepadManager::ButtonSelect);
        comboBox->addItem("L3", QGamepadManager::ButtonL3);
        comboBox->addItem("R3", QGamepadManager::ButtonR3);
        comboBox->addItem("Guide", QGamepadManager::ButtonGuide);

        return comboBox;
    }

};

#endif // TSSHORTCUT_H
