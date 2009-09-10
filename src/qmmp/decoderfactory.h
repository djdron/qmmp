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

#ifndef DECODERFACTORY_H
#define DECODERFACTORY_H

class QObject;
class QString;
class QIODevice;
class QWidget;
class QTranslator;

class Decoder;
class Output;
class FileInfo;
class MetaDataModel;

/*! @brief Helper class to store input plugin properies.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class DecoderProperties
{
public:
    /*!
     * Constructor
     */
    DecoderProperties()
    {
        hasAbout = FALSE;
        hasSettings = FALSE;
        noInput = FALSE;
        noOutput = FALSE;

    }
    QString name;        /*!< Input plugin full name */
    QString shortName;   /*!< Input plugin short name for internal usage */
    QString filter;      /*!< File filter (example: "*.mp3 *.ogg") */
    QString description; /*!< File filter description */
    QString contentType; /*!< Supported content types */
    QString protocols;   /*!< Supported protocols. Should be empty if plugin uses stream input. */
    bool hasAbout;       /*!< Should be \b true if plugin has about dialog, otherwise returns \b false */
    bool hasSettings;    /*!< Should be \b true if plugin has settings dialog, otherwise returns \b false */
    bool noInput;        /*!< Should be \b true if plugin has own input, otherwise returns \b false */
    bool noOutput;       /*!< Should be \b true if plugin has own output, otherwise returns \b false */
};
/*! @brief Input plugin interface (decoder factory).
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class DecoderFactory
{
public:
    /*!
    * Object destructor.
    */
    virtual ~DecoderFactory() {}
    /*!
     * Returns \b true if plugin supports \b source, otherwise returns \b false
     */
    virtual bool supports(const QString &source) const = 0;
    /*!
     * Returns \b true if plugin can decode data provided by \b d, otherwise returns \b false
     */
    virtual bool canDecode(QIODevice *d) const = 0;
    /*!
     * Returns general plugin properties.
     */
    virtual const DecoderProperties properties() const = 0;
    /*!
     * Creates decoder object.
     * @param parent Parent object.
     * @param input Input data (if required)
     * @param output Output object (if required)
     * @param path File path
     */
    virtual Decoder *create(QObject *parent, QIODevice *input = 0,
                            Output *output = 0, const QString &path = QString()) = 0;
    /*!
     * Extracts metadata and audio information from file \b path and returns a list of FileInfo items.
     * One file may contain several playlist items (for example: cda disk or flac with embedded cue)
     * @param fileName File path.
     * @param useMetaData Metadata usage (\b true - use, \b - do not use)
     */
    virtual QList<FileInfo *> createPlayList(const QString &fileName, bool useMetaData) = 0;
    /*!
     * Creats metadata object, which provides full access to file tags.
     * @param path File path.
     * @param parent Parent object.
     * @return MetaDataModel pointer.
     */
    virtual MetaDataModel* createMetaDataModel(const QString &path, QObject *parent = 0) = 0;
    /*!
     * Shows settings dialog.
     * @param parent Parent widget.
     */
    virtual void showSettings(QWidget *parent) = 0;
    /*!
     * Shows about dialog.
     * @param parent Parent widget.
     */
    virtual void showAbout(QWidget *parent) = 0;
    /*!
     * Creates QTranslator object of the system locale. Should return 0 if translation doesn't exist.
     * @param parent Parent object.
     */
    virtual QTranslator *createTranslator(QObject *parent) = 0;
};

Q_DECLARE_INTERFACE(DecoderFactory, "DecoderFactory/1.0");

#endif
