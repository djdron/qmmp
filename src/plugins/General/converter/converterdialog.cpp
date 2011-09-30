/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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

#include <qmmpui/playlistitem.h>
#include <qmmpui/metadataformatter.h>
#include "converterdialog.h"

ConverterDialog::ConverterDialog(QList <PlayListItem *> items,  QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);

    MetaDataFormatter formatter("%p%if(%p&%t, - ,)%t - %l");

    foreach(PlayListItem *item , items)
    {
        QString text = formatter.parse(item);
        QListWidgetItem *listItem = new QListWidgetItem(text);
        listItem->setData(Qt::UserRole, item->url());
        listItem->setCheckState(Qt::Checked);
        ui.itemsListWidget->addItem(listItem);
    }
}

QStringList ConverterDialog::selectedUrls() const
{
    QStringList out;
    for(int i = 0; i < ui.itemsListWidget->count(); i++)
    {
        if(ui.itemsListWidget->item(i)->checkState() == Qt::Checked)
            out << ui.itemsListWidget->item(i)->data(Qt::UserRole).toString();
    }
    return out;
}
