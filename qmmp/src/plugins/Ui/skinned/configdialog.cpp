/***************************************************************************
 *   Copyright (C) 2007-2011 by Ilya Kotov                                 *
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
#include <QDir>
#include <QSettings>
#include <QFontDialog>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QCheckBox>
#include <QRadioButton>
#include <QMenu>
#include <QMessageBox>
#include <qmmp/decoder.h>
#include <qmmp/output.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/outputfactory.h>
#include <qmmp/visualfactory.h>
#include <qmmp/effectfactory.h>
#include <qmmp/effect.h>
#include <qmmp/soundcore.h>
#include <qmmp/enginefactory.h>
#include <qmmp/abstractengine.h>
#include <qmmp/qmmpsettings.h>
#include <qmmp/inputsource.h>
#include <qmmp/inputsourcefactory.h>
#include <qmmpui/generalfactory.h>
#include <qmmpui/general.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistmodel.h>
#include "shortcutdialog.h"
#include "actionmanager.h"
#include "shortcutitem.h"
#include "popupsettings.h"
#include "skin.h"
#include "pluginitem.h"
#include "configdialog.h"
#include "skinreader.h"

ConfigDialog::ConfigDialog (QWidget *parent)
    : QDialog (parent)
{
    ui.setupUi (this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, false);
    ui.preferencesButton->setEnabled(false);
    ui.informationButton->setEnabled(false);
    connect (ui.mainFontButton, SIGNAL (clicked()), SLOT (setMainFont()));
    connect (ui.plFontButton, SIGNAL (clicked()), SLOT (setPlFont()));
    connect (this, SIGNAL(rejected()),SLOT(saveSettings()));
    connect (ui.fileDialogComboBox, SIGNAL (currentIndexChanged (int)), SLOT(updateDialogButton(int)));
    connect (ui.skinInstallButton, SIGNAL (clicked()), SLOT(installSkin()));
    connect (ui.skinReloadButton, SIGNAL (clicked()), SLOT(loadSkins()));
    connect (ui.listWidget, SIGNAL (itemClicked (QListWidgetItem *)), this, SLOT (changeSkin()));
    ui.listWidget->setIconSize (QSize (105,34));
    m_skin = Skin::instance();
    ui.replayGainModeComboBox->addItem (tr("Track"), QmmpSettings::REPLAYGAIN_TRACK);
    ui.replayGainModeComboBox->addItem (tr("Album"), QmmpSettings::REPLAYGAIN_ALBUM);
    ui.replayGainModeComboBox->addItem (tr("Disabled"), QmmpSettings::REPLAYGAIN_DISABLED);
    readSettings();
    m_reader = new SkinReader(this);
    loadSkins();
    loadPluginsInfo();
    loadShortcuts();
    loadFonts();
    createMenus();
    //setup icons
    ui.skinInstallButton->setIcon(QIcon::fromTheme("list-add"));
    ui.skinReloadButton->setIcon(QIcon::fromTheme("view-refresh"));
    ui.popupTemplateButton->setIcon(QIcon::fromTheme("configure"));
    ui.preferencesButton->setIcon(QIcon::fromTheme("configure"));
    ui.informationButton->setIcon(QIcon::fromTheme("dialog-information"));
    ui.fdInformationButton->setIcon(QIcon::fromTheme("dialog-information"));
    ui.outputInformationButton->setIcon(QIcon::fromTheme("dialog-information"));
    ui.outputPreferencesButton->setIcon(QIcon::fromTheme("configure"));
}

ConfigDialog::~ConfigDialog()
{}

void ConfigDialog::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    if (MediaPlayer *player = MediaPlayer::instance())
    {
        ui.formatLineEdit->setText(player->playListManager()->format());
        ui.metadataCheckBox->setChecked(player->playListManager()->useMetadata());
        ui.underscoresCheckBox->setChecked(player->playListManager()->convertUnderscore());
        ui.per20CheckBox->setChecked(player->playListManager()->convertTwenty());
    }
    settings.beginGroup("Skinned");
    ui.protocolCheckBox->setChecked(settings.value ("pl_show_protocol", false).toBool());
    ui.numbersCheckBox->setChecked(settings.value ("pl_show_numbers", true).toBool());
    ui.alignCheckBox->setChecked(settings.value ("pl_align_numbers", false).toBool());
    ui.anchorCheckBox->setChecked(settings.value("pl_show_anchor", false).toBool());
    ui.playlistsCheckBox->setChecked(settings.value("pl_show_plalists", false).toBool());
    ui.popupCheckBox->setChecked(settings.value("pl_show_popup", false).toBool());
    QmmpSettings *gs = QmmpSettings::instance();
    //proxy settings
    ui.enableProxyCheckBox->setChecked(gs->isProxyEnabled());
    ui.authProxyCheckBox->setChecked(gs->useProxyAuth());
    ui.hostLineEdit->setText(gs->proxy().host());
    if (gs->proxy().port(0))
        ui.portLineEdit->setText(QString::number(gs->proxy().port(0)));
    ui.proxyUserLineEdit->setText(gs->proxy().userName());
    ui.proxyPasswLineEdit->setText(gs->proxy().password());

    ui.hostLineEdit->setEnabled(ui.enableProxyCheckBox->isChecked());
    ui.portLineEdit->setEnabled(ui.enableProxyCheckBox->isChecked());
    ui.proxyUserLineEdit->setEnabled(ui.authProxyCheckBox->isChecked());
    ui.proxyPasswLineEdit->setEnabled(ui.authProxyCheckBox->isChecked());
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
    //resume playback
    ui.continuePlaybackCheckBox->setChecked(settings.value("General/resume_on_startup", false).toBool());
    //cover options
    ui.coverIncludeLineEdit->setText(gs->coverNameFilters(true).join(","));
    ui.coverExcludeLineEdit->setText(gs->coverNameFilters(false).join(","));
    ui.coverDepthSpinBox->setValue(gs->coverSearchDepth());
    ui.useCoverFilesCheckBox->setChecked(gs->useCoverFiles());
    //replay gain
    ui.clippingCheckBox->setChecked(gs->replayGainPreventClipping());
    ui.replayGainModeComboBox->setCurrentIndex(ui.replayGainModeComboBox->findData(gs->replayGainMode()));
    ui.preampDoubleSpinBox->setValue(gs->replayGainPreamp());
    ui.defaultGainDoubleSpinBox->setValue(gs->replayGainDefaultGain());
    //audio
    ui.softVolumeCheckBox->setChecked(gs->useSoftVolume());
    ui.use16BitCheckBox->setChecked(gs->use16BitOutput());
    ui.bufferSizeSpinBox->setValue(gs->bufferSize());
}

void ConfigDialog::on_contentsWidget_currentItemChanged (QListWidgetItem *current,
                                                         QListWidgetItem *previous)
{
    if (!current)
        current = previous;
    ui.stackedWidget->setCurrentIndex (ui.contentsWidget->row (current));
    ui.hiddenCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
    ui.hideOnCloseCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
}

void ConfigDialog::changeSkin()
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

void ConfigDialog::loadSkins()
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

void ConfigDialog::findSkins(const QString &path)
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

void ConfigDialog::loadPluginsInfo()
{
    ui.treeWidget->blockSignals(true);
    /*
        load transport plugin information
     */
    QTreeWidgetItem *item = new QTreeWidgetItem (ui.treeWidget, QStringList() << tr("Transports"));
    QList <InputSourceFactory *> *transports = InputSource::factories();
    QStringList files = InputSource::files();
    for (int i = 0; i < transports->count (); ++i)
        new PluginItem (item, transports->at(i), files.at (i));
    ui.treeWidget->addTopLevelItem(item);
    item->setExpanded(true);

    /*
        load input plugins information
    */
    item = new QTreeWidgetItem (ui.treeWidget, QStringList() << tr("Decoders"));
    QList <DecoderFactory *> *decoders = Decoder::factories();
    files = Decoder::files();
    for (int i = 0; i < decoders->count (); ++i)
        new PluginItem (item, decoders->at(i), files.at (i));
    ui.treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load audio engines information
    */
    item = new QTreeWidgetItem (ui.treeWidget, QStringList() << tr("Engines"));
    QList <EngineFactory *> *engines = AbstractEngine::factories();
    files = AbstractEngine::files();
    for (int i = 0; i < engines->count (); ++i)
        new PluginItem (item, engines->at(i), files.at (i));
    ui.treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load effect plugin information
    */
    item = new QTreeWidgetItem (ui.treeWidget, QStringList() << tr("Effects"));
    QList <EffectFactory *> *effects = Effect::factories();
    files = Effect::files();
    for (int i = 0; i < effects->count (); ++i)
        new PluginItem (item, effects->at(i), files.at (i));
    ui.treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load visual plugin information
    */
    item = new QTreeWidgetItem (ui.treeWidget, QStringList() << tr("Visualization"));
    QList <VisualFactory *> *visuals = Visual::factories();
    files = Visual::files();
    for (int i = 0; i < visuals->count (); ++i)
        new PluginItem (item, visuals->at(i), files.at (i));
    ui.treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load general plugin information
    */
    item = new QTreeWidgetItem (ui.treeWidget, QStringList() << tr("General"));
    QList <GeneralFactory *> *generals = General::factories();
    files = General::files();
    for (int i = 0; i < generals->count (); ++i)
        new PluginItem (item, generals->at(i), files.at (i));
    ui.treeWidget->addTopLevelItem(item);
    item->setExpanded(true);

    ui.treeWidget->blockSignals(false);
    ui.treeWidget->resizeColumnToContents(0);
    ui.treeWidget->resizeColumnToContents(1);
    /*
        load output plugins information
    */
    ui.outputInformationButton->setEnabled(false);
    ui.outputPreferencesButton->setEnabled(false);
    QList <OutputFactory *> *outputs = Output::factories();
    for (int i = 0; i < outputs->count (); ++i)
    {
        ui.outputComboBox->addItem(outputs->at(i)->properties().name);
        if(Output::currentFactory() == outputs->at(i))
        {
            ui.outputComboBox->setCurrentIndex(i);
            on_outputComboBox_activated (i);
        }
    }
    /*
        load file dialog information
    */
    foreach(FileDialogFactory *factory, FileDialog::registeredFactories())
    {
        ui.fileDialogComboBox->addItem(factory->properties().name);
        if (FileDialog::isEnabled(factory))
            ui.fileDialogComboBox->setCurrentIndex(ui.fileDialogComboBox->count()-1);
    }
}

void ConfigDialog::loadFonts()
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

void ConfigDialog::loadShortcuts()
{
    //playback
    QTreeWidgetItem *item = new QTreeWidgetItem (ui.shortcutTreeWidget, QStringList() << tr("Playback"));
    for(int i = ActionManager::PLAY; i <= ActionManager::CLEAR_QUEUE; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    ui.shortcutTreeWidget->addTopLevelItem(item);
    //view
    item = new QTreeWidgetItem (ui.shortcutTreeWidget, QStringList() << tr("View"));
    for(int i = ActionManager::SHOW_PLAYLIST; i <= ActionManager::WM_DOUBLE_SIZE; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    ui.shortcutTreeWidget->addTopLevelItem(item);
    //playlist
    item = new QTreeWidgetItem (ui.shortcutTreeWidget, QStringList() << tr("Playlist"));
    for(int i = ActionManager::PL_ADD_FILE; i <= ActionManager::PL_SHOW_MANAGER; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    ui.shortcutTreeWidget->addTopLevelItem(item);
    //misc
    item = new QTreeWidgetItem (ui.shortcutTreeWidget, QStringList() << tr("Misc"));
    for(int i = ActionManager::SETTINGS; i <= ActionManager::QUIT; ++i)
        new ShortcutItem(item, i);
    item->setExpanded(true);
    ui.shortcutTreeWidget->addTopLevelItem(item);

    ui.shortcutTreeWidget->resizeColumnToContents(0);
    ui.shortcutTreeWidget->resizeColumnToContents(1);
}

void ConfigDialog::setPlFont()
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

void ConfigDialog::setMainFont()
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

void ConfigDialog::on_preferencesButton_clicked()
{
    QTreeWidgetItem *item = ui.treeWidget->currentItem();
    if(item && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->showSettings(this);

}

void ConfigDialog::on_informationButton_clicked()
{
    QTreeWidgetItem *item = ui.treeWidget->currentItem();
    if(item && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->showAbout(this);
}

void ConfigDialog::createMenus()
{
    QMenu *menu = new QMenu(this);

    menu->addAction(tr("Artist"))->setData("%p");
    menu->addAction(tr("Album"))->setData("%a");
    menu->addAction(tr("Title"))->setData("%t");
    menu->addAction(tr("Track number"))->setData("%n");
    menu->addAction(tr("Two-digit track number"))->setData("%NN");
    menu->addAction(tr("Genre"))->setData("%g");
    menu->addAction(tr("Comment"))->setData("%c");
    menu->addAction(tr("Composer"))->setData("%C");
    menu->addAction(tr("Disc number"))->setData("%D");
    menu->addAction(tr("File name"))->setData("%f");
    menu->addAction(tr("File path"))->setData("%F");
    menu->addAction(tr("Year"))->setData("%y");
    menu->addAction(tr("Condition"))->setData("%if(%p&%t,%p - %t,%f)");

    ui.titleButton->setMenu(menu);
    ui.titleButton->setPopupMode(QToolButton::InstantPopup);
    connect(menu, SIGNAL(triggered (QAction *)), SLOT(addTitleString(QAction *)));
}

void ConfigDialog::addTitleString(QAction * a)
{
    if (ui.formatLineEdit->cursorPosition () < 1)
        ui.formatLineEdit->insert(a->data().toString());
    else
        ui.formatLineEdit->insert(" - "+a->data().toString());
}

void ConfigDialog::saveSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    if (MediaPlayer *player = MediaPlayer::instance())
    {
        player->playListManager()->setFormat(ui.formatLineEdit->text().trimmed());
        player->playListManager()->setUseMetadata(ui.metadataCheckBox->isChecked());
        player->playListManager()->setConvertUnderscore(ui.underscoresCheckBox->isChecked());
        player->playListManager()->setConvertTwenty(ui.per20CheckBox->isChecked());
    }
    settings.setValue ("pl_show_protocol", ui.protocolCheckBox->isChecked());
    settings.setValue ("pl_show_numbers", ui.numbersCheckBox->isChecked());
    settings.setValue ("pl_align_numbers", ui.alignCheckBox->isChecked());
    settings.setValue ("pl_show_anchor", ui.anchorCheckBox->isChecked());
    settings.setValue ("pl_show_plalists", ui.playlistsCheckBox->isChecked());
    settings.setValue ("pl_show_popup", ui.popupCheckBox->isChecked());
    FileDialog::setEnabled(FileDialog::registeredFactories().at(ui.fileDialogComboBox->currentIndex()));
    QmmpSettings *gs = QmmpSettings::instance();
    //proxy
    QUrl proxyUrl;
    proxyUrl.setHost(ui.hostLineEdit->text());
    proxyUrl.setPort(ui.portLineEdit->text().toUInt());
    proxyUrl.setUserName(ui.proxyUserLineEdit->text());
    proxyUrl.setPassword(ui.proxyPasswLineEdit->text());
    gs->setNetworkSettings(ui.enableProxyCheckBox->isChecked(),
                           ui.authProxyCheckBox->isChecked(),
                           proxyUrl);


    settings.setValue ("mw_opacity", 1.0 -  (double)ui.mwTransparencySlider->value()/100);
    settings.setValue ("eq_opacity", 1.0 -  (double)ui.eqTransparencySlider->value()/100);
    settings.setValue ("pl_opacity", 1.0 -  (double)ui.plTransparencySlider->value()/100);

    settings.setValue ("bitmap_font", ui.useBitmapCheckBox->isChecked());
    settings.setValue ("skin_cursors", ui.skinCursorsCheckBox->isChecked());
    settings.setValue ("skin_name", m_currentSkinName);
    settings.setValue ("start_hidden", ui.hiddenCheckBox->isChecked());
    settings.setValue ("hide_on_close", ui.hideOnCloseCheckBox->isChecked());
    settings.endGroup();
    settings.setValue ("General/resume_on_startup",  ui.continuePlaybackCheckBox->isChecked());
    gs->setCoverSettings(ui.coverIncludeLineEdit->text().split(","),
                         ui.coverExcludeLineEdit->text().split(","),
                         ui.coverDepthSpinBox->value(),
                         ui.useCoverFilesCheckBox->isChecked());
    int i = ui.replayGainModeComboBox->currentIndex();
    gs->setReplayGainSettings((QmmpSettings::ReplayGainMode)
                              ui.replayGainModeComboBox->itemData(i).toInt(),
                              ui.preampDoubleSpinBox->value(),
                              ui.defaultGainDoubleSpinBox->value(),
                              ui.clippingCheckBox->isChecked());
    gs->setAudioSettings(ui.softVolumeCheckBox->isChecked(), ui.use16BitCheckBox->isChecked());
    gs->setBufferSize(ui.bufferSizeSpinBox->value());
    QList <OutputFactory *> *outputs = Output::factories();
    if(ui.outputComboBox->currentIndex() >= 0 && outputs->count())
        Output::setCurrentFactory(outputs->at(ui.outputComboBox->currentIndex()));
}

void ConfigDialog::updateDialogButton(int index)
{
    ui.fdInformationButton->setEnabled(FileDialog::registeredFactories()[index]->properties().hasAbout);
}

void ConfigDialog::on_fdInformationButton_clicked()
{
    int index = ui.fileDialogComboBox->currentIndex ();
    FileDialog::registeredFactories()[index]->showAbout(this);
}

void ConfigDialog::installSkin()
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

void ConfigDialog::on_popupTemplateButton_clicked()
{
    PopupSettings *p = new PopupSettings(this);
    p->exec();
    p->deleteLater();
}

void ConfigDialog::on_treeWidget_itemChanged (QTreeWidgetItem *item, int column)
{
    if(column == 0 && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->setEnabled(item->checkState(0) == Qt::Checked);
}

void ConfigDialog::on_treeWidget_currentItemChanged (QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if(current->type() >= PluginItem::TRANSPORT)
    {
        ui.preferencesButton->setEnabled(dynamic_cast<PluginItem *>(current)->hasSettings());
        ui.informationButton->setEnabled(dynamic_cast<PluginItem *>(current)->hasAbout());
    }
    else
    {
        ui.preferencesButton->setEnabled(false);
        ui.informationButton->setEnabled(false);
    }
}

void ConfigDialog::on_outputComboBox_activated (int index)
{
    OutputFactory *factory = Output::factories()->at(index);
    ui.outputInformationButton->setEnabled(factory->properties().hasAbout);
    ui.outputPreferencesButton->setEnabled(factory->properties().hasSettings);
}

void ConfigDialog::on_outputPreferencesButton_clicked()
{
    int index = ui.outputComboBox->currentIndex();
    Output::factories()->at(index)->showSettings(this);
}

void ConfigDialog::on_outputInformationButton_clicked()
{
    int index = ui.outputComboBox->currentIndex();
    Output::factories()->at(index)->showAbout(this);
}

void ConfigDialog::on_changeShortcutButton_clicked()
{
    ShortcutItem *item = dynamic_cast<ShortcutItem *> (ui.shortcutTreeWidget->currentItem());
    if(!item)
        return;
    ShortcutDialog editor(item->action()->shortcut().toString(), this);
    if(editor.exec() == QDialog::Accepted)
    {
        item->action()->setShortcut(editor.key());
        item->setText(1, item->action()->shortcut().toString());
    }
}
