/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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
#include <QDir>
#include <QDirIterator>
#include <QSettings>
#include <QTextStream>
#include <QTextCodec>
#include <qmmp/decoder.h>
#include <qmmp/metadatamanager.h>
#ifdef WITH_ENCA
#include <enca.h>
#endif
#include "cueparser.h"

CUEParser::CUEParser(const QString &url)
{
    QString fileName = url;
    if(url.contains("://"))
    {
        QString p = QUrl(url).path();
        p.replace(QString(QUrl::toPercentEncoding("#")), "#");
        p.replace(QString(QUrl::toPercentEncoding("?")), "?");
        p.replace(QString(QUrl::toPercentEncoding("%")), "%");
        p.replace(QString(QUrl::toPercentEncoding(":")), ":");
        fileName = p;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("CUEParser: error: %s", qPrintable(file.errorString()));
        return;
    }
    QTextStream textStream (&file);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("CUE");
    QTextCodec *codec = QTextCodec::codecForName(settings.value("encoding","ISO-8859-1").toByteArray ());
    if(!codec)
        codec = QTextCodec::codecForName("UTF-8");
    m_dirty = settings.value("dirty_cue", true).toBool();
#ifdef WITH_ENCA
    EncaAnalyser analyser = 0;
    if(settings.value("use_enca", false).toBool())
    {
        analyser = enca_analyser_alloc(settings.value("enca_lang").toByteArray ().constData());
        if(analyser)
        {
            EncaEncoding encoding = enca_analyse(analyser,
                                                 (uchar *)file.readAll().constData(),
                                                 file.size());
            file.reset();
            if(encoding.charset != ENCA_CS_UNKNOWN)
            {
                codec = QTextCodec::codecForName(enca_charset_name(encoding.charset,ENCA_NAME_STYLE_ENCA));
                //qDebug("CUEParser: detected charset: %s",
                  //     enca_charset_name(encoding.charset,ENCA_NAME_STYLE_ENCA));
            }
        }
    }
#endif
    settings.endGroup();
    //qDebug("CUEParser: using %s encoding", codec->name().constData());
    textStream.setCodec(codec);
    QString album, genre, date, comment, artist, file_path;
    bool new_file = false;

    while (!textStream.atEnd())
    {
        QString line = textStream.readLine().trimmed();
        QStringList words = splitLine(line);
        if (words.size() < 2)
            continue;

        if (words[0] == "FILE")
        {
            if(!m_infoList.isEmpty())
            {
                QList <FileInfo *> f_list = MetaDataManager::instance()->createPlayList(file_path, false);
                qint64 l = f_list.isEmpty() ? 0 : f_list.at(0)->length() * 1000;
                if (l > m_offsets.last())
                    m_infoList.last().setLength(l - m_offsets.last());
                else
                    m_infoList.last().setLength(0);
            }
            file_path = getDirtyPath(fileName, QFileInfo(fileName).dir().filePath(words[1]));
            new_file = true;
        }

        else if (words[0] == "PERFORMER")
        {
            if(m_infoList.isEmpty())
                artist = words[1];
            else
                m_infoList.last().setMetaData(Qmmp::ARTIST, words[1]);
        }
        else if (words[0] == "TITLE")
        {
            if(m_infoList.isEmpty())
                album = words[1];
            else
                m_infoList.last().setMetaData(Qmmp::TITLE, words[1]);
        }
        else if (words[0] == "TRACK")
        {
            QString path = fileName;
            path.replace("%", QString(QUrl::toPercentEncoding("%"))); //replace special symbols
            path.replace("#", QString(QUrl::toPercentEncoding("#")));
            path.replace("?", QString(QUrl::toPercentEncoding("?")));
            path.replace(":", QString(QUrl::toPercentEncoding(":")));
            FileInfo info("cue://" + path + QString("#%1").arg(words[1].toInt()));
            info.setMetaData(Qmmp::TRACK, words[1].toInt());
            info.setMetaData(Qmmp::ALBUM, album);
            info.setMetaData(Qmmp::GENRE, genre);
            info.setMetaData(Qmmp::YEAR, date);
            info.setMetaData(Qmmp::COMMENT, comment);
            info.setMetaData(Qmmp::ARTIST, artist);
            m_infoList << info;
            m_offsets << 0;
            m_files << file_path;
        }
        else if (words[0] == "INDEX" && words[1] == "01")
        {
            if (m_infoList.isEmpty())
                continue;
            m_offsets.last() = getLength(words[2]);
            int c = m_infoList.count();
            if(c > 1 && !new_file)
                m_infoList[c - 2].setLength(m_offsets[c - 1] - m_offsets[c - 2]);
            new_file = false;
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
    file.close();
#ifdef WITH_ENCA
    if(analyser)
        enca_analyser_free(analyser);
#endif
    if(m_infoList.isEmpty())
    {
        qWarning("CUEParser: invalid cue file");
        return;
    }
    //calculate last item length
    QList <FileInfo *> f_list = MetaDataManager::instance()->createPlayList(file_path, false);
    qint64 l = f_list.isEmpty() ? 0 : f_list.at(0)->length() * 1000;
    if (l > m_offsets.last())
        m_infoList.last().setLength(l - m_offsets.last());
    else
        m_infoList.last().setLength(0);
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
        list.last()->setLength(list.last()->length()/1000);
    }
    return list;
}

const QString CUEParser::filePath(int track)
{
    return (track <= m_files.size()) ? m_files[track - 1] : QString();
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

const QString CUEParser::trackURL(int track)
{
    return m_infoList[track - 1].path();
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

qint64 CUEParser::getLength(const QString &str)
{
    QStringList list = str.split(":");
    if (list.size() == 2)
        return (qint64)list.at(0).toInt()*60000 + list.at(1).toInt()*1000;
    else if (list.size() == 3)
        return (qint64)list.at(0).toInt()*60000 + list.at(1).toInt()*1000 + list.at(2).toInt()*1000/75;
    return 0;
}

QString CUEParser::getDirtyPath(const QString &cue, const QString &path)
{

    if (Decoder::findByPath(path) || ! m_dirty)
        return path;

    QStringList candidates;
    QDirIterator it(QFileInfo(path).dir().path(), QDir::Files);
    while (it.hasNext())
    {
        it.next();
        QString f = it.filePath();
        if ((f != cue) && Decoder::findByPath(f))
            candidates.push_back(f);
    }

    if (candidates.empty())
        return path;
    else if (candidates.count() == 1)
        return candidates.first();

    int dot = cue.lastIndexOf('.');
    if (dot != -1)
    {
        QRegExp r(QRegExp::escape(cue.left(dot)) + "\\.[^\\.]+$");

        int index = candidates.indexOf(r);
        int rindex = candidates.lastIndexOf(r);

        if ((index != -1) && (index == rindex))
            return candidates[index];
    }
    dot = path.lastIndexOf('.');
    if (dot != -1)
    {
        QRegExp r(QRegExp::escape(path.left(dot)) + "\\.[^\\.]+$");

        int index = candidates.indexOf(r);
        int rindex = candidates.lastIndexOf(r);

        if ((index != -1) && (index == rindex))
            return candidates[index];
    }

    return path;
}
