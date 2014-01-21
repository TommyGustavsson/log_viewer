#include <ftp_files_proxy_model.h>
#include <QString>

namespace Log_viewer
{

    ftp_files_proxy_model::ftp_files_proxy_model(QObject *parent) :
        QSortFilterProxyModel(parent)
    {
    }

    bool ftp_files_proxy_model::lessThan(const QModelIndex &left, const QModelIndex &right) const
    {
        QString leftData = sourceModel()->data(left).toString();
        QString rightData = sourceModel()->data(right).toString();

        if (leftData.startsWith("[") && !rightData.startsWith("["))
            return true;
        if (!leftData.startsWith("[") && rightData.startsWith("["))
            return false;

        return (QString::localeAwareCompare(leftData, rightData) < 0);
    }

    bool ftp_files_proxy_model::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
    {
         QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
        //qDebug("filterAcceptsRow");
        QString data = sourceModel()->data(index).toString();
        if (sourceModel()->data(index).toString() == "[.]")
            return false;
        else
            return true;
    }

}
