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
#include "skinreader.h"
#include "skin.h"
#include "popupsettings.h"
#include "skinnedsettings.h"

SkinnedSettings::SkinnedSettings(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    ui.listWidget->setIconSize (QSize (105,34));
    m_skin = Skin::instance();
    m_reader = new SkinReader(this);
    connect(ui.skinReloadButton, SIGNAL (clicked()), SLOT(loadSkins()));
    readSettings();
    loadSkins();
    loadFonts();
}

SkinnedSettings::~SkinnedSettings()
{}

void SkinnedSettings::on_listWidget_itemClicked(QListWidgetItem *)
{
    int row = ui.listWidget->currentRow();
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
    if(ui.listWidget->currentItem())
        m_currentSkinName = ui.listWidget->currentItem()->text();
    else
        m_currentSkinName.clear();
}

void SkinnedSettings::on_plFontButton_clicked()
{
    bool ok;
    QFont font = ui.plFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        ui.plFontLabel->setFont(font);
        QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
        settings.setValue ("Skinned/pl_font", font.toString());
    }
}

void SkinnedSettings::on_mainFontButton_clicked()
{
    bool ok;
    QFont font = ui.mainFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        ui.mainFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        ui.mainFontLabel->setFont(font);
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

void SkinnedSettings::loadFonts()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString fontname = settings.value ("Skinned/pl_font").toString();
    QFont font = QApplication::font();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.plFontLabel->setFont(font);

    font = QApplication::font ();
    fontname = settings.value ("Skinned/mw_font").toString();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    ui.mainFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.mainFontLabel->setFont(font);
    ui.useBitmapCheckBox->setChecked(settings.value("Skinned/bitmap_font", false).toBool());
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
            ui.listWidget->addItem (item);
            m_skinList << fileInfo;
        }
    }
}

void SkinnedSettings::loadSkins()
{
    m_reader->generateThumbs();
    m_skinList.clear();
    ui.listWidget->clear();
    QFileInfo fileInfo (":/default");
    QPixmap preview = Skin::getPixmap ("main", QDir (fileInfo.filePath()));
    QListWidgetItem *item = new QListWidgetItem (fileInfo.fileName ());
    item->setIcon (preview);
    ui.listWidget->addItem (item);
    m_skinList << fileInfo;
    if(item->text() == m_currentSkinName)
        ui.listWidget->setCurrentItem(item);

    findSkins(QDir::homePath() +"/.qmmp/skins");
#ifdef Q_OS_WIN32
    findSkins(qApp->applicationDirPath()+"/skins");
#else
    findSkins(qApp->applicationDirPath()+"/../share/qmmp/skins");
#endif
    foreach(QString path, m_reader->skins())
    {
        QListWidgetItem *item = new QListWidgetItem (path.section('/', -1));
        item->setIcon (m_reader->getPreview(path));
        item->setToolTip(tr("Archived skin") + " " + path);
        ui.listWidget->addItem (item);
        m_skinList << QFileInfo(path);
        if(item->text() == m_currentSkinName)
            ui.listWidget->setCurrentItem(item);
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
    ui.protocolCheckBox->setChecked(settings.value ("pl_show_protocol", false).toBool());
    ui.numbersCheckBox->setChecked(settings.value ("pl_show_numbers", true).toBool());
    ui.alignCheckBox->setChecked(settings.value ("pl_align_numbers", false).toBool());
    ui.anchorCheckBox->setChecked(settings.value("pl_show_anchor", false).toBool());
    ui.playlistsCheckBox->setChecked(settings.value("pl_show_plalists", false).toBool());
    ui.popupCheckBox->setChecked(settings.value("pl_show_popup", false).toBool());
    //transparency
    ui.mwTransparencySlider->setValue(100 - settings.value("mw_opacity", 1.0).toDouble()*100);
    ui.eqTransparencySlider->setValue(100 - settings.value("eq_opacity", 1.0).toDouble()*100);
    ui.plTransparencySlider->setValue(100 - settings.value("pl_opacity", 1.0).toDouble()*100);
    //view
    ui.skinCursorsCheckBox->setChecked(settings.value("skin_cursors", false).toBool());
    m_currentSkinName = settings.value("skin_name", "default").toString();
    ui.hiddenCheckBox->setChecked(settings.value("start_hidden", false).toBool());
    ui.hideOnCloseCheckBox->setChecked(settings.value("hide_on_close", false).toBool());
    settings.endGroup();
}

void SkinnedSettings::writeSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    settings.setValue ("pl_show_protocol", ui.protocolCheckBox->isChecked());
    settings.setValue ("pl_show_numbers", ui.numbersCheckBox->isChecked());
    settings.setValue ("pl_align_numbers", ui.alignCheckBox->isChecked());
    settings.setValue ("pl_show_anchor", ui.anchorCheckBox->isChecked());
    settings.setValue ("pl_show_plalists", ui.playlistsCheckBox->isChecked());
    settings.setValue ("pl_show_popup", ui.popupCheckBox->isChecked());
    settings.setValue ("mw_opacity", 1.0 -  (double)ui.mwTransparencySlider->value()/100);
    settings.setValue ("eq_opacity", 1.0 -  (double)ui.eqTransparencySlider->value()/100);
    settings.setValue ("pl_opacity", 1.0 -  (double)ui.plTransparencySlider->value()/100);
    settings.setValue ("bitmap_font", ui.useBitmapCheckBox->isChecked());
    settings.setValue ("skin_cursors", ui.skinCursorsCheckBox->isChecked());
    settings.setValue ("skin_name", m_currentSkinName);
    settings.setValue ("start_hidden", ui.hiddenCheckBox->isChecked());
    settings.setValue ("hide_on_close", ui.hideOnCloseCheckBox->isChecked());
    settings.endGroup();
}
