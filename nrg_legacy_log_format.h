#ifndef NRG_SC_LOG_FORMAT_H
#define NRG_SC_LOG_FORMAT_H

#include <log_format.h>
#include <nrg_legacy_log_item.h>
#include <QString>
#include <QSharedPointer>
#include <QRegExp>

namespace Log_viewer
{

    struct NRG_legacy_log_format: public Log_format
    {
        explicit NRG_legacy_log_format(const QString origin, QObject *parent = 0) :
                Log_format(parent),
                m_origin(origin),
                m_start_seq(""),
                m_stop_seq("\r\n"),
                m_separator("|"),
                m_description("Legacy NRG log format")
        {
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_index)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_type)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_text)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_file)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_line)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_method)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_date)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_application)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_module)));            
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_thread)));
            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_origin)));

            m_log_columns.push_back(QSharedPointer<Log_column>(new Log_column(column_time)));
        }

        /*!
           This method returns true if we can match the passed line.

            A regular expression is used to match the first_line.
            NRG lecacy format expects the line to start with a date formated like this:
            \par 2010-09-08 12:06:12,079

            and shall be followed by a SPACE and a pipe char after the date.
         */

        bool match(const QString first_line) const {
            QRegExp reg_exp("\\d\\d\\d\\d-\\d\\d-\\d\\d[ ]\\d\\d[:]\\d\\d[:]\\d\\d[,]\\d\\d\\d[ ][\\|]");
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

        ~NRG_legacy_log_format() {}
    private:
        // Hide copy constructor
        NRG_legacy_log_format(const NRG_legacy_log_format&) : Log_format() {}

        Log_item_ptr create_log_item(const QString value, const QString origin) const {
            return QSharedPointer<NRG_legacy_log_item>(new NRG_legacy_log_item(value, m_separator, origin));
        }

        QString m_origin;
        QString m_start_seq;
        QString m_stop_seq;
        QString m_separator;
        QString m_description;

        Log_columns m_log_columns;
    };
}

#endif // NRG_SC_LOG_FORMAT_H
