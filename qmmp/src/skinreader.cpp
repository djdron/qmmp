/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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

#include <QDir>
#include <QList>
#include <QFileInfo>
#include <QProcess>
#include <QByteArray>
#include <QFile>

#include "skinreader.h"

SkinReader::SkinReader(QObject *parent)
        : QObject(parent)
{
    m_process = new QProcess(this);
    //create cache dir
    QDir dir(QDir::homePath() +"/.qmmp/");
    dir.mkdir("cache");
    dir.cd("cache");
    dir.mkdir("skins");
}


SkinReader::~SkinReader()
{}

void SkinReader::updateCache()
{
    QDir dir(QDir::homePath() +"/.qmmp/skins");
    dir.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList f = dir.entryInfoList();
    //clear removed skins from cache
    QDir cache_dir(QDir::homePath() +"/.qmmp/cache/skins");
    cache_dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);
    QFileInfoList d = cache_dir.entryInfoList();

    foreach(QFileInfo dirInfo, d)
    {
        bool del = TRUE;
        foreach(QFileInfo fileInfo, f)
        {
            if (fileInfo.baseName () == dirInfo.fileName ())
            {
                del = FALSE;
                break;
            }
        }
        if (del)
        {
            qDebug("SkinReader: deleting %s from cache",
                   qPrintable(dirInfo.fileName ()));

            cache_dir.cd(dirInfo.fileName ());
            cache_dir.setFilter(QDir::Files | QDir::Hidden);
            QFileInfoList removeList = cache_dir.entryInfoList();
            foreach(QFileInfo to_remove, removeList)
            cache_dir.remove(to_remove.fileName ());
            cache_dir.cdUp();
            cache_dir.rmdir(dirInfo.fileName ());
        }
    }
    //add new skins to cache
    foreach(QFileInfo fileInfo, f)
    {
        bool create = TRUE;
        foreach(QFileInfo dirInfo, d)
        {
            if (fileInfo.baseName () == dirInfo.fileName ())
            {
                create = FALSE;
                break;
            }
        }
        if (create)
        {
            qDebug("SkinReader: adding %s to cache",
                   qPrintable(fileInfo.fileName ()));
            cache_dir.mkdir(fileInfo.baseName ());
            QString name = fileInfo.fileName ().toLower();

            if (name.endsWith(".tgz") || name.endsWith(".tar.gz"))
                untar(fileInfo.filePath (), cache_dir.absolutePath ()+"/"+
                      fileInfo.baseName ());
            if (name.endsWith(".zip") || name.endsWith(".wsz"))
                unzip(fileInfo.filePath (), cache_dir.absolutePath ()+"/"+
                      fileInfo.baseName ());
        }
    }
}

void SkinReader::untar(const QString &from, const QString &to)
{
    QByteArray array;
    QStringList args;
    //list archive
    args << "tf" <<from;
    m_process->start("tar", args);
    m_process->waitForFinished();
    array = m_process->readAllStandardOutput ();
    QString str = QString(array);
    QStringList outputList = str.split("\n",QString::SkipEmptyParts);
    foreach(QString str, outputList)
    {
        str = str.trimmed();
        args.clear();
        args << "xvfk" << from << "-O" << str;
        m_process->start("tar", args);
        m_process->waitForFinished();
        array = m_process->readAllStandardOutput ();
        QString name = str.right(str.size() - str.indexOf("/",Qt::CaseInsensitive) - 1).trimmed().toLower();

        QFile file(to+"/"+name);
        file.open(QIODevice::WriteOnly);
        file.write(array);
        file.close();
    }
}

void SkinReader::unzip(const QString &from, const QString &to)
{
    QStringList args;
    args << "-j" << "-o" << "-d" << to << from;
    m_process->start("unzip", args);
    m_process->waitForFinished();
}

