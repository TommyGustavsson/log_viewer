#ifndef NRG_LOG_ITEM_H
#define NRG_LOG_ITEM_H

#include <log_item.h>

namespace Log_viewer
{

    class NRG_log_item : public Log_item
    {
    public:
        NRG_log_item(const QString value,
                     const QString separator,
                     const QString origin);
    protected:
        Log_type Log_type_from_string(const QString value);
    };
}

#endif // NRG_LOG_ITEM_H
