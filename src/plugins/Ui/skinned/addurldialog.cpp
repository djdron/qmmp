/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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

#include <QSettings>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QUrl>
#include <QMessageBox>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/playlistmodel.h>
#include <qmmp/qmmpsettings.h>
#include <qmmp/qmmp.h>
#include "addurldialog.h"

#define HISTORY_SIZE 10

AddUrlDialog::AddUrlDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_history = settings.value("URLDialog/history").toStringList();
    urlComboBox->addItems(m_history);
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
}

AddUrlDialog::~AddUrlDialog()
{
    if ( m_history.size() > HISTORY_SIZE)
        m_history.removeLast();
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("URLDialog/history", m_history);
}

QPointer<AddUrlDialog> AddUrlDialog::instance = 0;

void AddUrlDialog::popup(QWidget* parent,PlayListModel* model )
{
    if (!instance)
    {
        instance = new AddUrlDialog(parent);
        instance->setModel(model);
    }
    instance->show();
    instance->raise();
}

void AddUrlDialog::accept( )
{
    if (!urlComboBox->currentText().isEmpty())
    {
        QString s = urlComboBox->currentText().trimmed();
        if (!s.startsWith("http://") && !s.contains("://"))
            s.prepend("http://");
        m_history.removeAll(s);
        m_history.prepend(s);

        if (s.startsWith("http://"))
        {
            //try to download playlist
            PlaylistFormat* prs = PlaylistParser::instance()->findByPath(s);
            if (prs)
            {
                connect(m_http, SIGNAL(finished (QNetworkReply *)), SLOT(readResponse(QNetworkReply *)));
                QNetworkRequest request;
                request.setUrl(QUrl(s));
                request.setRawHeader("User-Agent", QString("qmmp/%1").arg(Qmmp::strVersion()).toAscii());
                addButton->setEnabled(false);
                m_http->get(request);
                return;
            }
        }
        m_model->add(s);
    }
    QDialog::accept();
}

void AddUrlDialog::readResponse(QNetworkReply *reply)
{
    addButton->setEnabled(true);
    disconnect(m_http, SIGNAL(finished (QNetworkReply *)), 0, 0);
    if (reply->error() != QNetworkReply::NoError)
        QMessageBox::critical (this, tr("Error"), reply->errorString ());
    else if (!urlComboBox->currentText().isEmpty())
    {
        QString s = urlComboBox->currentText();
        PlaylistFormat* prs = PlaylistParser::instance()->findByPath(s);
        if (prs)
        {
            m_model->add(prs->decode(reply->readAll()));
            QDialog::accept();
        }
    }
    reply->deleteLater();
}

void AddUrlDialog::setModel( PlayListModel *m )
{
    m_model = m;
}
