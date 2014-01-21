#ifndef FTP_TAIL_LIBCURL_H
#define FTP_TAIL_LIBCURL_H

#include <QObject>
#include <QSharedPointer>
#include <QThread>

#include <log_format.h>
#include <log_file_parser.h>
#include <protocol.h>

#include <curl/curl.h>
#include <curl/easy.h>

namespace Log_viewer
{

    class FTP_tail_libcurl : public QObject
    {

        Q_OBJECT

    public:
        FTP_tail_libcurl(QString path,
                         long restart_position,
                         QSharedPointer<Log_format> log_format_used,
                         Connection_type protocol,
                         QObject *parent);
        ~FTP_tail_libcurl();
        //void start_monitoring();

    signals:
        void file_size_updated(long);
        void logs_found(const Log_file_parser::Log_items& log_items);

    public:
        void run();
        void set_log_rotate_file_name(QString file_name);

    private:

        enum File_check_result
        {
            file_check_result_no_change = 0,
            file_check_result_size_increased,
            file_check_result_log_rotated
        };

        void get_new_data();
        void get_new_data(QString file, long restart_position);
        File_check_result new_data_available(Connection_type protocol);
        static size_t list_data_received(void *buffer, size_t size, size_t nmemb, void *data);
        Log_file_parser::Log_items m_log_items;

        QString m_path;
        QString m_log_rotate_file_name;
        QObject* m_parent;
        CURL* m_curl;
        Connection_type m_Connection_type;
        static QByteArray m_folder_list_data;

    private slots:
        void check_for_log_updated();
        void on_log_found(QSharedPointer<Log_item> item);
    };
}
#endif // FTP_TAIL_LIBCURL_H


