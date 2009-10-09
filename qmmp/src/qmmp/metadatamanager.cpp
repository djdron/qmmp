/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#include "decoder.h"
#include "decoderfactory.h"
#include "abstractengine.h"
#include "inputsource.h"
#include "metadatamanager.h"

MetaDataManager* MetaDataManager::m_instance = 0;

MetaDataManager::MetaDataManager()
{
    if(m_instance)
        qFatal("MetaDataManager is already created");
    m_instance = this;
    m_decoderFactories = Decoder::factories();
    m_engineFactories = AbstractEngine::factories();
    m_inputSourceFactories = InputSource::factories();
}

MetaDataManager::~MetaDataManager()
{
    m_instance = 0;
}

QList <FileInfo *> MetaDataManager::createPlayList(const QString &fileName, bool useMetaData)
{
    QList <FileInfo *> list;
    DecoderFactory *fact = 0;
    EngineFactory *efact = 0;

    if (!fileName.contains("://")) //local file
    {
        if((fact = Decoder::findByPath(fileName)))
            return fact->createPlayList(fileName, useMetaData);
        else if((efact = AbstractEngine::findByPath(fileName)))
            return efact->createPlayList(fileName, useMetaData);
        return list;
    }
    else
    {
        QString p = fileName.section("://",0,0);
        QStringList protocols;
        foreach(InputSourceFactory *f, *m_inputSourceFactories)
        {
            protocols << f->properties().protocols.split(" ", QString::SkipEmptyParts);
        }
        if(protocols.contains(p))
            list << new FileInfo(fileName);
    }

    return list;
}

QStringList MetaDataManager::filters()
{
    QStringList filters;
    foreach(DecoderFactory *fact, *m_decoderFactories)
    {
        if (Decoder::isEnabled(fact) && !fact->properties().filter.isEmpty())
            filters << fact->properties().description + " (" + fact->properties().filter + ")";
    }
    foreach(EngineFactory *fact, *m_engineFactories)
    {
        if (AbstractEngine::isEnabled(fact) && !fact->properties().filter.isEmpty())
            filters << fact->properties().description + " (" + fact->properties().filter + ")";
    }
    return filters;
}

QStringList MetaDataManager::nameFilters()
{
    QStringList filters;
    foreach(DecoderFactory *fact, *m_decoderFactories)
    {
        if (Decoder::isEnabled(fact))
            filters << fact->properties().filter.split(" ", QString::SkipEmptyParts);
    }
    foreach(EngineFactory *fact, *m_engineFactories)
    {
        if (AbstractEngine::isEnabled(fact))
            filters << fact->properties().filter.split(" ", QString::SkipEmptyParts);
    }
    return filters;
}

MetaDataManager *MetaDataManager::instance()
{
    if(!m_instance)
        new MetaDataManager();
    return m_instance;
}

void MetaDataManager::destroy()
{
    if(m_instance)
        delete m_instance;
}
