#ifndef   OUTPUTJACK_H
#define   OUTPUTJACK_H

class OutputJACK;

#include <output.h>
#include <QObject>
extern "C"
{
#include <jack/jack.h>
}

#include "bio2jack.h"

class OutputJACK : public Output
{
    Q_OBJECT
public:
    OutputJACK(QObject * parent = 0, bool useVolume = TRUE);
    ~OutputJACK();
    bool initialize();
    bool isInitialized() const
    {
        return m_inited;
    }
    void uninitialize();
    void configure(long, int, int, int);
    void stop();
    void pause();
    long written();
    long latency();
    void seek(long);

private:
    // thread run function
    void run();
    // helper functions
    void status();
    QString audio_device;
    bool m_inited, m_configure, m_pause, m_play, m_userStop;
    long m_totalWritten, m_currentSeconds, m_bps;
    int m_rate, m_frequency, m_channels, m_precision, jack_device;
    bool do_select;
    int audio_fd;
};


#endif

