/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QString>
#include <QHash>

#define ACTION(type, receiver, member) ActionManager::instance()->use(type, receiver, member)

class QAction;
class QSettings;



class ActionManager : public QObject
{
    Q_OBJECT
public:
    explicit ActionManager(QObject *parent = 0);
    ~ActionManager();

    enum Type
    {
        PLAY = 0,
        PAUSE,
        STOP,
        PREVIOUS,
        NEXT,
        PLAY_PAUSE,
        JUMP,

        REPEAT_ALL,
        REPEAT_TRACK,
        SHUFFLE,
        NO_PL_ADVANCE,
        STOP_AFTER_SELECTED,
        CLEAR_QUEUE,


        SETTINGS,
        ABOUT,
        ABOUT_QT
    };

    QAction *action(int type);
    QAction *use(int type, const QObject *receiver, const char *member);
    QList<int> actions();
    static ActionManager* instance();

private:
    QAction *createAction(QString name, QString confKey, QString key, QString iconName = QString());
    QAction *createAction2(QString name, QString confKey, QString key);
    QSettings *m_settings;
    QHash <int, QAction *> m_actions;
    static ActionManager *m_instance;

};

#endif // ACTIONMANAGER_H
