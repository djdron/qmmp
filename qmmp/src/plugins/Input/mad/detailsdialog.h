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
#ifndef DETAILSDIALOG_H
#define DETAILSDIALOG_H

#include <QDialog>

#include "ui_detailsdialog.h"

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class QTextCodec;

class DetailsDialog : public QDialog
{
    Q_OBJECT
public:
    DetailsDialog(QWidget *parent = 0, const QString &path = 0);

    ~DetailsDialog();

protected:
    virtual void closeEvent (QCloseEvent *);

private slots:
    void save();
    void create();
    void deleteTag();
    void loadTag();

private:
    void loadMPEGInfo();
    uint selectedTag();
    Ui::DetailsDialog ui;
    QString m_path;
    QTextCodec *m_codec_v1;
    QTextCodec *m_codec_v2;
    bool m_rw;

};

#endif
