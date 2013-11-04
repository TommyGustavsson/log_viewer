#ifndef COLOR_THEME_H
#define COLOR_THEME_H

#include <QColor>

struct Color_theme
{
    Color_theme(const QColor array[5]);
    QColor c[5];
    QString name;
};

#endif // COLOR_THEME_H
