#include <QtGui>
#include <taglib/tag.h>
#include <taglib/fileref.h>

#include "detailsdialog.h"
#include "decoder_flac.h"
#include "decoderflacfactory.h"


// DecoderFLACFactory

bool DecoderFLACFactory::supports(const QString &source) const
{

    return (source.right(5).toLower() == ".flac");
}

bool DecoderFLACFactory::canDecode(QIODevice *input) const
{
    Q_UNUSED(input);
    return FALSE;
}

const DecoderProperties DecoderFLACFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("FLAC Plugin");
    properties.filter = "*.flac";
    properties.description = tr("FLAC Files");
    //properties.contentType = ;
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    return properties;
}

Decoder *DecoderFLACFactory::create(QObject *parent, QIODevice *input,
                                    Output *output)
{
    return new DecoderFLAC(parent, this, input, output);
}

FileTag *DecoderFLACFactory::createTag(const QString &source)
{
    FileTag *ftag = new FileTag();

    TagLib::FileRef fileRef(source.toLocal8Bit ());
    TagLib::Tag *tag = fileRef.tag();

    if (tag && !tag->isEmpty())
    {
        ftag->setValue(FileTag::ALBUM,
                       QString::fromUtf8(tag->album().toCString(TRUE)).trimmed());
        ftag->setValue(FileTag::ARTIST,
                       QString::fromUtf8(tag->artist().toCString(TRUE)).trimmed());
        ftag->setValue(FileTag::COMMENT,
                       QString::fromUtf8(tag->comment().toCString(TRUE)).trimmed());
        ftag->setValue(FileTag::GENRE,
                       QString::fromUtf8(tag->genre().toCString(TRUE)).trimmed());
        ftag->setValue(FileTag::TITLE,
                       QString::fromUtf8(tag->title().toCString(TRUE)).trimmed());
        ftag->setValue(FileTag::YEAR, tag->year());
        ftag->setValue(FileTag::TRACK, tag->track());
    }

    if (fileRef.audioProperties())
        ftag->setValue(FileTag::LENGTH, fileRef.audioProperties()->length());

    return ftag;
}

QObject* DecoderFLACFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
    return d;
}

void DecoderFLACFactory::showSettings(QWidget *)
{}

void DecoderFLACFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About FLAC Audio Plugin"),
                        tr("Qmmp FLAC Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderFLACFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/flac_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderFLACFactory)
