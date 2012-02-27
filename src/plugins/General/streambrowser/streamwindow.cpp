/***************************************************************************
 *   Copyright (C) 2012 by Ilya Kotov                                      *
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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QUrl>
#include <QRegExp>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <qmmp/qmmpsettings.h>
#include <qmmp/qmmp.h>
#include <qmmpui/playlistmanager.h>
#include "streamwindow.h"

StreamWindow::StreamWindow(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false);
    m_requestReply = 0;

    ui.addPushButton->setIcon(QIcon::fromTheme("list-add"));
    ui.updatePushButton->setIcon(QIcon::fromTheme("view-refresh"));

    m_icecastModel = new QStandardItemModel(this);
    m_icecastModel->setHorizontalHeaderLabels(QStringList() << tr("Name")
                                       << tr("Genre")
                                       << tr("Bitrate")
                                       << tr("Format"));
    m_filterModel = new QSortFilterProxyModel(this);
    m_filterModel->setSourceModel(m_icecastModel);
    m_filterModel->setDynamicSortFilter(true);
    m_filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);


    ui.icecastTableView->setModel(m_filterModel);
    ui.icecastTableView->verticalHeader()->setDefaultSectionSize(fontMetrics().height());
    ui.icecastTableView->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui.icecastTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui.statusLabel->hide();


    m_http = new QNetworkAccessManager(this);
     //load global proxy settings
    QmmpSettings *gs = QmmpSettings::instance();
    if (gs->isProxyEnabled())
    {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, gs->proxy().host(),  gs->proxy().port());
        if(gs->useProxyAuth())
        {
            proxy.setUser(gs->proxy().userName());
            proxy.setPassword(gs->proxy().password());
        }
        m_http->setProxy(proxy);
    }
    connect(m_http, SIGNAL(finished (QNetworkReply *)), SLOT(showText(QNetworkReply *)));
    //read settings
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("StreamBrowser");
    restoreGeometry(settings.value("geometry").toByteArray());
    ui.icecastTableView->horizontalHeader()->restoreState(settings.value("icecast_headers").toByteArray());
    settings.endGroup();
    //create cache dir
    QString path = QFileInfo(Qmmp::configFile()).absoluteDir().path();
    QDir dir(path);
    if(!dir.exists("streambrowser"))
        dir.mkdir("streambrowser");
    //read cache
    QFile file(path + "/streambrowser/icecast.xml");
    if(file.open(QIODevice::ReadOnly))
        readIceCast(&file);
    else
        on_updatePushButton_clicked();
}

StreamWindow::~StreamWindow()
{
}

void StreamWindow::showText(QNetworkReply *reply)
{
    ui.statusLabel->setText(tr("Done"));
    if (reply->error() != QNetworkReply::NoError)
    {
        ui.statusLabel->setText(tr("Error"));
        QMessageBox::warning (this, tr("Error"), reply->errorString());
        m_requestReply = 0;
        reply->deleteLater();
        return;
    }
    if(m_requestReply == reply)
    {
        m_requestReply = 0;
        readIceCast(reply);
    }
    reply->deleteLater();
}

void StreamWindow::on_updatePushButton_clicked()
{
    QNetworkRequest request;
    request.setUrl(QUrl("http://dir.xiph.org/yp.xml"));
    request.setRawHeader("User-Agent", QString("qmmp/%1").arg(Qmmp::strVersion()).toAscii());
    m_requestReply = m_http->get(request);
    ui.statusLabel->setText(tr("Receiving"));
    ui.statusLabel->show();
}

void StreamWindow::on_addPushButton_clicked()
{
    QModelIndexList indexes = ui.icecastTableView->selectionModel()->selectedRows(0);
    QStringList urls;
    foreach(QModelIndex index, indexes)
    {
        QModelIndex source_index = m_filterModel->mapToSource(index);
        urls.append(m_icecastModel->item(source_index.row(),0)->data().toString());
    }
    urls.removeDuplicates();
    PlayListManager::instance()->add(urls);
}

void StreamWindow::on_filterLineEdit_textChanged(const QString &text)
{
    m_filterModel->setFilterFixedString(text);
}

void StreamWindow::closeEvent(QCloseEvent *)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("StreamBrowser");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("icecast_headers", ui.icecastTableView->horizontalHeader()->saveState());
    settings.endGroup();

    QString path = QFileInfo(Qmmp::configFile()).absoluteDir().path();
    QFile file(path + "/streambrowser/icecast.xml");
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter writer(&file);
    writer.setCodec("UTF-8");
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("directory");
    for(int i = 0; i < m_icecastModel->rowCount(); ++i)
    {
        writer.writeStartElement("entry");
        writer.writeTextElement("server_name", m_icecastModel->item(i,0)->text());
        writer.writeTextElement("listen_url", m_icecastModel->item(i,0)->data().toString());
        writer.writeTextElement("genre", m_icecastModel->item(i,1)->text());
        writer.writeTextElement("bitrate", m_icecastModel->item(i,2)->text());
        writer.writeTextElement("server_type", m_icecastModel->item(i,3)->text());
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeEndDocument();
}

void StreamWindow::readIceCast(QIODevice *input)
{
    m_icecastModel->removeRows(0, m_icecastModel->rowCount());
    QXmlStreamReader xml(input);
    QString currentTag, server_name, listen_url, genre, bitrate, server_type;
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            currentTag = xml.name().toString();
        }
        else if (xml.isEndElement())
        {
            if (xml.name() == "entry")
            {
                m_icecastModel->appendRow(QList<QStandardItem *> ()
                                          << new QStandardItem(server_name)
                                          << new QStandardItem(genre)
                                          << new QStandardItem(bitrate)
                                          << new QStandardItem(server_type));

                QStandardItem *item = m_icecastModel->item(m_icecastModel->rowCount()-1, 0);
                item->setToolTip(server_name + "\n" + listen_url);
                item->setData(listen_url);

                server_name.clear();
                listen_url.clear();
                genre.clear();
                bitrate.clear();
                server_type.clear();
            }

        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (currentTag == "server_name")
                server_name += xml.text().toString();
            else if (currentTag == "listen_url")
                listen_url += xml.text().toString();
            else if (currentTag == "genre")
                genre += xml.text().toString();
            else if (currentTag == "bitrate")
                bitrate += xml.text().toString();
            else if(currentTag == "server_type")
                server_type += xml.text().toString();
        }
    }
    if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
    {
        qWarning("StreamWindow: xml error: %lld: %s", xml.lineNumber(), qPrintable(xml.errorString()));
    }
}
