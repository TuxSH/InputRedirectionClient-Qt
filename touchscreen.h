#include "global.h"
#include "gpmanager.h"
#include "tsshortcut.h"
#include <QPainter>

#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "tsshortcut.h"

struct TouchScreen : public QWidget {
    Q_OBJECT
public slots:
    void ShowContextMenu(const QPoint& pos);

private:
    QLabel *bgLabel;
    TsShortcut tsShortcutGui;

public:
    TouchScreen(QWidget *parent = nullptr);

    bool ellipNeedDraw;

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
    void paintEvent(QPaintEvent* e);

   ~TouchScreen(void)
    {
        qDebug() << "DECON";
        tsShortcutGui.close();
    }
};

#endif // TOUCHSCREEN_H
