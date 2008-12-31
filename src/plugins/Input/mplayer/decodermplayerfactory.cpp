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

#include "decoder_mplayer.h"
#include "decodermplayerfactory.h"


// DecoderMplayerFactory

bool DecoderMplayerFactory::supports(const QString &source) const
{
    QStringList filters = MplayerInfo::filters();
    foreach(QString filter, filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (regexp.exactMatch(source))
            return TRUE;
    }
    return FALSE;
}

bool DecoderMplayerFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderMplayerFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Mplayer Plugin");
    properties.shortName = "mplayer";
    properties.filter = MplayerInfo::filters().join(" ");
    properties.description = tr("Video Files");
    //properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.protocols = "file";
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    properties.noInput = TRUE;
    properties.noOutput = TRUE;
    return properties;
}

Decoder *DecoderMplayerFactory::create(QObject *parent, QIODevice *input,
                                      Output *output, const QString &url)
{
    Q_UNUSED(input);
    Q_UNUSED(output);
    return new DecoderMplayer(parent, this, url);
}

QList<FileInfo *> DecoderMplayerFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    Q_UNUSED(useMetaData);
    QList<FileInfo *> info;
    info << MplayerInfo::createFileInfo(fileName);
    return info;
}

QObject* DecoderMplayerFactory::showDetails(QWidget *, const QString &)
{
    return 0;
}

void DecoderMplayerFactory::showSettings(QWidget *)
{
     /*SettingsDialog *s = new SettingsDialog(parent);
     s->show();*/
}

void DecoderMplayerFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Mplayer Audio Plugin"),
                        tr("Qmmp Mplayer Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderMplayerFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/phonon_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderMplayerFactory)
