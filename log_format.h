#ifndef LOG_FORMAT_H
#define LOG_FORMAT_H


#include <QString>
#include <QSharedPointer>

#include <log_item.h>
#include <log_column.h>
#include <start_stop_buffer.h>

namespace Log_viewer
{

    class Log_format : public QObject
    {
        Q_OBJECT

    public:
        explicit Log_format(QObject *parent = 0);
        virtual QString get_description() const = 0;
        virtual QString get_start_seq() const = 0;
        virtual QString get_stop_seq() const = 0;
        virtual QString get_origin() const = 0;
        virtual bool match(const QString first_line) const = 0;
        virtual Log_columns get_columns() const = 0;
        virtual void add_line(const QString line);
        virtual ~Log_format() {}

    signals:
        void log_found(QSharedPointer<Log_item> item);

    public slots:
        void on_line_found(const QString value);
    private:
        // Hide copy constructor
        Log_format(const Log_format&) : QObject(0) {}

        virtual QSharedPointer<Log_item> create_log_item(const QString value, const QString origin) const = 0;
        QSharedPointer<Start_stop_buffer> m_start_stop_buffer;
    };
}


#endif // LOG_FORMAT_H
