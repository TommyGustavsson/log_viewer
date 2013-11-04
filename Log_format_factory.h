#ifndef LOG_FORMAT_FACTORY_H
#define LOG_FORMAT_FACTORY_H

#include <log_format.h>
#include <QSharedPointer>

namespace Log_viewer
{
    class Log_format_factory
    {
    public:
        Log_format_factory() {}

        static Log_format_factory *instance;

        QSharedPointer<Log_format> create(const QString first_line, const QString origin);
    private:
        // hide copy constructor
        Log_format_factory(const Log_format_factory&) {}
    };
}

#endif // LOG_FORMAT_FACTORY_H
