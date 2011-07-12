/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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
#ifndef PLUGINITEM_P_H
#define PLUGINITEM_P_H

#include <QTreeWidgetItem>

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class QWidget;
class InputSourceFactory;
class DecoderFactory;
class EngineFactory;
class OutputFactory;
class VisualFactory;
class EffectFactory;
class GeneralFactory;

class PluginItem : public QTreeWidgetItem
{
public:

    PluginItem(QTreeWidgetItem *parent, InputSourceFactory *factory, const QString &path);
    PluginItem(QTreeWidgetItem *parent, DecoderFactory *factory, const QString &path);
    PluginItem(QTreeWidgetItem *parent, EngineFactory *factory, const QString &path);
    PluginItem(QTreeWidgetItem *parent, EffectFactory *factory, const QString &path);
    PluginItem(QTreeWidgetItem *parent, VisualFactory *factory, const QString &path);
    PluginItem(QTreeWidgetItem *parent, GeneralFactory *factory, const QString &path);
    ~PluginItem();

    enum PluginType
    {
        TRANSPORT = QTreeWidgetItem::UserType,
        DECODER,
        ENGINE,
        EFFECT,
        VISUAL,
        GENERAL
    };

    bool hasAbout() const;
    bool hasSettings() const;
    void showAbout(QWidget *parent);
    void showSettings(QWidget *parent);
    void setEnabled(bool enabled);


private:
    bool m_has_about;
    bool m_has_config;
    void *m_factory;
};

#endif
