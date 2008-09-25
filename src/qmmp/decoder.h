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
#include <QUrl>

#include "fileinfo.h"

class QObject;
class QIODevice;

class Decoder;
class DecoderFactory;
class Buffer;
class Recycler;
class Output;
class Visualization;
class Effect;
class StateHandler;


class Decoder : public QThread
{
    Q_OBJECT
public:
    Decoder(QObject *parent, DecoderFactory *d, QIODevice *input = 0, Output *output = 0);

    virtual ~Decoder();

    // Standard Decoder API
    virtual bool initialize() = 0;
    virtual qint64 lengthInSeconds() = 0;
    virtual void seek(double) = 0;
    virtual void stop() = 0;
    //virtual void pause() = 0;
    //virtual void stop() = 0;

    DecoderFactory *factory() const;
    QIODevice *input();
    Output *output();
    QMutex *mutex();
    QWaitCondition *cond();
    StateHandler *stateHandler();

    void setEQ(int bands[10], int preamp);
    void setEQEnabled(bool on);

    void setVolume(int, int);
    void volume(int*, int*);

    // static methods
    static QStringList all();
    static bool supports(const QString &);
    //static Decoder *create(QObject *, const QString &, QIODevice *, Output *);
    static DecoderFactory *findByPath(const QString&);
    static DecoderFactory *findByMime(const QString&);
    static DecoderFactory *findByContent(QIODevice *);
    static DecoderFactory *findByURL(const QUrl &url);
    static FileInfo *getFileInfo(const QString &fileName);
    static QStringList filters();
    static QStringList nameFilters();
    static QList<DecoderFactory*> *factories();
    static QStringList files();
    static void setEnabled(DecoderFactory* factory, bool enable = TRUE);
    static bool isEnabled(DecoderFactory* factory);

signals:
    void finished();

protected:
    void configure(quint32 srate, int chan, int bps);
    qint64 produceSound(char *data, qint64 size, quint32 brate, int chan);
    void finish();

private:
    void changeVolume(char *data, qint64 size, int chan);
    static void checkFactories();

    DecoderFactory *m_factory;
    QList <Effect*> m_effects;
    QIODevice *m_input;
    Output *m_output;

    QMutex m_mutex;
    QWaitCondition m_waitCondition;

    uint blksize;
    bool m_eqInited;
    bool m_useEQ;
    bool m_useVolume;
    int m_volL, m_volR, m_volLF, m_volRF;
    StateHandler *m_handler;
    static QList<DecoderFactory*> *m_factories;
    static QStringList m_files;

};

#endif // DECODER_H
