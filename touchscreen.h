#include "global.h"

#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

struct TouchScreen : public QDialog {
private:
    QLabel *bgLabel;
   // QSize touchScreenSize;
  //  QPoint touchScreenPosition;

public:
    TouchScreen(QWidget *parent = nullptr) : QDialog(parent)
    {
        this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
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

        void resizeEvent(QResizeEvent* e);
        void mousePressEvent(QMouseEvent *ev);
        void mouseMoveEvent(QMouseEvent *ev);
        void mouseReleaseEvent(QMouseEvent *ev);
        void closeEvent(QCloseEvent *ev);

        QPoint getTouchScreenPosition();
        QSize getTouchScreenSize();
        bool isTouchScreenPressed();
        void setTouchScreenPressed(bool b);

};

#endif // TOUCHSCREEN_H
