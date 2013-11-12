#ifndef NRG_LOG_FORMAT_H
#define NRG_LOG_FORMAT_H

#include <log_format.h>
#include <nrg_log_item.h>
#include <QString>
#include <QSharedPointer>
#include <QRegExp>

namespace Log_viewer
{

    struct NRG_log_format: public Log_format
    {
        explicit NRG_log_format(const QString origin, QObject *parent = 0) :
                Log_format(parent),
                m_origin(origin),
                m_start_seq("\2"),
                m_stop_seq("\3"),
                m_separator("\t"),
                m_description("NRG log format")
        {
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_index)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_type)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_text)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_date)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_application)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_module)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_file)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_thread)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_origin)));
        }

        /*!
           This method returns true if we can match the passed line.

            A regular expression is used to match the first_line.
            NRG format expects the line to start with "start of text" 0x02, followed by a date formated like this:
            \par 2010-09-08 12:06:12,079864

            and shall be followed by TAB char after the date.
         */

        bool match(const QString first_line) const {
            QRegExp reg_exp("\\x02\\d\\d\\d\\d-\\d\\d-\\d\\d[ ]\\d\\d[:]\\d\\d[:]\\d\\d[,]\\d\\d\\d\\t");
            return (reg_exp.indexIn(first_line) != -1);
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

        ~NRG_log_format() {}
    private:
        // Hide copy constructor
        NRG_log_format(const NRG_log_format&) : Log_format() {}

        QSharedPointer<Log_item> create_log_item(const QString value, const QString origin) const {
            return QSharedPointer<NRG_log_item>(new NRG_log_item(value, m_separator, origin));
        }

        QString m_origin;
        QString m_start_seq;
        QString m_stop_seq;
        QString m_separator;
        QString m_description;

        Log_columns m_log_columns;
    };
}

#endif // NRG_LOG_FORMAT_H
