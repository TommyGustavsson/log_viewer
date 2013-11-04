#include "file_neighbor_model.h"

#include <QFileInfo>
#include <QDir>
#include <QFont>
#include <QBrush>

namespace Log_viewer
{

    File_neighbor_model::File_neighbor_model(QObject *parent):
            QAbstractListModel(parent),
            m_active_item(-1)
    {
        connect(&m_watcher,
                SIGNAL(directoryChanged(QString)),
                this,
                SLOT(on_neighbor_folder_changed(QString)));
    }

    int File_neighbor_model::rowCount( const QModelIndex& parent) const
    {
        Q_UNUSED(parent);
        return m_neighbors.size();
    }

    QVariant File_neighbor_model::data(const QModelIndex& index, int role) const
    {
        if(role == Qt::DisplayRole)
        {
            return QVariant(m_neighbors.at(index.row()).fileName());
        }
        else if(role == Qt::ForegroundRole)
        {
            if(m_active_item != index.row()) {
                QBrush brush(QColor(127,124,132,255));
                return brush;
            }
            return QVariant();
        }
        else if(role == Qt::ToolTipRole)
        {
            return QVariant(m_neighbors.at(index.row()).filePath());
        }
        else
        {
            return QVariant();
        }
    }

    void File_neighbor_model::add(const QString& file)
    {
        beginInsertRows(QModelIndex(), m_neighbors.size(), m_neighbors.size());
        QFileInfo file_info(file);
        m_neighbors.push_back(file_info);
        endInsertRows();
    }

    void File_neighbor_model::remove(int index)
    {
        beginRemoveRows(QModelIndex(), index, index);
        m_neighbors.removeAt(index);
        if(m_active_item == index)
        {
            clear_active_item();
        }
        endRemoveRows();
    }

    void File_neighbor_model::clear()
    {
        beginRemoveRows(QModelIndex(), 0, m_neighbors.size());
        clear_active_item();
        m_neighbors.clear();
        endRemoveRows();
    }

    void File_neighbor_model::populate(const QString& file_name)
    {
        clear_active_item();
        QFileInfo file_info(file_name);

        if(file_info.exists())
        {
            QDir dir = file_info.dir();

            dir.setFilter(QDir::Files | QDir::NoSymLinks);
            dir.setSorting(QDir::Name);

            QFileInfoList list = dir.entryInfoList();

            clear();
            beginInsertRows(QModelIndex(), 0, list.size());
            for (int i = 0; i < list.size(); ++i)
            {
                m_neighbors.push_back(list.at(i));
                if(list.at(i).filePath() == file_name)
                {
                    m_active_item = i;
                }
            }
            endInsertRows();

            start_watching_folder(file_info.dir().path());

            emit populated();
        }
    }

    const QFileInfo File_neighbor_model::get_item(const QModelIndex& index) const
    {
        return m_neighbors.at(index.row());
    }

    void File_neighbor_model::set_active_item(int item)
    {
        m_active_item = item;
        dataChanged(createIndex(item, 0), createIndex(item, 0));
    }

    void File_neighbor_model::clear_active_item()
    {
        m_active_item = -1;
        reset();
    }

    void File_neighbor_model::set_active_item(const QString& file_name)
    {
        for(int i = 0; i < m_neighbors.size(); i++)
        {
            if(m_neighbors.at(i).filePath().compare(file_name, Qt::CaseInsensitive) == 0)
            {
                set_active_item(i);
                break;
            }
        }
    }

    void File_neighbor_model::start_watching_folder(const QString& folder)
    {
        if(!m_current_folder.isEmpty())
        {
            m_watcher.removePath(m_current_folder);
        }
        m_current_folder = folder;
        m_watcher.addPath(folder);
    }

    void File_neighbor_model::on_neighbor_folder_changed(const QString& folder)
    {
        if((m_active_item >= 0) && (m_active_item < m_neighbors.size()))
        {
            QString active_item = m_neighbors.at(m_active_item).filePath();
            populate(active_item);
        }
        else
        {
            populate(QString("%0/%1").arg(folder).arg("somefile.nofile"));
        }
    }
}
