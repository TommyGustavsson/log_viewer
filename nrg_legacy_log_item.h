#ifndef NRG_SC_LOG_ITEM_H
#define NRG_SC_LOG_ITEM_H

#include <log_item.h>

namespace Log_viewer
{

    class NRG_legacy_log_item : public Log_item
    {
    public:
        NRG_legacy_log_item(const QString& value,
                        const QString& separator,
                        const QString& origin);
    protected:
        Log_type Log_type_from_string(const QString value);
    };
}

#endif // NRG_SC_LOG_ITEM_H
