#include "log_item_preview_textedit.h"

Log_item_preview_textEdit::Log_item_preview_textEdit(QWidget *parent) :
    QTextEdit(parent)
{

}

void Log_item_preview_textEdit::show_item(const QString& row,
                                          const QString& text,
                                          const QString& type,
                                          const QString& file,
                                          const QString& line,
                                          const QString& origin,
                                          const String_color_list& list,
                                          const QString& filter_text)
{
    const QString span_tag = "<span style=\"background-color:%0\">%1</span>";

    QString content = text;

    if(!filter_text.isEmpty())
    {
        content.replace(filter_text,
                     QString(span_tag).arg(QColor("gray").name()).arg(filter_text),
                     Qt::CaseInsensitive);
    }

    QListIterator<String_color_pair > i(list);
    while (i.hasNext())
    {
        String_color_pair pair = i.next();
        QColor color = pair.second;
        QString highlight_text = pair.first;
        content.replace(highlight_text,
                        QString(span_tag).arg(color.name()).arg(highlight_text),
                        Qt::CaseInsensitive);
    }

    content = QString("<h4>Index: %0 Type: %1</h4> %2 <hr />").arg(row).arg(type).arg(content);
    QString file_line = QString("%0 (%1)").arg(file).arg(line);
    content = QString("%0<ul> <li>File: %1</li> <li>Origin: %2</li> </ul>").arg(content).arg(file_line).arg(origin);

    setHtml(content);
}

void Log_item_preview_textEdit::clear_item()
{
    clear();
}
