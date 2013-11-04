#include <tail.h>

#include <QFile>
#include <QTextStream>
#include <Log_format_factory.h>

namespace Log_viewer
{
    Tail::Tail(const QString& file,
               qint64 file_pos,
               QSharedPointer<Log_format> log_format,
               QObject *parent) :
        QObject(parent),
        m_log_format(log_format),
        m_watcher(this),
        m_file_name(file),
        m_file_stream_end(file_pos)
    {
        m_watcher.addPath(file);

        connect(&m_watcher,
                SIGNAL(fileChanged(QString)),
                this,
                SLOT(on_file_changed(QString)));
    }

    void Tail::on_file_changed(const QString& file)
    {
        // Bug in QT QFileSystemWatcher?
        m_watcher.removePath(file);
        m_watcher.addPath(file);

        QFile changed_file(file);
        if (!changed_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            emit tail_failed(QString("Tail failed, could not open file: %0").arg(file));
            return;
        }

        try
        {
            qint64 file_size = changed_file.size();
            if((file_size < m_file_stream_end))
            {
                emit tail_empty();
                m_file_stream_end = 0;
            }
            else
            {
                QTextStream file_stream(&changed_file);
                file_stream.seek(m_file_stream_end);

                while (!file_stream.atEnd())
                {
                    QString line = file_stream.readLine();

                    // create + add new item
                    if(!line.isEmpty())
                    {
                        m_log_format->add_line(line);
                    }
                }
                m_file_stream_end = file_stream.pos();
            }
            changed_file.close();
        }
        catch(...)
        {
            changed_file.close();
        }
    }

    // ----------------------------------------------------------------------------

    void Tail::on_log_found(QSharedPointer<Log_item> item)
    {
        emit tail_log_found(item);
    }
}

