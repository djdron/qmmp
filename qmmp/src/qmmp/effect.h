/***************************************************************************
 *   Copyright (C) 2007-2009 by Ilya Kotov                                 *
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

class EffectFactory;

/*! @brief The Effect class provides the base interface class of audio effects.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class Effect : public QObject
{
    Q_OBJECT
public:
    /*!
     * Object contsructor.
     * @param parent Parent object.
     */
    Effect(QObject *parent = 0);
    /*!
     * Destructor.
     */
    virtual ~Effect();
    /*!
     * Adds effect to the input data pointer \b in_data with the size \b size.
     * Result is stored in the output data \b out_data.
     * Return value is the size of the output data.
     * Subclass should implement this function.
     */
    virtual ulong process(char *in_data, const ulong size, char **out_data) = 0;
    /*!
     * Prepares object for usage.
     * Subclasses that reimplement this function must call the base implementation.
     * @param freq Sample rate.
     * @param chan Number of channels.
     * @param res Bits per sample.
     */
    virtual void configure(quint32 freq, int chan, int res);
    /*!
     * Returns samplerate.
     */
    quint32 sampleRate();
    /*!
     * Returns channels number.
     */
    int channels();
    /*!
     * Returns bits per sample.
     */
    int bitsPerSample();
    /*!
     * Creates a list of enabled effects.
     * @param parent Parent object of all created Effect objects.
     */
    static QList<Effect*> create(QObject *parent);
    /*!
     * Returns a list of effect factories.
     */
    static QList<EffectFactory*> *effectFactories();
    /*!
     * Returns a list of effect plugin file names.
     */
    static QStringList effectFiles();
    /*!
     * Sets whether the effect plugin is enabled.
     * @param factory Effect plugin factory.
     * @param enable Plugin enable state (\b true - enable, \b false - disable)
     */
    static void setEnabled(EffectFactory* factory, bool enable = TRUE);
    /*!
     * Returns \b true if input plugin is enabled, otherwise returns \b false
     * @param factory Effect plugin factory.
     */
    static bool isEnabled(EffectFactory* factory);

private:
    quint32 m_freq;
    int m_chan;
    int m_res;
};

#endif
