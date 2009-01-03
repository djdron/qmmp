/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
    properties.shortName = "modplug";
    properties.hasAbout = TRUE;
    properties.hasSettings = TRUE;
    properties.noInput = TRUE;
    properties.protocols = "file";
    return properties;
}

Decoder *DecoderModPlugFactory::create(QObject *parent, QIODevice *input,
                                       Output *output, const QString &path)
{
    Q_UNUSED(input);
    return new DecoderModPlug(parent, this, output, path);
}

QList<FileInfo *> DecoderModPlugFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    QList <FileInfo*> list;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    if (!useMetaData || settings.value("UseFileName", FALSE).toBool())
    {
        list << new FileInfo(fileName);
        list.at(0)->setMetaData(Qmmp::TITLE, fileName.section('/',-1));
        return list;
    }
    ArchiveReader reader(0);
    QByteArray buffer;
    if (reader.isSupported(fileName))
    {
        buffer = reader.unpack(fileName);
    }
    else
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            qWarning("DecoderModPlugFactory: error: %s", qPrintable(file.errorString ()));
            return list;
        }
        buffer = file.readAll();
        file.close();
    }
    CSoundFile* soundFile = new CSoundFile();
    soundFile->Create((uchar*) buffer.data(), buffer.size());
    list << new FileInfo(fileName);
    list.at(0)->setLength((int) soundFile->GetSongTime());
    list.at(0)->setMetaData(Qmmp::TITLE, QString::fromUtf8(soundFile->GetTitle()));
    soundFile->Destroy();
    delete soundFile;
    return list;
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
