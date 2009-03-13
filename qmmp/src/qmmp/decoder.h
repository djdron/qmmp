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
#include <QList>
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

/*! @brief The Decoder class provides the base interface class of audio decoders.
 * @author Brad Hughes <bhughes@trolltech.com>
 * @author Ilya Kotov <forkotov@hotmail.ru>
 */
class Decoder : public QThread
{
    Q_OBJECT
public:
    /*!
     * Object contsructor for decoders with QIODevice-based input.
     * @param parent Parent object.
     * @param d Decoder's factory object.
     * @param input QIODevice-based input source.
     * @param output Output object.
     */
    Decoder(QObject *parent, DecoderFactory *d, QIODevice *input = 0, Output *output = 0);
    /*!
     * Object contsructor for decoders without QIODevice-based input.
     * @param parent Parent object.
     * @param d Decoder's factory object.
     * @param output Output object.
     */
    Decoder(QObject *parent, DecoderFactory *d, Output *output);
    /*!
     * Destructor.
     */
    virtual ~Decoder();
    /*!
     * Prepares decoder for usage.
     * Subclass should reimplement this function.
     */
    virtual bool initialize() = 0;
    /*!
     * Returns the total time in milliseconds.
     * Subclass should reimplement this function.
     */
    virtual qint64 totalTime() = 0;
    /*!
     * Requests a seek to the time \b time indicated, specified in milliseconds.
     * Subclass should reimplement this function.
     */
    virtual void seek(qint64 time) = 0;
    /*!
     * Requests playback to stop
     * Subclass should reimplement this function.
     */
    virtual void stop() = 0;
    /*!
     * Requests playback to pause. If it was paused already, playback should resume.
     * Subclass with own output should reimplement this function.
     */
    virtual void pause(){};
    /*!
     * Returns decoder's factory object.
     */
    DecoderFactory *factory() const;
    /*!
     * Returns decoder input or 0 if input is not specified.
     */
    QIODevice *input();
    /*!
     * Returns decoder output or 0 if output is not specified.
     */
    Output *output();
    /*!
     * Returns mutex pointer.
     */
    QMutex *mutex();
    /*!
     * Returns wait condition pointer.
     */
    QWaitCondition *cond();
    /*!
     * Returns StateHandler object pointer.
     */
    StateHandler *stateHandler();
    /*!
     * Sets StateHandler pointer. May be used to override default state handler.
     * @param hanlder StateHandler pointer;
     */
    void setStateHandler(StateHandler *handler);
    /*!
     * Sets equalizer settings. Each item of \p bands[] and \p reamp should be \b -20.0..20.0
     * Subclass with own equalizer should reimplement this function.
     */
    virtual void setEQ(double bands[10], double preamp);
    /*!
     * Enables equalizer if \p on is \b true or disables it if \p on is \b false
     * Subclass with own equalizer should reimplement this function.
     */
    virtual void setEQEnabled(bool on);
    /*!
     * Returns \b true if \b file is supported by input plugins, otherwise \b false
     */
    static bool supports(const QString &file);
    /*!
     * Returns DecoderFactory pointer which supports file \b path or 0 if file \b path is unsupported
     */
    static DecoderFactory *findByPath(const QString &path);
    /*!
     * Returns DecoderFactory pointer which supports mime type \b mime or \b 0 if mime type \b mime is unsupported
     */
    static DecoderFactory *findByMime(const QString &mime);
    /*!
     * Returns DecoderFactory pointer which supports data provided by QIODevice \b input
     * or \b 0 if data is unsupported.
     */
    static DecoderFactory *findByContent(QIODevice *input);
    /*!
     * Returns DecoderFactory pointer which supports \b url or \b 0 if \b url is not supported.
     */
    static DecoderFactory *findByURL(const QUrl &url);
    /*!
     * Extracts metadata and audio information from file \b path and returns a list of FileInfo items.
     * One file may contain several playlist items (for example: cda disk or flac with embedded cue)
     * @param path Source file path.
     * @param useMetaData Metadata usage (\b true - use, \b - do not use)
     */
    static QList <FileInfo *> createPlayList(const QString &path, bool useMetaData = TRUE);
    /*!
     * Returns a list of file name filters with description, i.e. "MPEG Files (*.mp3 *.mpg)"
     */
    static QStringList filters();
    /*!
     * Returns a list of file name filters, i.e. "*.mp3 *.mpg"
     */
    static QStringList nameFilters();
    /*!
     * Returns a list of decoder factories.
     */
    static QList<DecoderFactory*> *factories();
    /*!
     * Returns a list of input plugin file names.
     */
    static QStringList files();
    /*!
     * Sets whether the input plugin is enabled.
     * @param factory Decoder plugin factory.
     * @param enabled Plugin enable state (\b true - enable, \b false - disable)
     */
    static void setEnabled(DecoderFactory* factory, bool enable = TRUE);
    /*!
     * Returns \b true if input plugin is enabled, otherwise \b false
     * @param factory Decoder plugin factory.
     */
    static bool isEnabled(DecoderFactory* factory);

signals:
    /*!
     * Emitted when the decoder has finished playback.
     */
    void playbackFinished();

protected:
    /*!
     * Use this function inside initialize() reimplementation to tell other plugins about audio parameters.
     * @param srate Sample rate.
     * @param chan Number of channels.
     * @param bps Bits per sample.
     */
    void configure(quint32 srate, int chan, int bps);
    /*!
     * Sends audio data to the output plugin.
     * @param data Pointer to audio data.
     * @param size Audio data size.
     * @param chan Number of channels.
     */
    qint64 produceSound(char *data, qint64 size, quint32 brate, int chan);

protected slots:
    /*!
     * Subclass should call this slot when decoding is finished.
     */
    void finish();

private:
    void init();
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
    StateHandler *m_handler;
    static QList<DecoderFactory*> *m_factories;
    static DecoderFactory *m_lastFactory;
    static QStringList m_files;

};

#endif // DECODER_H
