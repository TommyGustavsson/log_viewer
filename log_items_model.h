#ifndef LOG_ITEMS_MODEL_H
#define LOG_ITEMS_MODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QVector>
#include <log_item.h>
#include <log_file_parser.h>
#include <QColor>


namespace Log_viewer
{

    class Log_items_model : public QAbstractTableModel
    {
    public:
        typedef QPair<QString, QColor> Highlight_text_pair;
        typedef QList<Highlight_text_pair> Color_list;

    public:
        explicit Log_items_model(QObject *parent = 0);

        // inherited
        int rowCount(const QModelIndex& parent) const;
        int columnCount(const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;
        QVariant headerData(int section,
                            Qt::Orientation orientation,
                            int role) const;

        void set_columns(const Log_columns& columns) {
            m_columns = columns;
        }

        void add(QSharedPointer<Log_item> item);
        void add(const Log_file_parser::Log_items& items);        
        void clear();
        QSharedPointer<Log_item> get_item_at(int index) {
            return m_items.at(index);
        }

        void add_highlight(const QColor& color, const QString& text);
        void clear_highlight(const QColor& color);
        void clear_all_highlights();
        void begin_highlight();
        void end_highlight();

        const Color_list get_highlights() const {
            return m_highlight_text_list;
        }

        void set_type_highlight(bool enabled);
        void set_use_regex_for_highlight(bool enabled);
        bool get_use_regex_for_highlight() {
            return m_use_regex_for_highlight;
        }

        int get_column_index_from_type(Column_type type);

    private:
        QList< QSharedPointer<Log_item> > m_items;
        Color_list m_highlight_text_list;
        bool m_type_highlight_enabled;
        bool m_use_regex_for_highlight;
        Log_columns m_columns;
    };
}

#endif // LOG_ITEMS_MODEL_H
