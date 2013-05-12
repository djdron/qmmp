/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef PLUGINCACHE_H
#define PLUGINCACHE_H

#include <QString>
#include <QObject>
#include <QSettings>

class DecoderFactory;

/*! @internal
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PluginCache
{
public:
    PluginCache(const QString &file, QSettings *settings);

    const QString shortName() const;
    const QString file() const;
    bool hasError() const;


    DecoderFactory *decoderFactory();

    //OutputFactory *outputFactory();
    //EngineFactory *engineFactory();

private:
    QObject *instance();
    QString m_path;
    QString m_shortName;
    bool m_error;
    QObject *m_instance;
    DecoderFactory *m_decoderFactory;
};

#endif // PLUGINCACHE_H
