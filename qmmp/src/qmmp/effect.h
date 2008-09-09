/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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
#ifndef EFFECT_H
#define EFFECT_H

#include <QObject>
#include <QList>
#include <QStringList>


/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class EffectFactory;

class Effect : public QObject
{
    Q_OBJECT
public:
    Effect(QObject *parent = 0);

    virtual ~Effect();

    /*/*!
     * Adds effect to the input data pointer \b in_data with the size \b size.
     * Result is stored in the output data \b out_data.
     * Return value is the size of the output data. Output data size should not be more then \b size.
     * Subclass should implement this function.
     */
    virtual const ulong process(char *in_data, const ulong size, char **out_data) = 0;

    //virtual const ulong process(char *in_data, const ulong size, char *out_data) = 0;
    //virtual bool process(char *in_data, char *out_data, const ulong maxsize, ulong &rbytes, ulong &wbytes) = 0;


    virtual void configure(qint64 freq, int chan, int res);

    /*!
     * Returns samplerate.
     * This function should be reimplemented if subclass changes default samplerate.
     */
    virtual const qint64 sampleRate();

    /*!
     * Returns channel number.
     * This function should be reimplemented if subclass changes default channel number.
     */
    virtual const int channels();

    /*!
     * Returns bit depth.
     * This function should be reimplemented if subclass changes default resolution.
     */
    virtual const int bitsPerSample();


    static QList<Effect*> create(QObject *parent);
    static QList<EffectFactory*> *effectFactories();
    static QStringList effectFiles();
    static void setEnabled(EffectFactory* factory, bool enable = TRUE);
    static bool isEnabled(EffectFactory* factory);

private:
    ulong m_freq;
    int m_chan;
    int m_res;
};

#endif
