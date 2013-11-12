#include <start_stop_buffer.h>

namespace Log_viewer
{

    // ----------------------------------------------------------------------------

    Start_stop_buffer::Start_stop_buffer(QObject* parent) : QObject(parent),
            m_start_seq(""), m_stop_seq("\r")
    {

    }

    // ----------------------------------------------------------------------------

    Start_stop_buffer::Start_stop_buffer(const QString start_seq, const QString stop_seq) :
            m_start_seq(start_seq), m_stop_seq(stop_seq)
    {

    }

    // ----------------------------------------------------------------------------

    void Start_stop_buffer::add(const QString data, const QString origin)
    {
        m_buffer.append(data);

        if(m_start_seq.isEmpty() && m_stop_seq.isEmpty())
        {
            emit string_found(QString(m_buffer), origin);
            m_buffer.clear();
            return;
        }

        int count = m_buffer.count(m_stop_seq);
        for(int i = 0; i < count; i++)
        {
            int found_at = -1;

            // Trim chars before start sequence
            if(!m_start_seq.isEmpty())
            {
                found_at = m_buffer.indexOf(m_start_seq);
                if(found_at >= 0)
                {
                    m_buffer.remove(0, found_at + m_start_seq.length());
                }
            }

            found_at = m_buffer.indexOf(m_stop_seq);
            QString buf = m_buffer;
            buf.truncate(found_at);
            if(!buf.isEmpty())
                emit string_found(buf, origin);
            m_buffer.remove(0, found_at + m_stop_seq.length());
        }
    }

    // ----------------------------------------------------------------------------

    void Start_stop_buffer::set_start_seq(const QString value)
    {
        m_start_seq = value;
    }

    // ----------------------------------------------------------------------------

    void Start_stop_buffer::set_stop_seq(const QString value)
    {
        m_stop_seq = value;
    }
}
