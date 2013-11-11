#include <log_items_model.h>
#include <QBrush>

namespace Log_viewer
{

    Log_items_model::Log_items_model(QObject *parent):
            QAbstractTableModel(parent),
            m_type_highlight_enabled(true),
            m_use_regex_for_highlight(false)
    {

    }

    // ----------------------------------------------------------------------------

    int Log_items_model::rowCount(const QModelIndex& parent) const
    {
        if(parent.isValid())
            return 0;
        else
            return m_items.count();
    }

    // ----------------------------------------------------------------------------

    int Log_items_model::columnCount(const QModelIndex& parent) const
    {
        Q_UNUSED(parent);

        int count = m_columns.size();
        return count;
    }

    // ----------------------------------------------------------------------------

    QVariant Log_items_model::data(const QModelIndex& index, int role) const
    {

        switch(role)
        {
        case Qt::DisplayRole:
            {
                QSharedPointer<Log_item> item = m_items.at(index.row());
                if(m_columns.at(index.column())->type == column_index)
                {
                    return item->get_index();
                }
                else
                {
                    return m_columns.at(index.column())->get_value_for_log_item(*item);
                }
            }
        case Qt::BackgroundRole:
            // This enables the highlight of text
            if((m_columns.at(index.column())->type == column_text) && !m_highlight_text_list.isEmpty())
            {
                QListIterator<Highlight_text_pair> iterator(m_highlight_text_list);
                while(iterator.hasNext())
                {
                    Highlight_text_pair highlight = iterator.next();
                    if(m_use_regex_for_highlight)
                    {
                        QRegExp reg_exp(highlight.first);
                        if (reg_exp.indexIn(m_items.at(index.row())->get_text()) != -1)
                        {
                            return QVariant(highlight.second);
                        }
                    }
                    else
                    {
                        if(m_items.at(index.row())->get_text().contains(highlight.first, Qt::CaseInsensitive))
                        {
                            return QVariant(highlight.second);
                        }
                    }
                }
            }

            // color code by type
            if(m_type_highlight_enabled)
            {
                QColor color;
                switch(m_items.at(index.row())->get_log_type())
                {

                    case lt_info :
                        color.setNamedColor("dimgray");
                        break;
                    case lt_warning :
                        color.setNamedColor("khaki");
                        break;
                    case lt_error :
                        color.setNamedColor("lightcoral");
                        break;
                    case lt_fatal :
                        color.setNamedColor("red");
                        break;
                    case lt_trace :
                        color.setNamedColor("aquamarine");
                        break;
                    case lt_debug :
                        color.setNamedColor("lightsteelblue");
                        break;
                    case lt_always :
                        color.setNamedColor("lavender");
                        break;
                    case lt_application : return QVariant(Qt::gray);
                    case lt_unknown: return QVariant(Qt::gray);
                    case lt_none : return QVariant(Qt::gray);
                    default : return QVariant(Qt::white);
                }
                return QVariant(color.darker(150));
            }
            else
            {
                if(m_items.at(index.row())->get_log_type() == lt_application)
                {
                    return QVariant(Qt::gray);
                }
                else
                {
                    return QVariant(Qt::white);
                }
            }
            break;
        case Qt::ForegroundRole:
        {
            QColor color;
            switch(m_items.at(index.row())->get_log_type())
            {
                case lt_info :
                    color.setNamedColor("silver");
                    break;
                case lt_warning :
                    color.setNamedColor("khaki");
                    break;
                case lt_error :
                    color.setNamedColor("lightpink");
                    break;
                case lt_fatal :
                    color.setNamedColor("beige");
                    break;
                case lt_trace :
                    color.setNamedColor("darkslategray");
                    break;
                case lt_debug :
                    color.setNamedColor("darkslategray");
                    break;
                case lt_always :
                    color.setNamedColor("darkslategray");
                    break;
                case lt_application : return QVariant(Qt::white);
                case lt_unknown: return QVariant(Qt::white);
                case lt_none : return QVariant(Qt::white);
                default : return QVariant(Qt::white);
            }
            return QVariant(color);
        }
        default:
            return QVariant();
            break;
        }

    }

    // ----------------------------------------------------------------------------

    QVariant Log_items_model::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        {
            return m_columns.at(section)->caption;
        }
        else if ((role == Qt::SizeHintRole)  && (orientation == Qt::Horizontal))
        {
            return QVariant(m_columns.at(section)->size_hint);
        }
        return QVariant();
    }

    // ----------------------------------------------------------------------------

    void Log_items_model::add(QSharedPointer<Log_item> item)
    {
        int count = m_items.count();
        this->beginInsertRows(QModelIndex(), count, count);
        m_items<< item;
        item->set_index(count);
        this->endInsertRows();
    }

    // ----------------------------------------------------------------------------

    void Log_items_model::add(const Log_file_parser::Log_items& items)
    {
        int count = m_items.count();
        this->beginInsertRows(QModelIndex(), count, count + items.size() - 1);

        for(int i = 0; i < items.size(); i++)
        {
            QSharedPointer<Log_item> item = items.at(i);
            m_items << item;
            item->set_index(count + i);
        }
        this->endInsertRows();
    }

    // ----------------------------------------------------------------------------

    void Log_items_model::clear()
    {
        this->beginResetModel();
        m_items.clear();
        this->endResetModel();
    }

    // ----------------------------------------------------------------------------

    void Log_items_model::add_highlight(const QColor& color, const QString& text)
    {
        if(text.isEmpty())
        {
            clear_highlight(color);
        }
        else
        {
            Highlight_text_pair pair(text, color);
            m_highlight_text_list.push_back(pair);
        }
    }

    // ----------------------------------------------------------------------------

    void Log_items_model::clear_highlight(const QColor& color)
    {
        for(int i = 0; i < m_highlight_text_list.size(); i++)
        {
            if(m_highlight_text_list.at(i).second == color)
            {
                m_highlight_text_list.removeAt(i);
                break;
            }
        }
    }

    // ----------------------------------------------------------------------------

    void Log_items_model::clear_all_highlights()
    {
        m_highlight_text_list.clear();
    }

    // ----------------------------------------------------------------------------

    int Log_items_model::get_column_index_from_type(Column_type type)
    {
        int col = 0;
        for(int i = 0; i < m_columns.size(); i++)
        {
            if(m_columns.at(i)->type == type)
            {
                col = i;
                break;
            }
        }
        return col;
    }

    // ----------------------------------------------------------------------------

    void Log_items_model::set_type_highlight(bool enabled)
    {
        m_type_highlight_enabled = enabled;

        end_highlight();
    }

    // ----------------------------------------------------------------------------

    void Log_items_model::begin_highlight()
    {        
        //beginResetModel();
    }

    // ----------------------------------------------------------------------------

    void Log_items_model::end_highlight()
    {
        int col = get_column_index_from_type(column_text);
        dataChanged(createIndex(0, col), createIndex(m_items.size()-1, col));
    }

    void Log_items_model::set_use_regex_for_highlight(bool enabled)
    {
        m_use_regex_for_highlight = enabled;
    }
}
