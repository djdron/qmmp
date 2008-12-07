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

#include "detailsdialog.h"
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

Decoder *DecoderWildMidiFactory::create(QObject *parent, QIODevice *input,
                                        Output *output, const QString &path)
{
    Q_UNUSED(input);
    return new DecoderWildMidi(parent, this, output, path);
}

QList<FileInfo *> DecoderWildMidiFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    QList <FileInfo*> list;
    FileInfo *info = new FileInfo(fileName);
    list << info;
    return list;
}

QObject* DecoderWildMidiFactory::showDetails(QWidget *parent, const QString &path)
{
    /*DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();*/
    return 0;
}

void DecoderWildMidiFactory::showSettings(QWidget *)
{}

void DecoderWildMidiFactory::showAbout(QWidget *parent)
{
    /*QMessageBox::about (parent, tr("About WildMidi Audio Plugin"),
                        tr("Qmmp WildMidi Audio Plugin")+"\n"+
                        tr("WildMidi library version:") +
                        QString(" %1").arg(WavpackGetLibraryVersionString ())+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));*/
}

QTranslator *DecoderWildMidiFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/wildmidi_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderWildMidiFactory)
