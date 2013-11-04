#ifndef LOG_COLUMN_H
#define LOG_COLUMN_H

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QSize>


namespace Log_viewer
{
    class Log_item;

    enum Column_type
    {
        column_index = 0,
        column_type,
        column_text,
        column_date,
        column_time,
        column_file,
        column_method,
        column_line,
        column_application,
        column_module,
        column_thread,
        column_origin,
        column_count
    };

    struct Log_column
    {
        explicit Log_column(Column_type a_type);

        QString get_value_for_log_item(const Log_item& log_item) const;

        QString caption;
        Column_type type;
        QSize size_hint;
    private:
        // Hide copy constructor
        Log_column(const Log_column&) {}
    };

    typedef QList<QSharedPointer<Log_column> > Log_columns;

}

#endif // LOG_COLUMN_H
