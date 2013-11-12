#ifndef LOG_CLIENT_H
#define LOG_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSharedPointer>
#include <QHostAddress>
#include <log_format.h>

namespace Log_viewer
{

    class Log_client : public QObject
    {
        Q_OBJECT

    public:
        Log_client(QObject* parent, QTcpSocket* socket);
        Log_client(QObject* parent, QUdpSocket* socket);

        QString get_address() const {
            if (m_socket_type == stTCP)
            {
                return m_socket_client->peerAddress().toString();
            }
            else
            {
                return m_udp_id;
            }
        }

    private slots:
        void on_client_write();
        void on_client_disconnected();
        void on_log_found(QSharedPointer<Log_item> item);

    signals:
        void disconnected(const Log_client* client);
        void log_found(QSharedPointer<Log_item> log_item);
        void format_selected(QSharedPointer<Log_format> format);

    private:
        // hide copy constructor
        Log_client(const Log_client& client);
        bool get_log_format(const QString line);

        QAbstractSocket *m_socket_client;
        QSharedPointer<Log_format> m_log_format;

        enum Socket_type {stUDP, stTCP} m_socket_type;
        QString m_udp_id;
    };
}

#endif // LOG_CLIENT_H
