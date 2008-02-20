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
#include <qmmp/decoder.h>

#include "fileloader.h"
#include "mediafile.h"

FileLoader::FileLoader(QObject *parent)
	: QThread(parent),m_files_to_load(),m_directory()
{
   m_filters = Decoder::nameFilters();
	m_finished = false;
}


FileLoader::~FileLoader()
{
	qWarning("FileLoader::~FileLoader()");
}


void FileLoader::addFiles(const QStringList &files)
{
	if (files.isEmpty ())
		return;
	
	foreach(QString s, files)
	{
		if (Decoder::supports(s))
			emit newMediaFile(new MediaFile(s));
		if(m_finished) return;
	}
}


void FileLoader::addDirectory(const QString& s)
{	
	QDir dir(s);
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Name);
	QFileInfoList l = dir.entryInfoList(m_filters);
	for (int i = 0; i < l.size(); ++i)
	{
		QFileInfo fileInfo = l.at(i);
		QString suff = fileInfo.completeSuffix(); 
		list << fileInfo;
		  
		if (Decoder::supports(fileInfo.absoluteFilePath ()))
			emit newMediaFile(new MediaFile(fileInfo.absoluteFilePath ()));
		if(m_finished) return;
	}
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	dir.setSorting(QDir::Name);
	l.clear();
	l = dir.entryInfoList();
	if (l.size() > 0)
		for (int i = 0; i < l.size(); ++i)
	{
		QFileInfo fileInfo = l.at(i);
		addDirectory(fileInfo.absoluteFilePath ());
		if(m_finished) return;
	}
}


void FileLoader::run()
{
	if(!m_files_to_load.isEmpty())
		addFiles(m_files_to_load);
	else if(!m_directory.isEmpty())
		addDirectory(m_directory);
}



void FileLoader::setFilesToLoad(const QStringList & l)
{
	m_files_to_load = l;
	m_directory = QString();
}

void FileLoader::setDirectoryToLoad(const QString & d)
{
	m_directory = d;
	m_files_to_load.clear();
}

void FileLoader::finish()
{
	m_finished = true;
}
