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

#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include <QSettings>
#include <QTextStream>
#include <QTextCodec>

#include <qmmp/decoder.h>

#include "cueparser.h"

CUEParser::CUEParser(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("CUEParser: error: %s", qPrintable(file.errorString()));
        return;
    }
    QString album, genre, date, comment;
    QTextStream textStream (&file);
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    QTextCodec *codec = QTextCodec::codecForName(settings.value("CUE/encoding","ISO-8859-1").toByteArray ());
    textStream.setCodec(codec);
    while (!textStream.atEnd())
    {
        QString line = textStream.readLine().trimmed();
        QStringList words = splitLine(line);
        if (words.size() < 2)
            continue;

        if (words[0] == "FILE")
        {
            m_filePath = QUrl(fileName).path ();
            m_filePath = QFileInfo(m_filePath).dir().filePath(words[1]);
        }
        else if (words[0] == "PERFORMER")
        {
            if (m_infoList.isEmpty())
                continue;
            else
                m_infoList.last().setMetaData(Qmmp::ARTIST, words[1]);

        }
        else if (words[0] == "TITLE")
        {
            if (m_infoList.isEmpty())
                album = words[1];
            else
                m_infoList.last().setMetaData(Qmmp::TITLE, words[1]);
        }
        else if (words[0] == "TRACK")
        {
            FileInfo info("cue://" + fileName + QString("#%1").arg(words[1].toInt()));
            info.setMetaData(Qmmp::TRACK, words[1].toInt());
            m_infoList << info;
            m_offsets << 0;
        }
        else if (words[0] == "INDEX")
        {
            if (m_infoList.isEmpty())
                continue;
            m_infoList.last ().setLength(getLength(words[2]));
            m_offsets.last() = getLength(words[2]);
        }
        else if (words[0] == "REM")
        {
            if (words.size() < 3)
                continue;
            if (words[1] == "GENRE")
                genre = words[2];
            else if (words[1] == "DATE")
                date = words[2];
            else if (words[1] == "COMMENT")
                comment = words[2];
        }
    }
    //calculate length
    for (int i = 0; i < m_infoList.size() - 1; ++i)
        m_infoList[i].setLength(m_infoList[i+1].length() - m_infoList[i].length());
    //calculate last item length
    QList <FileInfo *> f_list;
    f_list = Decoder::createPlayList(m_filePath);
    qint64 l = f_list.isEmpty() ? 0 : f_list.at(0)->length();
    if (l > m_infoList.last().length())
        m_infoList.last().setLength(l - m_infoList.last().length());
    else
        m_infoList.last().setLength(0);

    for(int i = 0; i < m_infoList.size(); ++i)
    {
        m_infoList[i].setMetaData(Qmmp::ALBUM, album);
        m_infoList[i].setMetaData(Qmmp::GENRE, genre);
        m_infoList[i].setMetaData(Qmmp::YEAR, date);
        m_infoList[i].setMetaData(Qmmp::COMMENT, comment);
    }
    file.close();
}


CUEParser::~CUEParser()
{
}

QList<FileInfo*> CUEParser::createPlayList()
{
    QList<FileInfo*> list;
    foreach(FileInfo info, m_infoList)
    {
        list << new FileInfo(info);
    }
    return list;
}

const QString CUEParser::filePath()
{
    return m_filePath;
}

qint64 CUEParser::offset(int track)
{
    return m_offsets.at(track - 1);
}

qint64 CUEParser::length(int track)
{
    return m_infoList.at(track - 1).length();
}

int CUEParser::count()
{
    return m_infoList.count();
}

FileInfo *CUEParser::info(int track)
{
    return &m_infoList[track - 1];
}

QStringList CUEParser::splitLine(const QString &line)
{
    //qDebug("row string = %s",qPrintable(line));
    QStringList list;
    QString buf = line.trimmed();
    if (buf.isEmpty())
        return list;
    while (!buf.isEmpty())
    {
        //qDebug(qPrintable(buf));
        if (buf.startsWith('"'))
        {
            int end = buf.indexOf('"',1);
            list << buf.mid (1, end - 1);
            buf.remove (0, end+1);
        }
        else
        {
            int end = buf.indexOf(' ', 0);
            if (end < 0)
                end = buf.size();
            list << buf.mid (0, end);
            buf.remove (0, end);
        }
        buf = buf.trimmed();
    }
    return list;
}

int CUEParser::getLength(const QString &str)
{
    QStringList list = str.split(":");
    if (list.size() < 2)
        return 0;
    return list.at(0).toInt()*60 + list.at(1).toInt();
}
