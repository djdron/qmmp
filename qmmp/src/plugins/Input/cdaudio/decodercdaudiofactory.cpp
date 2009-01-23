/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#include <cdio/version.h>
#include "settingsdialog.h"
#include "decoder_cdaudio.h"
#include "decodercdaudiofactory.h"


// DecoderCDAudioFactory

bool DecoderCDAudioFactory::supports(const QString &source) const
{
    return source == "/" || source.startsWith("/dev");
}

bool DecoderCDAudioFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderCDAudioFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("CD Audio Plugin");
    properties.shortName = "cdaudio";
    properties.protocols = "cdda";
    properties.hasAbout = TRUE;
    properties.noInput = TRUE;
    properties.hasSettings = TRUE;
    return properties;
}

Decoder *DecoderCDAudioFactory::create(QObject *parent, QIODevice *input,
                                       Output *output, const QString &url)
{
    Q_UNUSED(input);
    return new DecoderCDAudio(parent, this, url, output);
}

QList<FileInfo *> DecoderCDAudioFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    Q_UNUSED(useMetaData);
    QList <FileInfo*> list;
    QList <CDATrack> tracks = DecoderCDAudio::generateTrackList(QUrl(fileName).path());
    foreach(CDATrack t, tracks)
    list << new FileInfo(t.info);
    return list;
}

QObject* DecoderCDAudioFactory::showDetails(QWidget *parent, const QString &path)
{
    Q_UNUSED(parent);
    Q_UNUSED(path);
    return 0;
}

void DecoderCDAudioFactory::showSettings(QWidget *parent)
{
    SettingsDialog *d = new SettingsDialog(parent);
    d->show();
}

void DecoderCDAudioFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About CD Audio Plugin"),
                        tr("Qmmp CD Audio Plugin")+"\n"+
                        tr("Compiled against libcdio version:")+ " "+CDIO_VERSION+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>")+"\n"+
                        tr("Usage: open cdda:/// using Add URL dialog or command line"));
}

QTranslator *DecoderCDAudioFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/cdaudio_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderCDAudioFactory)
