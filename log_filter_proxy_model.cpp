#include <log_filter_proxy_model.h>
#include <log_item.h>

#include <QRegExp>
#include <log_items_model.h>
#include <QStringList>

namespace Log_viewer
{

    Log_filter_proxy_model::Log_filter_proxy_model(QObject *parent) :
        QSortFilterProxyModel(parent),
        m_text_filter(""),
        m_use_regex(false)
    {
    }

    // ----------------------------------------------------------------------------

    bool Log_filter_proxy_model::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
    {
        Q_UNUSED(source_parent);

        Log_items_model* items_model = dynamic_cast<Log_items_model*>(sourceModel());
        QString module = items_model->get_item_at(source_row)->get_module();
        QString file = items_model->get_item_at(source_row)->get_file();
        int index = items_model->get_item_at(source_row)->get_index();

        return (
                ((m_type_filter.size() == 0) || type_filter(source_row)) &&
                ((m_text_filter.isEmpty()) || text_filter(source_row)) &&
                ((m_module_filter.isEmpty()) || module.contains(m_module_filter, Qt::CaseInsensitive)) &&
                ((m_file_filter.isEmpty()) || file.contains(m_file_filter, Qt::CaseInsensitive)) &&
                ((m_index_filter.isEmpty()) || ((index >= m_index_min) && (index <= m_index_max)))
                );

    }

    // ----------------------------------------------------------------------------

    void Log_filter_proxy_model::set_text_filter(const QString& text)
    {
        m_text_filter = text;
    }

    // ----------------------------------------------------------------------------

    void Log_filter_proxy_model::set_index_filter(const QString& index)
    {
        QStringList index_list = index.split("-");
        if(index_list.size() == 1) {
            m_index_filter = index;
            QString index_str = index_list[0];
            int first_index = index_str.toInt();
            m_index_max = first_index;
            m_index_min = first_index;
        }
        else if (index_list.size() == 2) {
            m_index_filter = index;
            QString index_str = index_list[0];
            int first_index = index_str.toInt();
            index_str = index_list[1];
            int second_index = index_str.toInt();
            m_index_filter = index;
            m_index_max = second_index;
            m_index_min = first_index;
        }
        else {
            m_index_filter = "";
            m_index_max = 0;
            m_index_min = 0;
        }

    }

    // ----------------------------------------------------------------------------

    void Log_filter_proxy_model::set_module_filter(const QString& module)
    {
        m_module_filter = module;
    }

    // ----------------------------------------------------------------------------

    void Log_filter_proxy_model::set_file_filter(const QString& file)
    {
        m_file_filter = file;
    }

    // ----------------------------------------------------------------------------

    void Log_filter_proxy_model::add_type_filter(Log_type type)
    {        
        m_type_filter.push_back(type);
    }

    // ----------------------------------------------------------------------------

    void Log_filter_proxy_model::clear_type_filter()
    {
        m_type_filter.clear();
    }

    void Log_filter_proxy_model::clear_filter()
    {
        m_type_filter.clear();
        m_text_filter.clear();
        m_file_filter.clear();
        m_module_filter.clear();
        m_index_filter.clear();

        emit filter_cleared();
    }

    // ----------------------------------------------------------------------------

    void Log_filter_proxy_model::apply()
    {
        invalidate();
    }

    // ----------------------------------------------------------------------------

    bool Log_filter_proxy_model::type_filter(int source_row) const
    {
        Log_items_model* items_model = dynamic_cast<Log_items_model*>(sourceModel());

        foreach(Log_type type, m_type_filter)
        {
            if(type == items_model->get_item_at(source_row)->get_log_type())
                return true;
        }

        return false;
    }

    // ----------------------------------------------------------------------------

    bool Log_filter_proxy_model::text_filter(int source_row) const
    {
        Log_items_model* items_model = dynamic_cast<Log_items_model*>(sourceModel());
        QString text = items_model->get_item_at(source_row)->get_text();
        //QString text = sourceModel()->index(source_row, column_text).data().toString();

        if(m_use_regex)
        {
            QRegExp reg_exp(m_text_filter);
            return (reg_exp.indexIn(text) != -1);
        }
        else
        {
            return text.contains(m_text_filter, Qt::CaseInsensitive);
        }
    }

    // ----------------------------------------------------------------------------

    void Log_filter_proxy_model::set_use_regex(bool enabled)
    {
        m_use_regex = enabled;
    }
}
