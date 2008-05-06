#include <QtGui>

extern "C"{
#if defined HAVE_FFMPEG_AVFORMAT_H
#include <ffmpeg/avformat.h>
#elif defined HAVE_LIBAVFORMAT_AVFORMAT_H
#include <libavformat/avformat.h>
#else
#include <avformat.h>
#endif

#if defined HAVE_FFMPEG_AVCODEC_H
#include <ffmpeg/avcodec.h>
#elif defined HAVE_LIBAVCODEC_AVCODEC_H
#include <libavcodec/avcodec.h>
#else
#include <avcodec.h>
#endif
}

#include "detailsdialog.h"
#include "decoder_ffmpeg.h"
#include "decoderffmpegfactory.h"


// DecoderFFmpegFactory

bool DecoderFFmpegFactory::supports(const QString &source) const
{

    return (source.right(4).toLower() == ".wma");
}

bool DecoderFFmpegFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderFFmpegFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("FFMPEG Plugin");
    properties.filter = "*.wma";
    properties.description = tr("WMA Files");
    //properties.contentType = "";
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    return properties;
}

Decoder *DecoderFFmpegFactory::create(QObject *parent, QIODevice *input,
                                      Output *output)
{
    return new DecoderFFmpeg(parent, this, input, output);
}

FileTag *DecoderFFmpegFactory::createTag(const QString &source)
{
    FileTag *ftag = new FileTag();
    avcodec_init();
    avcodec_register_all();
    av_register_all();
    AVFormatContext *in;

    if (av_open_input_file(&in, source.toLocal8Bit(), NULL,0, NULL) < 0)
        return ftag;
    av_find_stream_info(in);
    ftag->setValue(FileTag::ALBUM, QString::fromUtf8(in->album).trimmed());
    ftag->setValue(FileTag::ARTIST, QString::fromUtf8(in->author).trimmed());
    ftag->setValue(FileTag::COMMENT, QString::fromUtf8(in->comment).trimmed());
    ftag->setValue(FileTag::GENRE, QString::fromUtf8(in->genre).trimmed());
    ftag->setValue(FileTag::TITLE, QString::fromUtf8(in->title).trimmed());
    ftag->setValue(FileTag::YEAR, in->year);
    ftag->setValue(FileTag::TRACK, in->track);
    ftag->setValue(FileTag::LENGTH ,int(in->duration/AV_TIME_BASE));
    av_close_input_file(in);
    return ftag;
}

QObject* DecoderFFmpegFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
    return d;
}

void DecoderFFmpegFactory::showSettings(QWidget *)
{}

void DecoderFFmpegFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About FFmpeg Audio Plugin"),
                        tr("Qmmp FFmpeg Audio Plugin")+"\n"+
                        tr("Suppored formats: WMA")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderFFmpegFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/ffmpeg_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderFFmpegFactory)
