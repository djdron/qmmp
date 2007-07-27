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


#include "fileinfo.h"
#include "filetag.h"

class QObject;
class QIODevice;

class Decoder;
class DecoderFactory;
class Buffer;
class Recycler;
class Output;
class Visualization;



class DecoderState
{
public:
    enum Type { Decoding, Stopped, Finished, Error };

    DecoderState(Type t)
            : m_type(t), m_error_msg(0)
    {}

    DecoderState(const QString &e)
            : m_type(Error)
    {
        m_error_msg = new QString(e);
    }

    DecoderState()
            : m_type(Stopped), m_error_msg(0)
    {}

    ~DecoderState()
    {
        if (m_error_msg)
           delete m_error_msg;
    }

    const QString *errorMessage() const
    {
        return m_error_msg;
    }
    const Type &type() const
    {
        return m_type;
    }

private:
    Type m_type;
    const QString *m_error_msg;
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
        return out;
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
    void setEQEnabled(bool on) { m_useEQ = on; };

    // static methods
    static QStringList all();
    static bool supports(const QString &);
    //static void registerFactory(DecoderFactory *);
    static Decoder *create(QObject *, const QString &, QIODevice *, Output *);
    static DecoderFactory *findFactory(const QString&);
    static DecoderFactory *findByContentType(const QString&);
    static FileTag *createTag(const QString&);
    static QString filter();
    static QStringList nameFilters();
    static QList<DecoderFactory*> *decoderFactories();
    static QStringList decoderFiles();

signals:
    void stateChanged(const DecoderState&);

protected:
    void dispatch(DecoderState::Type);
    void dispatch(const DecoderState&);
    void error(const QString&);

private:
    DecoderFactory *fctry;

    QList<QObject*> listeners;
    QIODevice *in;
    Output *out;

    QMutex mtx;
    QWaitCondition cnd;

    uint blksize;
    bool m_eqInited;
    bool m_useEQ;
};

#endif // DECODER_H
