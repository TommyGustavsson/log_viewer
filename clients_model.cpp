#include <clients_model.h>

namespace Log_viewer
{

    // ----------------------------------------------------------------------------

    Clients_model::Clients_model(QObject *parent) :
        QAbstractListModel(parent)
    {
    }

    // ----------------------------------------------------------------------------

    int Clients_model::rowCount(const QModelIndex& parent) const
    {
        if(parent.isValid())
            return 0;
        else
            return m_clients.count();
    }

    // ----------------------------------------------------------------------------

    QVariant Clients_model::data(const QModelIndex& index, int role) const
    {
        if(role == Qt::DisplayRole)
        {
            return m_clients.at(index.row())->get_address();
        }
        else if(role == Qt::BackgroundRole)
        {
            return QVariant();
        }
        else
        {
            return QVariant();
        }
    }

    // ----------------------------------------------------------------------------

    void Clients_model::add(QSharedPointer<Log_client> item)
    {
        this->beginInsertRows(QModelIndex(), m_clients.count(), m_clients.count());
        m_clients.append(item);
        this->endInsertRows();
    }

    // ----------------------------------------------------------------------------

    void Clients_model::remove(const Log_client* client)
    {
        int index = -1;
        for(int i = 0; i < m_clients.size(); i++)
        {
            if(m_clients.at(i).data() == client)
            {
                index = i;
                break;
            }
        }

        this->beginRemoveRows(QModelIndex(), index, index);
        m_clients.removeAt(index);
        this->endRemoveRows();
    }

    // ----------------------------------------------------------------------------

    void Clients_model::clear()
    {
        this->beginRemoveRows(QModelIndex(), 0, m_clients.count());
        m_clients.clear();
        this->endRemoveRows();
    }
}
