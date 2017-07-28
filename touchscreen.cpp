#include "touchscreen.h"
#include "global.h"

void TouchScreen::setTouchScreenPressed(bool b)
{
    touchScreenPressed = b;
}

bool TouchScreen::isTouchScreenPressed()
{
   return touchScreenPressed;
}

QSize TouchScreen::getTouchScreenSize()
{
    return touchScreenSize;
}

QPoint TouchScreen::getTouchScreenPosition()
{
    return touchScreenPosition;
}

TouchScreen::TouchScreen(QWidget *parent) : QDialog(parent)
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
    this->setWindowTitle(tr("InputRedirectionClient-Qt - Touch screen"));

    bgLabel = new QLabel(this);

    updatePixmap();

    bgLabel->setFixedHeight(TOUCH_SCREEN_HEIGHT);
    bgLabel->setFixedWidth(TOUCH_SCREEN_WIDTH);
    bgLabel->setScaledContents(true);
    bgLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void TouchScreen::resizeEvent(QResizeEvent* e)
{
        QSize newWinSize = e->size();
        QSize curWinSize = e->oldSize();
        QSize propWinSize = e->size();

        if(curWinSize.height() != newWinSize.height())
        {
            propWinSize.setWidth((TOUCH_SCREEN_WIDTH*newWinSize.height())/TOUCH_SCREEN_HEIGHT);
           propWinSize.setHeight(newWinSize.height());
        }

        if(curWinSize.width() != newWinSize.width())
        {
            propWinSize.setWidth(newWinSize.width());
           propWinSize.setHeight((TOUCH_SCREEN_HEIGHT*newWinSize.width())/TOUCH_SCREEN_WIDTH);
        }

        touchScreenSize = propWinSize;
        this->resize(propWinSize);
        bgLabel->setFixedHeight(this->height());
        bgLabel->setFixedWidth(this->width());

        tsRatio = (double)this->width() / (double)TOUCH_SCREEN_WIDTH;
}

void TouchScreen::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        touchScreenPressed = true;
        touchScreenPosition = ev->pos();
       // sendFrame();
    }
    if(ev->button() == Qt::RightButton)
    {

       QString strPic = QFileDialog::getOpenFileName(this,
                      tr("Open Touchscreen Image (320x240)"), "MyDocuments",
                      tr("Image Files (*.jpg *.jpeg *.png *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)"));

        if(!strPic.isNull())
        {
            settings.setValue("tsBackgroundImage", strPic);
            updatePixmap();
        }
    }
}

void TouchScreen::mouseMoveEvent(QMouseEvent *ev)
{
    if(touchScreenPressed && (ev->buttons() & Qt::LeftButton))
    {
        touchScreenPosition = ev->pos();
        //sendFrame();
    }
}

void TouchScreen::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        touchScreenPressed = false;
        //sendFrame();
    }
}

void TouchScreen::closeEvent(QCloseEvent *ev)
{
    touchScreenPressed = false;
   // sendFrame();
    ev->accept();
}

void TouchScreen::updatePixmap(void)
{
    QString strPic = settings.value("tsBackgroundImage", "").toString();//qApp->QCoreApplication::applicationDirPath()+"/Touchscreen.jpg";
    QPixmap newPic(strPic);

    if (newPic.isNull())
    {
        newPic = QPixmap(TOUCH_SCREEN_WIDTH, TOUCH_SCREEN_HEIGHT);
        newPic.fill(Qt::transparent);
    }

    QPainter painter(&newPic);
    QPen pen;
    pen.setWidth(2);

    if (touchButton1 != QGamepadManager::ButtonInvalid)
    {
        pen.setColor(Qt::red);
        painter.setPen(pen);

        painter.drawEllipse(QPoint(touchButton1X, touchButton1Y), 3, 3);
    }
    if (touchButton2 != QGamepadManager::ButtonInvalid)
    {
        pen.setColor(Qt::blue);
        painter.setPen(pen);

        painter.drawEllipse(QPoint(touchButton2X, touchButton2Y), 3, 3);
    }
    if (touchButton3 != QGamepadManager::ButtonInvalid)
    {
        pen.setColor(Qt::green);
        painter.setPen(pen);

        painter.drawEllipse(QPoint(touchButton3X, touchButton3Y), 3, 3);
    }
    if (touchButton4 != QGamepadManager::ButtonInvalid)
    {
        pen.setColor(Qt::yellow);
        painter.setPen(pen);

        painter.drawEllipse(QPoint(touchButton4X, touchButton4Y), 3, 3);
    }

    bgLabel->setPixmap(newPic);
}

void TouchScreen::clearImage(void)
{
    settings.setValue("tsBackgroundImage", "");
    updatePixmap();
}
