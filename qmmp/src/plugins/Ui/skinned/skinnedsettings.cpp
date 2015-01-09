/***************************************************************************
 *   Copyright (C) 2011-2014 by Ilya Kotov                                 *
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
#include "skinreader.h"
#include "skin.h"
#include "popupsettings.h"
#include "skinnedsettings.h"

SkinnedSettings::SkinnedSettings(QWidget *parent) : QWidget(parent)
{
    m_ui.setupUi(this);
    m_ui.listWidget->setIconSize (QSize (105,34));
    m_skin = Skin::instance();
    m_reader = new SkinReader(this);
    connect(m_ui.skinReloadButton, SIGNAL (clicked()), SLOT(loadSkins()));
    readSettings();
    loadSkins();
    loadFonts();
    //setup icons
    m_ui.skinInstallButton->setIcon(QIcon::fromTheme("list-add"));
    m_ui.skinReloadButton->setIcon(QIcon::fromTheme("view-refresh"));
    m_ui.popupTemplateButton->setIcon(QIcon::fromTheme("configure"));
}

SkinnedSettings::~SkinnedSettings()
{}

void SkinnedSettings::on_listWidget_itemClicked(QListWidgetItem *)
{
    int row = m_ui.listWidget->currentRow();
    QString path;
    if (m_skinList.at (row).isDir())
    {
        path = m_skinList.at (row).canonicalFilePath();
        m_skin->setSkin (path);
    }
    else if (m_skinList.at (row).isFile())
    {
        m_reader->unpackSkin(m_skinList.at (row).canonicalFilePath());
        m_skin->setSkin(QDir::homePath() +"/.qmmp/cache/skin");
    }
    if(m_ui.listWidget->currentItem())
        m_currentSkinName = m_ui.listWidget->currentItem()->text();
    else
        m_currentSkinName.clear();
}

void SkinnedSettings::on_plFontButton_clicked()
{
    bool ok;
    QFont font = m_ui.plFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        m_ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        m_ui.plFontLabel->setFont(font);
        QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
        settings.setValue ("Skinned/pl_font", font.toString());
    }
}

void SkinnedSettings::on_mainFontButton_clicked()
{
    bool ok;
    QFont font = m_ui.mainFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        m_ui.mainFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        m_ui.mainFontLabel->setFont(font);
        QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
        settings.setValue ("Skinned/mw_font", font.toString());
    }
}

void SkinnedSettings::on_skinInstallButton_clicked()
{
    QStringList files = FileDialog::getOpenFileNames(this,tr("Select Skin Files"), QDir::homePath(),
                                                     tr("Skin files") + " (*.tar.gz *.tgz *.tar.bz2 *.zip *.wsz)");
    foreach(QString path, files)
    {
        QFile file(path);
        file.copy(QDir::homePath() +"/.qmmp/skins/" + QFileInfo(path).fileName());
    }
    loadSkins();
}

void SkinnedSettings::showEvent(QShowEvent *)
{
    m_ui.hiddenCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
    m_ui.hideOnCloseCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
}

void SkinnedSettings::loadFonts()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString fontname = settings.value ("Skinned/pl_font").toString();
    QFont font = QApplication::font();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    m_ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    m_ui.plFontLabel->setFont(font);

    font = QApplication::font ();
    fontname = settings.value ("Skinned/mw_font").toString();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    m_ui.mainFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    m_ui.mainFontLabel->setFont(font);
    m_ui.useBitmapCheckBox->setChecked(settings.value("Skinned/bitmap_font", false).toBool());
}

void SkinnedSettings::findSkins(const QString &path)
{
    QDir dir(path);
    dir.setFilter (QDir::Dirs | QDir::NoDotAndDotDot);
    QList <QFileInfo> fileList = dir.entryInfoList();
    if (fileList.count() == 0)
        return;
    foreach (QFileInfo fileInfo, fileList)
    {
        QPixmap preview = Skin::getPixmap ("main", QDir(fileInfo.filePath ()));
        if (!preview.isNull())
        {
            QListWidgetItem *item = new QListWidgetItem (fileInfo.fileName ());
            item->setIcon (preview);
            item->setToolTip(tr("Unarchived skin") + " " + fileInfo.filePath ());
            m_ui.listWidget->addItem (item);
            m_skinList << fileInfo;
        }
    }
}

void SkinnedSettings::loadSkins()
{
    m_reader->generateThumbs();
    m_skinList.clear();
    m_ui.listWidget->clear();
    QFileInfo fileInfo (":/default");
    QPixmap preview = Skin::getPixmap ("main", QDir (fileInfo.filePath()));
    QListWidgetItem *item = new QListWidgetItem (fileInfo.fileName ());
    item->setIcon (preview);
    m_ui.listWidget->addItem (item);
    m_skinList << fileInfo;

    findSkins(QDir::homePath() +"/.qmmp/skins");
#if defined(Q_OS_WIN) && !defined(Q_OS_CYGWIN)
    findSkins(qApp->applicationDirPath()+"/skins");
#else
    findSkins(qApp->applicationDirPath()+"/../share/qmmp/skins");
#endif
    foreach(QString path, m_reader->skins())
    {
        QListWidgetItem *item = new QListWidgetItem (path.section('/', -1));
        item->setIcon (m_reader->getPreview(path));
        item->setToolTip(tr("Archived skin") + " " + path);
        m_ui.listWidget->addItem (item);
        m_skinList << QFileInfo(path);
    }
    for(int i = 0; i < m_ui.listWidget->count(); ++i)
    {
        if(m_ui.listWidget->item(i)->text() == m_currentSkinName)
        {
            m_ui.listWidget->setCurrentRow(i, QItemSelectionModel::Select);
            break;
        }
    }
}

void SkinnedSettings::on_popupTemplateButton_clicked()
{
    PopupSettings *p = new PopupSettings(this);
    p->exec();
    p->deleteLater();
}

void SkinnedSettings::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    //playlist
    m_ui.protocolCheckBox->setChecked(settings.value ("pl_show_protocol", false).toBool());
    m_ui.numbersCheckBox->setChecked(settings.value ("pl_show_numbers", true).toBool());
    m_ui.alignCheckBox->setChecked(settings.value ("pl_align_numbers", false).toBool());
    m_ui.anchorCheckBox->setChecked(settings.value("pl_show_anchor", false).toBool());
    m_ui.playlistsCheckBox->setChecked(settings.value("pl_show_plalists", false).toBool());
    m_ui.popupCheckBox->setChecked(settings.value("pl_show_popup", false).toBool());
    m_ui.plSeplineEdit->setText(settings.value("pl_separator", "|").toString());
    m_ui.showNewPLCheckBox->setChecked(settings.value("pl_show_create_button", false).toBool());
    //transparency
    m_ui.mwTransparencySlider->setValue(100 - settings.value("mw_opacity", 1.0).toDouble()*100);
    m_ui.eqTransparencySlider->setValue(100 - settings.value("eq_opacity", 1.0).toDouble()*100);
    m_ui.plTransparencySlider->setValue(100 - settings.value("pl_opacity", 1.0).toDouble()*100);
    //view
    m_ui.skinCursorsCheckBox->setChecked(settings.value("skin_cursors", false).toBool());
    m_currentSkinName = settings.value("skin_name", "default").toString();
    m_ui.hiddenCheckBox->setChecked(settings.value("start_hidden", false).toBool());
    m_ui.hideOnCloseCheckBox->setChecked(settings.value("hide_on_close", false).toBool());
    settings.endGroup();
}

void SkinnedSettings::writeSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    settings.setValue ("pl_show_protocol", m_ui.protocolCheckBox->isChecked());
    settings.setValue ("pl_show_numbers", m_ui.numbersCheckBox->isChecked());
    settings.setValue ("pl_align_numbers", m_ui.alignCheckBox->isChecked());
    settings.setValue ("pl_show_anchor", m_ui.anchorCheckBox->isChecked());
    settings.setValue ("pl_show_plalists", m_ui.playlistsCheckBox->isChecked());
    settings.setValue ("pl_show_popup", m_ui.popupCheckBox->isChecked());
    settings.setValue ("pl_separator", m_ui.plSeplineEdit->text());
    settings.setValue ("pl_show_create_button", m_ui.showNewPLCheckBox->isChecked());
    settings.setValue ("mw_opacity", 1.0 -  (double)m_ui.mwTransparencySlider->value()/100);
    settings.setValue ("eq_opacity", 1.0 -  (double)m_ui.eqTransparencySlider->value()/100);
    settings.setValue ("pl_opacity", 1.0 -  (double)m_ui.plTransparencySlider->value()/100);
    settings.setValue ("bitmap_font", m_ui.useBitmapCheckBox->isChecked());
    settings.setValue ("skin_cursors", m_ui.skinCursorsCheckBox->isChecked());
    settings.setValue ("skin_name", m_currentSkinName);
    settings.setValue ("start_hidden", m_ui.hiddenCheckBox->isChecked());
    settings.setValue ("hide_on_close", m_ui.hideOnCloseCheckBox->isChecked());
    settings.endGroup();
}
