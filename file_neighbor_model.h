#ifndef FILE_NEIGHBOR_MODEL_H
#define FILE_NEIGHBOR_MODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QList>
#include <QFileInfo>
#include <QFileSystemWatcher>

namespace Log_viewer
{

    class File_neighbor_model : public QAbstractListModel
    {
        Q_OBJECT
    public:
        explicit File_neighbor_model(QObject *parent = 0);

        // inherited
        int rowCount( const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;

        void populate(const QString& file_name);
        void add(const QString& file);
        void remove(int index);
        void clear();
        const QFileInfo get_item(const QModelIndex& index) const;

        void set_active_item(int item);
        void set_active_item(const QString& file_name);
        int get_active_item() {
            return m_active_item;
        }

        void clear_active_item();

    signals:
        void populated();
    public slots:
        void on_neighbor_folder_changed(const QString& folder);

    private:
        void start_watching_folder(const QString& folder);

        QList<QFileInfo> m_neighbors;
        int m_active_item;
        QFileSystemWatcher m_watcher;
        QString m_current_folder;
    };
}

#endif // FILE_NEIGHBOR_MODEL_H
