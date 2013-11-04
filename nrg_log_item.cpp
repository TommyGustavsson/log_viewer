#include <nrg_log_item.h>
#include <QStringList>

namespace Log_viewer
{

    NRG_log_item::NRG_log_item(const QString value,
                               const QString separator,
                               const QString origin) : Log_item()
    {
        QStringList log_item_list = value.split(separator);

        m_date = log_item_list[0];
        m_type  = Log_type_from_string(log_item_list[1]);
        m_module = log_item_list[2];
        //?? = log_item_list[3];
        m_text = log_item_list[4];
        m_application = log_item_list[5];
        m_thread = log_item_list[6];
        m_file = log_item_list[7]; // extract row number
        m_origin = origin;
    }

    // ----------------------------------------------------------------------------

    Log_type NRG_log_item::Log_type_from_string(const QString value)
    {
        QString trimmed_value = value.trimmed();

        if(trimmed_value.isEmpty()) return lt_none;
        else if(trimmed_value == "INFO") return lt_info;
        else if(trimmed_value == "WARN") return lt_warning;
        else if(trimmed_value == "FATAL") return lt_fatal;
        else if(trimmed_value == "ERROR") return lt_error;
        else if(trimmed_value == "TRACE") return lt_trace;
        else if(trimmed_value == "DEBUG") return lt_debug;
        else if(trimmed_value == "OFF") return lt_always;
        else return lt_unknown;
    }
}
