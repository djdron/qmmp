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

#include <QKeyEvent>
#include "shortcutdialog.h"

ShortcutDialog::ShortcutDialog(const QString &key, QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    ui.keyLineEdit->setText(key);
}

ShortcutDialog::~ShortcutDialog()
{
}

void ShortcutDialog::keyPressEvent (QKeyEvent *event)
{
    int key = event->key();
    switch (key)
    {
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Meta:
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
    case Qt::Key_Super_L:
    case Qt::Key_Super_R:
    case Qt::Key_Menu:
    case 0:
    case Qt::Key_unknown:
        key = 0;
        ui.keyLineEdit->clear();
        QWidget::keyPressEvent(event);
        return;
    }
    QKeySequence seq(event->modifiers() + event->key());
    ui.keyLineEdit->setText(seq.toString());
    QWidget::keyPressEvent(event);
}

const QString ShortcutDialog::key()
{
    return ui.keyLineEdit->text();
}
