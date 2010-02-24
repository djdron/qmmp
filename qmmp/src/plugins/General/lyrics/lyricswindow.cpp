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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QUrl>
#include <QRegExp>
#include <qmmp/qmmpsettings.h>
#include <qmmp/qmmp.h>
#include "lyricswindow.h"

LyricsWindow::LyricsWindow(const QString &artist, const QString &title, QWidget *parent)
        : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Dialog);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    ui.artistLineEdit->setText(artist);
    ui.titleLineEdit->setText(title);
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
    on_searchPushButton_clicked();
}


LyricsWindow::~LyricsWindow()
{
}

void LyricsWindow::showText(QNetworkReply *reply)
{
    ui.stateLabel->setText(tr("Done"));
    if (reply->error() != QNetworkReply::NoError)
    {
        ui.stateLabel->setText(tr("Error"));
        ui.textEdit->setText(reply->errorString());
        return;
    }
    QString content = QString::fromUtf8(reply->readAll().constData());

    QRegExp artist_regexp("<div id=\\\"artist\\\">([^<]*)</div>");
    QRegExp title_regexp("<div id=\\\"title\\\">([^<]*)</div>");
    QRegExp lyrics_regexp("<div id=\\\"lyrics\\\">([^<]*)</div>");
    artist_regexp.indexIn(content);
    title_regexp.indexIn(content);
    content.replace("<br />", "[br /]");
    lyrics_regexp.indexIn(content);

    QString text = "<h2>" +artist_regexp.cap(1) + " - " + title_regexp.cap(1) + "</h2>";
    QString lyrics = lyrics_regexp.cap(1);
    lyrics.replace("[br /]", "<br />");
    if(lyrics.trimmed().isEmpty())
        ui.textEdit->setHtml("<b>" + tr("Not found") + "</b>");
    else
    {
        text += lyrics;
        ui.textEdit->setHtml(text);
    }
    reply->deleteLater();
}

void LyricsWindow::on_searchPushButton_clicked()
{
    ui.stateLabel->setText(tr("Receiving"));
    setWindowTitle(QString(tr("Lyrics: %1 - %2")).arg(ui.artistLineEdit->text())
                   .arg(ui.titleLineEdit->text()));
    QNetworkRequest request;
    request.setUrl(QUrl("http://www.lyricsplugin.com/winamp03/plugin/?artist=" +
                        ui.artistLineEdit->text()+"&title=" + ui.titleLineEdit->text()));
    request.setRawHeader("User-Agent", QString("qmmp/%1").arg(Qmmp::strVersion()).toAscii());
    m_http->get(request);
}
