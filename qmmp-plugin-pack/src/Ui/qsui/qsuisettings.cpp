/***************************************************************************
 *   Copyright (C) 2011-2013 by Ilya Kotov                                 *
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

#include <QSettings>
#include <QDir>
#include <QFontDialog>
#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/uihelper.h>
#include "actionmanager.h"
#include "shortcutitem.h"
#include "shortcutdialog.h"
#include "popupsettings.h"
#include "qsuisettings.h"

QSUISettings::QSUISettings(QWidget *parent) : QWidget(parent)
{
    m_ui.setupUi(this);
    //setup icons
    m_ui.popupTemplateButton->setIcon(QIcon::fromTheme("configure"));
    //load settings
    loadShortcuts();
    readSettings();
    loadFonts();
}

QSUISettings::~QSUISettings()
{}

void QSUISettings::on_plFontButton_clicked()
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

void QSUISettings::showEvent(QShowEvent *)
{
    m_ui.hiddenCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
    m_ui.hideOnCloseCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
}

void QSUISettings::loadFonts()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString fontname = settings.value ("Simple/pl_font").toString();
    QFont font = QApplication::font();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    m_ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    m_ui.plFontLabel->setFont(font);
}

void QSUISettings::on_popupTemplateButton_clicked()
{
    PopupSettings *p = new PopupSettings(this);
    p->exec();
    p->deleteLater();
}

void QSUISettings::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    //playlist
    m_ui.protocolCheckBox->setChecked(settings.value("pl_show_protocol", false).toBool());
    m_ui.numbersCheckBox->setChecked(settings.value("pl_show_numbers", true).toBool());
    m_ui.alignCheckBox->setChecked(settings.value("pl_align_numbers", false).toBool());
    m_ui.anchorCheckBox->setChecked(settings.value("pl_show_anchor", false).toBool());
    m_ui.popupCheckBox->setChecked(settings.value("pl_show_popup", false).toBool());
    //tabs
    m_ui.tabsClosableCheckBox->setChecked(settings.value("pl_tabs_closable", false).toBool());
    m_ui.showNewPLCheckBox->setChecked(settings.value("pl_show_new_pl_button", false).toBool());
    m_ui.showTabListMenuCheckBox->setChecked(settings.value("pl_show_tab_list_menu", false).toBool());
    //view
    m_ui.hiddenCheckBox->setChecked(settings.value("start_hidden", false).toBool());
    m_ui.hideOnCloseCheckBox->setChecked(settings.value("hide_on_close", false).toBool());
    //analyzer colors
    m_ui.aColor1->setColor(settings.value("vis_color1", "#BECBFF").toString());
    m_ui.aColor2->setColor(settings.value("vis_color2", "#BECBFF").toString());
    m_ui.aColor3->setColor(settings.value("vis_color3", "#BECBFF").toString());
    m_ui.peaksColor->setColor(settings.value("vis_peak_color", "#DDDDDD").toString());
    m_ui.bgColor->setColor(settings.value("vis_bg_color", "Black").toString());
    //playlist colors
    QString normal_bg = palette().color(QPalette::Base).name();
    QString alternate = palette().color(QPalette::AlternateBase).name();
    QString selected_bg = palette().color(QPalette::Highlight).name();
    QString normal = palette().color(QPalette::Text).name();
    QString current = palette().color(QPalette::Text).name();
    QString highlighted = palette().color(QPalette::HighlightedText).name();
    m_ui.plSystemColorsCheckBox->setChecked(settings.value("pl_system_colors", true).toBool());
    m_ui.plBg1Color->setColor(settings.value("pl_bg1_color", normal_bg).toString());
    m_ui.plBg2Color->setColor(settings.value("pl_bg2_color", alternate).toString());
    m_ui.plHlColor->setColor(settings.value("pl_highlight_color", selected_bg).toString());
    m_ui.plTextNormalColor->setColor(settings.value("pl_normal_text_color", normal).toString());
    m_ui.plTextCurrentColor->setColor(settings.value("pl_current_text_color", current).toString());
    m_ui.plTextHlCurrentColor->setColor(settings.value("pl_hl_text_color", highlighted).toString());
    settings.endGroup();
}

void QSUISettings::writeSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");

    settings.setValue("pl_show_protocol", m_ui.protocolCheckBox->isChecked());
    settings.setValue("pl_show_numbers", m_ui.numbersCheckBox->isChecked());
    settings.setValue("pl_align_numbers", m_ui.alignCheckBox->isChecked());
    settings.setValue("pl_show_anchor", m_ui.anchorCheckBox->isChecked());
    settings.setValue("pl_show_popup", m_ui.popupCheckBox->isChecked());
    settings.setValue("pl_tabs_closable", m_ui.tabsClosableCheckBox->isChecked());
    settings.setValue("pl_show_new_pl_button", m_ui.showNewPLCheckBox->isChecked());
    settings.setValue("pl_show_tab_list_menu", m_ui.showTabListMenuCheckBox->isChecked());
    settings.setValue("start_hidden", m_ui.hiddenCheckBox->isChecked());
    settings.setValue("hide_on_close", m_ui.hideOnCloseCheckBox->isChecked());
    settings.setValue("vis_color1", m_ui.aColor1->colorName());
    settings.setValue("vis_color2", m_ui.aColor2->colorName());
    settings.setValue("vis_color3", m_ui.aColor3->colorName());
    settings.setValue("vis_peak_color", m_ui.peaksColor->colorName());
    settings.setValue("vis_bg_color", m_ui.bgColor->colorName());
    settings.setValue("pl_system_colors", m_ui.plSystemColorsCheckBox->isChecked());
    settings.setValue("pl_bg1_color", m_ui.plBg1Color->colorName());
    settings.setValue("pl_bg2_color", m_ui.plBg2Color->colorName());
    settings.setValue("pl_highlight_color", m_ui.plHlColor->colorName());
    settings.setValue("pl_normal_text_color", m_ui.plTextNormalColor->colorName());
    settings.setValue("pl_current_text_color", m_ui.plTextCurrentColor->colorName());
    settings.setValue("pl_hl_text_color", m_ui.plTextHlCurrentColor->colorName());
    settings.endGroup();
}

void QSUISettings::loadShortcuts()
{
    //playback
    QTreeWidgetItem *item = new QTreeWidgetItem (m_ui.shortcutTreeWidget, QStringList() << tr("Playback"));
    for(int i = ActionManager::PLAY; i <= ActionManager::CLEAR_QUEUE; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    m_ui.shortcutTreeWidget->addTopLevelItem(item);
    //view
    item = new QTreeWidgetItem (m_ui.shortcutTreeWidget, QStringList() << tr("View"));
    for(int i = ActionManager::WM_ALLWAYS_ON_TOP; i <= ActionManager::UI_BLOCK_TOOLBARS; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    m_ui.shortcutTreeWidget->addTopLevelItem(item);
    //volume
    item = new QTreeWidgetItem (m_ui.shortcutTreeWidget, QStringList() << tr("Volume"));
    for(int i = ActionManager::VOL_ENC; i <= ActionManager::VOL_MUTE; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    m_ui.shortcutTreeWidget->addTopLevelItem(item);
    //playlist
    item = new QTreeWidgetItem (m_ui.shortcutTreeWidget, QStringList() << tr("Playlist"));
    for(int i = ActionManager::PL_ADD_FILE; i <= ActionManager::PL_GROUP_TRACKS; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    m_ui.shortcutTreeWidget->addTopLevelItem(item);
    //misc
    item = new QTreeWidgetItem (m_ui.shortcutTreeWidget, QStringList() << tr("Misc"));
    for(int i = ActionManager::EQUALIZER; i <= ActionManager::QUIT; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    m_ui.shortcutTreeWidget->addTopLevelItem(item);

    m_ui.shortcutTreeWidget->resizeColumnToContents(0);
    m_ui.shortcutTreeWidget->resizeColumnToContents(1);
}

void QSUISettings::on_changeShortcutButton_clicked()
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
