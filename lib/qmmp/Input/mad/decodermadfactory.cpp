#include <QtGui>
#include <QDialog>
#include <QMessageBox>
#include <mad.h>
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
    char buf[16 * 512];

    if (input->peek(buf,sizeof(buf)) == sizeof(buf))
    {
        struct mad_stream stream;
        struct mad_header header;
        int dec_res;

        mad_stream_init (&stream);
        mad_header_init (&header);
        mad_stream_buffer (&stream, (unsigned char *) buf, sizeof(buf));
        stream.error = MAD_ERROR_NONE;

        while((dec_res = mad_header_decode(&header, &stream)) == -1
                && MAD_RECOVERABLE(stream.error))
        ;
        return dec_res != -1 ? TRUE: FALSE;
    }
    return FALSE;
}

const DecoderProperties DecoderMADFactory::properties() const
{
    DecoderProperties properties;
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
