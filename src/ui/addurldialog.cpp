/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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

#include "addurldialog.h"

#include "playlistmodel.h"

AddUrlDialog::AddUrlDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
}

AddUrlDialog::~AddUrlDialog()
{}



QPointer<AddUrlDialog> AddUrlDialog::instance = 0;

void AddUrlDialog::popup(QWidget* parent,PlayListModel* model )
{
    if(!instance) 
    {
        instance = new AddUrlDialog(parent);
        instance->setModel(model);
    }
    
    instance->show();
    instance->raise();
}

void AddUrlDialog::accept( )
{
    if(!urlComboBox->currentText().isEmpty())
    {
        QString s = urlComboBox->currentText();
        if(!s.startsWith("http://"))
            s.prepend("http://");
        
        m_model->addFile(s);
    }
    
    QDialog::accept();
}

void AddUrlDialog::setModel( PlayListModel *m )
{
    m_model = m;
}
