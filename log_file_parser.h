#ifndef LOG_FILE_PARSER_H
#define LOG_FILE_PARSER_H

#include <QObject>
#include <QSharedPointer>
#include <QTextStream>
#include <log_item.h>
#include <log_format.h>

namespace Log_viewer
{

    class Log_file_parser : public QObject
    {
        Q_OBJECT
    public:
        typedef QList<QSharedPointer<Log_item> > Log_items;

        explicit Log_file_parser(QObject *parent = 0);
        virtual ~Log_file_parser() {}

        void open(const QString file);
        void open_text(QTextStream& text_stream);
        void cancel();
        QSharedPointer<Log_format> get_current_log_format() {
            return m_log_format;
        }

    signals:
        void logs_found(const Log_file_parser::Log_items& log_items);
        void format_selected(QSharedPointer<Log_format> format);
        void file_opened(const QString file, qint64 size);
        void error(const QString error);
        void file_open_progress(int progress);

    private slots:
        void on_log_found(QSharedPointer<Log_item> item);

    private:
        bool get_log_format(const QString line);
        void add_file_info();
        void add_statistics();
        void calculate_statistics(QSharedPointer<Log_item> item);
        void clear_statistics();
        QString format_file_size(qint64 file_size);

        QSharedPointer<Log_format> m_log_format;
        QString m_file_name;
        Log_items m_log_items;
        bool m_opening;

        // statistics
        int m_no_of_errors;
        int m_no_of_warnings;
        int m_no_of_fatal;
        int m_no_of_lines;
    };
}

#endif // LOG_FILE_PARSER_H
