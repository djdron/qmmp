/***************************************************************************
 *   Copyright (C) 2008-2012 by Ilya Kotov                                 *
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
#ifndef VOLUMECONTROL_P_H
#define VOLUMECONTROL_P_H

#include <QObject>
#include "qmmp.h"
#include "volume.h"
#include "buffer.h"

class QTimer;
class SoftwareVolume;

/*! @internal
 * @brief The VolumeControl class provides volume control access
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class VolumeControl : public QObject
{
    Q_OBJECT
public:
    /*!
     * Object constructor.
     * @param parent Parent object.
     */
    VolumeControl(QObject *parent = 0);
    /*!
     * Destructor.
     */
    ~VolumeControl();
    /*!
     * Setups volume level.
     * Subclass should reimplement this fucntion.
     * @param left Left channel volume level. It should be \b 0..100
     * @param right Right channel volume level. It should be \b 0..100
     */
    void setVolume(int left, int right);
    /*!
     * Returns left channel volume.
     */
    int left();
    /*!
     * Returns right channel volume.
     */
    int right();

signals:
    /*!
     * Emitted when volume is changed.
     * @param left Left channel volume level. It should be \b 0..100
     * @param right Right channel volume level. It should be \b 0..100
     */
    void volumeChanged(int left, int right);

public slots:
    /*!
     * Forces the volumeChanged signal to emit.
     */
    void checkVolume();
    /*!
     * Updates volume configuration
     */
    void reload();

private:
    int m_left, m_right;
    bool m_prev_block;
    Volume *m_volume;
    QTimer *m_timer;

};
/*! @internal
 * @brief The SoftwareVolume class provides access to the software volume control.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class SoftwareVolume : public Volume
{
public:
    SoftwareVolume();
    ~SoftwareVolume();

    void setVolume(const VolumeSettings &v);
    VolumeSettings volume() const;
    void changeVolume(Buffer *b, int chan, Qmmp::AudioFormat format);

    static SoftwareVolume *instance();

private:
    int m_left, m_right;
    double m_scaleLeft, m_scaleRight;
    static SoftwareVolume *m_instance;
};

#endif
