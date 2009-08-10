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
#ifndef ABSTRACTDETAILSDIALOG_H
#define ABSTRACTDETAILSDIALOG_H

#include <QDialog>
#include <QMap>

#include "qmmp.h"

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

namespace Ui
{
    class AbstractDetailsDialog;
}
class QAbstractButton;

class AbstractDetailsDialog : public QDialog
{
    Q_OBJECT
public:
    AbstractDetailsDialog(QWidget *parent = 0);

    ~AbstractDetailsDialog();

    void hideSaveButton(bool hide = TRUE);
    void blockSaveButton(bool block = TRUE);

protected:
    virtual void writeTags();

    void setMetaData(Qmmp::MetaData key, const QString &value);
    void setMetaData(Qmmp::MetaData key, int value);
    void setAudioProperties(QMap <QString, QString> p);
    const QString strMetaData(Qmmp::MetaData key);
    int intMetaData(Qmmp::MetaData key);

private slots:
    void processButton(QAbstractButton *);

private:
    Ui::AbstractDetailsDialog *ui;
};

#endif
