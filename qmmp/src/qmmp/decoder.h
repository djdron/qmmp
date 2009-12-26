// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef DECODER_H
#define DECODER_H

#include <QList>
#include <QStringList>
#include <QUrl>
#include <QList>
#include <QPixmap>
#include <QMap>
#include "fileinfo.h"
#include "qmmp.h"
#include "audioparameters.h"

class Decoder;
class DecoderFactory;
class QIODevice;

/*! @brief The Decoder class provides the base interface class of audio decoders.
 * @author Brad Hughes <bhughes@trolltech.com>
 * @author Ilya Kotov <forkotov@hotmail.ru>
 */
class Decoder
{
public:
    /*!
     *
     */
    Decoder(QIODevice *input = 0);
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
     */
    virtual void seek(qint64 time) = 0;
    /*!
     * Reads up to \b maxSize bytes of decoded audio to \b data
     * Returns the number of bytes read, or -1 if an error occurred.
     * In most cases subclass should reimplement this function.
     */
    virtual qint64 read(char *data, qint64 maxSize) = 0;
    /*!
     * Returns current bitrate (in kbps).
     * Subclass should reimplement this function.
     */
    virtual int bitrate() = 0;
    virtual void next();
    virtual const QString nextURL();

    AudioParameters audioParameters() const;
    QMap<Qmmp::ReplayGainKey, double> replayGainInfo() const;
    QIODevice *input();
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
     * Returns DecoderFactory pointer which supports protocol \b p or \b 0 if \b url is not supported.
     */
    static DecoderFactory *findByProtocol(const QString &p);
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
     * @param enable Plugin enable state (\b true - enable, \b false - disable)
     */
    static void setEnabled(DecoderFactory* factory, bool enable = TRUE);
    /*!
     * Returns \b true if input plugin is enabled, otherwise returns \b false
     * @param factory Decoder plugin factory.
     */
    static bool isEnabled(DecoderFactory* factory);

protected:
    /*!
     * Use this function inside initialize() reimplementation to tell other plugins about audio parameters.
     * @param srate Sample rate.
     * @param chan Number of channels.
     * @param bps Bits per sample.
     * @param rg ReplayGaing information.
     */
    void configure(quint32 srate, int chan, int bps, const QMap<Qmmp::ReplayGainKey,double> &rg);
    /*!
     * Use this function inside initialize() reimplementation to tell other plugins about audio parameters.
     * @param srate Sample rate.
     * @param chan Number of channels.
     * @param bps Bits per sample.
     */
    void configure(quint32 srate, int chan, int bps);

private:
    static void checkFactories();
    static QList<DecoderFactory*> *m_factories;
    static DecoderFactory *m_lastFactory;
    static QStringList m_files;
    AudioParameters m_parameters;
    QIODevice *m_input;
    QMap <Qmmp::ReplayGainKey, double> m_rg; //replay gain information
};

#endif // DECODER_H
