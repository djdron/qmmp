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
#ifndef DBUSADAPTOR_H
#define DBUSADAPTOR_H

#include <QtDBus>

class Control;

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class DBUSAdaptor : public QDBusAbstractAdaptor
{
Q_OBJECT
Q_CLASSINFO("D-Bus Interface", "org.qmmp.dbus")
Q_PROPERTY(int volume READ volume WRITE setVolume)
Q_PROPERTY(int balance READ balance WRITE setBalance)
Q_PROPERTY(int length READ length)
Q_PROPERTY(int year READ year)
Q_PROPERTY(QString title READ title)
Q_PROPERTY(QString artist READ artist)
Q_PROPERTY(QString album READ album)
Q_PROPERTY(QString comment READ comment)
Q_PROPERTY(QString genre READ genre)
Q_PROPERTY(bool isPlaying READ isPlaying)
Q_PROPERTY(bool isPaused READ isPaused)
Q_PROPERTY(bool isStopped READ isStopped)
Q_PROPERTY(int elapsedTime READ elapsedTime)


public:
    DBUSAdaptor(Control *ctrl, QObject *parent = 0);

    ~DBUSAdaptor();

    int volume();
    void setVolume(int);
    int balance();
    void setBalance(int);
    int length();
    int year();
    QString title();
    QString artist();
    QString album();
    QString comment();
    QString genre();
    bool isPlaying();
    bool isPaused();
    bool isStopped();
    int elapsedTime();

signals:
    void started();
    void paused();
    void stopped();
    void volumeChanged(int vol, int bal);
    void timeChanged(int newTime);

public slots:
    void play();
    void stop();
    void next();
    void previous();
    void pause();
    void toggleVisibility();
    void exit();
    void seek(int time);

private slots:
    void processState();
    void processVolume();
    void processTime();

private:
    Control *m_control;
};

#endif
