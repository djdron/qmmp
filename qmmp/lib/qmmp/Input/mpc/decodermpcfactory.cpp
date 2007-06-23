#include <QtGui>
#include <taglib/tag.h>
#include <taglib/fileref.h>

#include "detailsdialog.h"
#include "decoder_mpc.h"
#include "tag.h"
#include "decodermpcfactory.h"


// DecoderMPCFactory

bool DecoderMPCFactory::supports(const QString &source) const
{

    return (source.right(4).toLower() == ".mpc");
}

const QString &DecoderMPCFactory::name() const
{
    static QString name (tr("Musepack Plugin"));
    return name;
}


const QString &DecoderMPCFactory::filter() const
{
    static QString filter("*.mpc");
    return filter;
}


const QString &DecoderMPCFactory::description() const
{
    static QString desc(tr("Musepack Files"));
    return desc;
}


Decoder *DecoderMPCFactory::create(QObject *parent, QIODevice *input,
                                   Output *output)
{
    return new DecoderMPC(parent, this, input, output);
}

FileTag *DecoderMPCFactory::createTag(const QString &source)
{
    FileTag *tag = new Tag(source);
    return tag;
}

void DecoderMPCFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
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
