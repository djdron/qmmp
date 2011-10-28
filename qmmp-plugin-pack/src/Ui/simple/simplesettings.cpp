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

#include <QSettings>
#include <QDir>
#include <QFontDialog>
#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/uihelper.h>
#include "actionmanager.h"
#include "shortcutitem.h"
#include "shortcutdialog.h"
#include "simplesettings.h"

SimpleSettings::SimpleSettings(QWidget *parent) : QWidget(parent)
{
    m_ui.setupUi(this);
    //setup icons
    m_ui.popupTemplateButton->setIcon(QIcon::fromTheme("configure"));
    //load settings
    loadShortcuts();
    readSettings();
    loadFonts();
}

SimpleSettings::~SimpleSettings()
{}

void SimpleSettings::on_plFontButton_clicked()
{
    bool ok;
    QFont font = m_ui.plFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        m_ui.plFontLabel->setText(font.family () + " " + QString::number(font.pointSize ()));
        m_ui.plFontLabel->setFont(font);
        QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
        settings.setValue("Simple/pl_font", font.toString());
    }
}

void SimpleSettings::showEvent(QShowEvent *)
{
    m_ui.hiddenCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
    m_ui.hideOnCloseCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
}

void SimpleSettings::loadFonts()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString fontname = settings.value ("Simple/pl_font").toString();
    QFont font = QApplication::font();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    m_ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    m_ui.plFontLabel->setFont(font);
}

void SimpleSettings::on_popupTemplateButton_clicked()
{
    /*PopupSettings *p = new PopupSettings(this);
    p->exec();
    p->deleteLater();*/
}

void SimpleSettings::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    //playlist
    m_ui.tabsClosableCheckBox->setChecked(settings.value("pl_tabs_closable", false).toBool());
    m_ui.protocolCheckBox->setChecked(settings.value("pl_show_protocol", false).toBool());
    m_ui.numbersCheckBox->setChecked(settings.value("pl_show_numbers", true).toBool());
    m_ui.alignCheckBox->setChecked(settings.value("pl_align_numbers", false).toBool());
    m_ui.anchorCheckBox->setChecked(settings.value("pl_show_anchor", false).toBool());
    m_ui.popupCheckBox->setChecked(settings.value("pl_show_popup", false).toBool());
    //view
    m_ui.hiddenCheckBox->setChecked(settings.value("start_hidden", false).toBool());
    m_ui.hideOnCloseCheckBox->setChecked(settings.value("hide_on_close", false).toBool());
    settings.endGroup();
}

void SimpleSettings::writeSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    settings.setValue ("pl_tabs_closable", m_ui.tabsClosableCheckBox->isChecked());
    settings.setValue ("pl_show_protocol", m_ui.protocolCheckBox->isChecked());
    settings.setValue ("pl_show_numbers", m_ui.numbersCheckBox->isChecked());
    settings.setValue ("pl_align_numbers", m_ui.alignCheckBox->isChecked());
    settings.setValue ("pl_show_anchor", m_ui.anchorCheckBox->isChecked());
    settings.setValue ("pl_show_popup", m_ui.popupCheckBox->isChecked());
    settings.setValue ("start_hidden", m_ui.hiddenCheckBox->isChecked());
    settings.setValue ("hide_on_close", m_ui.hideOnCloseCheckBox->isChecked());
    settings.endGroup();
}

void SimpleSettings::loadShortcuts()
{
    //playback
    QTreeWidgetItem *item = new QTreeWidgetItem (m_ui.shortcutTreeWidget, QStringList() << tr("Playback"));
    for(int i = ActionManager::PLAY; i <= ActionManager::CLEAR_QUEUE; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    m_ui.shortcutTreeWidget->addTopLevelItem(item);
    //view
    item = new QTreeWidgetItem (m_ui.shortcutTreeWidget, QStringList() << tr("View"));
    for(int i = ActionManager::WM_ALLWAYS_ON_TOP; i <= ActionManager::WM_STICKY; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    m_ui.shortcutTreeWidget->addTopLevelItem(item);
    //playlist
    item = new QTreeWidgetItem (m_ui.shortcutTreeWidget, QStringList() << tr("Playlist"));
    for(int i = ActionManager::PL_ADD_FILE; i <= ActionManager::PL_SELECT_PREVIOUS; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    m_ui.shortcutTreeWidget->addTopLevelItem(item);
    //misc
    item = new QTreeWidgetItem (m_ui.shortcutTreeWidget, QStringList() << tr("Misc"));
    for(int i = ActionManager::SETTINGS; i <= ActionManager::QUIT; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    m_ui.shortcutTreeWidget->addTopLevelItem(item);

    m_ui.shortcutTreeWidget->resizeColumnToContents(0);
    m_ui.shortcutTreeWidget->resizeColumnToContents(1);
}

void SimpleSettings::on_changeShortcutButton_clicked()
{
    ShortcutItem *item = dynamic_cast<ShortcutItem *> (m_ui.shortcutTreeWidget->currentItem());
    if(!item)
        return;
    ShortcutDialog editor(item->action()->shortcut().toString(), this);
    if(editor.exec() == QDialog::Accepted)
    {
        item->action()->setShortcut(editor.key());
        item->setText(1, item->action()->shortcut().toString());
    }
}
