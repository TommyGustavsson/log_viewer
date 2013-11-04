#include <app_log_item.h>

namespace Log_viewer
{

    App_log_item::App_log_item(const QString value,
                               const QString separator,
                               const QString origin) : Log_item()
    {
        Q_UNUSED(separator);
        Q_UNUSED(origin);

        m_type = lt_application;
        m_text = value;
    }

    // ----------------------------------------------------------------------------

    Log_type App_log_item::Log_type_from_string(const QString value)
    {
        Q_UNUSED(value);

        return lt_unknown;
    }
}
