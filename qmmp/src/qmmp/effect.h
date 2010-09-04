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

#include <QList>
#include <QStringList>
#include "audioparameters.h"
#include "buffer.h"

class EffectFactory;

/*! @brief The Effect class provides the base interface class of audio effects.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class Effect
{
public:
    /*!
     * Object contsructor.
     */
    Effect();
    /*!
     * Destructor.
     */
    virtual ~Effect();
    /*!
     * Adds audio effect to the buffer \b b.
     * Subclass should implement this function.
     */
    virtual void applyEffect(Buffer *b) = 0;
    /*!
     * Prepares object for usage.
     * Subclasses that reimplement this function must call the base implementation.
     * @param srate Sample rate.
     * @param chan Number of channels.
     * @param f Audio format.
     */
    virtual void configure(quint32 srate = 44100, int chan = 2, Qmmp::AudioFormat f = Qmmp::PCM_S16LE);
    /*!
     * Returns samplerate.
     */
    quint32 sampleRate();
    /*!
     * Returns channels number.
     */
    int channels();
    /*!
     * Returns audio format.
     */
    Qmmp::AudioFormat format();
    /*!
     * Returns audio parameters for output data.
     */
    const AudioParameters audioParameters() const;
    /*!
     * Returns assigned factory object.
     */
    EffectFactory* factory() const;
    /*!
     * Creates effect object from \b factory. Returns effect objects if factory is enabled,
     * otherwise returns \b 0.
     */
    static Effect* create(EffectFactory *factory);
    /*!
     * Returns a list of effect factories.
     */
    static QList<EffectFactory*> *factories();
    /*!
     * Returns a list of effect plugin file names.
     */
    static QStringList files();
    /*!
     * Sets whether the effect plugin is enabled.
     * @param factory Effect plugin factory.
     * @param enable Plugin enable state (\b true - enable, \b false - disable)
     */
    static void setEnabled(EffectFactory* factory, bool enable = true);
    /*!
     * Returns \b true if input plugin is enabled, otherwise returns \b false
     * @param factory Effect plugin factory.
     */
    static bool isEnabled(EffectFactory* factory);

private:
    EffectFactory *m_factory;
    quint32 m_freq;
    int m_chan;
    Qmmp::AudioFormat m_format;
    static void checkFactories();
    static QList<EffectFactory*> *m_factories;
    static QStringList m_files;
};

#endif
