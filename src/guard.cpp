/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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

#include  "guard.h"

//#define USE_SEMAPHORE

#ifdef USE_SEMAPHORE 
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <errno.h>
	#include <sys/sem.h>
#else
	#include <QSettings>
#endif

#include <QDir>

bool Guard::create(const QString& filepath)
{
#ifdef USE_SEMAPHORE
	key_t key;
	int sem_num = 1;
	if( (key = ftok(filepath.toAscii().data(),'A')) < 0)
	{
		qWarning("Warning: Unable get access to key...");
		return false;
	}

	if(semget(key,sem_num,IPC_CREAT | IPC_EXCL ) < 0)
	{
		if(errno == EEXIST)
		{
			qWarning("Warning: Semaphore with %d  key already exists...",key);
		}
		return false;
	}

	return true;
#else
	Q_UNUSED(filepath)
	QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
	settings.beginGroup("Application");
	settings.setValue("IS_RUNNING",TRUE);
	settings.endGroup();
	return true;
#endif
}

bool Guard::exists(const QString& filepath)
{
#ifdef USE_SEMAPHORE
	key_t key;
	int sem_num = 1;
	int sem_id;

	if( (key = ftok(filepath.toAscii().data(),'A')) < 0)
	{
		qWarning("Warning: Unable get access to key");
		return false;
	}

	if( (sem_id = semget(key,sem_num,0)) < 0 )
		return false;
	return true;
#else
	Q_UNUSED(filepath)
	QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
	settings.beginGroup("Application");
	bool res  = settings.value("IS_RUNNING",FALSE).toBool();
	settings.endGroup();
	return res;
#endif
}

bool Guard::destroy(const QString& filepath)
{
#ifdef USE_SEMAPHORE
	key_t key;
	int sem_num = 1;
	int sem_id;

	if( (key = ftok(filepath.toAscii().data(),'A')) < 0)
	{
		qWarning("Warning: Unable get access to key");
		return false;
	}
	if( (sem_id = semget(key,sem_num,0)) < 0 )
	{
		qWarning("Unable get semaphore with key %d",key);
		return false;
	}

	if(semctl(sem_id,1,IPC_RMID) < 0)
	{
		qWarning("Unable remove semaphore with key %d",key);
		return false;
	}
	return true;
#else
	Q_UNUSED(filepath)
	QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
	settings.beginGroup("Application");
	settings.setValue("IS_RUNNING",FALSE);
	settings.endGroup();
	return true;
#endif
}
