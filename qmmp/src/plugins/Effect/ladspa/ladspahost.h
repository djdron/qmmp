/***************************************************************************
 *   Copyright (C) 2002,2003 Nick Lamb <njl195@zepler.org.uk>              *
 *   Copyright (C) 2005 Giacomo Lozito <city_hunter@users.sf.net>          *
 *   Copyright (C) 2009 by Ilya Kotov <forkotov02@hotmail.ru>              *
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
#ifndef LADSPAHOST_H
#define LADSPAHOST_H

#include <QMutex>
#include <QList>
#include <QObject>
#include <qmmp/qmmp.h>
#include "ladspa.h"

class QWidget;

#define MAX_SAMPLES 8192
#define MAX_KNOBS 64

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class LADSPAPlugin
{
public:
    QString name;
    QString fileName;
    long id;
    long unique_id;
    bool stereo;
};

class LADSPAControl
{
public:
    enum Type
    {
        BUTTON = 0,
        SLIDER,
        LABEL
    };
    double min;
    double max;
    double step;
    LADSPA_Data *value;
    int type;
    QString name;
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
    QList <LADSPAControl*> controls;
};


class LADSPAHost : public QObject
{
Q_OBJECT
public:
    LADSPAHost(QObject *parent);

    virtual ~LADSPAHost();

    int applyEffect(qint16 *d, int length);
    void configure(quint32 freq, int chan, Qmmp::AudioFormat format);
    QList <LADSPAPlugin *> plugins();
    QList <LADSPAEffect *> effects();
    LADSPAEffect *addPlugin(LADSPAPlugin * plugin);
    void unload(LADSPAEffect *instance);
    static LADSPAHost* instance();

private:
    void bootPlugin(LADSPAEffect *instance);
    void findAllPlugins();
    void findPlugins(const QString &path);
    LADSPAEffect *load(const QString &path, long num);
    void portAssign(LADSPAEffect *instance);
    void initialize(LADSPAEffect *instance);

    QList <LADSPAPlugin *> m_plugins;
    QList <LADSPAEffect *> m_effects;

    LADSPA_Data m_left[MAX_SAMPLES], m_right[MAX_SAMPLES], m_trash[MAX_SAMPLES];

    static LADSPAHost *m_instance;
    int m_chan, m_prec;
    quint32 m_freq;
};

#endif
