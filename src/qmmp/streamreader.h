/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
#ifndef STREAMREADER_H
#define STREAMREADER_H

#include <QObject>
#include <QIODevice>
#include <QUrl>

class QFileInfo;

class Downloader;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class StreamReader : public QIODevice
{
    Q_OBJECT
public:
    StreamReader(const QString &name, QObject *parent = 0);

    ~StreamReader();

    /** 
     *  QIODevice API
     */
    bool atEnd () const;
    qint64 bytesAvailable () const;
    qint64 bytesToWrite () const;
    bool canReadLine () const;
    void close ();
    bool isSequential () const;
    bool open ( OpenMode mode );
    //qint64 pos () const;
    bool reset ();
    bool seek ( qint64 pos );
    qint64 size () const;
    bool waitForBytesWritten ( int msecs );
    bool waitForReadyRead ( int msecs );

    /**
     *  returns content type of a stream
     */
    const QString &contentType();
    void downloadFile();

signals:
    void readyRead();
    void bufferingProgress(int);

protected:
    qint64 readData(char*, qint64);
    qint64 writeData(const char*, qint64);

private:
    //void downloadFile();
    void fillBuffer();
    QUrl m_url;
    QString m_contentType;
    Downloader *m_downloader;
};

#endif
