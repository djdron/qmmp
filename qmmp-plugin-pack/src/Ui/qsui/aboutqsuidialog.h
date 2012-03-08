/***************************************************************************
 *   Copyright (C) 2011-2012 by Ilya Kotov                                 *
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

#ifndef ABOUTQSUIDIALOG_H
#define ABOUTQSUIDIALOG_H

#include <QDialog>
#include "ui_aboutqsuidialog.h"

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class AboutQSUIDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutQSUIDialog(QWidget *parent = 0);
    virtual ~AboutQSUIDialog();

private:
    Ui::AboutQSUIDialog m_ui;
    QString loadAbout();
    QString getStringFromResource(const QString& res_file);
};

#endif // ABOUTQSUIDIALOG_H
