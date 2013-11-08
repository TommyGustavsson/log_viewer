#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSharedPointer>
#include <log_file_parser.h>
#include <QStringList>


namespace Log_viewer
{
    class Log_items_model;
    class Log_filter_proxy_model;
    class Clients_model;
    class File_neighbor_model;
    class Log_client;
    class Tail;
    class Log_format;
    class ftp_files_model;

    enum Socket_type {stTCP, stUDP, stNone};

    class Log_manager : public QObject
    {
        Q_OBJECT

    public:
        Log_manager();
        virtual ~Log_manager();

        static Log_manager *instance;

        bool listen(int port, Socket_type socket_type);
        void close();
        void open_log_files(const QStringList& files);
        void open_log(const QString& file);
        void open_log_from_clipboard();
        void open_ftp(const QString &host, int port, const QString &userName, const QString &password);
        void apply_filter(const QString& text, const QString& file, const QString& module, const QString& index);
        void clear_filter();
        void tail_current_file();
        void clear_tail();

        void clear_log_items();

        Log_items_model* log_items_model() const {
            return m_log_items_model;
        }

        ftp_files_model* ftp_model() const {
            return m_ftp_files_model;
        }

        Clients_model* clients_model() const {
            return m_clients_model;
        }

        Log_filter_proxy_model* log_filter_proxy_model() const {
            return m_log_filter_proxy_model;
        }

        File_neighbor_model* file_neighbor_model() const {
            return m_file_neighbor_model;
        }

        int get_port() {
            return m_tcp_server->serverPort();
        }

        QString get_error() {
            return m_tcp_server->errorString();
        }

        QString get_current_file() {
            return m_current_file;
        }

    signals:
        void client_connected(const QString& client_address);
        void client_disconnected(const QString& client_address);
        void server_listening(bool listening);
        void file_opened(const QString& file);
        void error(const QString& text);
        void tail_cleared();
        void log_items_empty();
        void log_items_not_empty();
        void log_format_selected(const QString& format);
        void log_proxy_filter_cleared();
        void ftp_file_downloaded(const QString& file_name);

    private:
        void signal_log_not_empty();
        void setup_default_columns();
        void create_udp_socket(int port);

        QTcpServer *m_tcp_server;
        QUdpSocket *m_udp_socket;

        Log_items_model* m_log_items_model;
        Log_filter_proxy_model* m_log_filter_proxy_model;
        Clients_model* m_clients_model;
        File_neighbor_model* m_file_neighbor_model;
        Log_file_parser m_file_parser;
        QSharedPointer<Tail> m_tail;
        QString m_current_file;
        qint64 m_current_file_size;
        ftp_files_model* m_ftp_files_model;
        QString m_current_log_format_description;

        bool log_items_empty_signaled;
        int m_process_event_counter;
        Socket_type m_socket_type;

    private slots:
        void on_client_connected();
        void on_log_found(QSharedPointer<Log_item>);
        void on_logs_found(const Log_file_parser::Log_items& items);
        void on_client_disconnected(const Log_client* client);
        void on_file_opened(const QString& file, qint64 size);
        void on_file_parser_error(const QString& text);
        void on_tail_log_found(QSharedPointer<Log_item> item);
        void on_tail_empty();
        void on_tail_failed(const QString& message);
        void on_log_format_selected(QSharedPointer<Log_format> format);
        void on_file_neighbor_model_populated();
        void on_filter_cleared();
        void on_ftp_file_downloaded(const QString file_name);
    };
}

#endif // LOG_MANAGER_H
