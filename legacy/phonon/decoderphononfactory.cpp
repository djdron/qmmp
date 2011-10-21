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

#include <phonon/phononnamespace.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <phonon/videowidget.h>
#include <phonon/effect.h>
#include <phonon/effectparameter.h>

#include "decoder_phonon.h"
//#include "settingsdialog.h"
#include "decoderphononfactory.h"


// DecoderPhononFactory

bool DecoderPhononFactory::supports(const QString &source) const
{
    QStringList filters;
    QStringList mimeTypes = Phonon::BackendCapabilities::availableMimeTypes();
    return source.right(4).toLower() == ".avi";
}

bool DecoderPhononFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderPhononFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Phonon Plugin");
    properties.shortName = "phonon";
    properties.filter = "*.avi";
    properties.description = tr("Phonon Files");
    //properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.protocols = "file";
    properties.hasAbout = FALSE;
    properties.hasSettings = FALSE;
    properties.noInput = TRUE;
    properties.noOutput = TRUE;
    return properties;
}

Decoder *DecoderPhononFactory::create(QObject *parent, QIODevice *input,
                                      Output *output, const QString &url)
{
    Q_UNUSED(input);
    Q_UNUSED(output);
    return new DecoderPhonon(parent, this, url);
}

QList<FileInfo *> DecoderPhononFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    Q_UNUSED(useMetaData);
    QList<FileInfo *> info;
    info << new FileInfo(fileName);
    return info;
}

QObject* DecoderPhononFactory::showDetails(QWidget *, const QString &)
{
    return 0;
}

void DecoderPhononFactory::showSettings(QWidget *parent)
{
     /*SettingsDialog *s = new SettingsDialog(parent);
     s->show();*/
}

void DecoderPhononFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Phonon Audio Plugin"),
                        tr("Qmmp Phonon Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderPhononFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/phonon_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderPhononFactory)
