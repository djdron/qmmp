/***************************************************************************
*   Copyright (C) 2006 by Ilya Kotov                                      *
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


#include "aboutdialog.h"

#include <QFile>
#include <QTextStream>

static QString getstringFromResource(const QString& res_file)
{
    QString ret_string;
    QFile file(res_file);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&file);
        ts.setCodec("UTF-8");
        ret_string = ts.readAll();
        file.close();
    }
    return ret_string;
}

AboutDialog::AboutDialog(QWidget* parent, Qt::WFlags fl)
        : QDialog( parent, fl )
{
    setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    licenseTextEdit->setPlainText(getstringFromResource(":COPYING"));
    aboutTextEdit->setHtml(getstringFromResource(tr(":/html/about_en.html")));
    authorsTextEdit->setPlainText(getstringFromResource(tr(":/html/authors_en.txt")));
    thanksToTextEdit->setPlainText(getstringFromResource(tr(":/html/thanks_en.txt")));
}

AboutDialog::~AboutDialog()
{}

/*$SPECIALIZATION$*/
void AboutDialog::accept()
{
    QDialog::accept();
}




