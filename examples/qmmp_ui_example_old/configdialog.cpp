/***************************************************************************
 *   Copyright (C) 2007-2010 by Ilya Kotov                                 *
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
#include <qmmpui/generalhandler.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistmodel.h>
#include "pluginitem.h"
#include "configdialog.h"

ConfigDialog::ConfigDialog (QWidget *parent)
        : QDialog (parent)
{
    ui.setupUi (this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, false);
    ui.preferencesButton->setEnabled(false);
    ui.informationButton->setEnabled(false);
    connect (this, SIGNAL(rejected()),SLOT(saveSettings()));
    connect (ui.fileDialogComboBox, SIGNAL (currentIndexChanged (int)), SLOT(updateDialogButton(int)));
    ui.replayGainModeComboBox->addItem (tr("Track"), QmmpSettings::REPLAYGAIN_TRACK);
    ui.replayGainModeComboBox->addItem (tr("Album"), QmmpSettings::REPLAYGAIN_ALBUM);
    ui.replayGainModeComboBox->addItem (tr("Disabled"), QmmpSettings::REPLAYGAIN_DISABLED);
    readSettings();
    loadPluginsInfo();
    createMenus();    
}

ConfigDialog::~ConfigDialog()
{}

void ConfigDialog::readSettings()
{
    if (MediaPlayer *player = MediaPlayer::instance())
    {
        ui.formatLineEdit->setText(player->playListManager()->format());
        ui.metadataCheckBox->setChecked(player->playListManager()->useMetadata());
        ui.underscoresCheckBox->setChecked(player->playListManager()->convertUnderscore());
        ui.per20CheckBox->setChecked(player->playListManager()->convertTwenty());
    }
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
    //QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    if (MediaPlayer *player = MediaPlayer::instance())
    {
        player->playListManager()->setFormat(ui.formatLineEdit->text().trimmed());
        player->playListManager()->setUseMetadata(ui.metadataCheckBox->isChecked());
        player->playListManager()->setConvertUnderscore(ui.underscoresCheckBox->isChecked());
        player->playListManager()->setConvertTwenty(ui.per20CheckBox->isChecked());
    }
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
