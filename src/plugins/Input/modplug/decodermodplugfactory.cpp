#include <QtGui>
#include <QStringList>

#include <libmodplug/stdafx.h>
#include <libmodplug/it_defs.h>
#include <libmodplug/sndfile.h>

#include "settingsdialog.h"
#include "detailsdialog.h"
#include "decoder_modplug.h"
#include "archivereader.h"
#include "decodermodplugfactory.h"


// DecoderModPlugFactory

bool DecoderModPlugFactory::supports(const QString &source) const
{
    QString lExt = source.section(".",-1).toLower();
    lExt.prepend(".");
    QStringList lExtList;
    lExtList << ".amf" << ".ams" << ".dbm" << ".dbf"  << ".dsm" << ".far" << ".mdl"
    << ".stm" << ".ult" << ".ult" << ".j2b" << ".mt2" << ".mdz" /*<< ".mdr"*/ << ".mdgz"
    << ".mdbz"  << ".mod" << ".s3z" << /*".s3r" <<*/ ".s3gz" << ".s3m" << ".xmz" /*<< ".xmr"*/
    << ".xmgz" << ".itz"  << /*".itr" <<*/ ".itgz" << ".dmf" << ".umx" << ".it" << ".669"
    << ".xm" << ".mtm" << ".psm" << ".ft2";
    if (lExtList.contains(lExt))
        return TRUE;
    return FALSE;
}

bool DecoderModPlugFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderModPlugFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("ModPlug Plugin");
    properties.filter = (QString) "*.amf *.ams *.dbm *.dbf *.dsm *.far *.mdl *.stm *.ult" +
                        "*.j2b *.mt2 *.mdz *.mdr *.mdgz *.mdbz *.mod *.s3z *.s3r *.s3gz *.s3m *.xmz"+
                        "*.xmr *.xmgz *.itz *.itr *.itgz *.dmf *.umx *.it *.669 *.xm *.mtm *.psm *.ft2";
    properties.description = tr("ModPlug Files");
    //properties.contentType = ;
    properties.hasAbout = TRUE;
    properties.hasSettings = TRUE;
    return properties;
}

Decoder *DecoderModPlugFactory::create(QObject *parent, QIODevice *input,
                                       Output *output)
{
    return new DecoderModPlug(parent, this, input, output);
}

FileTag *DecoderModPlugFactory::createTag(const QString &source)
{
    FileTag *ftag = new FileTag();
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    if (settings.value("UseFileName", FALSE).toBool())
    {
        ftag->setValue(FileTag::TITLE, source.section('/',-1));
        return ftag;
    }
    ArchiveReader reader(0);
    QByteArray buffer;
    if (reader.isSupported(source))
    {
        buffer = reader.unpack(source);
    }
    else
    {
        QFile file(source);
        if (!file.open(QIODevice::ReadOnly))
        {
            qWarning("DecoderModPlugFactory: error: %s", qPrintable(file.errorString ()));
            return ftag;
        }
        buffer = file.readAll();
        file.close();
    }
    CSoundFile* soundFile = new CSoundFile();
    soundFile->Create((uchar*) buffer.data(), buffer.size());

    ftag->setValue(FileTag::LENGTH, (int) soundFile->GetSongTime());
    ftag->setValue(FileTag::TITLE, QString::fromUtf8(soundFile->GetTitle()));
    soundFile->Destroy();
    delete soundFile;
    return ftag;
}

QObject* DecoderModPlugFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
    return d;
}

void DecoderModPlugFactory::showSettings(QWidget *parent)
{
    SettingsDialog *d = new SettingsDialog(parent);
    d->show();
}

void DecoderModPlugFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About ModPlug Audio Plugin"),
                        tr("Qmmp ModPlug Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>")+"\n"+
                        tr("Based on the Modplug Plugin for Xmms")+"\n"+
                        tr("Modplug Plugin developers:")+"\n"+
                        tr("Olivier Lapicque <olivierl@jps.net>")+"\n"+
                        tr("Kenton Varda <temporal@gauge3d.org>")+"\n"+
                        tr("Konstanty Bialkowski <konstanty@ieee.org>"));
}

QTranslator *DecoderModPlugFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/modplug_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderModPlugFactory)
