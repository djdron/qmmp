/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#include <QPushButton>

#include "ui_abstractdetailsdialog.h"
#include "abstractdetailsdialog.h"

AbstractDetailsDialog::AbstractDetailsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui = new Ui::AbstractDetailsDialog();
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->buttonBox, SIGNAL(clicked (QAbstractButton *)),SLOT(processButton(QAbstractButton *)));
}

AbstractDetailsDialog::~AbstractDetailsDialog()
{
    delete ui;
}

void AbstractDetailsDialog::hideSaveButton(bool hide)
{
    ui->buttonBox->button(QDialogButtonBox::Save)->setHidden(hide);
}

void AbstractDetailsDialog::blockSaveButton(bool block)
{
    ui->buttonBox->button(QDialogButtonBox::Save)->setDisabled(block);
}

void AbstractDetailsDialog::setMetaData(Qmmp::MetaData key, const QString &value)
{
    switch((int) key)
    {
    case Qmmp::TITLE:
        ui->titleLineEdit->setText(value);
        break;
    case Qmmp::ARTIST:
        ui->artistLineEdit->setText(value);
        break;
    case Qmmp::ALBUM:
        ui->albumLineEdit->setText(value);
        break;
    case Qmmp::COMMENT:
        ui->commentBrowser->setPlainText(value);
        break;
    case Qmmp::GENRE:
        ui->genreLineEdit->setText(value);
        break;
    case Qmmp::YEAR:
        ui->yearSpinBox->setValue(value.toInt());
        break;
    case Qmmp::TRACK:
        ui->trackSpinBox->setValue(value.toInt());
        break;
    case Qmmp::URL:
        ui->pathLineEdit->setText(value);
        setWindowTitle (value.section('/',-1));
    }
}

void AbstractDetailsDialog::writeTags()
{

}

void AbstractDetailsDialog::setMetaData(Qmmp::MetaData key, int value)
{
    setMetaData(key, QString("%1").arg(value));
}

void AbstractDetailsDialog::setAudioProperties(QMap <QString, QString> p)
{
    QString formattedText;
    formattedText.append("<TABLE>");
    foreach(QString key, p.keys())
    {
        formattedText.append("<tr>");
        formattedText.append("<td>" + key + ":</td> <td style=\"padding-left: 5px; \"><b>"
                             + p.value(key) + "</b></td>");
        formattedText.append("</tr>");
    }
    formattedText.append("</TABLE>");
    ui->propertiesLabel->setText(formattedText);
}

void AbstractDetailsDialog::processButton(QAbstractButton *button)
{
    switch((int) ui->buttonBox->standardButton(button))
    {
    case QDialogButtonBox::Close:
        close();
        break;
    case QDialogButtonBox::Save:
        writeTags();
        break;
    }
}

const QString AbstractDetailsDialog::strMetaData(Qmmp::MetaData key)
{
    switch((int) key)
    {
    case Qmmp::TITLE:
        return ui->titleLineEdit->text();
    case Qmmp::ARTIST:
        return ui->artistLineEdit->text();
    case Qmmp::ALBUM:
        return ui->albumLineEdit->text();
    case Qmmp::COMMENT:
        return ui->commentBrowser->toPlainText();
    case Qmmp::GENRE:
        return ui->genreLineEdit->text();
    case Qmmp::YEAR:
        return QString("%1").arg(ui->yearSpinBox->value());
    case Qmmp::TRACK:
        return QString("%1").arg(ui->trackSpinBox->value());
    case Qmmp::URL:
        return ui->pathLineEdit->text();
    }
}

int AbstractDetailsDialog::intMetaData(Qmmp::MetaData key)
{
    return strMetaData(key).toInt();
}
