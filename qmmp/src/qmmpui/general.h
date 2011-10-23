/***************************************************************************
 *   Copyright (C) 2008-2011 by Ilya Kotov                                 *
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
#ifndef GENERAL_H
#define GENERAL_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include "generalfactory.h"

/*! @brief The General class provides simple access to general plugins
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class General
{
public:
    /*!
     * Prepares general plugins for usage.
     * @param parent Parent object.
     */
    static void create(QObject *parent);
    /*!
     * Returns a list of the loaded general plugin factories.
     */
    static QList<GeneralFactory*> *factories();
    /*!
     * Returns plugin file path.
     * @param factory General plugin factory.
     */
    static QString file(GeneralFactory *factory);
    /*!
     * Sets whether the general plugin is enabled.
     * @param factory General plugin factory.
     * @param enable Plugin enable state (\b true - enable, \b false - disable)
     */
    static void setEnabled(GeneralFactory* factory, bool enable = true);
    /*!
     * Shows configuration dialog and updates settings automatically.
     * @param factory General plugin factory.
     * @param parentWidget Parent widget.
     */
    static void showSettings(GeneralFactory* factory, QWidget* parentWidget);
    /*!
     * Returns \b true if general plugin is enabled, otherwise returns \b false
     * @param factory General plugin factory.
     */
    static bool isEnabled(GeneralFactory* factory);

private:
    static void checkFactories();
    static QList<GeneralFactory*> *m_factories;
    static QHash <GeneralFactory*, QObject*> *m_generals;
    static QHash <GeneralFactory*, QString> *m_files;
    static QObject *m_parent;
};

#endif
