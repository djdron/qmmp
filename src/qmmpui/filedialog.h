/**************************************************************************
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

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QString>
#include <QStringList>

#include <QFileDialog>
#include <QMap>

#include "filedialogfactory.h"


/*! @brief The FileDialog class is the base interface class of the file dialogs.
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class FileDialog : public QObject
{
    Q_OBJECT
public:
    /*!
     * Enum of available file dialog modes
     */
    enum Mode
    {
        AddFile = 0,   /*!< Get existing file */
        AddDir,        /*!< Get existing directory */
        AddFiles,      /*!< Get existing files */
        AddDirs,       /*!< Get existing directories */
        AddDirsFiles,  /*!< Get existing files and directories */
        SaveFile       /*!< Get existing or non-existing file */
    };
    /*!
     * This is a static function that will open modal file dialog and
     * return an existing directory selected by the user.
     * @param parent Parent widget.
     * @param caption Dialog title.
     * @param dir Default directory.
     */
    static QString getExistingDirectory(QWidget *parent = 0,
                                        const QString &caption = QString(),
                                        const QString &dir = QString());
    /*!
     * This is a static function that will open modal file dialog and
     * return an existing file selected by the user.
     * @param parent Parent widget.
     * @param caption Dialog title.
     * @param dir Default directory.
     * @param filter Filer used by file dialog
     * (example: "Audio (*.mp3 *.ogg);;Text files (*.txt);;XML files (*.xml)").
     * @param selectedFilter Default selected filter
     */
    static QString getOpenFileName(QWidget *parent = 0,
                                   const QString &caption = QString(),
                                   const QString &dir = QString(),
                                   const QString &filter = QString(),
                                   QString *selectedFilter = 0);
    /*!
     * This is a static function that will open modal file dialog and
     * return an existing file selected by the user.
     * @param parent Parent widget.
     * @param caption Dialog title.
     * @param dir Default directory.
     * @param filter Filer used by file dialog
     * (example: "Audio (*.mp3 *.ogg);;Text files (*.txt);;XML files (*.xml)").
     * @param selectedFilter Default selected filter.
     */
    static QStringList getOpenFileNames(QWidget *parent = 0,
                                        const QString &caption = QString(),
                                        const QString &dir = QString(),
                                        const QString &filter = QString(),
                                        QString *selectedFilter = 0);
    /*!
     * This is a static function that will open modal file dialog and
     * return a file name selected by the user. The file does not have to exist.
     * @param parent Parent widget.
     * @param caption Dialog title.
     * @param dir Default directory.
     * @param filter Filer used by file dialog
     * (example: "Audio (*.mp3 *.ogg);;Text files (*.txt);;XML files (*.xml)").
     * @param selectedFilter Default selected filter.
     */
    static QString getSaveFileName (QWidget *parent = 0,
                                    const QString &caption = QString(),
                                    const QString &dir = QString(),
                                    const QString &filter = QString(),
                                    QString *selectedFilter = 0);
    /*!
     * Opens nonmodal file dialog. Selected file dialog should support nonmodal mode.
     * Otherwise this function creates modal dialog.
     * @param parent Parent widget.
     * @param mode File dialog mode.
     * @param dir Default directory.
     * @param receiver Receiver QObject.
     * @param member Receiver slot.
     * @param caption Dialog title.
     * @param filters Filer used by file dialog
     * (example: "Audio (*.mp3 *.ogg);;Text files (*.txt);;XML files (*.xml)").
     *
     * Usage: FileDialog::popup(this, FileDialog::AddDirs, &m_lastDir,
     *                 m_playListModel, SLOT(addFileList(const QStringList&)),
     *                 tr("Choose a directory"));
     */
    static void popup(QWidget *parent = 0,
                      Mode mode = AddFiles,
                      QString *dir = 0,
                      QObject *receiver = 0,
                      const char *member = 0,
                      const QString &caption = QString(),
                      const QString &filters = QString());
    /*!
     * Returns a list of registered file dialog factories.
     */
    static QList <FileDialogFactory*> registeredFactories();
    /*!
     * Returns \b true if selected file dialog doesn't support nonmodal mode, otherwise returns \b false
     */
    static bool isModal();
    /*!
     * Selects current file dialog factory.
     */
    static void setEnabled(FileDialogFactory *factory);
    /*!
     * Returns \b true if file dialog \b factory is used by default, otherwise returns \b false
     */
    static bool isEnabled(FileDialogFactory *factory);

signals:
    /*!
     * Emitted when the add button has pressed. Subclass should emit this signal.
     */
    void filesAdded(const QStringList&);
protected:
    /*!
     * Object constructor.
     */
    FileDialog();
    /*!
     * This is a function that will open modal file dialog and
     * return an existing directory selected by the user.
     *
     * @param parent Parent widget.
     * @param caption Dialog title.
     * @param dir Default directory.
     */
    virtual QString existingDirectory(QWidget *parent,
                                      const QString &caption,
                                      const QString &dir);
    /*!
     * This is a function that will open modal file dialog and
     * return an existing file selected by the user.
     * Subclass should reimplement this function.
     * @param parent Parent widget.
     * @param caption Dialog title.
     * @param dir Default directory.
     * @param filter Filer used by file dialog
     * (example: "Audio (*.mp3 *.ogg);;Text files (*.txt);;XML files (*.xml)").
     * @param selectedFilter Default selected filter
     */
    virtual QString openFileName( QWidget *parent,
                                  const QString &caption,
                                  const QString &dir,
                                  const QString &filter,
                                  QString *selectedFilter);
    /*!
     * This is a function that will open modal file dialog and
     * return an existing files selected by the user.
     * Subclass should reimplement this function.
     * @param parent Parent widget.
     * @param caption Dialog title.
     * @param dir Default directory.
     * @param filter Filer used by file dialog
     * (example: "Audio (*.mp3 *.ogg);;Text files (*.txt);;XML files (*.xml)").
     * @param selectedFilter Default selected filter
     */
    virtual QStringList openFileNames(QWidget *parent,
                                      const QString &caption,
                                      const QString &dir,
                                      const QString &filter,
                                      QString *selectedFilter);
    /*!
    * This is a function that will open modal file dialog and
    * return a file name selected by the user. The file does not have to exist.
    * Subclass should reimplement this function.
    * @param parent Parent widget.
    * @param caption Dialog title.
    * @param dir Default directory.
    * @param filter Filer used by file dialog
    * (example: "Audio (*.mp3 *.ogg);;Text files (*.txt);;XML files (*.xml)").
    * @param selectedFilter Default selected filter.
    */
    virtual QString saveFileName ( QWidget *parent ,
                                   const QString &caption,
                                   const QString &dir,
                                   const QString &filter ,
                                   QString *selectedFilter);
    /*!
     * Returns \b true if file dialog doesn't support nonmodal mode, otherwise returns \b false
     * Subclass should reimplement this function.
     */
    virtual bool modal()const
    {
        return true;
    };
    /*!
     * Object destructor
     */
    virtual ~FileDialog()
    {
        ;
    };
    /*!
     * Opens nonmodal file dialog. Selected file dialog should support nonmodal mode.
     * Otherwise this function does nothing.
     * Nonmodal dialog subclass should reimplement this function.
     * @param dir Default directory.
     * @param mode File dialog mode.
     * @param caption Dialog title.
     * @param mask Filer used by file dialog
     */
    virtual void raise(const QString &dir = QString(),
                       Mode mode = AddFiles,
                       const QString &caption = QString(),
                       const QStringList &mask = QStringList())
    {
        Q_UNUSED(dir);
        Q_UNUSED(mode);
        Q_UNUSED(caption);
        Q_UNUSED(mask);
    }
    /*!
     * Returns a pointer to the selected file dialog instance.
     */
    static FileDialog* instance();
    /*!
     * Returns a pointer to the default file dialog instance.
     */
    static FileDialog* defaultInstance();
    /*!
     * Registers file dialog \b factory
     * Returns \b false if \b factory is already registered, otherwise returns \b true
     */
    static bool registerFactory(FileDialogFactory *factory);

private slots:
    void updateLastDir(const QStringList&);

private:
    void init(QObject* receiver, const char* member, QString *dir);
    static void registerBuiltinFactories();
    static void registerExternalFactories();
    static QMap <QString,FileDialogFactory*> factories;
    static FileDialog* _instance;
    static QString m_current_factory;
    bool m_initialized;
    QString *m_lastDir;
};

#endif


