#include <log_manager.h>

#include <singleton.h>

#include <g3_log_format.h>
#include <nrg_log_format.h>
#include <log_items_model.h>
#include <clients_model.h>
#include <log_filter_proxy_model.h>
#include <file_neighbor_model.h>
#include <tail.h>
#include <QFuture.h>
#include <QtConcurrentRun>
#include <QClipboard>
#include <QTextStream>
#include <QApplication>
#include <ftp_files_model.h>
#include <ftp_files_proxy_model.h>

namespace Log_viewer
{

    // Instantiate singleton
    Log_manager* Log_manager::instance = &Singleton<Log_manager>::Instance();

    // ----------------------------------------------------------------------------

    Log_manager::Log_manager():
            QObject(NULL),
            m_file_parser(NULL),
            m_current_file(""),
            m_current_file_size(0),
            log_items_empty_signaled(true),
            m_socket_type(stNone)
    {
        m_log_items_model = new Log_items_model(this);
        m_clients_model = new Clients_model(this);
        m_log_filter_proxy_model = new Log_filter_proxy_model(this);
        m_file_neighbor_model = new File_neighbor_model(this);
        m_ftp_files_model = new ftp_files_model(this);

        m_ftp_files_proxy_model = new ftp_files_proxy_model(this);
        m_ftp_files_proxy_model->setSourceModel(m_ftp_files_model);

        m_log_filter_proxy_model->setSourceModel(m_log_items_model);
        m_log_filter_proxy_model->setDynamicSortFilter(true);

        m_tcp_server = new QTcpServer(this);
        connect(m_tcp_server,
                SIGNAL(newConnection()),
                this,
                SLOT(on_client_connected()));
        connect(&m_file_parser,
                SIGNAL(logs_found(Log_file_parser::Log_items)),
                this,
                SLOT(on_logs_found(Log_file_parser::Log_items)));
        connect(&m_file_parser,
                SIGNAL(file_opened(QString, qint64)),
                this,
                SLOT(on_file_opened(QString, qint64)));
        connect(&m_file_parser,
                SIGNAL(error(QString)),
                this,
                SLOT(on_file_parser_error(QString)));
        connect(&m_file_parser,
                SIGNAL(format_selected(QSharedPointer<Log_format>)),
                this,
                SLOT(on_log_format_selected(QSharedPointer<Log_format>)));
        connect(m_file_neighbor_model,
                SIGNAL(populated()),
                this,
                SLOT(on_file_neighbor_model_populated()));
        connect(m_log_filter_proxy_model,
                SIGNAL(filter_cleared()),
                this,
                SLOT(on_filter_cleared()));
        connect(m_ftp_files_model,
                SIGNAL(fileDownloaded(QString)),
                this,
                SLOT(on_ftp_file_downloaded(QString)));
        connect(m_ftp_files_model,
                SIGNAL(downloadingFtpFile(QString)),
                this,
                SLOT(on_downloading_ftp_file(QString)));

        setup_default_columns();
    }

    // ----------------------------------------------------------------------------

    Log_manager::~Log_manager()
    {

    }

    void Log_manager::on_ftp_file_downloaded(const QString file_name)
    {
        clear_ftp_tail();
        clear_log_items();
        m_file_parser.open(file_name);
        emit ftp_file_downloaded(file_name);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::signal_log_not_empty()
    {
        if(log_items_empty_signaled)
        {
            emit log_items_not_empty();
            log_items_empty_signaled = false;
        }
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_log_found(QSharedPointer<Log_item> log_item)
    {
        m_log_items_model->add(log_item);
        signal_log_not_empty();

        if(m_process_event_counter++ >= 1000) {
            QApplication::processEvents();
            m_process_event_counter = 0;
        }
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_logs_found(const Log_file_parser::Log_items& items)
    {
        m_log_items_model->add(items);
        signal_log_not_empty();

        m_process_event_counter += items.count();
        if(m_process_event_counter >= 1000) {
            QApplication::processEvents();
            m_process_event_counter = 0;
        }
    }

    // ----------------------------------------------------------------------------

    void Log_manager::create_udp_socket(int port)
    {
        m_udp_socket = new QUdpSocket(this);
        m_udp_socket->bind(port, QUdpSocket::ShareAddress);
        QSharedPointer<Log_client> client = QSharedPointer<Log_client>(new Log_client(this, m_udp_socket));

        m_clients_model->add(client);
        connect(client.data(),
                SIGNAL(log_found(QSharedPointer<Log_item>)),
                this,
                SLOT(on_log_found(QSharedPointer<Log_item>)));
        connect(client.data(),
                SIGNAL(disconnected(const Log_client*)),
                this,
                SLOT(on_client_disconnected(const Log_client*)));
        connect(client.data(),
                SIGNAL(format_selected(QSharedPointer<Log_format>)),
                this,
                SLOT(on_log_format_selected(QSharedPointer<Log_format>)));
        emit client_connected(client->get_address());
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_client_connected()
    {
        QTcpSocket* socket = m_tcp_server->nextPendingConnection();
        if(socket != NULL)
        {
            QSharedPointer<Log_client> client = QSharedPointer<Log_client>(new Log_client(this, socket));

            m_clients_model->add(client);
            connect(client.data(),
                    SIGNAL(log_found(QSharedPointer<Log_item>)),
                    this,
                    SLOT(on_log_found(QSharedPointer<Log_item>)));
            connect(client.data(),
                    SIGNAL(disconnected(const Log_client*)),
                    this,
                    SLOT(on_client_disconnected(const Log_client*)));
            connect(client.data(),
                    SIGNAL(format_selected(QSharedPointer<Log_format>)),
                    this,
                    SLOT(on_log_format_selected(QSharedPointer<Log_format>)));
            emit client_connected(client->get_address());
        }
    }

    // ----------------------------------------------------------------------------

    void Log_manager::open_ftp(const QString &host, int port, const QString &userName, const QString &password, Connection_type protocol)
    {
        QtConcurrent::run(m_ftp_files_model, &ftp_files_model::connectToServer, host, port, userName, password, protocol);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::tail_ftp(const QString &host, int port, const QString &userName, const QString &password, Connection_type protocol)
    {
        QString protocol_string = (protocol == FTP ? "ftp" : "sftp");
        QString url = QString("%1://%2:%3@%4:%5/%6").arg(protocol_string).arg(userName).arg(password).arg(host).arg(port).arg(m_remote_ftp_file_name);

        disconnect(m_file_parser.get_current_log_format().data(),
                   SIGNAL(log_found(Log_item_ptr)),
                   0,
                   0);

        m_FTP_tail_libcurl = QSharedPointer<FTP_tail_libcurl>(
                new FTP_tail_libcurl(url,
                                     m_current_file_size,
                                     m_file_parser.get_current_log_format(),
                                     protocol,
                                     this));

        m_FTP_tail_libcurl->moveToThread(&m_ftp_tail_thread);
        m_ftp_tail_thread.start();

        connect(m_FTP_tail_libcurl.data(),
                SIGNAL(file_size_updated(long)),
                this,
                SLOT(on_file_size_updated(long)));

        connect(m_FTP_tail_libcurl.data(),
                SIGNAL(logs_found(Log_file_parser::Log_items)),
                this,
                SLOT(on_logs_found(Log_file_parser::Log_items)));

        connect(this,
                SIGNAL(check_for_log_updated()),
                m_FTP_tail_libcurl.data(),
                SLOT(check_for_log_updated()));


        emit check_for_log_updated();
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_client_disconnected(const Log_client* client)
    {
        emit client_disconnected(client->get_address());
        m_clients_model->remove(client);
    }

    // ----------------------------------------------------------------------------

    bool Log_manager::listen(int port, Socket_type socket_type)
    {
        close();
        clear_tail();
        clear_ftp_tail();
        clear_log_items();
        m_socket_type = socket_type;

        bool result = false;

        if (socket_type == stTCP)
        {
            result = m_tcp_server->listen(QHostAddress::Any, port);
        }
        else
        {
            result = true;
            create_udp_socket(port);
        }

        emit server_listening(result);

        return result;
    }

    // ----------------------------------------------------------------------------

    void Log_manager::close()
    {
        m_tcp_server->close();
        m_clients_model->clear();

        if (m_socket_type == stUDP)
        {
            m_udp_socket->close();
            delete m_udp_socket;
        }

        m_socket_type = stNone;

        emit server_listening(false);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::open_log_files(const QStringList& files) {
        clear_log_items();
        emit local_file_opened();
        for(int i = 0; i < files.size(); i++)
        {
            QString file = files.at(i);
            m_log_filter_proxy_model->clear_filter();

            open_log(file);

            break; // only allow one file, for now...
        }
    }

    // ----------------------------------------------------------------------------

    void Log_manager::open_log(const QString &file)
    {
        m_file_parser.open(file);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::open_log_from_clipboard()
    {
        QClipboard *clipboard = QApplication::clipboard();
        QString log_text = clipboard->text();
        QTextStream log_text_stream(&log_text);

        clear_log_items();
        m_file_parser.open_text(log_text_stream);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::clear_log_items()
    {
        m_log_items_model->clear();

        log_items_empty_signaled = true;
        emit log_items_empty();
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_file_opened(const QString& file, qint64 size)
    {
        clear_tail();
        m_file_neighbor_model->populate(file);
        m_file_neighbor_model->set_active_item(file);
        m_current_file = file;
        m_current_file_size = size;
        emit file_opened(file);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_file_parser_error(const QString& text)
    {
        emit error(text);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::apply_filter(const QString& text, const QString& file, const QString& module, const QString& index, const QString &origin)
    {
        m_log_filter_proxy_model->set_text_filter(text);
        m_log_filter_proxy_model->set_module_filter(module);
        m_log_filter_proxy_model->set_file_filter(file);
        m_log_filter_proxy_model->set_index_filter(index);
        m_log_filter_proxy_model->set_origin_filter(origin);
        m_log_filter_proxy_model->apply();
    }

    // ----------------------------------------------------------------------------

    void Log_manager::tail_current_file()
    {
        m_tail.clear();
        m_tail = QSharedPointer<Tail>(new Tail(m_current_file,
                                               m_current_file_size,
                                               m_file_parser.get_current_log_format()));

        connect(m_tail.data(),
                SIGNAL(tail_log_found(QSharedPointer<Log_item>)),
                this,
                SLOT(on_tail_log_found(QSharedPointer<Log_item>)));
        connect(m_tail.data(),
                SIGNAL(tail_empty()),
                this,
                SLOT(on_tail_empty()));
        connect(m_tail.data(),
                SIGNAL(tail_failed(QString)),
                this,
                SLOT(on_tail_failed(QString)));
    }

    // ----------------------------------------------------------------------------

    void Log_manager::clear_tail()
    {
        m_tail.clear();
        emit tail_cleared();
    }

    // ----------------------------------------------------------------------------

    void Log_manager::clear_ftp_tail()
    {
        if (m_FTP_tail_libcurl)
        {
            m_ftp_tail_thread.quit();
            m_ftp_tail_thread.wait();
            emit ftp_tail_cleared();
        }
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_tail_log_found(QSharedPointer<Log_item> item)
    {
        m_log_items_model->add(item);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_tail_empty()
    {
        clear_log_items();
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_tail_failed(const QString& message)
    {
        emit error(message);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_log_format_selected(QSharedPointer<Log_format> format)
    {
        //m_log_items_model->set_columns(format->get_columns());

        m_log_filter_proxy_model->setSourceModel(m_log_items_model);
        m_log_filter_proxy_model->setDynamicSortFilter(true);

        // No need to re-populate the columns and ruin the users customization of
        // the columns if we select the same log format again.
        if(format->get_description() != m_current_log_format_description)
        {
            m_log_items_model->set_columns(format->get_columns());
            emit log_format_selected(format->get_description());
        }
        m_current_log_format_description = format->get_description();

        m_log_filter_proxy_model->setSourceModel(m_log_items_model);
        m_log_filter_proxy_model->setDynamicSortFilter(true);



        //emit log_format_selected(format->get_description());
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_file_neighbor_model_populated()
    {

    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_filter_cleared()
    {
        emit log_proxy_filter_cleared();
    }

    // ----------------------------------------------------------------------------

    void Log_manager::setup_default_columns()
    {
        Log_columns log_columns;
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_index)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_type)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_text)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_date)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_time)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_application)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_module)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_file)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_line)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_thread)));
        log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_origin)));

        m_log_items_model->set_columns(log_columns);
    }

    // ----------------------------------------------------------------------------

    void Log_manager::clear_filter()
    {
        m_log_filter_proxy_model->clear_filter();
        m_log_filter_proxy_model->apply();
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_downloading_ftp_file(const QString remote_file_name)
    {
        m_remote_ftp_file_name = remote_file_name;
        emit downloading_file();
    }

    // ----------------------------------------------------------------------------

    void Log_manager::on_file_size_updated(long current_file_size)
    {
        qDebug("Current filesize set to: " + QString::number(current_file_size).toAscii());
        m_current_file_size = current_file_size;
    }

    // ----------------------------------------------------------------------------

    void Log_manager::update_transfer_progress(double bytes_done, double bytes_total)
    {
        emit file_transfer_progress(bytes_done, bytes_total);
    }
}
