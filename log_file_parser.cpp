#include <log_file_parser.h>

#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <g3_log_format.h>
#include <nrg_log_format.h>
#include <nrg_legacy_log_format.h>
#include <Log_format_factory.h>
#include <app_log_item.h>

namespace Log_viewer
{

    Log_file_parser::Log_file_parser(QObject *parent) :
        QObject(parent), m_file_name(""), m_opening(false)
    {

    }

    // ----------------------------------------------------------------------------

    bool Log_file_parser::get_log_format(const QString line)
    {
        m_log_format = Log_format_factory::instance->create(line, m_file_name);

        connect(m_log_format.data(),
                SIGNAL(log_found(Log_item_ptr)),
                this,
                SLOT(on_log_found(Log_item_ptr)));
        emit format_selected(m_log_format);

        return true;
    }

    // ----------------------------------------------------------------------------

    void Log_file_parser::open(const QString file)
    {
        m_file_name = file;

        QFile log_file(file);
        if (!log_file.open(QIODevice::ReadOnly))
        {
            emit error(QString("Failed to open file %0").arg(m_file_name));
            return;
        }

        m_opening = true;
        try
        {
            QTextStream text_stream(&log_file);

            open_text(text_stream);

            m_opening = false;
            emit file_opened(m_file_name, text_stream.pos());
        }
        catch(...)
        {
            m_opening = false;
        }

    }

    void Log_file_parser::open_text(QTextStream& text_stream)
    {
        if(text_stream.atEnd())
        {
            emit error(QString("Empty log text"));
            return;
        }

        if(!get_log_format(text_stream.read(100)))
        {
            emit error(QString("Error reading log text: %0").arg(m_log_format->get_description()));
            return;
        }

        m_log_items.clear();
        clear_statistics();

        text_stream.seek(0);

        while (!text_stream.atEnd())
        {
            m_log_format->add_line(text_stream.read(200), m_file_name);
        }

        if(m_log_items.size() > 0)
        {
            emit logs_found(m_log_items);
            m_log_items.clear();
        }
    }

    // ----------------------------------------------------------------------------

    void Log_file_parser::cancel()
    {

    }

    // ----------------------------------------------------------------------------

    void Log_file_parser::on_log_found(Log_item_ptr item)
    {
        m_log_items.push_back(item);
        // calculate_statistics(item);

        if((m_opening == false) || (m_log_items.size() >= 1000))
        {
            emit logs_found(m_log_items);
            m_log_items.clear();
        }
    }

    // ----------------------------------------------------------------------------

    void Log_file_parser::add_file_info()
    {
        QFileInfo file_info(m_file_name);

        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item("", "", "")));
        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item(QString("File: %0").arg(file_info.fileName()), "", "")));
        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item(QString("Path: %0").arg(file_info.absolutePath()), "", "")));
        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item(QString("Size: %0").arg(format_file_size(file_info.size())), "", "")));
        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item(QString("Last modified: %0").arg(file_info.lastModified().toString()), "", "")));
        if(!file_info.isWritable())
        {
            m_log_items.push_back(QSharedPointer<App_log_item>(
                    new App_log_item("File is read only.", "", "")));
        }
        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item(QString("Log format: %0").arg(m_log_format->get_description()), "", "")));
        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item("", "", "")));

        emit logs_found(m_log_items);
        m_log_items.clear();
    }

    // ----------------------------------------------------------------------------

    void Log_file_parser::add_statistics()
    {
        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item("", "", "")));
        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item(QString("Lines: %0 Fatal: %1 Errors: %2 Warnings: %3").
                                 arg(m_no_of_lines).
                                 arg(m_no_of_fatal).
                                 arg(m_no_of_errors).
                                 arg(m_no_of_warnings), "", "")));
        m_log_items.push_back(QSharedPointer<App_log_item>(
                new App_log_item("", "", "")));

        emit logs_found(m_log_items);
        m_log_items.clear();
    }

    // ----------------------------------------------------------------------------

    void Log_file_parser::calculate_statistics(Log_item_ptr item)
    {
        switch(item->get_log_type())
        {
            case lt_warning :
                m_no_of_warnings++;
                break;
            case lt_error :
                m_no_of_errors++;
                break;
            case lt_fatal :
                m_no_of_fatal++;
                break;
            default:
                break;
        }

        m_no_of_lines++;
    }

    // ----------------------------------------------------------------------------

    void Log_file_parser::clear_statistics()
    {
        m_no_of_errors = 0;
        m_no_of_warnings = 0;
        m_no_of_fatal = 0;
        m_no_of_lines = 0;
    }

    // ----------------------------------------------------------------------------

    QString Log_file_parser::format_file_size(qint64 file_size)
    {
        const int dwKB = 1024;          // Kilobyte
        const int dwMB = 1024 * dwKB;   // Megabyte
        const int dwGB = 1024 * dwMB;   // Gigabyte

        int dwNumber;
        QString strNumber;

        if (file_size < dwKB)
        {
            strNumber = QString("%0 B").arg(file_size);
        }
        else
        {
            if (file_size < dwMB)
            {
              dwNumber = file_size / dwKB;
              strNumber = QString("%0.2 KB").arg(dwNumber);
            }
            else
            {
                if (file_size < dwGB)
                {
                    dwNumber = file_size / dwMB;
                    strNumber = QString("%0.2 MB").arg(dwNumber);
                }
                else
                {
                    if (file_size >= dwGB)
                    {
                        dwNumber = file_size / dwGB;
                        strNumber = QString("%0.2 GB").arg(dwNumber);
                    }
                }
            }
        }

        return strNumber;
    }

}
