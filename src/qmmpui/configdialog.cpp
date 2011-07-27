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
#include <QTreeWidgetItem>
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
#include "ui_configdialog.h"
#include "pluginitem_p.h"
#include "generalfactory.h"
#include "general.h"
#include "uihelper.h"
#include "uiloader.h"
#include "filedialog.h"
#include "mediaplayer.h"
#include "playlistmodel.h"
#include "configdialog.h"

ConfigDialog::ConfigDialog (QWidget *parent) : QDialog (parent)
{
    m_ui = new Ui::ConfigDialog;
    m_ui->setupUi (this);
    m_insert_row = 0;
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, false);
    m_ui->preferencesButton->setEnabled(false);
    m_ui->informationButton->setEnabled(false);
    connect (this, SIGNAL(rejected()),SLOT(saveSettings()));
    connect (m_ui->fileDialogComboBox, SIGNAL (currentIndexChanged (int)), SLOT(updateDialogButton(int)));
    m_ui->replayGainModeComboBox->addItem (tr("Track"), QmmpSettings::REPLAYGAIN_TRACK);
    m_ui->replayGainModeComboBox->addItem (tr("Album"), QmmpSettings::REPLAYGAIN_ALBUM);
    m_ui->replayGainModeComboBox->addItem (tr("Disabled"), QmmpSettings::REPLAYGAIN_DISABLED);
    readSettings();
    loadPluginsInfo();
    createMenus();
    //setup icons
    m_ui->preferencesButton->setIcon(QIcon::fromTheme("configure"));
    m_ui->informationButton->setIcon(QIcon::fromTheme("dialog-information"));
    m_ui->fdInformationButton->setIcon(QIcon::fromTheme("dialog-information"));
    m_ui->outputInformationButton->setIcon(QIcon::fromTheme("dialog-information"));
    m_ui->uiInformationButton->setIcon(QIcon::fromTheme("dialog-information"));
    m_ui->outputPreferencesButton->setIcon(QIcon::fromTheme("configure"));
}

ConfigDialog::~ConfigDialog()
{
    delete m_ui;
}

void ConfigDialog::addPage(const QString &name, QWidget *widget, const QIcon &icon)
{
    m_ui->stackedWidget->insertWidget(m_insert_row, widget);
    m_ui->contentsWidget->insertItem (m_insert_row, name);
    m_ui->contentsWidget->item(m_insert_row)->setIcon(icon);
    m_ui->contentsWidget->setCurrentRow(0);
    m_insert_row++;
}

void ConfigDialog::readSettings()
{
    if (MediaPlayer::instance())
    {
        MediaPlayer *player = MediaPlayer::instance();
        m_ui->formatLineEdit->setText(player->playListManager()->format());
        m_ui->metadataCheckBox->setChecked(player->playListManager()->useMetadata());
        m_ui->underscoresCheckBox->setChecked(player->playListManager()->convertUnderscore());
        m_ui->per20CheckBox->setChecked(player->playListManager()->convertTwenty());
    }
    //proxy settings
    QmmpSettings *gs = QmmpSettings::instance();
    m_ui->enableProxyCheckBox->setChecked(gs->isProxyEnabled());
    m_ui->authProxyCheckBox->setChecked(gs->useProxyAuth());
    m_ui->hostLineEdit->setText(gs->proxy().host());
    if (gs->proxy().port(0))
        m_ui->portLineEdit->setText(QString::number(gs->proxy().port(0)));
    m_ui->proxyUserLineEdit->setText(gs->proxy().userName());
    m_ui->proxyPasswLineEdit->setText(gs->proxy().password());

    m_ui->hostLineEdit->setEnabled(m_ui->enableProxyCheckBox->isChecked());
    m_ui->portLineEdit->setEnabled(m_ui->enableProxyCheckBox->isChecked());
    m_ui->proxyUserLineEdit->setEnabled(m_ui->authProxyCheckBox->isChecked());
    m_ui->proxyPasswLineEdit->setEnabled(m_ui->authProxyCheckBox->isChecked());
    //resume playback
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    m_ui->continuePlaybackCheckBox->setChecked(settings.value("General/resume_on_startup", false).toBool());
    //cover options
    m_ui->coverIncludeLineEdit->setText(gs->coverNameFilters(true).join(","));
    m_ui->coverExcludeLineEdit->setText(gs->coverNameFilters(false).join(","));
    m_ui->coverDepthSpinBox->setValue(gs->coverSearchDepth());
    m_ui->useCoverFilesCheckBox->setChecked(gs->useCoverFiles());
    //replay gain
    m_ui->clippingCheckBox->setChecked(gs->replayGainPreventClipping());
    m_ui->replayGainModeComboBox->setCurrentIndex(m_ui->replayGainModeComboBox->findData(gs->replayGainMode()));
    m_ui->preampDoubleSpinBox->setValue(gs->replayGainPreamp());
    m_ui->defaultGainDoubleSpinBox->setValue(gs->replayGainDefaultGain());
    //audio
    m_ui->softVolumeCheckBox->setChecked(gs->useSoftVolume());
    m_ui->use16BitCheckBox->setChecked(gs->use16BitOutput());
    m_ui->bufferSizeSpinBox->setValue(gs->bufferSize());
    //geometry
    resize(settings.value("ConfigDialog/window_size", QSize(700,470)).toSize());
    QList<QVariant> var_sizes = settings.value("ConfigDialog/splitter_sizes").toList();
    if(var_sizes.count() != 2)
    {
        var_sizes.clear();
        var_sizes << 180 << width()-180;
    }
    QList<int> sizes;
    sizes << var_sizes.first().toInt() << var_sizes.last().toInt();
    m_ui->splitter->setSizes(sizes);
}

void ConfigDialog::on_contentsWidget_currentItemChanged (QListWidgetItem *current,
                                                         QListWidgetItem *previous)
{
    if (!current)
        current = previous;
    m_ui->stackedWidget->setCurrentIndex (m_ui->contentsWidget->row (current));
}

void ConfigDialog::loadPluginsInfo()
{
    m_ui->treeWidget->blockSignals(true);
    /*
        load transport plugin information
     */
    QTreeWidgetItem *item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Transports"));
    QList <InputSourceFactory *> *transports = InputSource::factories();
    QStringList files = InputSource::files();
    for (int i = 0; i < transports->count (); ++i)
        new PluginItem (item, transports->at(i), files.at (i));
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load input plugins information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Decoders"));
    QList <DecoderFactory *> *decoders = Decoder::factories();
    files = Decoder::files();
    for (int i = 0; i < decoders->count (); ++i)
        new PluginItem (item, decoders->at(i), files.at (i));
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load audio engines information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Engines"));
    QList <EngineFactory *> *engines = AbstractEngine::factories();
    files = AbstractEngine::files();
    for (int i = 0; i < engines->count (); ++i)
        new PluginItem (item, engines->at(i), files.at (i));
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load effect plugin information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Effects"));
    QList <EffectFactory *> *effects = Effect::factories();
    files = Effect::files();
    for (int i = 0; i < effects->count (); ++i)
        new PluginItem (item, effects->at(i), files.at (i));
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load visual plugin information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Visualization"));
    QList <VisualFactory *> *visuals = Visual::factories();
    files = Visual::files();
    for (int i = 0; i < visuals->count (); ++i)
        new PluginItem (item, visuals->at(i), files.at (i));
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load general plugin information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("General"));
    QList <GeneralFactory *> *generals = General::factories();
    files = General::files();
    for (int i = 0; i < generals->count (); ++i)
        new PluginItem (item, generals->at(i), files.at (i));
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);

    m_ui->treeWidget->blockSignals(false);
    m_ui->treeWidget->resizeColumnToContents(0);
    m_ui->treeWidget->resizeColumnToContents(1);
    /*
        load output plugins information
    */

    m_ui->outputInformationButton->setEnabled(false);
    m_ui->outputPreferencesButton->setEnabled(false);
    QList <OutputFactory *> *outputs = Output::factories();
    for (int i = 0; i < outputs->count (); ++i)
    {
        m_ui->outputComboBox->addItem(outputs->at(i)->properties().name);
        if(Output::currentFactory() == outputs->at(i))
        {
            m_ui->outputComboBox->setCurrentIndex(i);
            on_outputComboBox_activated (i);
        }
    }
    /*
        load file dialog information
    */
    foreach(FileDialogFactory *factory, FileDialog::registeredFactories())
    {
        m_ui->fileDialogComboBox->addItem(factory->properties().name);
        if (FileDialog::isEnabled(factory))
            m_ui->fileDialogComboBox->setCurrentIndex(m_ui->fileDialogComboBox->count()-1);
    }
    /*
        load ui information
    */
    m_ui->uiInformationButton->setEnabled(false);
    foreach(UiFactory *factory, *UiLoader::factories())
    {
        m_ui->uiComboBox->addItem(factory->properties().name);
        if (UiLoader::selected() == factory)
        {
            m_ui->uiComboBox->setCurrentIndex(m_ui->uiComboBox->count()-1);
            on_uiComboBox_activated(m_ui->uiComboBox->count()-1);
        }
    }
}

void ConfigDialog::on_preferencesButton_clicked()
{
    QTreeWidgetItem *item = m_ui->treeWidget->currentItem();
    if(item && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->showSettings(this);

}

void ConfigDialog::on_informationButton_clicked()
{
    QTreeWidgetItem *item = m_ui->treeWidget->currentItem();
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

    m_ui->titleButton->setMenu(menu);
    m_ui->titleButton->setPopupMode(QToolButton::InstantPopup);
    connect(menu, SIGNAL(triggered (QAction *)), SLOT(addTitleString(QAction *)));
}

void ConfigDialog::addTitleString(QAction * a)
{
    if (m_ui->formatLineEdit->cursorPosition () < 1)
        m_ui->formatLineEdit->insert(a->data().toString());
    else
        m_ui->formatLineEdit->insert(" - "+a->data().toString());
}

void ConfigDialog::saveSettings()
{
    if (MediaPlayer *player = MediaPlayer::instance())
    {
        player->playListManager()->setFormat(m_ui->formatLineEdit->text().trimmed());
        player->playListManager()->setUseMetadata(m_ui->metadataCheckBox->isChecked());
        player->playListManager()->setConvertUnderscore(m_ui->underscoresCheckBox->isChecked());
        player->playListManager()->setConvertTwenty(m_ui->per20CheckBox->isChecked());
    }

    FileDialog::setEnabled(FileDialog::registeredFactories().at(m_ui->fileDialogComboBox->currentIndex()));
    QmmpSettings *gs = QmmpSettings::instance();
    //proxy
    QUrl proxyUrl;
    proxyUrl.setHost(m_ui->hostLineEdit->text());
    proxyUrl.setPort(m_ui->portLineEdit->text().toUInt());
    proxyUrl.setUserName(m_ui->proxyUserLineEdit->text());
    proxyUrl.setPassword(m_ui->proxyPasswLineEdit->text());
    gs->setNetworkSettings(m_ui->enableProxyCheckBox->isChecked(),
                           m_ui->authProxyCheckBox->isChecked(),
                           proxyUrl);

    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue ("General/resume_on_startup",  m_ui->continuePlaybackCheckBox->isChecked());
    gs->setCoverSettings(m_ui->coverIncludeLineEdit->text().split(","),
                         m_ui->coverExcludeLineEdit->text().split(","),
                         m_ui->coverDepthSpinBox->value(),
                         m_ui->useCoverFilesCheckBox->isChecked());
    int i = m_ui->replayGainModeComboBox->currentIndex();
    gs->setReplayGainSettings((QmmpSettings::ReplayGainMode)
                              m_ui->replayGainModeComboBox->itemData(i).toInt(),
                              m_ui->preampDoubleSpinBox->value(),
                              m_ui->defaultGainDoubleSpinBox->value(),
                              m_ui->clippingCheckBox->isChecked());
    gs->setAudioSettings(m_ui->softVolumeCheckBox->isChecked(), m_ui->use16BitCheckBox->isChecked());
    gs->setBufferSize(m_ui->bufferSizeSpinBox->value());
    QList <OutputFactory *> *outputs = Output::factories();
    if(m_ui->outputComboBox->currentIndex() >= 0 && outputs->count())
        Output::setCurrentFactory(outputs->at(m_ui->outputComboBox->currentIndex()));

    QList<QVariant> var_sizes;
    var_sizes << m_ui->splitter->sizes().first() << m_ui->splitter->sizes().last();
    settings.setValue("ConfigDialog/splitter_sizes", var_sizes);
    settings.setValue("ConfigDialog/window_size", size());
}

void ConfigDialog::updateDialogButton(int index)
{
    m_ui->fdInformationButton->setEnabled(FileDialog::registeredFactories()[index]->properties().hasAbout);
}

void ConfigDialog::on_fdInformationButton_clicked()
{
    int index = m_ui->fileDialogComboBox->currentIndex ();
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
        m_ui->preferencesButton->setEnabled(dynamic_cast<PluginItem *>(current)->hasSettings());
        m_ui->informationButton->setEnabled(dynamic_cast<PluginItem *>(current)->hasAbout());
    }
    else
    {
        m_ui->preferencesButton->setEnabled(false);
        m_ui->informationButton->setEnabled(false);
    }
}

void ConfigDialog::on_outputComboBox_activated (int index)
{
    OutputFactory *factory = Output::factories()->at(index);
    m_ui->outputInformationButton->setEnabled(factory->properties().hasAbout);
    m_ui->outputPreferencesButton->setEnabled(factory->properties().hasSettings);
}

void ConfigDialog::on_outputPreferencesButton_clicked()
{
    int index = m_ui->outputComboBox->currentIndex();
    Output::factories()->at(index)->showSettings(this);
}

void ConfigDialog::on_uiComboBox_activated (int index)
{
    UiFactory *factory =  UiLoader::factories()->at(index);
    m_ui->uiInformationButton->setEnabled(factory->properties().hasAbout);
}

void ConfigDialog::on_outputInformationButton_clicked()
{
    int index = m_ui->outputComboBox->currentIndex();
    Output::factories()->at(index)->showAbout(this);
}

void ConfigDialog::on_uiInformationButton_clicked()
{
    int index = m_ui->uiComboBox->currentIndex();
    UiLoader::factories()->at(index)->showAbout(this);
}
