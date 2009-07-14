
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

/*! @brief The Output class provides the base interface class of audio outputs.
 * @author Brad Hughes <bhughes@trolltech.com>
 * @author Ilya Kotov <forkotov@hotmail.ru>
 */
class Output : public QThread
{
    Q_OBJECT
public:
    /*!
     * Object contsructor.
     * @param parent Parent object.
     */
    Output(QObject * parent = 0);
    /*!
     * Destructor.
     */
    ~Output();
    /*!
     * Prepares object for usage.
     * Subclass should reimplement this function.
     * @return initialization result (\b true - success, \b false - failure)
     */
    virtual bool initialize() = 0;
    /*!
     * Returns output interface latency in milliseconds.
     */
    virtual qint64 latency() = 0;
    /*!
     * Setups audio parameters of output interface.
     * @param freq Sample rate.
     * @param chan Number of channels.
     * @param bits Bits per sample
     */
    virtual void configure(quint32 freq, int chan, int bits);
    /*!
     * Requests playback to pause. If it was paused already, playback should resume.
     * Subclasses that reimplement this function must call the base implementation.
     */
    virtual void pause();
    /*!
     * Requests playback to stop.
     */
    void stop();
    /*!
     * Requests playback to finish.
     */
    void finish();
    /*!
     * Returns the number of bytes that were written.
     */
    qint64 written();
    /*!
     * Requests a seek to the time \b pos indicated, specified in milliseconds.
     */
    void seek(qint64 pos);
    /*!
     * Returns Recycler pointer.
     */
    Recycler *recycler();
    /*!
     * Returns mutex pointer.
     */
    QMutex *mutex();
    /*!
     * Sets StateHandler pointer. May be used to override default state handler.
     * @param handler StateHandler pointer;
     */
    void setStateHandler(StateHandler *handler);
    /*!
     * Returns samplerate.
     */
    quint32 sampleRate();
    /*!
     * Returns channels number.
     */
    int numChannels();
    /*!
     * Returns bits per sample.
     */
    int sampleSize();
    /*!
     * Creates selected output.
     * @param parent Parent object.
     * @return Output subclass object.
     */
    static Output *create(QObject *parent);
    /*!
     * Returns a list of output factories.
     */
    static QList<OutputFactory*> *outputFactories();
    /*!
     * Returns a list of output plugin file names.
     */
    static QStringList outputFiles();
    /*!
     * Selects current output \b factory.
     */
    static void setCurrentFactory(OutputFactory* factory);
    /*!
     * Returns selected output factory.
     */
    static OutputFactory *currentFactory();

protected:
    /*!
     * Writes up to \b maxSize bytes from \b data to the output interface device.
     * Returns the number of bytes written, or -1 if an error occurred.
     * Subclass should reimplement this function.
     */
    virtual qint64 writeAudio(unsigned char *data, qint64 maxSize) = 0;
    /*!
     * Writes all remaining plugin's internal data to audio output device.
     * Subclass should reimplement this function.
     */
    virtual void flush() = 0;

private:
    void run(); //thread run function
    void status();
    void changeVolume(uchar *data, qint64 size, int chan);
    void dispatch(qint64 elapsed,
                  int bitrate,
                  int frequency,
                  int precision,
                  int channels);
    void dispatch(const Qmmp::State &state);
    void dispatchVisual(Buffer *, unsigned long, int, int);
    void clearVisuals();
    QMutex m_mutex;
    Recycler m_recycler;
    StateHandler *m_handler;
    quint32 m_frequency;
    int m_channels, m_precision, m_kbps;
    qint64 m_bytesPerMillisecond;
    bool m_userStop, m_pause;
    bool m_finish;
    qint64 m_totalWritten, m_currentMilliseconds;

    static void checkFactories();
    static void registerFactory(OutputFactory *);
    //TODO use QMap instead
    static QList<OutputFactory*> *m_factories;
    static QStringList m_files;
    static QTimer *m_timer;
};


#endif // OUTPUT_H
