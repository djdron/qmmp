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

#ifndef MPLAYERENGINE_H
#define MPLAYERENGINE_H

#include <QQueue>
#include <QString>
#include <qmmp/statehandler.h>
#include <qmmp/abstractengine.h>

class Output;
class QIDevice;
class DecoderPhonon;
class QMenu;
class QProcess;
class FileInfo;
class InputSource;

class MplayerInfo
{
public:
    static FileInfo *createFileInfo(const QString &path);
    static QStringList filters();
};

class MplayerEngine : public AbstractEngine
{
    Q_OBJECT
public:
    MplayerEngine(QObject *parent);
    virtual ~MplayerEngine();

    // Engine API
    bool play();
    bool enqueue(InputSource *source);
    bool initialize();
    qint64 totalTime();
    void seek(qint64);
    void stop();
    void pause();

    // Equalizer
    void setEQ(double bands[10], double preamp);
    void setEQEnabled(bool on);

private slots:
    void readStdOut();
    void startMplayerProcess();

private:
    int mplayer_pipe[2];
    QString m_url;
    QStringList m_args;
    QProcess *m_process;
    int m_bitrate;
    int m_samplerate;
    int m_channels;
    int m_bitsPerSample;
    qint64 m_currentTime;
    qint64 m_length;
    QQueue <QString> m_files;
};


#endif // MPLAYERENGINE_H
