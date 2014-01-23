#include <g3_log_item.h>
#include <QStringList>

namespace Log_viewer
{

    // ----------------------------------------------------------------------------

    G3_log_item::G3_log_item(const QString value,
                             const QString separator,
                             const QString origin) : Log_item()
    {
        QStringList log_item_list = value.split(separator);

        m_date = log_item_list[1];
        m_time = log_item_list[2];
        m_application = log_item_list[3];
        m_module = log_item_list[4];
        m_file = log_item_list[5];
        // 6 = empty
        m_thread = log_item_list[7];
        // 8 = empty
        // 9 = empty
        QString tmp = log_item_list[10];
        tmp.remove(0, 1);
        m_line =tmp.trimmed(); // in G3 this has a new line char at the end
        m_type = Log_type_from_string(log_item_list[11]);
        for(int i = 12; i < log_item_list.count() - 1; i++)
            m_text.append(log_item_list[i]);
        m_origin = log_item_list[0]; //origin;
    }

    // ----------------------------------------------------------------------------

    Log_type G3_log_item::Log_type_from_string(const QString value)
    {
        if((value == "INFO") || (value == "I")) return lt_info;
        else if((value == "WARN") || (value == "W")) return lt_warning;
        else if((value == "ERROR") || (value == "E")) return lt_error;
        else if((value == "TRACE") || (value == "T")) return lt_trace;
        else if((value == "DEBUG") || (value == "D")) return lt_debug;
        else return lt_unknown;
    }
}
