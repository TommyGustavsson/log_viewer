#ifndef DOC_MAIN_H
#define DOC_MAIN_H

/*!
    \mainpage

    LogViewer is developed to be able to quickly view and analyze log files or content from online log clients.

    \note
    A lot of emphasis has been put on providing a compelling look-and-feel to the application and to give the user
    a feeling of not feeling stupid using the application.

    \note
    One way of doing this is to provide more than one way of doing common tasks such as opening a log file, for
    example use short-cut, menu, toolbar, drag n' drop etc.

    \note
    Another way is to never popup any dialogs that informs the user that a entered value is out of range or
    prompts for input. Also make sure to disable all buttons and menus that the end user currently can't use. For
    example, if the filter input edit is empty the apply filer button is disabled, if the user starts to type the
    button is enabled. Use placeholderText in all edit fields to inform what's supposed to be entered.
    Example: the filter input has the placeholderText set to "Enter text to filter".


    <b>
    The documentation is intended to aid developers of the application, not to provide a user guide to the end-user.
    </b>

    The following sections are describing the implementation in more detail:
    - \ref GUI "The GUI"
    - \ref LogManager "The manager".
    - \ref LogFormats "Log Formats"

    A goal has been to follow the JSF Coding Standard
    (link: \link http://www2.research.att.com/~bs/JSF-AV-rules.pdf \endlink).


    \author Tommy Gustavsson
    \version 0.x

*/

/*!
    \page GUI The GUI

    The GUI is composed in the \ref Ui::Main_window "Main_window" class.
    No application logic shall be present in the \ref Ui::Main_window "Main_window" class, all application logic
    must be separated from the presentation. Use SIGNAL, SLOT, models etc. to accomplish this.

*/

/*!
    \page LogManager The manager

    The \ref Log_viewer::Log_manager "Log_manager" puts together the different parts of business logic that
    ends up acting like an application. The GUI part is only a visualization of this class.

    There are four models that provide access to data:
    - \ref Log_viewer::Log_items_model "Log_items_model": This model contains all log entries and defines the
        behaviour of such things as color typing, columns, text highlight.
    - \ref Log_viewer::Log_filter_proxy_model "Log_filter_proxy_model": This model is placed inbetween the
        QTableView and the \ref Log_viewer::Log_items_model "Log_items_model" to provide filtering.
      \code
        m_log_filter_proxy_model->setSourceModel(m_log_items_model);
      \endcode

    - \ref Log_viewer::Clients_model "Clients_model": This model holds the set of connected
        \ref Log_viewer::Log_client "Log_clients".
    - \ref Log_viewer::File_neighbor_model "File_neighbor_model": This models contains all files that are in the
        same folder as the last opened file.

    These models is accessable from the \ref Log_viewer::Log_manager "Log_manager" class.
    \code
        Log_items_model* log_items_model() const {
            return m_log_items_model;
        }

        Clients_model* clients_model() const {
            return m_clients_model;
        }

        Log_filter_proxy_model* log_filter_proxy_model() const {
            return m_log_filter_proxy_model;
        }

        File_neighbor_model* file_neighbor_model() const {
            return m_file_neighbor_model;
        }
    \endcode

    The \ref Log_viewer::Log_manager "Log_manager" is accessed through a \ref Singleton "singleton".
    \code
        Log_manager::instance->file_neighbor_model()
    \endcode

    The \ref Log_viewer::Log_manager "Log_manager" does not create any threads.
    Thread safety has not been the focus so far, perhaps this will need to be addressed. There are several
    operations that potentially can take a lot of time, such as filtering, highlightning, loading from file etc.
    With the current design the application will need to wait "freeze" until the operations has finished.

 */

/*!
    \page LogFormats Log Formats
    A log format is defined as a line of text where its data is separated by a single char or a string.
    The log format can also have a specific start and stop sequence to indicate start and end of text, this is
    especially good when using it over the network.

    \ref Log_viewer::Log_format "Log_format" is the interface for all inherited formats, override the pure virtual
    functions to implement a new format:
    \code
    public:
        virtual QString get_description() const = 0;
        virtual QString get_start_seq() const = 0;
        virtual QString get_stop_seq() const = 0;
        virtual QString get_origin() const = 0;
        virtual bool match(const QString first_line) const = 0;
    private:
        virtual QSharedPointer<Log_item> create_log_item(const QString value, const QString origin) const = 0;
    \endcode

    As seen above a \ref Log_viewer::Log_format "Log_format" class decendant is responsible for the creation
    of its log items.

    \ref Log_viewer::Log_item "Log_item" serves as the base class for log items. A log item is used to break down
    the line of text to its predefined data structure. The data structure will eventually end up as columns in
    the GUI. This is a list of the data structure, all access to these private variables is done with a getter.
    \code
        int m_index;
        Log_type m_type;
        QString m_text;
        QString m_date;
        QString m_time;
        QString m_file;
        QString m_line;
        QString m_application;
        QString m_module;
        QString m_thread;
        QString m_origin;
    \endcode

    In the name of simplicity for the end user all formats shall try to auto-sense its format when requested.
    To ensure a valid match regular expressions are used.
    \code
        virtual bool match(const QString first_line) const = 0;
    \endcode
    As an improvemnet the formats could be loaded on demand via QT plugin architecture.

    To add a format you need to modify the \ref Log_viewer::Log_format_factory "Log_format_factor" class.
    \code
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
    \endcode
 */

#endif // DOC_MAIN_H
