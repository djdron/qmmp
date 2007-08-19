#include <QtGui>

#include "detailsdialog.h"
#include "decoder_ffmpeg.h"
#include "tag.h"
#include "decoderffmpegfactory.h"


// DecoderFFmpegFactory

bool DecoderFFmpegFactory::supports(const QString &source) const
{

    return (source.right(4).toLower() == ".wma" || source.right(4).toLower() == ".wav");
}

bool DecoderFFmpegFactory::canDecode(QIODevice *input) const
{
    return FALSE;
}

const DecoderProperties DecoderFFmpegFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("WMA Files");
    properties.filter = "*.wma *.wav";
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
    FileTag *tag = new Tag(source);
    return tag;
}

void DecoderFFmpegFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
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
