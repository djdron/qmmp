/***************************************************************************
 *   Copyright (C) 2007-2008 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   Based on Promoe, an XMMS2 Client                                      *
 *   Copyright (C) 2005-2006 by XMMS2 Team                                 *
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

#include "skin.h"
#include "togglebutton.h"


ToggleButton::ToggleButton ( QWidget *parent,uint on_n,uint on_p,uint off_n,uint off_p )
      : PixmapWidget ( parent )
{
   m_on_n = on_n;
   m_on_p = on_p;
   m_off_n = off_n;
   m_off_p = off_p;
   m_on = FALSE;
   skin = Skin::getPointer();
   setON ( FALSE );
   connect ( skin, SIGNAL ( skinChanged() ), this, SLOT ( updateSkin() ) );
}


ToggleButton::~ToggleButton()
{}

bool ToggleButton::isChecked()
{
   return m_on;
}

void ToggleButton::updateSkin()
{
   //setPixmap ( skin->getButton ( name_normal ) );
   setON ( m_on );
}

void ToggleButton::click()
{
    m_on = !m_on;
    setON (m_on);
    emit clicked(m_on);
}

void ToggleButton::setON ( bool on )
{
   m_on = on;
   if ( on )
      setPixmap ( skin->getButton ( m_on_n ) );
   else
      setPixmap ( skin->getButton ( m_off_n ) );
}
void ToggleButton::mousePressEvent ( QMouseEvent* )
{
   if ( m_on )
      setPixmap ( skin->getButton ( m_off_p ) );
   else
      setPixmap ( skin->getButton ( m_on_p ) );
}

void ToggleButton::mouseReleaseEvent ( QMouseEvent* )
{
   m_on = !m_on;
   setON ( m_on );
   emit clicked( m_on );
}
