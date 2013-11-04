#ifndef START_STOP_BUFFER_H
#define START_STOP_BUFFER_H

#include <QObject>

namespace Log_viewer
{

    class Start_stop_buffer : public QObject
    {
        Q_OBJECT

    public:
        Start_stop_buffer(QObject* parent);
        Start_stop_buffer(const QString start_seq, const QString stop_seq);

        void set_start_seq(const QString value);
        void set_stop_seq(const QString value);

        void add(const QString data);

    signals:
        void string_found(const QString value);

    private:
        QString m_buffer;
        QString m_start_seq;
        QString m_stop_seq;
    };
}

#endif // START_STOP_BUFFER_H
