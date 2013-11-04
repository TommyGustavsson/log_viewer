#ifndef LOG_TABLE_VIEW_H
#define LOG_TABLE_VIEW_H

#include <QTableView>
#include <QStringList>

class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QAbstractItemModel;

class Log_table_view : public QTableView
{
    Q_OBJECT
public:
    explicit Log_table_view(QWidget *parent = 0);

    // inherited, override
    void setModel(QAbstractItemModel* model);
    void setAutoScrolling(bool enable);
    bool autoScrolling() { return m_auto_scrolling; }
signals:
    void files_dropped(const QStringList& files);
    void enable_scroll_lock(bool enable);
    void selected_row(const QString& row,
                      const QString& text,
                      const QString& type,
                      const QString& file,
                      const QString& line,
                      const QString& origin);

public slots:
    void on_scroll_valueChanged( int );
    void rowsInserted ( const QModelIndex &, int, int );

protected:
    // inherited, override
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void currentChanged(const QModelIndex &current,const QModelIndex &previous);

private:
    QStringList m_files_dropped;

    QScrollBar* m_scrollbar;
    bool m_auto_scrolling;

};

#endif // LOG_TABLE_VIEW_H
