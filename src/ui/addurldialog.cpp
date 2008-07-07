/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
#include <QHttp>
#include <QUrl>
#include <QMessageBox>
#include "addurldialog.h"
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include "playlistmodel.h"

#define HISTORY_SIZE 10

AddUrlDialog::AddUrlDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    m_history = settings.value("URLDialog/history").toStringList();
    urlComboBox->addItems(m_history);
    m_http = new QHttp(this);
    connect(m_http, SIGNAL(requestFinished (int, bool)), SLOT(processResponse(int, bool)));
    connect(m_http, SIGNAL(readyRead (const QHttpResponseHeader&)),
            SLOT(readResponse(const QHttpResponseHeader&)));

    //use global proxy settings
    if (settings.value ("Proxy/use_proxy", FALSE).toBool())
    {

        if (settings.value ("Proxy/authentication", FALSE).toBool())
            m_http->setProxy(settings.value("Proxy/host").toString(),
                             settings.value("Proxy/port").toInt(),
                             settings.value("Proxy/user").toString(),
                             settings.value("Proxy/passw").toString());
        else
            m_http->setProxy(settings.value("Proxy/host").toString(),
                             settings.value("Proxy/port").toInt());
    }
}

AddUrlDialog::~AddUrlDialog()
{
    if ( m_history.size() > HISTORY_SIZE)
        m_history.removeLast();
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.setValue("URLDialog/history", m_history);
    m_http->close();
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
        QString s = urlComboBox->currentText();
        if (!s.startsWith("http://"))
            s.prepend("http://");
        m_history.removeAll(s);
        m_history.prepend(s);
        PlaylistFormat* prs = PlaylistParser::instance()->findByPath(s);
        if (prs)
        {
            //download playlist;
            QUrl url(s);
            m_http->setHost(url.host(), url.port(80));
            m_http->get(url.path());
            addButton->setEnabled(FALSE);
            return;
        }
        m_model->addFile(s);
    }
    QDialog::accept();
}

void AddUrlDialog::processResponse(int, bool error)
{
    if (error)
    {
        QMessageBox::critical (this, tr("Error"), m_http->errorString ());
        QDialog::accept();
    }
}

void AddUrlDialog::readResponse(const QHttpResponseHeader&)
{
    QString s = urlComboBox->currentText();
    PlaylistFormat* prs = PlaylistParser::instance()->findByPath(s);
    if (prs)
    {
        m_model->addFiles(prs->decode(m_http->readAll()));
        return;
    }
    QDialog::accept();
}

void AddUrlDialog::setModel( PlayListModel *m )
{
    m_model = m;
}
