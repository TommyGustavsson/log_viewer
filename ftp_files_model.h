#ifndef FTP_FILES_MODEL_H
#define FTP_FILES_MODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QHash>
#include <QUrlInfo>
#include <QTcpSocket>

#include <protocol.h>

#include <curl/curl.h>
#include <curl/easy.h>

class QFile;

namespace Log_viewer
{

    class ftp_files_model : public QAbstractListModel
    {
        Q_OBJECT
    public:
        explicit ftp_files_model(QObject *parent = 0);
        ~ftp_files_model();

        // inherited
        int rowCount( const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;

        void connectToServer(const QString &host, int port, const QString &userName, const QString &password, Connection_type protocol);
        void openFileFromModelIndex(const QModelIndex index);
        void deleteFileFromModelIndex(const QModelIndex index);
        QString format_file_size(const qint64 file_size) const;
        void tailFile();
        void cancelDownload();

    signals:
        void fileDownloaded(const QString &fileName);
        void fileDownloadProgress(qint64 readBytes, qint64 totalBytes);
        void ftpStatusMessage(const QString &message);
        void ftpErrorMessage(const QString &message);
        void ftpConnected();
        void ftpDisconnected();
        void ftpFileListUpdated();
        void downloadCanceled();
        void downloadingFtpFile(const QString &remoteFileName);

    public slots:

    private slots:
        void disconnectAndConnect();
        void downloadFile(const QString fileName);

        void addToList(const QUrlInfo &urlInfo);
        void cdToParent();
        void updateDataTransferProgress(qint64 readBytes, qint64 totalBytes);

    private:
        void issueList();

        void downloadFile(QString tempFile, QString fileName);  
        bool deleteFile(QString fileName);

        static size_t list_data_received(void *buffer, size_t size, size_t nmemb, void *);
        static int progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
        static QByteArray m_folder_list_data;

        QList<QUrlInfo> m_objects;
        QString m_active_ftp_file;
        QString m_host;
        QString m_userName;
        int m_port;
        QString m_password;

        QHash<QString, bool> isDirectory;
        QString m_currentPath;
        QFile *file;
        QString m_url;

        CURL* m_curl;
        int m_data_port;
        int m_previous_size;
        int m_restart_position;
        Connection_type m_protocol;
    };
}

#endif // FTP_FILES_MODEL_H
