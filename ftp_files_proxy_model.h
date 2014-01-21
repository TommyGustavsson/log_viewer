#ifndef FTP_FILES_PROXY_MODEL_H
#define FTP_FILES_PROXY_MODEL_H

#include <QSortFilterProxyModel>

namespace Log_viewer
{

    class ftp_files_proxy_model : public QSortFilterProxyModel
    {
        Q_OBJECT

    public:
        explicit ftp_files_proxy_model(QObject *parent = 0);

    protected:
        bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    };

}

#endif // FTP_FILES_PROXY_MODEL_H
