#ifndef G3_LOG_FORMAT_H
#define G3_LOG_FORMAT_H

#include <log_format.h>
#include <g3_log_item.h>
#include <QString>
#include <QSharedPointer>


namespace Log_viewer
{

    class G3_log_format: public Log_format
    {
    public:
        explicit G3_log_format(const QString origin, QObject *parent = 0) :
                Log_format(parent),
                m_origin(origin),
                m_start_seq(":)"),
                m_stop_seq(":("),
                m_separator("/"),
                m_description("G3 legacy log format")
        {
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_index)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_type)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_text)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_date)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_time)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_application)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_module)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_file)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_line)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_thread)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_origin)));
        }

        bool match(const QString first_line) const {
            return (first_line.contains(m_start_seq));
        }

        QString get_start_seq() const {
            return m_start_seq;
        }

        QString get_stop_seq() const {
            return m_stop_seq;
        }

        QString get_description() const {
            return m_description;
        }

        Log_columns get_columns() const {
            return m_log_columns;
        }

        ~G3_log_format() {}
    private:
        // Hide copy constructor
        G3_log_format(const G3_log_format&) : Log_format() {}

        QSharedPointer<Log_item> create_log_item(const QString value, const QString origin) const {
            return QSharedPointer<G3_log_item>(new G3_log_item(value, m_separator, origin));
        }

        QString m_origin;
        QString m_start_seq;
        QString m_stop_seq;
        QString m_separator;
        QString m_description;

        Log_columns m_log_columns;

    };
}

#endif // G3_LOG_FORMAT_H
