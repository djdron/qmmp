/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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


#ifndef _KEYBOARDMANAGER_H
#define _KEYBOARDMANAGER_H

class PlayList;
class QKeyEvent;


/*!
 * Class \b KeyboardManager represents key handler object that processes
 * all key events passed to the \b PlayList
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class KeyboardManager
{
    public:
      /*!
       * Constructor. Takes \b PlayList object as an argument.
       */
        KeyboardManager (PlayList*);

        /*!
         * Handles key press events from \b PlayList object. Returns \b true 
         * if the key was handled, otherwise \b false.
         */
        bool handleKeyPress (QKeyEvent*);

        /*!
         * Handles key release events from \b PlayList object. Returns \b true 
         * if the key was handled, otherwise \b false.
         */
        bool handleKeyRelease (QKeyEvent*);

    protected:
        void keyUp (QKeyEvent* ke);
        void keyDown (QKeyEvent* ke);
        void keyPgUp (QKeyEvent* ke);
        void keyPgDown (QKeyEvent* ke);
        void keyEnter (QKeyEvent* ke);
        void keyHome(QKeyEvent* ke);
        void keyEnd(QKeyEvent* ke);
    private:
        PlayList* m_playlist;
};

#endif
