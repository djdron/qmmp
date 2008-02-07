#include <QtGui>
#include <taglib/tag.h>
#include <taglib/fileref.h>

#include "detailsdialog.h"
#include "decoder_mpc.h"
#include "decodermpcfactory.h"


// DecoderMPCFactory

bool DecoderMPCFactory::supports(const QString &source) const
{

    return (source.right(4).toLower() == ".mpc");
}

bool DecoderMPCFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderMPCFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Musepack Plugin");
    properties.filter = "*.mpc";
    properties.description = tr("Musepack Files");
    //properties.contentType = ;
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    return properties;
}

Decoder *DecoderMPCFactory::create(QObject *parent, QIODevice *input,
                                   Output *output)
{
    return new DecoderMPC(parent, this, input, output);
}

FileTag *DecoderMPCFactory::createTag(const QString &source)
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

QObject* DecoderMPCFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
    return d;
}

void DecoderMPCFactory::showSettings(QWidget *)
{}

void DecoderMPCFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Musepack Audio Plugin"),
                        tr("Qmmp Musepack Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderMPCFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/mpc_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderMPCFactory)
