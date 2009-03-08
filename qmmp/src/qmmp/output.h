
// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef   OUTPUT_H
#define   OUTPUT_H

class Output;

#include <QObject>
#include <QThread>
#include <QEvent>
#include <QList>
#include <QIODevice>
#include "visual.h"
#include "outputfactory.h"
#include "visualfactory.h"
#include "statehandler.h"

#include "recycler.h"

class QTimer;


class Output : public QThread
{
    Q_OBJECT
public:

    Output(QObject * parent = 0);
    ~Output();

      // abstract
    virtual bool initialize() = 0;
    virtual qint64 latency() = 0;
    virtual void configure(quint32, int, int);

    void pause();
    void stop();
    qint64 written();
    void seek(qint64);
    Recycler *recycler();
    QMutex *mutex();
    void setStateHandler(StateHandler *handler);
    quint32 sampleRate();
    int numChannels();
    int sampleSize();

    static void registerFactory(OutputFactory *);
    static Output *create(QObject *);
    static QList<OutputFactory*> *outputFactories();
    static QStringList outputFiles();
    static void setCurrentFactory(OutputFactory* factory);
    static OutputFactory *currentFactory();

protected:
    void dispatch(qint64 elapsed,
                  qint64 totalTime,
                  int bitrate,
                  int frequency,
                  int precision,
                  int channels);
    void dispatch(const Qmmp::State &state);
    void dispatchVisual(Buffer *, unsigned long, int, int);
    void clearVisuals();

    virtual qint64 writeAudio(unsigned char *data, qint64 maxSize) = 0;
    virtual void flush() = 0;

private:
    void run(); //thread run function
    void status();
    void changeVolume(uchar *data, qint64 size, int chan);
    QMutex m_mutex;
    Recycler m_recycler;
    StateHandler *m_handler;
    quint32 m_frequency;
    int m_channels, m_precision, m_kbps;
    qint64 m_bytesPerMillisecond;
    bool m_userStop, m_pause;
    qint64 m_totalWritten, m_currentMilliseconds;

    static void checkFactories();
    //TODO use QMap instead
    static QList<OutputFactory*> *m_factories;
    static QStringList m_files;
    static QTimer *m_timer;
};


#endif // OUTPUT_H
