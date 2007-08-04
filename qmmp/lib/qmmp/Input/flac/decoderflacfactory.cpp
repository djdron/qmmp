#include <QtGui>
#include <taglib/tag.h>
#include <taglib/fileref.h>

#include "detailsdialog.h"
#include "decoder_flac.h"
#include "tag.h"
#include "decoderflacfactory.h"


// DecoderFLACFactory

bool DecoderFLACFactory::supports(const QString &source) const
{

    return (source.right(5).toLower() == ".flac");
}

bool DecoderFLACFactory::canDecode(QIODevice *input) const
{
    static bool c = FALSE;
    return c;
}

const DecoderProperties &DecoderFLACFactory::properties() const
{
    static DecoderProperties properties;
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
    FileTag *tag = new Tag(source);
    return tag;
}

void DecoderFLACFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
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
