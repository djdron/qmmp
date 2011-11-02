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

#include <QFile>
#include <QTextStream>
#include <qmmp/qmmp.h>
#include "aboutsimpleuidialog.h"

AboutSimpleUiDialog::AboutSimpleUiDialog(QWidget *parent) :
    QDialog(parent)
{
    m_ui.setupUi(this);
    m_ui.aboutTextEdit->setHtml(loadAbout());
}

AboutSimpleUiDialog::~AboutSimpleUiDialog(){}

QString AboutSimpleUiDialog::loadAbout()
{
    QString text;
    text.append("<head>");
    text.append("<META content=\"text/html; charset=UTF-8\">");
    text.append("</head>");
    text.append("<h3>"+tr("Qmmp Simple User Interface (QSUI)")+"</h3>");
    text.append(tr("Qmmp version: <b>%1</b>").arg(Qmmp::strVersion()));
    text.append("<br>");
    text.append(tr("User interface version: <b>%1</b>").arg("0.6.0"));
    text.append("<p>");
    text.append(tr("Simple user interface based on standard widgets set."));
    text.append("</p>");
    text.append("<p>");
    text.append(tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>").replace("<", "&lt;")
                .replace(">", "&gt;"));
    text.append("</p>");
    return text;
}
