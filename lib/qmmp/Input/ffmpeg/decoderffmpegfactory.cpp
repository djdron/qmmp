#include <QtGui>

#include "detailsdialog.h"
#include "decoder_ffmpeg.h"
#include "tag.h"
#include "decoderffmpegfactory.h"


// DecoderFFmpegFactory

bool DecoderFFmpegFactory::supports(const QString &source) const
{
    
    return (source.right(4).toLower() == ".wma");
}

const QString &DecoderFFmpegFactory::name() const
{
    static QString name (tr("FFmpeg Plugin"));
    return name;
}


const QString &DecoderFFmpegFactory::filter() const
{
    static QString filter("*.wma");
    return filter;
}


const QString &DecoderFFmpegFactory::description() const
{
    static QString desc(tr("WMA Files"));
    return desc;
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
