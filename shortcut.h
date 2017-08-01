#ifndef SHORTCUT_H
#define SHORTCUT_H
#include <QMetaType>
#include <QDataStream>
#include <QColor>
#include <QGamepadManager>
#include <QPoint>
struct ShortCut

{
    QString name;
    QGamepadManager::GamepadButton button;
    QPoint pos;
    QColor color;

};Q_DECLARE_METATYPE(ShortCut)

extern QDataStream &operator<<(QDataStream &out, const ShortCut &obj);
extern QDataStream &operator>>(QDataStream &in, ShortCut &obj);


#endif // SHORTCUT_H
