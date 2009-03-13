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
#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <QObject>

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class VolumeControl : public QObject
{
    Q_OBJECT
public:
    VolumeControl(QObject *parent = 0);

    ~VolumeControl();

    virtual void setVolume(int left, int right) = 0;

    int left();
    int right();

    static VolumeControl *create(QObject *parent = 0);

signals:
    void volumeChanged(int left, int right);

public slots:
    void checkVolume();

protected:
    virtual void volume(int *left, int *right) = 0;

private:
    int m_left, m_right;

};
/*! @internal
 *  @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class SoftwareVolume : public VolumeControl
{
    Q_OBJECT
public:
    SoftwareVolume(QObject *parent = 0);

    ~SoftwareVolume();

    void setVolume(int left, int right);

    static SoftwareVolume *instance();
    static void setEnabled(bool b);

protected:
    void volume(int *left, int *right);

private:
    int m_left, m_right;
    static SoftwareVolume *m_instance;

};

#endif
