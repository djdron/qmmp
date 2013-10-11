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
#include <QSettings>
#include <qmmpui/playlisttrack.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/filedialog.h>
#include <qmmp/metadatamanager.h>
#include "rgscanner.h"
#include "gain_analysis.h"
#include "rgscandialog.h"

struct ReplayGainInfoItem
{
    QMap<Qmmp::ReplayGainKey, double> info;
    QString url;
    GainHandle_t *handle;
};

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

        if(!track->url().toLower().endsWith(".mp3"))
            continue;

        QString text = formatter.parse(track);
        QTableWidgetItem *item = new QTableWidgetItem(text);
        item->setData(Qt::UserRole, track->url());
        m_ui.tableWidget->insertRow(m_ui.tableWidget->rowCount());
        m_ui.tableWidget->setItem(m_ui.tableWidget->rowCount() - 1, 0, item);
        QProgressBar *progressBar = new QProgressBar(this);
        progressBar->setRange(0, 100);
        m_ui.tableWidget->setCellWidget(m_ui.tableWidget->rowCount() - 1, 1, progressBar);
    }

    m_ui.tableWidget->resizeColumnsToContents();
    m_ui.writeButton->setEnabled(false);

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    restoreGeometry(settings.value("RGScaner/geometry").toByteArray());
}

RGScanDialog::~RGScanDialog()
{
    stop();
}

void RGScanDialog::on_calculateButton_clicked()
{
    m_ui.writeButton->setEnabled(false);
    for(int i = 0; i < m_ui.tableWidget->rowCount(); ++i)
    {
        QString url = m_ui.tableWidget->item(i, 0)->data(Qt::UserRole).toString();
        RGScanner *scaner = new RGScanner();

        if(!scaner->prepare(url))
        {
            m_ui.tableWidget->setItem(i, 2, new QTableWidgetItem(tr("Error")));
            delete scaner;
            continue;
        }
        scaner->setAutoDelete(false);
        m_scanners.append(scaner);
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
        RGScanner *scanner = findScannerByUrl(url);
        if(!scanner)
            qFatal("RGScanDialog: unable to find scanner by URL!");
        m_ui.tableWidget->setItem(i, 2, new QTableWidgetItem(tr("%1 dB").arg(scanner->gain())));
        m_ui.tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(scanner->peak())));
        break;
    }

    bool stopped = true;

    foreach (RGScanner *scanner, m_scanners)
    {
        if(scanner->isRunning())
            stopped = false;
    }

    if(stopped)
    {
        qDebug("RGScanDialog: all threads finished");
        QThreadPool::globalInstance()->waitForDone();

        QMultiMap<QString, ReplayGainInfoItem*> itemGroupMap; //items grouped  by album

        //group by album name
        foreach (RGScanner *scanner, m_scanners)
        {
            ReplayGainInfoItem *item = new ReplayGainInfoItem;
            item->info[Qmmp::REPLAYGAIN_TRACK_GAIN] = scanner->gain();
            item->info[Qmmp::REPLAYGAIN_TRACK_PEAK] = scanner->peak();
            item->url = scanner->url();
            item->handle = scanner->handle();
            QString album = getAlbumName(item->url);
            itemGroupMap.insert(album, item);
        }
        //calculate album peak and gain
        foreach (QString album, itemGroupMap.keys())
        {
            QList<ReplayGainInfoItem*> items = itemGroupMap.values(album);
            GainHandle_t **a = (GainHandle_t **) malloc(items.count()*sizeof(GainHandle_t *));
            double album_peak = 0;
            for(int i = 0; i < items.count(); ++i)
            {
                a[i] = items[i]->handle;
                album_peak = qMax(items[i]->info[Qmmp::REPLAYGAIN_TRACK_PEAK], album_peak);
            }
            double album_gain = GetAlbumGain(a, items.count());
            free(a);
            foreach (ReplayGainInfoItem *item, items)
            {
                item->info[Qmmp::REPLAYGAIN_ALBUM_PEAK] = album_peak;
                item->info[Qmmp::REPLAYGAIN_ALBUM_GAIN] = album_gain;
            }
        }
        //clear scanners
        qDeleteAll(m_scanners);
        m_scanners.clear();

        //update table
        QList<ReplayGainInfoItem*> replayGainItemList = itemGroupMap.values();
        for(int i = 0; i < m_ui.tableWidget->rowCount(); ++i)
        {
            QString url = m_ui.tableWidget->item(i, 0)->data(Qt::UserRole).toString();
            foreach (ReplayGainInfoItem *item, replayGainItemList)
            {
                if(item->url == url)
                {
                    double album_gain = item->info[Qmmp::REPLAYGAIN_ALBUM_GAIN];
                    double album_peak = item->info[Qmmp::REPLAYGAIN_ALBUM_PEAK];
                    m_ui.tableWidget->setItem(i, 3, new QTableWidgetItem(tr("%1 dB").arg(album_gain)));
                    m_ui.tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(album_peak)));
                }
            }
        }

        //clear items
        qDeleteAll(replayGainItemList);
        replayGainItemList.clear();
        itemGroupMap.clear();

        m_ui.writeButton->setEnabled(true);
    }
}

void RGScanDialog::reject()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("RGScaner/geometry", saveGeometry());
    QDialog::reject();
}

void RGScanDialog::stop()
{
    if(m_scanners.isEmpty())
        return;
    foreach (RGScanner *scaner, m_scanners)
    {
        scaner->stop();
    }
    QThreadPool::globalInstance()->waitForDone();
    qDeleteAll(m_scanners);
    m_scanners.clear();
}

RGScanner *RGScanDialog::findScannerByUrl(const QString &url)
{
    foreach (RGScanner *scanner, m_scanners)
    {
        if(scanner->url() == url)
            return scanner;
    }
    return 0;
}

QString RGScanDialog::getAlbumName(const QString &url)
{
    QList <FileInfo *> infoList = MetaDataManager::instance()->createPlayList(url);
    if(infoList.isEmpty())
        return QString();
    QString album = infoList.first()->metaData(Qmmp::ALBUM);
    qDeleteAll(infoList);
    return album;
}
