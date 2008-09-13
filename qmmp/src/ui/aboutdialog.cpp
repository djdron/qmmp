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


#include <QFile>
#include <QTextStream>

#include <qmmp/decoder.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/output.h>
#include <qmmp/outputfactory.h>
#include <qmmp/visual.h>
#include <qmmp/visualfactory.h>
#include <qmmp/effect.h>
#include <qmmp/effectfactory.h>
#include <qmmpui/general.h>
#include <qmmpui/generalfactory.h>
#include "version.h"

#include "aboutdialog.h"

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
    aboutTextEdit->setHtml(loadAbout());
    authorsTextEdit->setPlainText(getstringFromResource(tr(":/txt/authors_en.txt")));
    thanksToTextEdit->setPlainText(getstringFromResource(tr(":/txt/thanks_en.txt")));
    translatorsTextEdit->setPlainText(getstringFromResource(tr(":/txt/translators_en.txt")));
}

AboutDialog::~AboutDialog()
{}

/*$SPECIALIZATION$*/
void AboutDialog::accept()
{
    QDialog::accept();
}

QString AboutDialog::loadAbout()
{
    QString text;
    text.append("<head>");
    text.append("<META content=\"text/html; charset=UTF-8\">");
    text.append("</head>");
    text.append("<h3>"+tr("Qt-based Multimedia Player (Qmmp)")+"</h3>");
    text.append("<h4>"+tr("Version:")+" "+ QMMP_STR_VERSION "</h4>");
    text.append("<p>"+getstringFromResource(tr(":txt/description_en.txt"))+"</p>");
    text.append("<h5>"+tr("Input plugins:")+"</h5>");
    text.append("<ul type=\"square\">");
    foreach(DecoderFactory *fact, *Decoder::decoderFactories())
    {
        text.append("<li>");
        text.append(fact->properties().name);
        text.append("</li>");
    }
    text.append("</ul>");
    text.append("<h5>"+tr("Output plugins:")+"</h5>");
    text.append("<ul type=\"square\">");
    foreach(OutputFactory *fact, *Output::outputFactories())
    {
        text.append("<li>");
        text.append(fact->properties().name);
        text.append("</li>");
    }
    text.append("</ul>");
    text.append("<h5>"+tr("Visual plugins:")+"</h5>");
    text.append("<ul type=\"square\">");
    foreach(VisualFactory *fact, *Visual::factories())
    {
        text.append("<li>");
        text.append(fact->properties().name);
        text.append("</li>");
    }
    text.append("</ul>");
    text.append("<h5>"+tr("Effect plugins:")+"</h5>");
    text.append("<ul type=\"square\">");
    foreach(EffectFactory *fact, *Effect::effectFactories())
    {
        text.append("<li>");
        text.append(fact->properties().name);
        text.append("</li>");
    }
    text.append("</ul>");
    text.append("<h5>"+tr("General plugins:")+"</h5>");
    text.append("<ul type=\"square\">");
    foreach(GeneralFactory *fact, *General::generalFactories())
    {
        text.append("<li>");
        text.append(fact->properties().name);
        text.append("</li>");
    }
    text.append("</ul>");

    return text;
}
