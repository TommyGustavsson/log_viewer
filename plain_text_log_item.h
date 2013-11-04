#ifndef PLAIN_TEXT_LOG_ITEM_H
#define PLAIN_TEXT_LOG_ITEM_H

#include <log_item.h>

namespace Log_viewer
{
    class Plain_text_log_item : public Log_item
    {
    public:
        explicit Plain_text_log_item(const QString& value);
    protected:
        Log_type Log_type_from_string(const QString) { return lt_info; }
    };
}

#endif // PLAIN_TEXT_LOG_ITEM_H
