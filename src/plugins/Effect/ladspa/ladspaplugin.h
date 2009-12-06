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
#ifndef LADSPAPLUGIN_H
#define LADSPAPLUGIN_H

#include <QMutex>
#include <QList>
#include <qmmp/effect.h>
#include <ladspa.h>

class QWidget;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

#define MAX_SAMPLES 8192
#define MAX_KNOBS 64


class LADSPAPlugin
{
public:
    QString name;
    QString fileName;
    long id;
    long unique_id;
    bool stereo;
};

class LADSPAEffect
{
public:
    void *library;
    QString fileName;
    bool stereo;
    const LADSPA_Descriptor *descriptor;
    LADSPA_Handle handle;	/* left or mono */
    LADSPA_Handle handle2;	/* right stereo */
    LADSPA_Data knobs[MAX_KNOBS];
    QWidget *widget;
};


class LADSPAHost : public Effect
{
    Q_OBJECT
public:
    LADSPAHost(QObject *parent = 0);

    virtual ~LADSPAHost();

    ulong process(char *in_data, const ulong size, char **out_data);
    void configure(quint32 freq, int chan, int res);
    QList <LADSPAPlugin *> plugins();
    QList <LADSPAEffect *> runningPlugins();
    LADSPAEffect *addPlugin(LADSPAPlugin * plugin);
    void unload(LADSPAEffect *instance);

    static LADSPAHost* instance();

private:
    int applyEffect(qint16 *d, int length);

    void bootPlugin(LADSPAEffect *instance);
    void findAllPlugins();
    void findPlugins(const QString &path);
    LADSPAEffect *load(const QString &path, long num);
    void portAssign(LADSPAEffect *instance);
    void draw_plugin(LADSPAEffect *instance);

    QList <LADSPAPlugin *> m_plugins;
    QList <LADSPAEffect *> m_effects;

    LADSPA_Data m_left[MAX_SAMPLES], m_right[MAX_SAMPLES], m_trash[MAX_SAMPLES];

    static LADSPAHost *m_instance;
};

#endif
