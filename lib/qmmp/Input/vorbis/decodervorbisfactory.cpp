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

bool DecoderVorbisFactory::canDecode(QIODevice *input) const
{
    char buf[36];
    if (input->peek(buf, 36) == 36 && !memcmp(buf, "OggS", 4)
            && !memcmp(buf + 29, "vorbis", 6))
        return TRUE;

    return FALSE;
}

const DecoderProperties DecoderVorbisFactory::properties() const
{
    DecoderProperties properties;
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
