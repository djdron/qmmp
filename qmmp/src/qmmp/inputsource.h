/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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

#ifndef INPUTSOURCE_H
#define INPUTSOURCE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QIODevice>
#include "inputsourcefactory.h"

/*! @brief The InputSource class provides the base interface class of transports.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class InputSource : public QObject
{
Q_OBJECT
public:
    /*!
     * Object contsructor.
     * @param url Input source path or url.
     * @param parent Parent object.
     */
    InputSource(const QString &url, QObject *parent = 0);
    /*!
     * Returns QIODevice-based object for I/O operations.
     * Subclass shoud reimplement this function.
     */
    virtual QIODevice *ioDevice() = 0;
    /*!
     * Prepares input data source for usage.
     * Subclass shoud reimplement this function.
     */
    virtual bool initialize() = 0;
    /*!
     * Returns \b true if transport is ready for usage; otherwise returns \b false.
     */
    virtual bool isReady() = 0;
    /*!
     * Returns content type of the input stream. Default implementation returns empty string.
     */
    virtual QString contentType() const;
    /*!
     * Returns input source path or url.
     */
    const QString url() const;
    /*!
     * Returns start position is ms;
     */
    qint64 offset() const;
    /*!
     * Sets start position to \b offset ms.
     */
    void setOffset(qint64 offset);
    /*!
     * Creates InputSource object.
     * @param url Input source path or url.
     * @param parent Parent object.
     * Returns \b 0 if the given url is not supported.
     */
    static InputSource *create(const QString &url, QObject *parent = 0);
    /*!
     * Returns a list of transport factories.
     */
    static QList<InputSourceFactory *> *factories();
    /*!
     * Returns a list of transport plugin file names.
     */
    static QStringList files();
    /*!
     * Returns a list of supported protocols.
     */
    static QStringList protocols();

signals:
    /*!
     * This signal is emitted when transport is ready for usage.
     * @param s Pointer of this object.
     */
    void ready(InputSource *s);

private:
    QString m_url;
    qint64 m_offset;
    static void checkFactories();
    static QList<InputSourceFactory*> *m_factories;
    static QStringList m_files;
};

#endif // INPUTSOURCE_H
