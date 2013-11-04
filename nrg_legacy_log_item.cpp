#include <nrg_legacy_log_item.h>
#include <QStringList>

namespace Log_viewer
{

    // ----------------------------------------------------------------------------

    NRG_legacy_log_item::NRG_legacy_log_item(const QString& value,
                                     const QString& separator,
                                     const QString& origin) : Log_item()
    {
        try
        {
            QStringList log_item_list = value.split(separator);

            // Do we have an incomplete line?
            if(log_item_list.count() < 7)
            {
                m_date = "";
                m_type  = lt_none;
                m_module = "";
                m_text = "";
                m_method = "";
                m_file = "";
                m_application = "";
                m_thread = "";
                m_origin = "";
                return;
            }

            m_date = log_item_list[0];
            m_type  = Log_type_from_string(log_item_list[1]);
            m_module = log_item_list[2];
            m_text = log_item_list[3];
            QString temp_file = log_item_list[4];
            temp_file.replace("::", "doublecolon");
            temp_file.replace(":", "|");
            temp_file.replace("doublecolon", "::");
            temp_file.replace(',', "|");
            QStringList temp_file_list = temp_file.split("|");
            if(temp_file_list.size() >= 3)
            {
                m_file = temp_file_list[0];
                m_line = temp_file_list[1];
                for(int i = 2; i < temp_file_list.size(); i++)
                {
                    m_method += temp_file_list[i];
                }
            }
            else
            {
                m_file = log_item_list[4];
            }
            m_application = log_item_list[5];
            m_thread = log_item_list[6];
            m_origin = origin;
        }
        catch(...)
        {

        }
    }

    // ----------------------------------------------------------------------------

    Log_type NRG_legacy_log_item::Log_type_from_string(const QString value)
    {
        QString type = value.trimmed();
        if(type == "INFO") return lt_info;
        else if(type == "WARN") return lt_warning;
        else if(type == "FATAL") return lt_fatal;
        else if(type == "ERROR") return lt_error;
        else if(type == "TRACE") return lt_trace;
        else if(type == "DEBUG") return lt_debug;
        else if(type == "OFF") return lt_always;
        else return lt_unknown;
    }

}
