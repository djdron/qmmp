/***************************************************************************
 *   Copyright (C) 2007-2008 by Ilya Kotov                                 *
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
extern "C"
{
#include <sndfile.h>
}

#include "decoder_sndfile.h"
#include "decodersndfilefactory.h"


// DecoderSndFileFactory

bool DecoderSndFileFactory::supports(const QString &source) const
{

    if ((source.right(3).toLower() == ".au") ||
            (source.right(4).toLower() == ".snd") ||
            (source.right(4).toLower() == ".aif") ||
            (source.right(5).toLower() == ".aiff") ||
            (source.right(5).toLower() == ".8svx") ||
            (source.right(4).toLower() == ".sph") ||
            (source.right(3).toLower() == ".sf") ||
            (source.right(4).toLower() == ".voc"))
        return TRUE;
    else if (source.right(4).toLower() == ".wav")
    {
        //try top open the file
        SF_INFO snd_info;
        SNDFILE *sndfile = sf_open(source.toLocal8Bit(), SFM_READ, &snd_info);
        if (!sndfile)
            return FALSE;
        sf_close (sndfile);
        sndfile = 0;
        return TRUE;
    }
    return FALSE;
}

bool DecoderSndFileFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderSndFileFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Sndfile Plugin");
    properties.filter = "*.wav *.au *.snd *.aif *.aiff *.8svx *.sph *.sf *.voc";
    properties.description = tr("PCM Files");
    //properties.contentType = "";
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    properties.noInput = TRUE;
    properties.protocols = "file";
    return properties;
}

Decoder *DecoderSndFileFactory::create(QObject *parent, QIODevice *input,
                                       Output *output, const QString &path)
{
    Q_UNUSED(input);
    return new DecoderSndFile(parent, this, output, path);
}

QList<FileInfo *> DecoderSndFileFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    QList <FileInfo *> list;
    SF_INFO snd_info;
    SNDFILE *sndfile = 0;
    memset (&snd_info, 0, sizeof(snd_info));
    snd_info.format = 0;
    sndfile = sf_open(fileName.toLocal8Bit(), SFM_READ, &snd_info);
    if (!sndfile)
        return list;

    list << new FileInfo(fileName);
    if (useMetaData)
    {
        if (sf_get_string(sndfile, SF_STR_TITLE))
        {
            char* title = strdup(sf_get_string(sndfile, SF_STR_TITLE));
            list.at(0)->setMetaData(Qmmp::TITLE, QString::fromUtf8(title).trimmed());
        }
        if (sf_get_string(sndfile, SF_STR_ARTIST))
        {
            char* artist = strdup(sf_get_string(sndfile, SF_STR_ARTIST));
            list.at(0)->setMetaData(Qmmp::ARTIST, QString::fromUtf8(artist).trimmed());
        }
        if (sf_get_string(sndfile, SF_STR_COMMENT))
        {
            char* comment = strdup(sf_get_string(sndfile, SF_STR_COMMENT));
            list.at(0)->setMetaData(Qmmp::COMMENT, QString::fromUtf8(comment).trimmed());
        }
    }

    list.at(0)->setLength(int(snd_info.frames / snd_info.samplerate));

    sf_close(sndfile);
    return list;
}

QObject* DecoderSndFileFactory::showDetails(QWidget *parent, const QString &path)
{
    Q_UNUSED(parent);
    Q_UNUSED(path);
    return 0;
}

void DecoderSndFileFactory::showSettings(QWidget *)
{}

void DecoderSndFileFactory::showAbout(QWidget *parent)
{
    char  version [128] ;
    sf_command (NULL, SFC_GET_LIB_VERSION, version, sizeof (version)) ;
    QMessageBox::about (parent, tr("About Sndfile Audio Plugin"),
                        tr("Qmmp Sndfile Audio Plugin")+"\n"+
                        tr("Compiled against")+" "+QString(version)+"\n" +
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderSndFileFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/sndfile_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderSndFileFactory)
