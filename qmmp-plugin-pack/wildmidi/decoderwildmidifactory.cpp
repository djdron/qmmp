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

//#include "detailsdialog.h"
#include "decoder_wildmidi.h"
#include "decoderwildmidifactory.h"


// DecoderWildMidiFactory

bool DecoderWildMidiFactory::supports(const QString &source) const
{

    return (source.right(4).toLower() == ".mid");
}

bool DecoderWildMidiFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderWildMidiFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("WildMidi Plugin");
    properties.filter = "*.mid";
    properties.description = tr("Midi Files");
    //properties.contentType = ;
    properties.hasAbout = FALSE;
    properties.hasSettings = FALSE;
    properties.noInput = TRUE;
    properties.protocols = "file";
    return properties;
}

Decoder *DecoderWildMidiFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderWildMidi(path);
}

QList<FileInfo *> DecoderWildMidiFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    QList <FileInfo*> list;
    FileInfo *info = new FileInfo(fileName);

    /*void *midi_ptr = WildMidi_Open (fileName.toLocal8Bit());
    if(midi_ptr)
    {
        //wm_info = new _WM_Info;
        _WM_Info *wm_info = WildMidi_GetInfo(midi_ptr);
        info->setLength(wm_info->approx_total_samples / 44100);
        qDebug("===== %lu", wm_info->approx_total_samples);
        WildMidi_Close(midi_ptr);
    }*/

    list << info;
    return list;
}

MetaDataModel* DecoderWildMidiFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    /*DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();*/
    return 0;
}

void DecoderWildMidiFactory::showSettings(QWidget *)
{}

void DecoderWildMidiFactory::showAbout(QWidget *parent)
{}

QTranslator *DecoderWildMidiFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/wildmidi_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderWildMidiFactory)
