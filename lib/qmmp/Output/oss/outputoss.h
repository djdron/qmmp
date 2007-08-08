// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef   OUTPUTOSS_H
#define   OUTPUTOSS_H

class OutputOSS;

#include <output.h>
#include <QObject>
/*
#if defined( Q_OS_WIN32 )
#include <dsound.h>
#include "constants.h"
#endif
*/

class OutputOSS : public Output
{
public:
    OutputOSS(QObject * parent = 0);
    virtual ~OutputOSS();

    bool isInitialized() const { return m_inited; }
    bool initialize();
    void uninitialize();
    void configure(long, int, int, int);
    void stop();
    void pause();
    long written();
    long latency();
    void seek(long);
    void setVolume(int l, int r);
    void checkVolume();

private:
    // thread run function
    void run();

    // helper functions
    void reset();
    void resetDSP();
    void status();
    void post();
    void sync();
    void openMixer();

    QString m_audio_device, m_mixer_device;

    bool m_inited, m_pause, m_play, m_userStop, m_master;
    long m_totalWritten, m_currentSeconds, m_bps;
    int stat;
    int m_rate, m_frequency, m_channels, m_precision;
//#if defined(_OS_UNIX_) || defined(Q_OS_UNIX)
    bool do_select;
    int m_audio_fd, m_mixer_fd;
    long bl, br;

/*#elif defined( _OS_WIN32_ ) || defined( Q_OS_WIN32 )
    LPDIRECTSOUND lpds;
    LPDIRECTSOUNDBUFFER lpdsb;
    LPDIRECTSOUNDNOTIFY lpdsn;
    PCMWAVEFORMAT pcmwf;
    Qt::HANDLE hNotifyEvent;
    DSBPOSITIONNOTIFY notifies[ BUFFERBLOCKS ];
    DWORD currentLatency;
// Not sure we need this
//   DSCAPS dsCaps;
//
//#endif
*/
};


#endif // __audiooutout_h
