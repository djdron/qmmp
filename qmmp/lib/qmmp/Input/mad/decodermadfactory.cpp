#include <QtGui>
#include <QDialog>
#include <QMessageBox>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/tfile.h>
#include <taglib/mpegfile.h>

#include "detailsdialog.h"
#include "settingsdialog.h"
#include "decoder_mad.h"
#include "id3tag.h"
#include "decodermadfactory.h"

// DecoderMADFactory

bool DecoderMADFactory::supports(const QString &source) const
{
    QString ext = source.right(4).toLower();
    return ext == ".mp1" || ext == ".mp2" || ext == ".mp3";
}

bool DecoderMADFactory::canDecode(QIODevice *input) const
{
    static bool c = FALSE;
    return c;
}

const DecoderProperties &DecoderMADFactory::properties() const
{
    static DecoderProperties properties;
    properties.name = tr("MPEG Plugin");
    properties.filter = "*.mp1 *.mp2 *.mp3";
    properties.description = tr("MPEG Files");
    properties.contentType = "audio/mp3;audio/mpeg";
    properties.hasAbout = TRUE;
    properties.hasSettings = TRUE;
    return properties;
}

Decoder *DecoderMADFactory::create(QObject *parent, QIODevice *input, Output *output)
{
    return new DecoderMAD(parent, this, input, output);
}

FileTag *DecoderMADFactory::createTag(const QString &source)
{
    FileTag *tag = new ID3Tag(source);
    return tag;
}

void DecoderMADFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
}

void DecoderMADFactory::showSettings(QWidget *parent)
{
    SettingsDialog *s = new SettingsDialog(parent);
    s -> show();
}

void DecoderMADFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About MPEG Audio Plugin"),
                        tr("Qmmp MPEG Audio Plugin")+"\n"+
                        tr("Compiled against libmad version:")+" "+
                        QString("%1.%2.%3%4").arg(MAD_VERSION_MAJOR)
                        .arg(MAD_VERSION_MINOR)
                        .arg(MAD_VERSION_PATCH).arg(MAD_VERSION_EXTRA)+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>")+"\n"+
                        tr("Source code based on mq3 progect")
                       );
}

QTranslator *DecoderMADFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/mad_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderMADFactory)
