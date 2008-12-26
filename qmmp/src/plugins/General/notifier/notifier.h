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
#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QPointer>

#include <qmmpui/general.h>
#include <qmmp/qmmp.h>

class PopupWidget;
class SoundCore;

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/

class Notifier : public General
{
Q_OBJECT
public:
    Notifier(QObject *parent = 0);

    ~Notifier();

private slots:
    void showMetaData();
    void showVolume(int, int);
    void setState(Qmmp::State state);

private:
    QPointer<PopupWidget> m_popupWidget;
    bool m_desktop, m_showVolume;
    bool m_psi;
    int m_l, m_r;
    SoundCore *m_core;

};

#endif
