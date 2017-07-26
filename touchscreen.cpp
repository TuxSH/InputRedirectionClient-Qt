#include "touchscreen.h"

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
}

void TouchScreen::mousePressEvent(QMouseEvent *ev)
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
                      tr("Open Touchscreen Image (320x240)"), lastOverlayPath,
                      tr("Image Files (*.jpg *.jpeg *.png *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)"));

        if(!strPic.isNull())
        {
            lastOverlayPath = strPic;
           QPixmap newPic(strPic);
           bgLabel->setPixmap(newPic);
        }
    }
}

void TouchScreen::mouseMoveEvent(QMouseEvent *ev)
{
    if(touchScreenPressed && (ev->buttons() & Qt::LeftButton))
    {
        touchScreenPosition = ev->pos();
        sendFrame();
    }
}

void TouchScreen::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        touchScreenPressed = false;
        sendFrame();
    }
}

void TouchScreen::closeEvent(QCloseEvent *ev)
{
    touchScreenPressed = false;
    sendFrame();
    ev->accept();
}
