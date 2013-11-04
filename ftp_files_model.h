#ifndef FTP_FILES_MODEL_H
#define FTP_FILES_MODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QHash>
#include <QUrlInfo>

class QFile;
class QFtp;

namespace Log_viewer
{

    class ftp_files_model : public QAbstractListModel
    {
        Q_OBJECT
    public:
        explicit ftp_files_model(QObject *parent = 0);

        // inherited
        int rowCount( const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;

        void connectToFTP(const QString &host, int port, const QString &userName, const QString &password);
        void openFileFromModelIndex(const QModelIndex index);
        QString format_file_size(const qint64 file_size) const;
        void cancelDownload();

    signals:
        void fileDownloaded(const QString &fileName);
        void fileDownloadProgress(qint64 readBytes, qint64 totalBytes);
        void ftpStatusMessage(const QString &message);
        void ftpErrorMessage(const QString &message);
        void ftpConnected();
        void ftpDisconnected();
        void downloadCanceled();

    public slots:

    private slots:
        void disconnectAndConnect();
        void downloadFile(const QString fileName);


        void ftpCommandFinished(int id	, bool error);
        void addToList(const QUrlInfo &urlInfo);
        void cdToParent();
        void updateDataTransferProgress(qint64 readBytes, qint64 totalBytes);

    private:
        void issueList();

        QList<QUrlInfo> m_files;
        QList<QUrlInfo> m_directories;
        QString m_host;
        QString m_userName;
        int m_port;
        QString m_password;

        QHash<QString, bool> isDirectory;
        QString currentPath;
        QFtp *ftp;
        QFile *file;
    };
}

#endif // FTP_FILES_MODEL_H
