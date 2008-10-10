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

#include "decoder_cue.h"
#include "cueparser.h"
#include "decodercuefactory.h"


// DecoderOggFactory

bool DecoderCUEFactory::supports(const QString &source) const
{
    return source.right(4).toLower() == ".cue";
}

bool DecoderCUEFactory::canDecode(QIODevice *input) const
{
    return FALSE;
}

const DecoderProperties DecoderCUEFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("CUE Plugin");
    properties.shortName = "cue";
    properties.filter = "*.cue";
    properties.description = tr("CUE Files");
    //properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.protocols = "cue";
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    properties.noInput = TRUE;
    properties.noOutput = TRUE;
    return properties;
}

Decoder *DecoderCUEFactory::create(QObject *parent, QIODevice *input,
                                      Output *output, const QString &url)
{
    return new DecoderCUE(parent, this, url);
}

//FileInfo *DecoderCUEFactory::createFileInfo(const QString &source)
QList<FileInfo *> DecoderCUEFactory::createPlayList(const QString &fileName)
{
    CUEParser parser(fileName);
    return parser.createPlayList();
}

QObject* DecoderCUEFactory::showDetails(QWidget *parent, const QString &path)
{
    return 0;
}

void DecoderCUEFactory::showSettings(QWidget *)
{}

void DecoderCUEFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About CUE Audio Plugin"),
                        tr("Qmmp CUE Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderCUEFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/cue_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderCUEFactory)
