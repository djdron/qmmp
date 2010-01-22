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

#include <QHttp>
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
    m_http = new QHttp(this);
     //load global proxy settings
    QmmpSettings *gs = QmmpSettings::instance();
    if (gs->isProxyEnabled())
        m_http->setProxy(gs->proxy().host(),
                         gs->proxy().port(),
                         gs->useProxyAuth() ? gs->proxy().userName() : QString(),
                         gs->useProxyAuth() ? gs->proxy().password() : QString());
    connect(m_http, SIGNAL(done(bool)), SLOT(showText(bool)));
    connect(m_http, SIGNAL(stateChanged(int)), SLOT(showState (int)));
    on_searchPushButton_clicked();
}


LyricsWindow::~LyricsWindow()
{
}

void LyricsWindow::showText(bool error)
{
    if (error)
    {
        ui.textEdit->setText(m_http->errorString());
        return;
    }
    QString content = QString::fromUtf8(m_http->readAll().constData());

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
}

void LyricsWindow::showState(int state)
{
    switch ((int) state)
    {
    case QHttp::Unconnected:
        ui.stateLabel->setText(tr("No connection"));
        break;
    case QHttp::HostLookup:
        ui.stateLabel->setText(tr("Looking up host..."));
        break;
    case QHttp::Connecting:
        ui.stateLabel->setText(tr("Connecting..."));
        break;
    case QHttp::Sending:
        ui.stateLabel->setText(tr("Sending request..."));
        break;
    case QHttp::Reading:
        ui.stateLabel->setText(tr("Receiving"));
        break;
    case QHttp::Connected:
        ui.stateLabel->setText(tr("Connected"));
        break;
    case QHttp::Closing:
        ui.stateLabel->setText(tr("Closing connection..."));
    }
}

void LyricsWindow::on_searchPushButton_clicked()
{
    m_http->setHost("www.lyricsplugin.com");
    setWindowTitle(QString(tr("Lyrics: %1 - %2")).arg(ui.artistLineEdit->text()).arg(ui.titleLineEdit->text()));
    m_http->get("/winamp03/plugin/?artist=" + QUrl::toPercentEncoding(ui.artistLineEdit->text())
                +"&title=" + QUrl::toPercentEncoding(ui.titleLineEdit->text()));
}
