/***************************************************************************
 *   Copyright (C) 2006-2012 by Ilya Kotov                                 *
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

#ifndef ADDURLDIALOG_H
#define ADDURLDIALOG_H

#include "ui_addurldialog.h"
#include <QDialog>
#include <QPointer>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;
class PlayListModel;

/**
   @author Vladimir Kuznetsov <vovanec@gmail.com>
 */

class AddUrlDialog : public QDialog , private Ui::AddUrlDialog
{
    Q_OBJECT
public:
    static void popup(QWidget* parent ,PlayListModel*);

protected:
    AddUrlDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~AddUrlDialog();

protected slots:
    virtual void accept();

private slots:
    void readResponse(QNetworkReply *reply);

private:
    void setModel(PlayListModel*);
    static QPointer<AddUrlDialog> instance;
    PlayListModel* m_model;
    QStringList m_history;
    QNetworkAccessManager *m_http;
    QUrl m_redirect_url;

};
#endif //ADDURLDIALOG_H
