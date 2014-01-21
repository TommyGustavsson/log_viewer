#include "ftp_files_model.h"
#include "log_manager.h"

#include <QtNetwork>
#include <QMessageBox>
#include <QUrlInfo>

namespace Log_viewer
{

    QByteArray ftp_files_model::m_folder_list_data;

    // ----------------------------------------------------------------------------

    ftp_files_model::ftp_files_model(QObject *parent) :
        QAbstractListModel(parent)
    {
        m_curl = curl_easy_init();
        m_objects.clear();
    }

    // ----------------------------------------------------------------------------

    ftp_files_model::~ftp_files_model()
    {
        curl_easy_cleanup(m_curl);
        curl_global_cleanup();
    }

    // ----------------------------------------------------------------------------

    int ftp_files_model::rowCount( const QModelIndex& parent) const
    {
        Q_UNUSED(parent);
        return m_objects.size();
    }

    // ----------------------------------------------------------------------------

    QVariant ftp_files_model::data(const QModelIndex& index, int role) const
    {
        QUrlInfo info;
        info = m_objects.at(index.row());

        if(role == Qt::DisplayRole)
        {
            QString value;

            if(info.isDir()) {
                value = QString("[%0]").arg(info.name());
            }
            else {
                value = info.name();
            }

            return QVariant(value);
        }
        if((role == Qt::ForegroundRole) && (info.isDir()))
        {
            QBrush brush(QColor(127,124,132,255));
            return brush;
        }
        if(role == Qt::ToolTipRole)
        {
            QString fileSize = format_file_size(info.size());
            return QVariant(QString("Size:%0 Last Modified:%1").arg(fileSize).arg(info.lastModified().toString("MMM dd  yyyy")));
        }
        else
        {
            return QVariant();
        }
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::disconnectAndConnect()
     {
        m_currentPath.clear();
        isDirectory.clear();

        QString authority;
        QString protocol_str = (m_protocol == FTP ? "ftp" : "sftp");
        if(m_userName.isEmpty()) {
            authority = QString("%1://%2:%3").arg(protocol_str).arg(m_host).arg(m_port);
        }
        else {
            authority = QString("%1://%2:%3@%4:%5").arg(protocol_str).arg(m_userName).arg(m_password).arg(m_host).arg(m_port);
        }
        qDebug(authority.toAscii());
        QUrl url(authority);
        m_url = url.toString();

        issueList();
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::connectToServer(const QString &host, int port, const QString &userName, const QString &password, Connection_type protocol)
    {
        m_host = host;
        m_port = port;
        m_userName = userName;
        m_password = password;
        m_protocol = protocol;
        disconnectAndConnect();
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::downloadFile(const QString fileName)
     {
        QString downloadPath = QDir::tempPath() + "/LogViewerFTPStorage/";
        QDir downloadDir(downloadPath);
        if(!downloadDir.exists(downloadPath)) {
            downloadDir.mkdir(downloadPath);
        }

        QString tempFile = downloadPath + fileName;
        qDebug(tempFile.toAscii());
        if (QFile::exists(tempFile))  {
            QFile::remove(tempFile);
        }

        file = new QFile(tempFile);
        if (!file->open(QIODevice::WriteOnly)) {
             emit ftpErrorMessage(tr("Unable to save the file %1: %2.").arg(fileName).arg(file->errorString()));
             delete file;
             return;
        }
        file->close();

        QtConcurrent::run(this, &ftp_files_model::downloadFile, tempFile, fileName);

        emit ftpStatusMessage(tr("Downloading %1...").arg(fileName));
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::downloadFile(QString tempFile, QString fileName)
    {
        CURLcode res;

        FILE* file_handle;
        file_handle = fopen(tempFile.toAscii(), "wb");

        if (file_handle == NULL)
        {
            return;
        }

        QString fullURL = m_url + "/" + m_currentPath + "/" + fileName;

        curl_easy_reset(m_curl);
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 60L);
        curl_easy_setopt(m_curl, CURLOPT_URL, fullURL.toStdString().c_str());
        curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(m_curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, file_handle);

        res = curl_easy_perform(m_curl);

        fclose(file_handle);
        emit fileDownloaded(tempFile);
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::cancelDownload()
    {
        delete file;
    }

    // ----------------------------------------------------------------------------

    size_t ftp_files_model::list_data_received(void *buffer, size_t size, size_t nmemb, void *)
    {
        long data_size = size*nmemb;
        m_folder_list_data.append(reinterpret_cast<const char*>(buffer), data_size);
        return data_size;
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::issueList()
    {
        CURLcode res;
        qDebug(QString("Issue Ftp::List").toAscii());
        beginRemoveRows(QModelIndex(), 0, m_objects.size());
        m_objects.clear();
        endRemoveRows();
        if (m_protocol == FTP) // Not needed for SFTP which already provides the [..] entry
        {
            beginInsertRows(QModelIndex(), 0, 0);
            QUrlInfo parentUrl("..", 0, "", "", 0, QDateTime::currentDateTime(), QDateTime::currentDateTime(), true, false, false, false, true, false);
            m_objects.push_back(parentUrl);
            endInsertRows();
        }
        QString fullURL = m_url + "/" + m_currentPath;
        if (!fullURL.endsWith("/"))
        {
            fullURL.append("/");
        }

        curl_easy_reset(m_curl);
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 60L);
        curl_easy_setopt(m_curl, CURLOPT_URL, fullURL.toStdString().c_str());
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, list_data_received);

        m_folder_list_data.clear();
        res = curl_easy_perform(m_curl);

        qDebug("Data received: " + QString(m_folder_list_data).toAscii());

        QTextStream listing(m_folder_list_data);
        QRegExp whitespace("\\s+");
        while (!listing.atEnd())
        {
            QString entry = listing.readLine();
            QStringList info_columns = entry.split(whitespace);
            QUrlInfo info;

            QString file_name;
            for (int i = 8; i < info_columns.size(); i++)
            {
                file_name.append(info_columns.at(i) + " ");
            }
            info.setName(file_name.trimmed());

            info.setDir(info_columns.at(0).startsWith("d"));
            info.setFile(!info_columns.at(0).startsWith("d"));
            info.setSize(info_columns.at(4).toLong());

            addToList(info);
        }
        emit ftpFileListUpdated();
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::addToList(const QUrlInfo &urlInfo)
     {
        beginInsertRows(QModelIndex(), m_objects.size(), m_objects.size());
        m_objects.push_back(urlInfo);
        endInsertRows();
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::cdToParent()
     {
        beginRemoveRows(QModelIndex(), 0, m_objects.size());
        m_objects.clear();
        endRemoveRows();
        isDirectory.clear();
        int last_dir_sep = m_currentPath.lastIndexOf('/');
        if (last_dir_sep != -1 )
        {
            m_currentPath = m_currentPath.left(last_dir_sep);
        }
        else
        {
            m_currentPath.clear();
        }
        issueList();
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::updateDataTransferProgress(qint64 readBytes,
                                               qint64 totalBytes)
    {
        // Change to use libcurl's method instead
        emit fileDownloadProgress(readBytes, totalBytes);
    }

    // ----------------------------------------------------------------------------

    QString ftp_files_model::format_file_size(const qint64 file_size) const
    {
        const int dwKB = 1024;          // Kilobyte
        const int dwMB = 1024 * dwKB;   // Megabyte
        const int dwGB = 1024 * dwMB;   // Gigabyte

        int dwNumber;
        QString strNumber;

        if (file_size < dwKB)
        {
            strNumber = QString("%0 B").arg(file_size);
        }
        else
        {
            if (file_size < dwMB)
            {
              dwNumber = file_size / dwKB;
              strNumber = QString("%0.2 KB").arg(dwNumber);
            }
            else
            {
                if (file_size < dwGB)
                {
                    dwNumber = file_size / dwMB;
                    strNumber = QString("%0.2 MB").arg(dwNumber);
                }
                else
                {
                    if (file_size >= dwGB)
                    {
                        dwNumber = file_size / dwGB;
                        strNumber = QString("%0.2 GB").arg(dwNumber);
                    }
                }
            }
        }

        return strNumber;
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::openFileFromModelIndex(const QModelIndex index)
    {
        QUrlInfo url;

        url = m_objects.at(index.row());

        if(url.isDir()) {
            if(url.name() == "..") {
                cdToParent();
            }
            else {
                if (m_currentPath.isEmpty()) {
                    m_currentPath = url.name();
                } else {
                    m_currentPath = m_currentPath + "/" + url.name();
                }
                issueList();
                qDebug(QString("Current path: %1").arg(m_currentPath).toAscii());
            }
        }
        else {
            emit downloadingFtpFile(m_currentPath + "/" + url.name());
            downloadFile(url.name());
        }
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::deleteFileFromModelIndex(const QModelIndex index)
    {
        bool result;
        QUrlInfo url;

        url = m_objects.at(index.row());

        result = deleteFile(url.name());

        if (result)
        {
            beginRemoveRows(QModelIndex(), index.row(), index.row());
            m_objects.removeAt(index.row());
            endRemoveRows();
            emit ftpFileListUpdated();
            emit ftpStatusMessage(tr("File deleted"));
        }
        else
        {
            emit ftpStatusMessage(tr("Could not delete file"));
        }
    }

    // ----------------------------------------------------------------------------

    bool ftp_files_model::deleteFile(QString fileName)
    {
        CURLcode res;
        curl_easy_reset(m_curl);
        char errorstr[500];
        QString fullURL = m_url + "/" + m_currentPath + "/" + fileName;

        struct curl_slist* headerlist;
        if (m_protocol == SFTP)
        {
            headerlist = curl_slist_append(NULL, QString("rm /" + m_currentPath + "/" + fileName).toStdString().c_str());
        }
        else if (m_protocol == FTP)
        {
            headerlist = curl_slist_append(NULL, QString("DELE /" + m_currentPath + "/" + fileName).toStdString().c_str());
        }

        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 60L);
        curl_easy_setopt(m_curl, CURLOPT_ERRORBUFFER, errorstr);
        curl_easy_setopt(m_curl, CURLOPT_URL, m_url.toStdString().c_str());
        curl_easy_setopt(m_curl, CURLOPT_POSTQUOTE, headerlist);

        res = curl_easy_perform(m_curl);
        curl_slist_free_all (headerlist);
        qDebug("res: " + res);

        return (res == CURLE_OK);
    }

     int ftp_files_model::progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
     {
        static qint64 last_update = 0;
        if (QDateTime::currentMSecsSinceEpoch() - last_update >= 500)
        {
            last_update = QDateTime::currentMSecsSinceEpoch();
            Log_manager::instance->update_transfer_progress(dlnow, dltotal);
        }
        return 0;
     }
}
