#include <QObject>
#include <QApplication>
#include <QtGlobal>
#include <QDir>
#include <QSettings>

#include "outputjack.h"
#include "constants.h"
#include "buffer.h"
#include "visual.h"

#include <stdio.h>
#include <string.h>

void OutputJACK::stop()
{
    m_userStop = TRUE;
}

void OutputJACK::status()
{
    long ct = (m_totalWritten - latency()) / m_bps;

    if (ct < 0)
        ct = 0;

    if (ct > m_currentSeconds)
    {
        m_currentSeconds = ct;
        dispatch(m_currentSeconds, m_totalWritten, m_rate,
                 m_frequency, m_precision, m_channels);
    }
}

long OutputJACK::written()
{
    return m_totalWritten;
}

void OutputJACK::seek(long pos)
{
    recycler()->mutex()->lock ();
    recycler()->clear();
    recycler()->mutex()->unlock();

    m_totalWritten = (pos * m_bps);
    m_currentSeconds = -1;
}

OutputJACK::OutputJACK(QObject * parent)
        : Output(parent), m_inited(FALSE), m_pause(FALSE), m_play(FALSE),
        m_userStop(FALSE), m_totalWritten(0), m_currentSeconds(-1),
        m_bps(-1), m_frequency(-1), m_channels(-1), m_precision(-1)
{
    JACK_Init();
}

OutputJACK::~OutputJACK()
{
    uninitialize();
}

void OutputJACK::configure(long freq, int chan, int prec, int brate)
{
    qDebug("OutputJACK: configure");
    m_precision = prec;
    m_channels = chan;
    m_frequency = freq;
    m_bps = freq * chan * (prec / 8);
    if(JACK_Open(&jack_device, prec, (unsigned long *)&freq, chan))
	{
        m_configure = FALSE;
	return;
	}
    else
        m_configure = TRUE;

    qDebug("OutputJACK: configure end");
}

void OutputJACK::pause()
{
    m_pause = (m_pause) ? FALSE : TRUE;
    {
        int state = m_pause ? OutputState::Paused: OutputState::Playing;
        dispatch(OutputState((OutputState::Type) state));
    }

}

bool OutputJACK::initialize()
{
    m_inited = m_pause = m_play = m_userStop = FALSE;
    m_currentSeconds = -1;
    m_totalWritten = 0;
    if (m_inited)
        m_inited = TRUE;
    m_inited = TRUE;
    m_configure = FALSE;
    jack_options_t options = JackNullOption;
    jack_status_t status;
    jack_client_t *client = jack_client_open ("test_qmmp", options, &status, NULL);
    if (client == NULL)
    {
        qDebug("jack_client_open() failed.");
        if (status & JackServerFailed)
        {
            qDebug("Unable to connect to JACK server.");
        }
    return FALSE;
    }
    jack_client_close (client);
    return TRUE;
}

long OutputJACK::latency()
{
    ulong used = 0;
    return used;
}

void OutputJACK::run()
{
    mutex()->lock ();
    if (! m_inited)
    {
        mutex()->unlock();
        return;
    }

    m_play = TRUE;
    Buffer *b = 0;
    bool done = FALSE;
    long m = 0;
    unsigned long l;

    dispatch(OutputState::Playing);

    mutex()->unlock();
    while (!done&&m_configure)
    {
        mutex()->lock ();
        recycler()->mutex()->lock ();
        done = m_userStop;

        while (! done && (recycler()->empty() || m_pause))
        {
            mutex()->unlock();
            recycler()->cond()->wakeOne();
            recycler()->cond()->wait(recycler()->mutex());
            mutex()->lock ();
            done = m_userStop;
            status();
        }

        if (! b)
        {
            b = recycler()->next();
            if (b->rate)
                m_rate = b->rate;
        }

        recycler()->cond()->wakeOne();
        recycler()->mutex()->unlock();

        if (b)
        {
            l = int(b->nbytes);
            unsigned char *buf = b->data;
            m_totalWritten += l;
            while (l > 0)
            {
                m = JACK_Write(jack_device, (unsigned char*)buf, l);
                if (!m)
                    usleep(2000);
                usleep(((m/m_channels)*100000)/m_frequency);
                l-=m;
                buf+=m;
            }

            status();
            dispatchVisual(b, m_totalWritten, m_channels, m_precision);
        }
        recycler()->mutex()->lock ();
        recycler()->done();
        recycler()->mutex()->unlock();
        b = 0;
        mutex()->unlock();
    }
    mutex()->lock ();
    m_play = FALSE;
    dispatch(OutputState::Stopped);
    mutex()->unlock();
}

void OutputJACK::uninitialize()
{
    if (!m_inited)
        return;
    m_inited = FALSE;
    m_inited = m_pause = m_play = m_userStop = FALSE;
    m_currentSeconds = -1;
    m_totalWritten = 0;
    if (m_configure)
        JACK_Close(jack_device);
    dispatch(OutputState::Stopped);
}

