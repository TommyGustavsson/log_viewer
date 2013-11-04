#include <log_table_view.h>

#include <log_filter_proxy_model.h>
#include <log_item.h>
#include <log_items_model.h>
#include <log_manager.h>

#include <QUrl>
#include <QHeaderView>
#include <QDragEnterEvent>
#include <QScrollBar>

Log_table_view::Log_table_view(QWidget *parent) :
    QTableView(parent), m_scrollbar(NULL), m_auto_scrolling(false)
{
    setAcceptDrops(true);
    horizontalHeader()->setMovable(true);

    m_scrollbar = this->verticalScrollBar();

    if(m_scrollbar != NULL)
    {
        connect(m_scrollbar, SIGNAL(valueChanged(int)),
                this,        SLOT(on_scroll_valueChanged(int)));
    }

    this->setAutoScroll(true);
}

// ----------------------------------------------------------------------------

void Log_table_view::setModel(QAbstractItemModel* model)
{
    QTableView::setModel(model);
    horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);


}

void Log_table_view::setAutoScrolling(bool enable)
{
    m_auto_scrolling = enable;
    if(m_auto_scrolling)
    {
        this->scrollToBottom();
    }
}

// ----------------------------------------------------------------------------

void Log_table_view::dragEnterEvent(QDragEnterEvent *event)
{
    setBackgroundRole(QPalette::Highlight);
    event->acceptProposedAction();
}

// ----------------------------------------------------------------------------

void Log_table_view::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

// ----------------------------------------------------------------------------

void Log_table_view::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        m_files_dropped.clear();
        QList<QUrl> urlList = mimeData->urls();
        QString text;
        for (int i = 0; i < urlList.size() && i < 32; ++i) {
            QString url = urlList.at(i).toLocalFile();
            m_files_dropped.push_back(url);
            break; // For now, only allow one file to be dropped at a time.
        }
        emit files_dropped(m_files_dropped);
    }
    setBackgroundRole(QPalette::NoRole);
    event->acceptProposedAction();
}

// ----------------------------------------------------------------------------

void Log_table_view::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void Log_table_view::currentChanged(const QModelIndex &current, const QModelIndex &)
{
    scrollTo(current);
    if(current.isValid())
    {
        QModelIndex source_index = Log_viewer::Log_manager::instance->log_filter_proxy_model()->mapToSource(current);
        QSharedPointer<Log_viewer::Log_item> item = Log_viewer::Log_manager::instance->log_items_model()->get_item_at(source_index.row());

        emit selected_row(QString("%0").arg(item->get_index()),
                          item->get_text(),
                          item->get_log_type_as_string(),
                          item->get_file(),
                          item->get_line(),
                          item->get_origin());
    }
}

void Log_table_view::on_scroll_valueChanged(int new_value)
{
    if(new_value >= m_scrollbar->maximum())
    {
        if(!m_auto_scrolling)
            emit enable_scroll_lock(true);

        m_auto_scrolling = true;
    } else {
        m_auto_scrolling = false;
        emit enable_scroll_lock(false);
    }
}
void Log_table_view::rowsInserted ( const QModelIndex &, int, int )
{
    if(m_auto_scrolling)
    {
        this->scrollToBottom();
    }
}

