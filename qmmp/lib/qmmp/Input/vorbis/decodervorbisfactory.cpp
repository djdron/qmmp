#include <QtGui>
#include <taglib/tag.h>
#include <taglib/fileref.h>

#include "detailsdialog.h"
#include "decoder_vorbis.h"
#include "tag.h"
#include "decodervorbisfactory.h"


// DecoderOggFactory

bool DecoderVorbisFactory::supports(const QString &source) const
{
    return source.right(4).toLower() == ".ogg";
}

/*const QString &DecoderVorbisFactory::name() const
{
    static QString name (tr("Ogg Vorbis Plugin"));
    return name;
}

const QString &DecoderVorbisFactory::filter() const
{
    static QString filter("*.ogg");
    return filter;
}


const QString &DecoderVorbisFactory::description() const
{
    static QString desc(tr("Ogg Vorbis Files"));
    return desc;
}

const QString &DecoderVorbisFactory::contentType() const
{
    static QString types("application/ogg;audio/x-vorbis+ogg");
    return types;
}*/

bool DecoderVorbisFactory::canDecode(QIODevice *input) const
{
    static bool c = FALSE;
    return c;
}

const DecoderProperties &DecoderVorbisFactory::properties() const
{
    static DecoderProperties properties;
    properties.name = tr("Ogg Vorbis Plugin");
    properties.filter = "*.ogg";
    properties.description = tr("Ogg Vorbis Files");
    properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    return properties;
}

Decoder *DecoderVorbisFactory::create(QObject *parent, QIODevice *input,
                                   Output *output)
{
    return new DecoderVorbis(parent, this, input, output);
}

FileTag *DecoderVorbisFactory::createTag(const QString &source)
{
    FileTag *tag = new Tag(source);
    return tag;
}

void DecoderVorbisFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
}

void DecoderVorbisFactory::showSettings(QWidget *)
{
    /*SettingsDialog *s = new SettingsDialog(parent);
    s -> show();*/
}

void DecoderVorbisFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Ogg Vorbis Audio Plugin"),
                        tr("Qmmp Ogg Vorbis Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>")+"\n"+
                        tr("Source code based on mq3 progect")
                       );
}

QTranslator *DecoderVorbisFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/vorbis_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderVorbisFactory)
