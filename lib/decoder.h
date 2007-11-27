// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef   DECODER_H
#define   DECODER_H

#include <QThread>
#include <QList>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>
#include <QStringList>

#include "filetag.h"

class QObject;
class QIODevice;

class Decoder;
class DecoderFactory;
class Buffer;
class Recycler;
class Output;
class Visualization;
class Effect;



class DecoderState
{
public:
    enum Type { Decoding, Stopped, Finished, Info, Error };

    DecoderState(const DecoderState &st)
            : m_error_msg(0), m_tag(0)
    {
        m_type = st.type();
        if (m_type == Info)
            m_tag = new FileTag(*st.tag());
        else if (m_type == Error)
            m_error_msg = new QString(*st.errorMessage());
    }


    DecoderState(Type t)
            : m_type(t), m_error_msg(0), m_tag(0)
{}

    DecoderState(const QString &e)
            : m_type(Error), m_tag(0)
    {
        m_error_msg = new QString(e);
    }

    DecoderState()
            : m_type(Stopped), m_error_msg(0), m_tag(0)
    {}

    DecoderState(const FileTag &tag)
            : m_type(Info), m_error_msg(0),  m_tag(0)
    {
        m_tag = new FileTag(tag);
    }

    ~DecoderState()
    {
        if (m_error_msg)
            delete m_error_msg;
        if (m_tag)
            delete m_tag;
    }

    const QString *errorMessage() const
    {
        return m_error_msg;
    }
    const Type &type() const
    {
        return m_type;
    }
    const FileTag *tag() const
    {
        return m_tag;
    }

private:
    Type m_type;
    const QString *m_error_msg;
    FileTag *m_tag;
};



class Decoder : public QThread
{
    Q_OBJECT
public:
    Decoder(QObject *parent, DecoderFactory *d,
            QIODevice *i, Output *o);
    virtual ~Decoder();

    // Standard Decoder API
    virtual bool initialize() = 0;
    virtual double lengthInSeconds() = 0;
    virtual void seek(double) = 0;
    virtual void stop() = 0;

    DecoderFactory *factory() const
    {
        return fctry;
    }

    QIODevice *input()
    {
        return in;
    }
    Output *output()
    {
        return m_output;
    }

    QMutex *mutex()
    {
        return &mtx;
    }
    QWaitCondition *cond()
    {
        return &cnd;
    }

    void setBlockSize(unsigned int sz)
    {
        blksize = sz;
    }

    unsigned int blockSize() const
    {
        return blksize;
    }
    ulong produceSound(char *data, ulong output_bytes, ulong bitrate, int nch);
    void setEQ(int bands[10], int preamp);
    void setEQEnabled(bool on)
    {
        m_useEQ = on;
    };

    // static methods
    static QStringList all();
    static bool supports(const QString &);
    //static void registerFactory(DecoderFactory *);
    static Decoder *create(QObject *, const QString &, QIODevice *, Output *);
    static DecoderFactory *findByPath(const QString&);
    static DecoderFactory *findByMime(const QString&);
    static DecoderFactory *findByContent(QIODevice *);
    static FileTag *createTag(const QString&);
    static QString filter();
    static QStringList nameFilters();
    static QList<DecoderFactory*> *decoderFactories();
    static QStringList decoderFiles();

signals:
    void stateChanged(const DecoderState&);

protected:
    void configure(long freq, int channels, int prec, int bitrate);
    void dispatch(DecoderState::Type);
    void dispatch(const DecoderState&);
    void dispatch(const FileTag&);
    void error(const QString&);

private:
    DecoderFactory *fctry;

    QList <Effect*> m_effects;
    QIODevice *in;
    Output *m_output;

    QMutex mtx;
    QWaitCondition cnd;

    uint blksize;
    bool m_eqInited;
    bool m_useEQ;

};

#endif // DECODER_H
