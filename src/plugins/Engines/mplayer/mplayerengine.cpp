/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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

#include <QObject>
#include <QProcess>
#include <QFile>
#include <QApplication>
#include <QAction>
#include <QMetaObject>
#include <QKeyEvent>
#include <QMenu>
#include <QRegExp>
#include <QSettings>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>
#include <qmmp/fileinfo.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/inputsource.h>
#include "mplayerengine.h"

#define MPLAYER_DEBUG

static QRegExp rx_av("^[AV]: *([0-9,:.-]+)");
static QRegExp rx_pause("^(.*)=(.*)PAUSE(.*)");
static QRegExp rx_end("^(.*)End of file(.*)");
static QRegExp rx_quit("^(.*)Quit(.*)");
static QRegExp rx_audio("^AUDIO: *([0-9,.]+) *Hz.*([0-9,.]+) *ch.*([0-9]+).* ([0-9,.]+) *kbit.*");


FileInfo *MplayerInfo::createFileInfo(const QString &path)
{
    QRegExp rx_id_length("^ID_LENGTH=([0-9,.]+)*");
    QStringList args;
    args << "-slave";
    args << "-identify";
    args << "-frames";
    args << "0";
    args << "-vo";
    args << "null";
    args << "-ao";
    args << "null";
    args << path;
    QProcess mplayer_process;
    mplayer_process.start("mplayer", args);
    mplayer_process.waitForFinished();
    QString str = QString::fromLocal8Bit(mplayer_process.readAll()).trimmed();
    FileInfo *info = new FileInfo(path);
    QStringList lines = str.split("\n");
    foreach(QString line, lines)
    {
        if (rx_id_length.indexIn(line) > -1)
            info->setLength((qint64) rx_id_length.cap(1).toDouble());
    }
#ifdef MPLAYER_DEBUG
    qDebug("%s",qPrintable(str));
#endif
    return info;
}

QStringList MplayerInfo::filters()
{
    QStringList filters;
    filters << "*.avi" << "*.mpg" << "*.mpeg" << "*.divx" << "*.qt" << "*.mov" << "*.wmv" << "*.asf"
    << "*.flv" << "*.3gp" << "*.mkv";
    return filters;
}

MplayerEngine::MplayerEngine(QObject *parent)
        : AbstractEngine(parent)
{
    //m_url = url;
    m_bitrate = 0;
    m_samplerate = 0;
    m_channels = 0;
    m_bitsPerSample = 0;
    m_length = 0;
    m_currentTime = 0;
    m_process = new QProcess(this);
    connect(m_process, SIGNAL(readyReadStandardOutput()), SLOT(readStdOut()));
}

MplayerEngine::~MplayerEngine()
{
    qDebug("%s",__FUNCTION__);
    m_process->close();
}

bool MplayerEngine::play()
{
    if(m_process->state() != QProcess::NotRunning)
        return FALSE;
    startMplayerProcess();
    return TRUE;
}

bool MplayerEngine::enqueue(InputSource *source)
{
    QString url = source->url();
    QStringList filters = MplayerInfo::filters();
    bool supports = FALSE;
    foreach(QString filter, filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        supports = regexp.exactMatch(source->url());
        if(supports)
            break;
    }
    if(!supports)
        return FALSE;
    source->deleteLater();
    if(m_process->state() == QProcess::NotRunning)
        m_url = url;
    else
        m_files.enqueue(url);
    return TRUE;
}

bool MplayerEngine::initialize()
{
    FileInfo *info = MplayerInfo::createFileInfo(m_url);
    m_length = info->length();
    delete info;
    m_args.clear();
    m_args << "-slave";
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QString ao_str = settings.value("mplayer/ao","default").toString();
    QString vo_str = settings.value("mplayer/vo","default").toString();
    if (ao_str != "default")
        m_args << "ao=" + ao_str;
    if (vo_str != "default")
        m_args << "vo=" + vo_str;

    if (settings.value("autosync", FALSE).toBool())
        m_args << QString("-autosync %1").arg(settings.value("autosync_factor", 100).toInt());

    m_args << m_url;
    return TRUE;
}

qint64 MplayerEngine::totalTime()
{
    return m_length * 1000;
}

void MplayerEngine::seek(qint64 pos)
{
    if (m_process->state() == QProcess::Running)
        m_process->write(QString("seek %1 \n").arg(pos/1000 - m_currentTime).toLocal8Bit ());
}

void MplayerEngine::stop()
{
    if (m_process->state() == QProcess::Running)
    {
        m_process->write("quit\n");
        m_process->waitForFinished(1500);
    }
    StateHandler::instance()->dispatch(Qmmp::Stopped);
    m_files.clear();
    m_url.clear();
}

void MplayerEngine::pause()
{
    m_process->write("pause\n");
}

void MplayerEngine::setEQ(double bands[10], double preamp)
{
    Q_UNUSED(bands[10]);
    Q_UNUSED(preamp);
}

void MplayerEngine::setEQEnabled(bool on)
{
    Q_UNUSED(on);
}

void MplayerEngine::readStdOut()
{
    QString line = QString::fromLocal8Bit(m_process->readAll ()).trimmed();
    QStringList lines = line.split("\n");
    foreach(line, lines)
    {
        if (rx_av.indexIn(line) > -1)
        {
            StateHandler::instance()->dispatch(Qmmp::Playing);
            m_currentTime = (qint64) rx_av.cap(1).toDouble();
            StateHandler::instance()->dispatch(m_currentTime * 1000,
                                               m_bitrate,
                                               m_samplerate,
                                               m_bitsPerSample,
                                               m_channels);
        }
        else if (rx_pause.indexIn(line) > -1)
        {
            StateHandler::instance()->dispatch(Qmmp::Paused);
        }
        else if (rx_end.indexIn(line) > -1)
        {
            if (m_process->state() == QProcess::Running)
                m_process->waitForFinished(3500);
            emit playbackFinished();
            if(!m_files.isEmpty())
            {
                StateHandler::instance()->dispatch(Qmmp::Stopped);
                m_url = m_files.dequeue();
                startMplayerProcess();
            }
            else
            {
                StateHandler::instance()->dispatch(Qmmp::Stopped);
                return;
            }
        }
        else if (rx_quit.indexIn(line) > -1)
        {
            if (m_process->state() == QProcess::Running)
                m_process->waitForFinished(1500);
            StateHandler::instance()->dispatch(Qmmp::Stopped);
        }
        else if (rx_audio.indexIn(line) > -1)
        {
            m_samplerate = rx_audio.cap(1).toInt();
            m_channels = rx_audio.cap(2).toInt();
            m_bitsPerSample = rx_audio.cap(3).toDouble();
            m_bitrate = rx_audio.cap(4).toDouble();
        }
#ifdef MPLAYER_DEBUG
        else
            qDebug("%s",qPrintable(line));
#endif
    }
}

void MplayerEngine::startMplayerProcess()
{
    initialize();
    m_process->start ("mplayer", m_args);
    StateHandler::instance()->dispatch(Qmmp::Playing);
    FileInfo *info = MplayerInfo::createFileInfo(m_url);
    StateHandler::instance()->dispatch(info->metaData());
    delete info;
}
