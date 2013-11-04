#ifndef LOG_ITEM_PREVIEW_TEXTEDIT_H
#define LOG_ITEM_PREVIEW_TEXTEDIT_H

#include <QTextEdit>
#include <QMap>

class Log_item_preview_textEdit : public QTextEdit
{
    Q_OBJECT
public:
    typedef QPair<QString, QColor> String_color_pair;
    typedef QList<String_color_pair > String_color_list;

    explicit Log_item_preview_textEdit(QWidget *parent = 0);

    void show_item(const QString& row,
                   const QString& text,
                   const QString& type,
                   const QString& file,
                   const QString& line,
                   const QString& origin,
                   const String_color_list& list,
                   const QString& filter_text);
    void clear_item();

signals:

public slots:

private:

};

#endif // LOG_ITEM_PREVIEW_TEXTEDIT_H
