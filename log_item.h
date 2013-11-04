#ifndef LOG_ITEM_H
#define LOG_ITEM_H

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QSize>

namespace Log_viewer
{
    enum Log_type
    {
        lt_none,
        lt_info,
        lt_warning,
        lt_error,
        lt_fatal,
        lt_trace,
        lt_debug,
        lt_always,
        lt_unknown,
        lt_application
    };

    class Log_item
    {
    public:
        Log_item();
        virtual ~Log_item() {}

        static QString get_log_type_as_string(Log_type type);
        QString get_log_type_as_string() const;

        Log_type get_log_type() const {
            return m_type;
        }

        QString get_text() const {
            return m_text;
        }

        QString get_date() const {
            return m_date;
        }

        QString get_time() const {
            return m_time;
        }

        QString get_file() const {
            return m_file;
        }

        QString get_method() const {
            return m_method;
        }

        QString get_line() const {
            return m_line;
        }

        QString get_application() const {
            return m_application;
        }

        QString get_module() const {
            return m_module;
        }

        QString get_thread() const {
            return m_thread;
        }

        QString get_origin() const {
            return m_origin;
        }

        int get_index() const {
            return m_index;
        }

        void set_index(int index) {
            m_index = index;
        }

        QString get_as_string(const QString& separator) const;

    protected:
        virtual Log_type Log_type_from_string(const QString value) = 0;

        int m_index;
        Log_type m_type;
        QString m_text;
        QString m_date;
        QString m_time;
        QString m_file;
        QString m_method;
        QString m_line;
        QString m_application;
        QString m_module;
        QString m_thread;
        QString m_origin;
    };

}

#endif // LOG_ITEM_H
