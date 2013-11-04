#ifndef APP_LOG_ITEM_H
#define APP_LOG_ITEM_H

#include <log_item.h>

namespace Log_viewer
{
    class App_log_item : public Log_item
    {
    public:
        App_log_item(const QString value,
                     const QString separator,
                     const QString origin);
    protected:
        Log_type Log_type_from_string(const QString value);
    };
}

#endif // APP_LOG_ITEM_H
