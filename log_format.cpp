#include <log_format.h>

namespace Log_viewer
{

    Log_format::Log_format(QObject *parent) : QObject(parent)
    {

    }

    void Log_format::add_line(const QString line, const QString origin)
    {
        if(m_start_stop_buffer.isNull())
        {
            m_start_stop_buffer =
                    QSharedPointer<Start_stop_buffer>(new Start_stop_buffer(get_start_seq(), get_stop_seq()));
            connect(m_start_stop_buffer.data(), SIGNAL(string_found(QString, QString)), this, SLOT(on_line_found(QString, QString)));
        }

        m_start_stop_buffer->add(line, origin);
    }

    void Log_format::on_line_found(const QString value, const QString origin)
    {
        emit log_found(create_log_item(value, origin));
    }

}
