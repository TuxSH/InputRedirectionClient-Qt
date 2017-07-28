#include "global.h"
#include "gpmanager.h"
#include <QPainter>

#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

struct TouchScreen : public QDialog {
private:
    QLabel *bgLabel;

public:
    TouchScreen(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent* e);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void closeEvent(QCloseEvent *ev);

    QPoint getTouchScreenPosition();
    QSize getTouchScreenSize();
    bool isTouchScreenPressed();
    void setTouchScreenPressed(bool b);
    void updatePixmap(void);
    void clearImage(void);
};

#endif // TOUCHSCREEN_H
