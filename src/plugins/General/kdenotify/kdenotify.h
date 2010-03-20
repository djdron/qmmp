/***************************************************************************
 *   Copyright (C) 2009 by Artur Guzik                                     *
 *   a.guzik88@gmail.com                                                   *
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

#ifndef KDENOTIFY_H
#define KDENOTIFY_H

#include <QList>
#include <QVariant>

#include "qmmpui/general.h"
#include "qmmp/qmmp.h"

#define DEFAULT_TEMPLATE "<b>%if(%t,%t,%f)</b>\n%if(%p,<br>%p,)\n%if(%a,<br>%a,)\n%if(%l,<br><b>%l</b>,)"

class QDBusInterface;

class KdeNotify : public General
{
    Q_OBJECT;
public:
    KdeNotify(QObject *parent = 0);
    ~KdeNotify();

private:
    QList<QVariant> prepareNotification();
    QDBusInterface *notifier;
    QString m_coverPath;
    int m_NotifyDelay;
    bool m_ShowCovers;
    bool m_UseFreedesktopSpec;
    QString m_template;
    QString m_imagesDir;
    
private slots:
    void showMetaData();
    QString totalTimeString();
};

#endif // KDENOTIFY_H
