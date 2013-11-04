#ifndef COLORED_FRAME_H
#define COLORED_FRAME_H

#include <QFrame>

class Colored_frame : public QFrame
{
    Q_OBJECT
public:
    explicit Colored_frame(QWidget *parent = 0);

protected:
    void contextMenuEvent ( QContextMenuEvent* event );
signals:
    void context_menu_event(QContextMenuEvent* event);

public slots:

};

#endif // COLORED_FRAME_H
