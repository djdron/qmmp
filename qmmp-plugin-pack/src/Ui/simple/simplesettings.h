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

#ifndef SIMPLESETTINGS_H
#define SIMPLESETTINGS_H

#include <QWidget>
#include <QFileInfo>
#include "ui_simplesettings.h"

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class SimpleSettings : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleSettings(QWidget *parent = 0);
    virtual ~SimpleSettings();
    void writeSettings();


private slots:
    void on_plFontButton_clicked();
    void on_popupTemplateButton_clicked();
    void on_changeShortcutButton_clicked();

private:
    void showEvent(QShowEvent *);
    void loadFonts();
    void readSettings();
    void loadShortcuts();

    Ui::SimpleSettings m_ui;
};

#endif // SIMPLESETTINGS_H
