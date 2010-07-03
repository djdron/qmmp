/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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

#ifndef EQSETTINGS_H
#define EQSETTINGS_H

class EqSettings
{
public:
    EqSettings();

    bool isEnabled() const;
    double gain(int chan) const;
    double preamp() const;
    void setEnabled(bool enabled = true);
    void setGain(int chan, double gain);
    void setPreamp(double preamp);

    void operator=(const EqSettings &s);
    bool operator==(const EqSettings &s) const;
    bool operator!=(const EqSettings &s) const;


private:
    double m_gain[10];
    double m_preamp;
    bool m_is_enabled;

};

#endif // EQSETTINGS_H
