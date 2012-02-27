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
#ifndef STREAMWINDOW_H
#define STREAMWINDOW_H

#include <QWidget>
#include "ui_streamwindow.h"

class QNetworkAccessManager;
class QNetworkReply;
class QStandardItemModel;
class QSortFilterProxyModel;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class StreamWindow : public QWidget
{
Q_OBJECT
public:
    StreamWindow(QWidget *parent = 0);

    ~StreamWindow();

private slots:
    void showText(QNetworkReply *reply);
    void on_updatePushButton_clicked();
    void on_addPushButton_clicked();
    void on_filterLineEdit_textChanged(const QString &text);

private:
    void closeEvent(QCloseEvent *);
    void readIceCast(QIODevice *input);
    Ui::StreamWindow ui;
    QNetworkAccessManager *m_http;
    QNetworkReply *m_requestReply;
    QString m_artist, m_title;
    QStandardItemModel *m_icecastModel;
    QSortFilterProxyModel *m_filterModel;
};

#endif
