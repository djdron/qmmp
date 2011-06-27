/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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
#include <QDesktopServices>
#include <QTextCodec>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <qmmp/metadatamanager.h>
#include <qmmp/metadatamodel.h>
#include <qmmp/tagmodel.h>
#include "ui_detailsdialog.h"
#include "playlistitem.h"
#include "tageditor_p.h"
#include "detailsdialog.h"

DetailsDialog::DetailsDialog(PlayListItem *item, QWidget *parent)
        : QDialog(parent)
{
    m_ui = new Ui::DetailsDialog;
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, false);
    m_metaDataModel = 0;
    m_item = item;
    m_ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_path = item->url();
    setWindowTitle (m_path.section('/',-1));
    m_ui->pathEdit->setText(m_path);
    m_ui->directoryButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_metaDataModel = MetaDataManager::instance()->createMetaDataModel(item->url(), this);

    if(m_metaDataModel)
    {
        foreach(TagModel *tagModel, m_metaDataModel->tags())
            m_ui->tabWidget->addTab(new TagEditor(tagModel, this), tagModel->name());

        foreach(QString title, m_metaDataModel->descriptions().keys())
        {
            QTextEdit *textEdit = new QTextEdit(this);
            textEdit->setReadOnly(true);
            textEdit->setPlainText(m_metaDataModel->descriptions().value(title));
            m_ui->tabWidget->addTab(textEdit, title);
        }
    }
    printInfo();
}

DetailsDialog::~DetailsDialog()
{
    delete m_ui;
}

void DetailsDialog:: on_directoryButton_clicked()
{
    QString dir_path;
    if(!m_path.contains("://")) //local file
        dir_path = QFileInfo(m_path).absolutePath();
    else if (m_path.contains(":///")) //pseudo-protocol
    {
        dir_path = QUrl(m_path).path();
        dir_path.replace(QString(QUrl::toPercentEncoding("#")), "#");
        dir_path.replace(QString(QUrl::toPercentEncoding("?")), "?");
        dir_path.replace(QString(QUrl::toPercentEncoding("%")), "%");
        dir_path = QFileInfo(dir_path).absolutePath();
    }
    else
        return;
#ifdef Q_WS_X11
    QProcess::execute("xdg-open", QStringList() << dir_path); //works with lxde
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir_path));
#endif
}

void DetailsDialog::printInfo()
{
    QList <FileInfo *> flist = MetaDataManager::instance()->createPlayList(m_path, true);
    QMap <Qmmp::MetaData, QString> metaData;
    if(!flist.isEmpty() && QFile::exists(m_item->url()))
        metaData = flist.at(0)->metaData();
    else
        metaData = *m_item;
    QString formattedText;
    formattedText.append("<TABLE>");
    //tags
    formattedText += formatRow(tr("Title"), metaData[Qmmp::TITLE]);
    formattedText += formatRow(tr("Artist"), metaData[Qmmp::ARTIST]);
    formattedText += formatRow(tr("Album"), metaData[Qmmp::ALBUM]);
    formattedText += formatRow(tr("Comment"), metaData[Qmmp::COMMENT]);
    formattedText += formatRow(tr("Genre"), metaData[Qmmp::GENRE]);
    formattedText += formatRow(tr("Composer"), metaData[Qmmp::COMPOSER]);
    if(metaData[Qmmp::YEAR] != "0")
        formattedText += formatRow(tr("Year"), metaData[Qmmp::YEAR]);
    if(metaData[Qmmp::TRACK] != "0")
        formattedText += formatRow(tr("Track"), metaData[Qmmp::TRACK]);
    if(metaData[Qmmp::DISCNUMBER] != "0")
        formattedText += formatRow(tr("Disc number"), metaData[Qmmp::DISCNUMBER]);
    //audio info
    if(!m_metaDataModel)
    {
        formattedText.append("</TABLE>");
        m_ui->textEdit->setHtml(formattedText);
        return;
    }
    QHash <QString, QString> ap = m_metaDataModel->audioProperties();
    //line
    if(formattedText.trimmed() != "<TABLE>")
    {
        formattedText.append("<tr>");
        formattedText.append("<td colspan=2>");
        formattedText.append("<hr>");
        formattedText.append("</td>");
        formattedText.append("</tr>");
    }

    foreach(QString key, ap.keys())
        formattedText += formatRow(key, ap.value(key));

    formattedText.append("</TABLE>");
    m_ui->textEdit->setHtml(formattedText);
}

QString DetailsDialog::formatRow(const QString key, const QString value)
{
    if(value.isEmpty())
        return QString();
    QString str("<tr>");
    str.append("<td><b>" + key + "</b></td> <td style=\"padding-left: 15px;\">" + value + "</td>");
    str.append("</tr>");
    return str;
}

void DetailsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(m_ui->buttonBox->standardButton(button) == QDialogButtonBox::Save)
    {
        TagEditor *tab = qobject_cast<TagEditor *> (m_ui->tabWidget->currentWidget());
        if(tab)
            tab->save();
    }
    else
        reject();
}
