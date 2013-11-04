#include "ftp_files_model.h"

#include <QtNetwork>
#include <QMessageBox>

namespace Log_viewer
{

    // ----------------------------------------------------------------------------

    ftp_files_model::ftp_files_model(QObject *parent) :
        QAbstractListModel(parent)
    {
    }

    // ----------------------------------------------------------------------------

    int ftp_files_model::rowCount( const QModelIndex& parent) const
    {
        Q_UNUSED(parent);
        return m_files.size() + m_directories.size();
    }

    // ----------------------------------------------------------------------------

    QVariant ftp_files_model::data(const QModelIndex& index, int role) const
    {
        QUrlInfo info;
        if(index.row() < m_directories.size()) {
            info = m_directories.at(index.row());
        }
        else {
            info = m_files.at(index.row() - m_directories.size());
        }

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
        if (ftp)  {
            ftp->abort();
            ftp->deleteLater();
            ftp = 0;
            emit ftpDisconnected();
        }

        ftp = new QFtp(this);
        connect(ftp, SIGNAL(commandFinished(int,bool)),
                this, SLOT(ftpCommandFinished(int,bool)));
        connect(ftp, SIGNAL(listInfo(QUrlInfo)),
                this, SLOT(addToList(QUrlInfo)));
        connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
                this, SLOT(updateDataTransferProgress(qint64,qint64)));

        m_files.clear();
        m_directories.clear();
        currentPath.clear();
        isDirectory.clear();

        QString authority;
        if(m_userName.isEmpty()) {
            authority = QString("ftp://%1:%2").arg(m_host).arg(m_port);
        }
        else {
            authority = QString("ftp://%1:%2@%3:%4").arg(m_userName).arg(m_password).arg(m_host).arg(m_port);
        }
        qDebug(authority.toAscii());
        QUrl url(authority);
        url.setPath(".");

        if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp"))  {
            ftp->connectToHost(m_host, m_port);
            ftp->login();
        } else  {
            ftp->connectToHost(url.host(), url.port(m_port));

            if (!url.userName().isEmpty())
                ftp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
            else
                ftp->login();
            if (!url.path().isEmpty())
                ftp->cd(url.path());
        }

        emit ftpStatusMessage(tr("Connecting to FTP server %1...").arg(m_host));
        qDebug(tr("Connecting to FTP server %1...").arg(m_host).toAscii());
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::connectToFTP(const QString &host, int port, const QString &userName, const QString &password)
    {
        m_host = host;
        m_port = port;
        m_userName = userName;
        m_password = password;
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

        ftp->get(fileName, file);

        emit ftpStatusMessage(tr("Downloading %1...").arg(fileName));
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::cancelDownload()
     {
        ftp->abort();

        if (file->exists())  {
            file->close();
            file->remove();
        }
        delete file;
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::ftpCommandFinished(int id, bool error)
     {
        if(error) {
            QString errorMsg = QString("FTP error: %1").arg(ftp->errorString());
            emit ftpErrorMessage(errorMsg);
        }

        /*switch(ftp->currentCommand())
        {
        case QFtp::None	: qDebug(QString("No command is being executed.").toAscii()); break;
        case QFtp::SetTransferMode: qDebug(QString("set the transfer mode.").toAscii()); break;
        case QFtp::SetProxy	: qDebug(QString("switch proxying on or off.").toAscii()); break;
        case QFtp::ConnectToHost: qDebug(QString("connectToHost() is being executed.").toAscii()); break;
        case QFtp::Login: qDebug(QString("login() is being executed.").toAscii()); break;
        case QFtp::Close: qDebug(QString("close() is being executed.").toAscii()); break;
        case QFtp::List	: qDebug(QString("list() is being executed.").toAscii()); break;
        case QFtp::Cd: qDebug(QString("cd() is being executed.").toAscii()); break;
        case QFtp::Get: qDebug(QString("get() is being executed.").toAscii()); break;
        case QFtp::Put: qDebug(QString("put() is being executed.").toAscii()); break;
        case QFtp::Remove: qDebug(QString("remove() is being executed.").toAscii()); break;
        case QFtp::Mkdir: qDebug(QString("mkdir() is being executed.").toAscii()); break;
        case QFtp::Rmdir: qDebug(QString("rmdir() is being executed.").toAscii()); break;
        case QFtp::Rename: qDebug(QString("rename() is being executed.").toAscii()); break;
        case QFtp::RawCommand: qDebug(QString("rawCommand() is being executed.").toAscii()); break;
        }*/

        if (ftp->currentCommand() == QFtp::ConnectToHost)  {
            if (error)  {
                emit ftpStatusMessage(tr("Unable to connect to the FTP server at %1.").arg(m_host));
                disconnectAndConnect();
                return;
            }
            emit ftpStatusMessage(tr("Logged onto %1.").arg(m_host));
            return;
        }

        if (ftp->currentCommand() == QFtp::Login) {
            issueList();
        }

        if (ftp->currentCommand() == QFtp::Get)  {
            if (error)  {
                file->close();
                file->remove();
                emit downloadCanceled();
            } else  {
                emit ftpStatusMessage(QString("FTP Download: %1").arg(file->fileName()));
                file->close();
                emit fileDownloaded(file->fileName());
            }
            delete file;
        } else if (ftp->currentCommand() == QFtp::List)  {
            if (isDirectory.isEmpty())  {
            }
        }
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::issueList()
    {
        qDebug(QString("Issue Ftp::List").toAscii());
        beginRemoveRows(QModelIndex(), 0, m_files.size() + m_directories.size());
        m_files.clear();
        m_directories.clear();
        endRemoveRows();
        beginInsertRows(QModelIndex(), 0, 0);
        QUrlInfo parentUrl("..", 0, "", "", 0, QDateTime::currentDateTime(), QDateTime::currentDateTime(), true, false, false, false, true, false);
        m_directories.push_back(parentUrl);
        endInsertRows();
        ftp->list();
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::addToList(const QUrlInfo &urlInfo)
     {
        beginInsertRows(QModelIndex(), m_files.size() + m_directories.size(), m_files.size() + m_directories.size());
        if(urlInfo.isDir()) {
            m_directories.push_back(urlInfo);
        }
        else {
            m_files.push_back(urlInfo);
        }
        endInsertRows();
    }


    void ftp_files_model::cdToParent()
     {
        beginRemoveRows(QModelIndex(), 0, m_files.size() + m_directories.size());
        m_files.clear();
        m_directories.clear();
        endRemoveRows();
        isDirectory.clear();
        currentPath = currentPath.left(currentPath.lastIndexOf('/'));
        if (currentPath.isEmpty())  {
            ftp->cd("/");
        } else  {
            ftp->cd(currentPath);
        }
        issueList();
    }

    // ----------------------------------------------------------------------------

    void ftp_files_model::updateDataTransferProgress(qint64 readBytes,
                                               qint64 totalBytes)
     {
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
        if(index.row() < m_directories.size()) {
            url = m_directories.at(index.row());
        }
        else {
            url = m_files.at(index.row() - m_directories.size());
        }

        if(url.isDir()) {
            if(url.name() == "..") {
                cdToParent();
            }
            else {
                currentPath = currentPath + "/" + url.name();
                ftp->cd(currentPath);
                issueList();
                qDebug(QString("Current path: %1").arg(currentPath).toAscii());
            }
        }
        else {
            downloadFile(url.name());
        }
    }
}
