#ifndef CLIENTS_MODEL_H
#define CLIENTS_MODEL_H

#include <log_client.h>

#include <QAbstractListModel>
#include <QSharedPointer>
#include <QList>

namespace Log_viewer
{

    class Clients_model : public QAbstractListModel
    {
        Q_OBJECT
    public:
        explicit Clients_model(QObject *parent = 0);

        // inherited
        int rowCount( const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;

        void add(QSharedPointer<Log_client> client);
        void remove(const Log_client* client);
        void clear();
    signals:

    public slots:

    private:
        QList< QSharedPointer<Log_client> > m_clients;

    };
}

#endif // CLIENTS_MODEL_H
