#ifndef UNKNOWN_LOG_FORMAT_H
#define UNKNOWN_LOG_FORMAT_H

#include <log_format.h>
#include <QString>
#include <QSharedPointer>
#include <plain_text_log_item.h>

namespace Log_viewer
{

    struct Unknown_log_format: public Log_format
    {
        explicit Unknown_log_format(QObject *parent = 0) :
                Log_format(parent),
                m_start_seq(""),
                m_stop_seq("\n"),
                m_separator(""),
                m_description("Unknown log format")
        {
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_index)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_text)));
        }

        bool match(const QString) const {
            return true;
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

        QString get_origin() const {
            return "";
        }

        Log_columns get_columns() const {
            return m_log_columns;
        }

        ~Unknown_log_format() {}
    private:
        // Hide copy constructor
        Unknown_log_format(const Unknown_log_format&) : Log_format() {}

        QSharedPointer<Log_item> create_log_item(const QString value, const QString) const {
            return QSharedPointer<Plain_text_log_item>(new Plain_text_log_item(value));
        }

        QString m_start_seq;
        QString m_stop_seq;
        QString m_separator;
        QString m_description;

        Log_columns m_log_columns;
    };
}

#endif // UNKNOWN_LOG_FORMAT_H
