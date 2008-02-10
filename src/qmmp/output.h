
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

#include "recycler.h"

class QTimer;


class OutputState
{
public:

    enum Type { Playing, Buffering, Info, Paused, Stopped, Volume, Error };

    OutputState()
            : m_type(Stopped),  m_error_msg(0), m_elasped_seconds(0),
            m_written_bytes(0), m_brate(0), m_freq(0), m_prec(0), m_chan(0),
            m_left(0), m_right(0)
    {}
    OutputState(const OutputState &st)
            : m_type(Stopped),  m_error_msg(0), m_elasped_seconds(0),
            m_written_bytes(0), m_brate(0), m_freq(0), m_prec(0), m_chan(0),
            m_left(0), m_right(0)
    {
        m_type = st.type();
        if (m_type == Info)
        {
            m_elasped_seconds = st.elapsedSeconds();
            m_written_bytes = st.writtenBytes();
            m_brate = st.bitrate();
            m_freq = st.frequency();
            m_prec = st.precision();
            m_chan = st.channels();
            m_left = st.leftVolume();
            m_right = st.rightVolume();
        }
        else if (m_type == Error)
            m_error_msg = new QString(*st.errorMessage());
    }

    OutputState(Type t)
            : m_type(t),  m_error_msg(0), m_elasped_seconds(0),
            m_written_bytes(0), m_brate(0), m_freq(0), m_prec(0), m_chan(0),
            m_left(0), m_right(0)
{}
    OutputState(long s, unsigned long w, int b, int f, int p, int c)
            : m_type(Info), m_error_msg(0), m_elasped_seconds(s),
            m_written_bytes(w), m_brate(b), m_freq(f), m_prec(p), m_chan(c),
            m_left(0), m_right(0)
    {}
    OutputState(int L, int R)
            : m_type(Volume), m_error_msg(0), m_elasped_seconds(0),
            m_written_bytes(0), m_brate(0), m_freq(0), m_prec(0), m_chan(0),
            m_left(L), m_right(R)
    {}
    OutputState(const QString &e)
            : m_type(Error), m_elasped_seconds(0), m_written_bytes(0),
            m_brate(0), m_freq(0), m_prec(0), m_chan(0),
            m_left(0), m_right(0)
    {
        m_error_msg = new QString(e);
    }
    ~OutputState()
    {
        if (m_error_msg)
            delete m_error_msg;
    }

    const QString *errorMessage() const
    {
        return m_error_msg;
    }

    const long &elapsedSeconds() const
    {
        return m_elasped_seconds;
    }
    const unsigned long &writtenBytes() const
    {
        return m_written_bytes;
    }
    const int &bitrate() const
    {
        return m_brate;
    }
    const int &frequency() const
    {
        return m_freq;
    }
    const int &precision() const
    {
        return m_prec;
    }
    const int &channels() const
    {
        return m_chan;
    }
    const Type &type() const
    {
        return m_type;
    }
    const int leftVolume() const
    {
        return m_left;
    }
    const int rightVolume() const
    {
        return m_right;
    }

private:
    Type m_type;
    QString *m_error_msg;
    long m_elasped_seconds;
    unsigned long m_written_bytes;
    int m_brate, m_freq, m_prec, m_chan;
    int m_left, m_right;    //volume
};




class Output : public QThread
{
    Q_OBJECT
public:

    Output(QObject * parent = 0);
    ~Output();

    Recycler *recycler()
    {
        return &r;
    }

    QMutex *mutex()
    {
        return &mtx;
    }

    //visualization
    void addVisual(Visual*);
    void removeVisual(Visual*);
    void addVisual(VisualFactory *factory, QWidget *parent);
    void removeVisual(VisualFactory *factory);

    // abstract
    virtual bool isInitialized() const = 0;
    virtual bool initialize() = 0;
    virtual void uninitialize() = 0;
    virtual void configure(long, int, int, int) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual long written() = 0;
    virtual long latency() = 0;
    virtual void seek(long) = 0;
    virtual void setVolume(int, int){};
    virtual void volume(int*, int*){};

    static void registerFactory(OutputFactory *);
    static Output *create(QObject *);
    static QList<OutputFactory*> *outputFactories();
    static QStringList outputFiles();
    static void setEnabled(OutputFactory* factory);
    static bool isEnabled(OutputFactory* factory);

public slots:
    void checkVolume();
    void checkSoftwareVolume();

signals:
    void stateChanged(const OutputState&);

protected:
    void dispatch(OutputState::Type);
    void dispatch(long s, unsigned long w, int b, int f, int p, int c);
    void dispatch(const OutputState&);
    void dispatchVolume(int L, int R);
    void error(const QString &e);
    void dispatchVisual(Buffer *, unsigned long, int, int);
    void clearVisuals();

private:
    QMutex mtx;
    Recycler r;
    QList<Visual*> visuals; //external visualization
    QMap<VisualFactory*, Visual*> m_vis_map; //internal visualization
    int m_bl, m_br;
};


#endif // OUTPUT_H
