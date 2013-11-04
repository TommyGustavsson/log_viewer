#include <log_item.h>

#include <QStringList>

namespace Log_viewer
{

    // ----------------------------------------------------------------------------

    Log_item::Log_item()
    {

    }

    // ----------------------------------------------------------------------------

    QString Log_item::get_as_string(const QString& separator) const
    {
        QStringList list;
        list.push_back(QString("%0").arg(m_index));
        list.push_back(get_log_type_as_string());
        list.push_back(m_text);
        list.push_back(m_date);
        list.push_back(m_time);
        list.push_back(m_line);
        list.push_back(m_application);
        list.push_back(m_module);
        list.push_back(m_thread);
        list.push_back(m_origin);

        return list.join(separator);
    }

    // ----------------------------------------------------------------------------

    QString Log_item::get_log_type_as_string() const
    {
        return Log_item::get_log_type_as_string(m_type);
    }

    // ----------------------------------------------------------------------------

    QString Log_item::get_log_type_as_string(Log_type type)
    {
        switch(type)
        {
            case lt_info : return "Info";
            case lt_warning : return "Warning";
            case lt_error : return "Error";
            case lt_fatal : return "Fatal";
            case lt_trace : return "Trace";
            case lt_debug : return "Debug";
            case lt_always : return "Always";
            case lt_none : return "";
            case lt_application : return "";
            default : return "Unknown";
        }
    }
}

