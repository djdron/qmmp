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
#ifndef COMMANDLINEOPTION_H
#define COMMANDLINEOPTION_H


class CommandLineManager;
class QTranslator;
class QString;
class QObject;

/*! @brief Abstract base class of the command line plugins.
 * @author Vladimir Kuznetsov <vovanec@gmail.ru>
 */
class CommandLineOption
{
public:
    /*!
     * Returns \b true if \b opt_str string can be processed,
     * otherise \b false
     */
    virtual bool identify(const QString& opt_str)const = 0;
    /*!
     * Command line option name
     */
    virtual const QString name()const = 0;
    /*!
     * Help string.
     */
    virtual const QString helpString()const = 0;
    /*!
     * Parses \b opt_str args(if needed), executes command.
     */
    virtual void executeCommand(const QString& opt_str, CommandLineManager* clm) = 0;
    /*!
     * Creates translator with parent object \b parent
     */
    virtual QTranslator *createTranslator(QObject *parent) = 0;
    /*!
     * Object destructor
     */
    virtual ~CommandLineOption() {}
};

Q_DECLARE_INTERFACE(CommandLineOption,"CommandLineOptionInterface/1.0");

#endif
