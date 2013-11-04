#include "plain_text_log_item.h"

namespace Log_viewer
{

    Plain_text_log_item::Plain_text_log_item(const QString& value)
    {
        m_text = value;
        m_type = lt_info;
    }

}
