
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
    virtual bool isInitialized() const = 0;
    virtual bool initialize() = 0;
    virtual void uninitialize() = 0;
    virtual void configure(quint32, int, int) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual qint64 written() = 0;
    virtual qint64 latency() = 0;
    virtual void seek(long) = 0;
    virtual void setVolume(int, int){};
    virtual void volume(int *, int *){};

    Recycler *recycler();
    QMutex *mutex();
    void setStateHandler(StateHandler *handler);

    static void registerFactory(OutputFactory *);
    static Output *create(QObject *);
    static QList<OutputFactory*> *outputFactories();
    static QStringList outputFiles();
    static void setEnabled(OutputFactory* factory);
    static bool isEnabled(OutputFactory* factory);

public slots:
    void checkVolume();
    void checkSoftwareVolume();

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

private:
    QMutex m_mutex;
    Recycler m_recycler;
    int m_bl, m_br;
    StateHandler *m_handler;

    static void checkFactories();
    static QList<OutputFactory*> *m_factories;
    static QStringList m_files;
    static QTimer *m_timer;
};


#endif // OUTPUT_H
