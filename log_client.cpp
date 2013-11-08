#include <log_client.h>

#include <Log_format_factory.h>

namespace Log_viewer
{

    Log_client::Log_client(QObject* parent, QTcpSocket* socket):
            QObject(parent), m_socket_client(socket)
    {
        m_socket_type = stTCP;
        connect(socket,
                SIGNAL(disconnected()),
                this,
                SLOT(on_client_disconnected()));
        connect(socket,
                SIGNAL(readyRead()),
                this,
                SLOT(on_client_write()));
    }

    // ----------------------------------------------------------------------------

    Log_client::Log_client(QObject* parent, QUdpSocket* socket):
            QObject(parent), m_socket_client(socket)
    {
        m_socket_type = stUDP;
//        m_udp_socket = new QUdpSocket(this);
//        m_udp_socket->bind(port, QUdpSocket::ShareAddress);

        connect(socket, SIGNAL(readyRead()),
                this, SLOT(on_client_write()));
    }

    // ----------------------------------------------------------------------------

    void Log_client::on_client_disconnected()
    {
        m_log_format.clear();
        emit disconnected(this);
    }

    bool Log_client::get_log_format(const QString line)
    {
        m_log_format = Log_format_factory::instance->create(line, m_socket_client->peerAddress().toString());

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
        if (m_socket_type == stTCP)
        {
            char buf[1024];
            int read_cnt = 0;

            do
            {
                read_cnt = m_socket_client->readLine(buf, sizeof(buf));
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
        else if (m_socket_type == stUDP)
        {
            QUdpSocket *udp_socket = dynamic_cast<QUdpSocket*>(m_socket_client);
            while (udp_socket->hasPendingDatagrams())
            {
                 QByteArray datagram;
                 datagram.resize(udp_socket->pendingDatagramSize());
                 udp_socket->readDatagram(datagram.data(), datagram.size());

                 QString line = QString::fromLocal8Bit(datagram.data(), datagram.size());
                 if(m_log_format.isNull())
                 {
                     if(get_log_format(line))
                         m_log_format->add_line(line);
                     else
                         m_log_format.clear();
                 }
                 else
                     m_log_format->add_line(line);
             }
        }

    }

    // ----------------------------------------------------------------------------

    void Log_client::on_log_found(QSharedPointer<Log_item> item)
    {
        emit log_found(item);
    }

}
