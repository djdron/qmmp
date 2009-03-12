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
#ifndef GENERALHANDLER_H
#define GENERALHANDLER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QPointer>

class QAction;
class QMenu;
class QWidget;
class General;
class Control;
class GeneralFactory;
class CommandLineManager;

/*! @brief The GeneralHandler class provied simple api to access general and command line plugins.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class GeneralHandler : public QObject
{
    Q_OBJECT
public:
    /*!
     * Object constructor,
     * @param parent Parent object
     */
    GeneralHandler(QObject *parent = 0);
    /*!
     * Destructor
     */
    ~GeneralHandler();
    /*!
     * Enum of menu type
     */
    enum MenuType
    {
        TOOLS_MENU = 0, /*!< tools menu */
        PLAYLIST_MENU   /*!< playlist context menu */
    };
    /*!
     * Sets whether the general plugin is enabled.
     * @param factory General plugin factory.
     * @param enabled Plugin enable state (\b true - enable, \b false - disable)
     */
    void setEnabled(GeneralFactory* factory, bool enable);
    /*!
     * Shows configuration dialog and updates settings automatically.
     * @param factory General plugin factory.
     * @param parentWidget Parent widget.
     */
    void showSettings(GeneralFactory* factory, QWidget* parentWidget);
    /*!
     * Returns \b true if one of the general plugin can change visibility, otherwise \b false
     */
    bool visibilityControl();
    /*!
     * Executes command by command line plugins
     * @param opt_str Command line option string
     */
    void executeCommand(const QString &opt_str);
    /*!
     * Adds the newly created action to the menu with type \b type.
     * Menu with type \b type should be created first.
     * @param action Pointer of action.
     * @param type Menu type.
     */
    void addAction(QAction *action, MenuType type = TOOLS_MENU);
    /*!
     * Removes action \b action from created menus.
     */
    void removeAction(QAction *action);
    /*!
     * Returns a list of actions of the menu type \b type
     */
    QList<QAction *> actions(MenuType type = TOOLS_MENU);
    /*!
     * Creates menu with type \b type
     * @param type Menu type.
     * @param title Menu title.
     * @param paren Parent widget
     */
    QMenu *createMenu(MenuType type, const QString &title = QString(), QWidget *parent = 0);
    /*!
     * Returns a pointer to the object's instance.
     */
    static GeneralHandler* instance();

signals:
    /*!
     * Emited when some general plugin requires to exit.
     * Use it to quit application.
     */
    void exitCalled();
    /*!
     * Emited when some general plugin requires to toggle visibility.
     * Use it to show/hide player's window.
     */
    void toggleVisibilityCalled();

private slots:
    void removeAction(QObject *action);

private:
    QMap <GeneralFactory*, General*> m_generals;
    CommandLineManager *m_commandLineManager;
    QList <QAction*> m_toolsActions;
    QList <QAction*> m_playlistActions;
    QPointer<QMenu> m_toolsMenu;
    QPointer<QMenu> m_playlistMenu;
    static GeneralHandler* m_instance;
};

#endif
