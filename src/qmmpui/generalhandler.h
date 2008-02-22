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
#ifndef GENERALHANDLER_H
#define GENERALHANDLER_H

#include <QObject>

#include "songinfo.h"

class General;
class GeneralFactory;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class GeneralHandler : public QObject
{
    Q_OBJECT
public:
    GeneralHandler(QObject *parent = 0);

    ~GeneralHandler();

    void setSongInfo(const SongInfo &info);
    void updateVolume(int left, int right);
    void setEnabled(GeneralFactory* factory, bool enable);
    void showSettings(GeneralFactory* factory, QWidget* parentWidget);
    static GeneralHandler* instance();

signals:
    void playCalled();
    void pauseCalled();
    void stopCalled();
    void nextCalled();
    void previousCalled();
    void exitCalled();
    void toggleVisibilityCalled();
    void volumeChanged(int left, int right);

public slots:
    void setState(uint state);

private slots:
    void processCommand(uint command);

private:
    void connectSignals(General*);
    QMap <GeneralFactory*, General*> m_generals;
    SongInfo m_songInfo;
    uint m_state;
    int m_left, m_right;
    static GeneralHandler* m_instance;

};

#endif
