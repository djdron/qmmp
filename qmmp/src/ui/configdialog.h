/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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
#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>

#include "ui_configdialog.h"


/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class QFileInfo;

class Skin;
class InputPluginItem;
class OutputPluginItem;
class VisualPluginItem;
class EffectPluginItem;
class GeneralPluginItem;

class ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    ConfigDialog(QWidget *parent = 0);

    ~ConfigDialog();

private slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void changeSkin();
    void setPlFont();
    void setMainFont();
    void showPluginSettings();
    void showPluginInfo();
    void addTitleString( QAction * );
    void saveSettings();

private:
    void readSettings();
    void loadSkins();
    void findSkins(const QString &path);
    void loadPluginsInfo();
    void loadFonts();
    void createMenus();


    QList <QFileInfo> m_skinList;
    Ui::ConfigDialog ui;
    Skin *m_skin;
    QPixmap pixmap;

    QList <InputPluginItem*> m_inputPluginItems;
    QList <OutputPluginItem*> m_outputPluginItems;
    QList <VisualPluginItem*> m_visualPluginItems;
    QList <EffectPluginItem*> m_effectPluginItems;
    QList <GeneralPluginItem*> m_generalPluginItems;
};

#endif
