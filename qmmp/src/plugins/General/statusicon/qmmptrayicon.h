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
#ifndef QMMPTRAYICON_H
#define QMMPTRAYICON_H

#include <QSystemTrayIcon>
#include <QPointer>

class QEvent;
class QWheelEvent;
class StatusIconPopupWidget;

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class QmmpTrayIcon : public QSystemTrayIcon
{
Q_OBJECT
public:
    QmmpTrayIcon(QObject *parent = 0);
    ~QmmpTrayIcon();

    void showNiceToolTip(bool value);
    void setNiceToolTipDelay(int value);
    void setNiceToolTipOpacity(qreal value);
    void setSplitFileName(bool value);

protected:
    bool event(QEvent *e);

private:
    void wheelEvent(QWheelEvent *e);
    void showToolTip();

    bool m_showNiceToolTip;
    bool m_splitFileName;
    qint32 m_niceToolTipDelay;
    qreal m_niceToolTipOpacity;
    QPointer<StatusIconPopupWidget> m_PopupWidget;

};

#endif
