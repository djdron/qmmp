#include <QtGui>
#include <QDialog>
#include <QMessageBox>
#include <mad.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/apetag.h>
#include <taglib/tfile.h>
#include <taglib/mpegfile.h>

#include "detailsdialog.h"
#include "settingsdialog.h"
#include "decoder_mad.h"
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

        while ((dec_res = mad_header_decode(&header, &stream)) == -1
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
    FileTag *ftag = new FileTag();
    TagLib::Tag *tag = 0;
    TagLib::MPEG::File fileRef(source.toLocal8Bit ());

    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("MAD");
    QTextCodec *codec_v1 =
        QTextCodec::codecForName(settings.value("ID3v1_encoding","UTF-8" )
                                 .toByteArray ());
    QTextCodec *codec_v2 =
        QTextCodec::codecForName(settings.value("ID3v2_encoding","UTF-8" )
                                 .toByteArray ());
    if (!codec_v1)
        codec_v1 = QTextCodec::codecForName ("UTF-8");
    if (!codec_v2)
        codec_v2 = QTextCodec::codecForName ("UTF-8");

    QTextCodec *codec = 0;

    uint tag_array[3];
    tag_array[0] = settings.value("tag_1", SettingsDialog::ID3v2).toInt();
    tag_array[1] = settings.value("tag_2", SettingsDialog::Disabled).toInt();
    tag_array[2] = settings.value("tag_3", SettingsDialog::Disabled).toInt();


    for (int i = 0; i < 3; ++i)
    {
        switch ((uint) tag_array[i])
        {
        case SettingsDialog::ID3v1:
        {
            codec = codec_v1;
            tag = fileRef.ID3v1Tag();
            break;
        }
        case SettingsDialog::ID3v2:
        {
            codec = codec_v2;
            tag = fileRef.ID3v2Tag();
            break;
        }
        case SettingsDialog::APE:
        {
            codec = QTextCodec::codecForName ("UTF-8");
            tag = fileRef.APETag();
            break;
        }
        case SettingsDialog::Disabled:
        {
            break;
        }
        }
        if(tag && !tag->isEmpty())
            break;
    }
    settings.endGroup();

    if (tag && codec)
    {
        bool utf = codec->name ().contains("UTF");
        TagLib::String album = tag->album();
        TagLib::String artist = tag->artist();
        TagLib::String comment = tag->comment();
        TagLib::String genre = tag->genre();
        TagLib::String title = tag->title();

        ftag->setValue(FileTag::ALBUM,
                       codec->toUnicode(album.toCString(utf)).trimmed());
        ftag->setValue(FileTag::ARTIST,
                       codec->toUnicode(artist.toCString(utf)).trimmed());
        ftag->setValue(FileTag::COMMENT,
                       codec->toUnicode(comment.toCString(utf)).trimmed());
        ftag->setValue(FileTag::GENRE,
                       codec->toUnicode(genre.toCString(utf)).trimmed());
        ftag->setValue(FileTag::TITLE,
                       codec->toUnicode(title.toCString(utf)).trimmed());
        ftag->setValue(FileTag::YEAR,
                       tag->year());
        ftag->setValue(FileTag::TRACK,
                       tag->track());
    }
    if (fileRef.audioProperties())
        ftag->setValue(FileTag::LENGTH,fileRef.audioProperties()->length());
    return ftag;
}

QObject* DecoderMADFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
    return d;
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
