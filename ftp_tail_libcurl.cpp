#include "ftp_tail_libcurl.h"

#include <QTimer>
#include <QTextStream>
#include <QStringList>
#include <QRegExp>
#include <QCoreApplication>

namespace Log_viewer
{
    QByteArray FTP_tail_libcurl::m_folder_list_data;

    QSharedPointer<Log_format> log_format;

    // Declared here to enable use in static libcurl callback functions
    long m_current_file_pos;

    size_t get_size_from_header(void *ptr, size_t size, size_t nmemb, void *data);
    size_t data_received(void *buffer, size_t size, size_t nmemb, void *stream);

    FTP_tail_libcurl::FTP_tail_libcurl(QString path,
                                       long restart_position,
                                       QSharedPointer<Log_format> log_format_used,
                                       Connection_type protocol,
                                       QObject *parent) :
        m_path(path),
        m_parent(parent),
        m_Connection_type(protocol)
    {
        m_curl = curl_easy_init();

        m_current_file_pos = restart_position;
        m_log_rotate_file_name = "";
        log_format = log_format_used;

        connect(log_format.data(),
                SIGNAL(log_found(Log_item_ptr)),
                this,
                SLOT(on_log_found(Log_item_ptr)));
    }

    FTP_tail_libcurl::~FTP_tail_libcurl()
    {
        curl_easy_cleanup(m_curl);
    }

    void FTP_tail_libcurl::set_log_rotate_file_name(QString file_name)
    {
        m_log_rotate_file_name = file_name;
    }

    // Returns true if the remote file has changed in size.
    FTP_tail_libcurl::File_check_result FTP_tail_libcurl::new_data_available(Connection_type protocol)
    {
        CURLcode res;
        double filesize = 0;

        if (protocol == FTP)
        {
            curl_easy_reset(m_curl);
            //curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(m_curl, CURLOPT_URL, m_path.toStdString().c_str());
            curl_easy_setopt(m_curl, CURLOPT_NOBODY, 1L );
            curl_easy_setopt(m_curl, CURLOPT_FILETIME, 1L);
            //curl_easy_setopt(m_curl, CURLOPT_HEADER, 0L);
            curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, get_size_from_header);

            res = curl_easy_perform(m_curl);

            if(CURLE_OK == res)
            {
                res = curl_easy_getinfo( m_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &filesize);
                if((CURLE_OK != res) || (filesize <= 0))
                {
                    qDebug("Get size failed, assuming no change");
                    return file_check_result_no_change;
                }
            }
        }
        else if (protocol == SFTP)
        {
            m_folder_list_data.clear();

            std::string path = m_path.left(m_path.lastIndexOf("/")).append("/").toStdString();

            curl_easy_reset(m_curl);

            curl_easy_setopt(m_curl, CURLOPT_URL, path.c_str());
            curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 10L);
            curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, list_data_received);

            res = curl_easy_perform(m_curl);

            if (res != CURLE_OK)
            {
                qDebug("Get size failed, assuming no change");
                return file_check_result_no_change;
            }

            QString filename = m_path.right(m_path.length() - m_path.lastIndexOf("/") - 1);
            QTextStream listing(m_folder_list_data);
            QRegExp whitespace("\\s+");
            while (!listing.atEnd())
            {
                QString entry = listing.readLine();
                QStringList info_columns = entry.split(whitespace);

                if(info_columns.last() == filename)
                {
                    filesize = info_columns.at(4).toDouble();
                    break;
                }
            }
        }

        qDebug(QString("filesize : %1 bytes\n").arg(filesize,0,'f',0).toAscii());
        if (filesize > m_current_file_pos) // New logs
        {
            return file_check_result_size_increased;
        }
        else if (filesize < m_current_file_pos) // Log file rotation, restart from beginning of file
        {
            return file_check_result_log_rotated;
          //restart_pos = 0;
        }
        else // No change
        {
            return file_check_result_no_change;
        }

        //return file_check_result_no_change;
    }

    void FTP_tail_libcurl::run()
    {
        check_for_log_updated();
    }

    void FTP_tail_libcurl::check_for_log_updated()
    {
        qDebug("Checking for new data");
        File_check_result result = new_data_available(m_Connection_type);

        if (result == file_check_result_size_increased)
        {
            qDebug("New data available, getting it");
            get_new_data();
        }
        else if (result == file_check_result_log_rotated)
        {
            if (m_log_rotate_file_name != "")
            {
                // Get the last data from the rotated log file
                get_new_data(m_log_rotate_file_name, m_current_file_pos);
            }
            m_current_file_pos = 0;
            get_new_data();
        }
        else if (result == file_check_result_no_change)
        {
            qDebug("No new data available");
        }

        qDebug("Done with new data check");

        QTimer::singleShot(100, this, SLOT(check_for_log_updated()));
    }

    void FTP_tail_libcurl::get_new_data()
    {
        CURLcode res;

        curl_easy_reset(m_curl);
        curl_easy_setopt(m_curl, CURLOPT_URL, m_path.toStdString().c_str());
        curl_easy_setopt(m_curl, CURLOPT_RESUME_FROM , m_current_file_pos);
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, data_received);

        res = curl_easy_perform(m_curl);

        QCoreApplication::processEvents();

        if(m_log_items.size() > 0)
        {
            emit logs_found(m_log_items);
            m_log_items.clear();
        }

        emit file_size_updated(m_current_file_pos);
    }

    void FTP_tail_libcurl::get_new_data(QString file, long restart_position)
    {
        CURLcode res;

        curl_easy_reset(m_curl);
        curl_easy_setopt(m_curl, CURLOPT_URL, file.toStdString().c_str());
        curl_easy_setopt(m_curl, CURLOPT_RESUME_FROM , restart_position);
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, data_received);

        res = curl_easy_perform(m_curl);

        QCoreApplication::processEvents();

        if(m_log_items.size() > 0)
        {
            emit logs_found(m_log_items);
            m_log_items.clear();
        }

        emit file_size_updated(m_current_file_pos);
    }

    void FTP_tail_libcurl::on_log_found(Log_item_ptr item)
    {
        m_log_items.push_back(item);
        // calculate_statistics(item);

        if(m_log_items.size() >= 1000)
        {
            emit logs_found(m_log_items);
            m_log_items.clear();
        }
    }

    size_t get_size_from_header(void *ptr, size_t size, size_t nmemb, void *)
    {
        return (size_t)(size * nmemb);
    }

    size_t data_received(void *buffer, size_t size, size_t nmemb, void *)
    {
        long data_size = size*nmemb;
        m_current_file_pos += data_size;
        log_format->add_line(QString::fromAscii(reinterpret_cast<const char*>(buffer), data_size), "FTP");
        return data_size;
    }

    size_t FTP_tail_libcurl::list_data_received(void *buffer, size_t size, size_t nmemb, void *)
    {
        long data_size = size*nmemb;
        m_folder_list_data.append(reinterpret_cast<const char*>(buffer), data_size);
        return data_size;
    }
}
