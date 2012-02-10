/***************************************************************************
 *   Copyright (C) 2012 by Ilya Kotov                                      *
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

#ifndef QMMPUISETTINGS_H
#define QMMPUISETTINGS_H

#include <QObject>
#include <QStringList>

/*! @brief The QmmpUiSettings class provides access to global libqmmpui library settings.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class QmmpUiSettings : public QObject
{
    Q_OBJECT
public:
    /*!
     * Constructor.
     * @param parent Parent object.
     * This functions is for internal usage only, use QmmpSettings::instance() instead.
     */
    explicit QmmpUiSettings(QObject *parent = 0);
    /*!
     * Destructor.
     */
    virtual ~QmmpUiSettings();
    /*!
     * Returns state of the "Convert underscores to blanks" option (\b true - enabled, \b false - disabled).
     */
    bool convertUnderscore() const;
    /*!
     * Returns state of the "Convert %20 to blanks" option (\b true - enabled, \b false - disabled).
     */
    bool convertTwenty() const;
    /*!
     * Returns the state of metadata usage (\b true - use, \b false - not use).
     */
    bool useMetadata() const;
    /*!
     * Returns title format string.
     */
    const QString format() const;
    /*!
     * Sets the "Convert underscores to blanks" option state to \b enabled
     * @param enabled Option state (\b true - enabled, \b false - disabled)
     */
    void setConvertUnderscore(bool enabled);
    /*!
     * Sets the "Convert %20 to blanks" option state to \b enabled
     * @param enabled Option state (\b true - enabled, \b false - disabled)
     */
    void setConvertTwenty(bool enabled);
    /*!
     * Sets short title template.
     * @param format title template. \sa MetaDataFormatter
     */
    void setFormat(const QString &format);
    /*!
     * Sets metadata usage option state to \b enabled
     * @param enabled Option state (\b true - enabled, \b false - disabled)
     */
    void setUseMetadata(bool enabled);
    /*!
     * Returns \b true if option "Resume On Startup" is enabled, otherwise returns \b false
     */
    bool resumeOnStartup() const;
    /*!
     * Sets the "Resume on startup" option state to \b enabled
     * @param enabled Option state (\b true - enabled, \b false - disabled)
     */
    void setResumeOnStartup(bool enabled);
    /*!
     * Returns a list of filters which should be used for directory scanning.
     */
    QStringList restrictFilters() const;
    /*!
     * Disables all builtin directory scanning filters and sets them to \b filters (Example: *.mp3;*.ogg).
     */
    void setRestrictFilters(const QString &filters);
    /*!
     * Returns a list of filters which should be excluded from incoming files while directory scanning.
     */
    QStringList excludeFilters() const;
    /*!
     * Sets a list of filters which should be excluded from incoming files while directory scanning.
     * @param filters A set of filters separated by semicolon (Example: *.cue;*.ogg).
     */
    void setExcludeFilters(const QString &filters);
    /*!
     * Returns \b true if defaut playlist is enabled, othewise returns \b false.
     * Default playlist receives files from command line.
     */
    bool useDefaultPlayList() const;
    /*!
     * Returns default playlist name.
     */
    QString defaultPlayListName() const;
    /*!
     * Sets default playlist options.
     * @param name Default playlist name.
     * @param enabled Default playlist state (\b true - enabled, \b false - disabled).
     */
    void setDefaultPlayList(const QString &name, bool enabled = true);
    /*!
     * Returns a pointer to the QmmpUiSettings instance.
     */
    static QmmpUiSettings* instance();

public slots:
    /*!
     * Writes all unsaved settings to configuration file
     */
    void sync();

private:
    static QmmpUiSettings* m_instance;
    //playlist
    bool m_convertUnderscore, m_convertTwenty;
    bool m_useMetadata;
    QString m_format;
    //general
    bool m_resume_on_startup;
    QStringList m_exclude_filters, m_restrict_filters;
    //default playlist
    bool m_use_default_pl;
    QString m_default_pl_name;


};

#endif // QMMPUISETTINGS_H
