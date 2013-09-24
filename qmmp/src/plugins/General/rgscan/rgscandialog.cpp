/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QSettings>
#include <QProgressBar>
#include <QThreadPool>
#include <qmmpui/playlisttrack.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/filedialog.h>
#include "rgscaner.h"
#include "rgscandialog.h"

RGScanDialog::RGScanDialog(QList <PlayListTrack *> tracks,  QWidget *parent) : QDialog(parent)
{
    m_ui.setupUi(this);
    m_ui.tableWidget->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 2);
    m_ui.tableWidget->verticalHeader()->setResizeMode(QHeaderView::Fixed);

    MetaDataFormatter formatter("%p%if(%p&%t, - ,)%t - %l");

    //FIXME remove dupliacates
    foreach(PlayListTrack *track , tracks)
    {
        if(track->length() == 0 || track->url().contains("://"))
            continue;

        QString text = formatter.parse(track);
        QTableWidgetItem *item = new QTableWidgetItem(text);
        item->setData(Qt::UserRole, track->url());
        m_ui.tableWidget->insertRow(m_ui.tableWidget->rowCount());
        m_ui.tableWidget->setItem(m_ui.tableWidget->rowCount() - 1, 0, item);
        QProgressBar *progressBar = new QProgressBar(this);
        progressBar->setRange(0, 100);
        //progressBar->setValue(50);
        m_ui.tableWidget->setCellWidget(m_ui.tableWidget->rowCount() - 1, 1, progressBar);
    }

    m_ui.tableWidget->resizeColumnsToContents();
}

RGScanDialog::~RGScanDialog()
{
    stop();
}

void RGScanDialog::on_calculateButton_clicked()
{
    for(int i = 0; i < m_ui.tableWidget->rowCount(); ++i)
    {
        QString url = m_ui.tableWidget->item(i, 0)->data(Qt::UserRole).toString();
        RGScaner *scaner = new RGScaner();
        m_scaners.append(scaner);
        scaner->prepare(url);
        scaner->setAutoDelete(false);
        connect(scaner, SIGNAL(progress(int)), m_ui.tableWidget->cellWidget(i, 1), SLOT(setValue(int)));
        connect(scaner, SIGNAL(finished(QString)), SLOT(onScanFinished(QString)));
        QThreadPool::globalInstance()->start(scaner);
    }
}

void RGScanDialog::onScanFinished(QString url)
{
    for(int i = 0; i < m_ui.tableWidget->rowCount(); ++i)
    {
        if(url != m_ui.tableWidget->item(i, 0)->data(Qt::UserRole).toString())
            continue;
        m_ui.tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(m_scaners.at(i)->gain())));
    }

    bool stopped = true;

    foreach (RGScaner *scaner, m_scaners)
    {
        if(scaner->isRunning())
            stopped = false;
    }

    if(stopped)
    {
        qDebug("RGScanDialog: all threads finished");
        QThreadPool::globalInstance()->waitForDone();
        qDeleteAll(m_scaners);
        m_scaners.clear();
    }
}

void RGScanDialog::stop()
{
    if(m_scaners.isEmpty())
        return;
    foreach (RGScaner *scaner, m_scaners)
    {
        scaner->stop();
    }
    QThreadPool::globalInstance()->waitForDone();

    qDeleteAll(m_scaners);
    m_scaners.clear();
}
