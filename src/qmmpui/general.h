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
#ifndef GENERAL_H
#define GENERAL_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include "generalfactory.h"

/*! @brief The General class provides the basic functionality for the general plugin objects
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class General : public QObject
{
    Q_OBJECT
public:
    /*!
     * Object constructor,
     * @param parent Parent object
     */
    General(QObject *parent = 0);
    /*!
     * Destructor
     */
    ~General();
    /*!
     * Returns a list of the loaded general plugin factories.
     */
    static QList<GeneralFactory*> *generalFactories();
    /*!
     * Returns a list of the loaded general plugin files.
     */
    static QStringList generalFiles();
    /*!
     * Sets whether the general plugin is enabled.
     * @param factory General plugin factory.
     * @param enable Plugin enable state (\b true - enable, \b false - disable)
     */
    static void setEnabled(GeneralFactory* factory, bool enable = TRUE);
    /*!
     * Returns \b true if general plugin is enabled, otherwise returns \b false
     * @param factory General plugin factory.
     */
    static bool isEnabled(GeneralFactory* factory);

signals:
    /*!
     * Emitted when exit() slot is called.
     */
    void exitCalled();
    /*!
     * Emitted when toggleVisibility() slot is called.
     */
    void toggleVisibilityCalled();

public slots:
    /*!
     * Tells the player to exit.
     */
    void exit();
    /*!
     * Toggles player window visibility.
     */
    void toggleVisibility();

private:
    QMap <uint, QString> m_strValues;
    QMap <uint, uint> m_numValues;
};

#endif
