/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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
#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <QObject>

/*! @brief The VolumeControl class provides the base interface class for volume control.
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
    virtual void setVolume(int left, int right) = 0;
    /*!
     * Returns left channel volume.
     */
    int left();
    /*!
     * Returns right channel volume.
     */
    int right();
    /*!
     * Creates output volume control object if implemented, \b otherwise it creates SoftwareVolume object.
     * @param parent Parent object.
     */
    static VolumeControl *create(QObject *parent = 0);

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

protected:
    /*!
     * Gets current volume.
     * @param left Pointer to the left volume level.
     * @param right Pointer to the right volume level
     */
    virtual void volume(int *left, int *right) = 0;

private:
    int m_left, m_right;
    bool m_prev_block;

};
/*! @brief The SoftwareVolume class provides access to the software volume control.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class SoftwareVolume : public VolumeControl
{
    Q_OBJECT
public:
    /*!
     * Object constructor.
     * @param parent Parent object.
     */
    SoftwareVolume(QObject *parent = 0);
    /*!
     * Destructor.
     */
    ~SoftwareVolume();
    /*!
     * Setups volume level.
     * Subclass should reimplement this fucntion.
     * @param left Left channel volume level. It should be \b 0..100
     * @param right Right channel volume level. It should be \b 0..100
     */
    void setVolume(int left, int right);
    /*!
     * Changes volume of buffer.
     * @param data Pointer to the buffer.
     * @param size Buffer size in bytes.
     * @param chan Number of channels.
     * @param bits Sample size in bits.
     */
    void changeVolume(uchar *data, qint64 size, int chan, int bits);
    /*!
     * Returns software volume object instance.
     */
    static SoftwareVolume *instance();
    /*!
     * This funtion allows to force software volume for all output plugins.
     * @param b Software volume enable state (\b true - enable, \b false - disable)
     */
    static void setEnabled(bool b);

protected:
    /*! @internal
     * Gets current volume.
     * @param left Pointer to the left volume level.
     * @param right Pointer to the right volume level
     */
    void volume(int *left, int *right);

private:
    int m_left, m_right;
    double m_scaleLeft, m_scaleRight;
    static SoftwareVolume *m_instance;
};

#endif
