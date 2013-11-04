#ifndef LOG_FILTER_PROXY_MODEL_H
#define LOG_FILTER_PROXY_MODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>
#include <log_item.h>

namespace Log_viewer
{

    class Log_filter_proxy_model : public QSortFilterProxyModel
    {
        Q_OBJECT
    public:
        explicit Log_filter_proxy_model(QObject *parent = 0);

        // override
        bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
        void set_text_filter(const QString& text);
        void set_index_filter(const QString& index);
        void set_module_filter(const QString& module);
        void set_file_filter(const QString& file);
        void add_type_filter(Log_type type);
        void clear_type_filter();
        void clear_filter();

        void apply();

        void set_use_regex(bool enabled);
        QString get_text_filter() {
            return m_text_filter;
        }

    signals:
        void filter_cleared();

    public slots:

    private:
        bool type_filter(int source_row) const;
        bool text_filter(int source_row) const;

        QString m_text_filter;
        QString m_index_filter;
        QString m_module_filter;
        QString m_file_filter;
        int m_index_min;
        int m_index_max;
        QList<Log_type> m_type_filter;
        int m_column;
        bool m_use_regex;
    };
}

#endif // LOG_FILTER_PROXY_MODEL_H
