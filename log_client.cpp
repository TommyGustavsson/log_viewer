#include <log_client.h>

#include <Log_format_factory.h>

namespace Log_viewer
{

    Log_client::Log_client(QObject* parent, QTcpSocket* socket):
            QObject(parent), m_tcp_client(socket)
    {
        connect(m_tcp_client,
                SIGNAL(disconnected()),
                this,
                SLOT(on_client_disconnected()));
        connect(m_tcp_client,
                SIGNAL(readyRead()),
                this,
                SLOT(on_client_write()));
    }

    // ----------------------------------------------------------------------------

    void Log_client::on_client_disconnected()
    {
        m_log_format.clear();
        emit disconnected(this);
    }

    bool Log_client::get_log_format(const QString line)
    {
        m_log_format = Log_format_factory::instance->create(line, m_tcp_client->peerAddress().toString());

        connect(m_log_format.data(),
                SIGNAL(log_found(QSharedPointer<Log_item>)),
                this,
                SLOT(on_log_found(QSharedPointer<Log_item>)));
        emit format_selected(m_log_format);

        return true;
    }

    // ----------------------------------------------------------------------------

    void Log_client::on_client_write()
    {
        char buf[1024];
        int read_cnt = 0;

        do
        {
            read_cnt = m_tcp_client->readLine(buf, sizeof(buf));
            QString line = QString::fromLocal8Bit(buf, read_cnt);
            if(m_log_format.isNull())
            {
                if(get_log_format(line))
                    m_log_format->add_line(line);
                else
                    m_log_format.clear();
            }
            else
                m_log_format->add_line(line);

        } while (read_cnt > 0);

    }

    // ----------------------------------------------------------------------------

    void Log_client::on_log_found(QSharedPointer<Log_item> item)
    {
        emit log_found(item);
    }
}
