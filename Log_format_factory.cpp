#include <Log_format_factory.h>

#include <singleton.h>
#include <g3_log_format.h>
#include <nrg_log_format.h>
#include <nrg_legacy_log_format.h>
#include <unknown_log_format.h>

namespace Log_viewer
{

    // Instantiate singleton
    Log_format_factory* Log_format_factory::instance = &Singleton<Log_format_factory>::Instance();

    QSharedPointer<Log_format> Log_format_factory::create(const QString first_line, const QString origin)
    {
        QSharedPointer<Log_format> format;

        format = QSharedPointer<G3_log_format>(new G3_log_format(origin));
        if(format->match(first_line))
            return format;

        format = QSharedPointer<NRG_log_format>(new NRG_log_format(origin));
        if(format->match(first_line))
            return format;

        format = QSharedPointer<NRG_legacy_log_format>(new NRG_legacy_log_format(origin));
        if(format->match(first_line))
            return format;

        // Add new format here...

        return QSharedPointer<Unknown_log_format>(new Unknown_log_format());
    }
}
