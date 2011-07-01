/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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
#ifndef UI_H
#define UI_H

#include <QStringList>
#include <QMap>
#include "uifactory.h"

/*! @brief The UiLoader provides user interface plugins access
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class UiLoader
{
public:
    /*!
     * Returns a list of the loaded ui plugin factories.
     */
    static QList<UiFactory*> *factories();
    /*!
     * Returns a list of the loaded ui plugin files.
     */
    static QStringList files();
    /*!
     * Selects active user interface factory.
     * @param factory Ui plugin factory.
     */
    static void setCurrentUiFactory(UiFactory* factory);
    /*!
     * Returns \b true if general plugin is enabled, otherwise returns \b false
     * @param factory General plugin factory.
     */
    static UiFactory *currentUiFactory();

private:
    static QList<UiFactory*> *m_factories;
    static QStringList m_files;
    static void checkFactories();
};

#endif //UI_H
