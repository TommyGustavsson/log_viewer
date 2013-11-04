#include "colored_frame.h"

Colored_frame::Colored_frame(QWidget *parent) :
    QFrame(parent)
{
}

void Colored_frame::contextMenuEvent ( QContextMenuEvent* event )
{
    QFrame::contextMenuEvent(event);
    emit context_menu_event(event);
}
