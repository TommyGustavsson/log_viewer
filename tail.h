#ifndef TAIL_H
#define TAIL_H

#include <QObject>
#include <log_format.h>
#include <QSharedPointer>
#include <QFileSystemWatcher>

namespace Log_viewer
{

    class Tail : public QObject
    {
        Q_OBJECT
    public:
        Tail(const QString& file,
             qint64 file_pos,
             QSharedPointer<Log_format> log_format,
             QObject *parent = 0);

        virtual ~Tail() {}

    signals:
        void tail_log_found(QSharedPointer<Log_item> item);
        void tail_empty();
        void tail_failed(const QString& message);

    private slots:
        void on_file_changed(const QString& file);
        void on_log_found(QSharedPointer<Log_item> item);

    private:
        // Hide copy constructor
        Tail(const Tail&): QObject(0) {}

        QSharedPointer<Log_format> m_log_format;
        QFileSystemWatcher m_watcher;
        QString m_file_name;
        int m_file_stream_end;
    };
}
#endif // TAIL_H
