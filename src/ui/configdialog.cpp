/***************************************************************************
 *   Copyright (C) 2007-2009 by Ilya Kotov                                 *
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
#include <qmmp/metadatamanager.h>
#include <qmmpui/generalfactory.h>
#include <qmmpui/general.h>
#include <qmmpui/generalhandler.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistmodel.h>
#include "popupsettings.h"
#include "skin.h"
#include "pluginitem.h"
#include "configdialog.h"
#include "skinreader.h"

ConfigDialog::ConfigDialog (QWidget *parent)
        : QDialog (parent)
{
    ui.setupUi (this);
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    setAttribute(Qt::WA_DeleteOnClose, FALSE);
    ui.preferencesButton->setEnabled(FALSE);
    ui.informationButton->setEnabled(FALSE);
    connect (ui. contentsWidget,
             SIGNAL (currentItemChanged (QListWidgetItem *, QListWidgetItem *)),
             this, SLOT (changePage (QListWidgetItem *, QListWidgetItem*)));
    connect (ui.mainFontButton, SIGNAL (clicked()), SLOT (setMainFont()));
    connect (ui.plFontButton, SIGNAL (clicked()), SLOT (setPlFont()));
    connect (ui.preferencesButton, SIGNAL(clicked()), SLOT (showPluginSettings()));
    connect (ui.informationButton, SIGNAL(clicked()), SLOT (showPluginInfo()));
    connect (this, SIGNAL(accepted()),SLOT(saveSettings()));
    connect (ui.inputPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.outputPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.visualPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.generalPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.effectPluginTable, SIGNAL(cellPressed(int, int)), SLOT(updateButtons()));
    connect (ui.pluginsTab, SIGNAL(currentChanged(int)), SLOT(updateButtons()));
    connect (ui.fileDialogComboBox, SIGNAL (currentIndexChanged (int)), SLOT(updateDialogButton(int)));
    connect (ui.fdInformationButton, SIGNAL (clicked()), SLOT(showFileDialogInfo()));
    connect (ui.skinInstallButton, SIGNAL (clicked()), SLOT(installSkin()));
    connect (ui.skinReloadButton, SIGNAL (clicked()), SLOT(loadSkins()));
    connect (ui.listWidget, SIGNAL (itemClicked (QListWidgetItem *)), this, SLOT (changeSkin()));
    ui.listWidget->setIconSize (QSize (105,34));
    m_skin = Skin::instance();
    readSettings();
    m_reader = new SkinReader(this);
    loadSkins();
    loadPluginsInfo();
    loadFonts();
    createMenus();
}

ConfigDialog::~ConfigDialog()
{
    while (!m_enginePluginItems.isEmpty())
        delete m_enginePluginItems.takeFirst();
    while (!m_outputPluginItems.isEmpty())
        delete m_outputPluginItems.takeFirst();
    while (!m_inputPluginItems.isEmpty())
        delete m_inputPluginItems.takeFirst();
    while (!m_visualPluginItems.isEmpty())
        delete m_visualPluginItems.takeFirst();
    while (!m_effectPluginItems.isEmpty())
        delete m_effectPluginItems.takeFirst();
}

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
    ui.protocolCheckBox->setChecked(settings.value ("PlayList/show_protocol", FALSE).toBool());
    ui.numbersCheckBox->setChecked(settings.value ("PlayList/show_numbers", TRUE).toBool());
    ui.playlistsCheckBox->setChecked(settings.value("PlayList/show_plalists", FALSE).toBool());
    ui.popupCheckBox->setChecked(settings.value("PlayList/show_popup", FALSE).toBool());
    //proxy settings
    ui.enableProxyCheckBox->setChecked(Qmmp::useProxy());
    ui.authProxyCheckBox->setChecked(Qmmp::useProxyAuth());
    ui.hostLineEdit->setText(Qmmp::proxy().host());
    if (Qmmp::proxy().port(0))
        ui.portLineEdit->setText(QString::number(Qmmp::proxy().port(0)));
    ui.proxyUserLineEdit->setText(Qmmp::proxy().userName());
    ui.proxyPasswLineEdit->setText(Qmmp::proxy().password());

    ui.hostLineEdit->setEnabled(ui.enableProxyCheckBox->isChecked());
    ui.portLineEdit->setEnabled(ui.enableProxyCheckBox->isChecked());
    ui.proxyUserLineEdit->setEnabled(ui.authProxyCheckBox->isChecked());
    ui.proxyPasswLineEdit->setEnabled(ui.authProxyCheckBox->isChecked());

    ui.hiddenCheckBox->setChecked(settings.value("MainWindow/start_hidden", FALSE).toBool());
    ui.hideOnCloseCheckBox->setChecked(settings.value("MainWindow/hide_on_close", FALSE).toBool());
    //volume
    ui.softVolumeCheckBox->setChecked(SoundCore::instance()->softwareVolume());
    connect(ui.softVolumeCheckBox, SIGNAL(clicked(bool)), SoundCore::instance(), SLOT(setSoftwareVolume(bool)));
    //transparency
    ui.mwTransparencySlider->setValue(100 - settings.value("MainWindow/opacity", 1.0).toDouble()*100);
    ui.eqTransparencySlider->setValue(100 - settings.value("Equalizer/opacity", 1.0).toDouble()*100);
    ui.plTransparencySlider->setValue(100 - settings.value("PlayList/opacity", 1.0).toDouble()*100);
    //compatibility
    ui.openboxCheckBox->setChecked(settings.value("General/openbox_compat", FALSE).toBool());
    ui.metacityCheckBox->setChecked(settings.value("General/metacity_compat", FALSE).toBool());
    //skin options
    ui.skinCursorsCheckBox->setChecked(settings.value("General/skin_cursors", FALSE).toBool());
    ui.doubleSizeCheckBox->setChecked(settings.value("General/double_size", FALSE).toBool());
    //cover options
    ui.coverIncludeLineEdit->setText(MetaDataManager::instance()->coverNameFilters(TRUE).join(","));
    ui.coverExcludeLineEdit->setText(MetaDataManager::instance()->coverNameFilters(FALSE).join(","));
    ui.coverDepthSpinBox->setValue(MetaDataManager::instance()->coverSearchDepth());
}

void ConfigDialog::changePage (QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;
    ui.stackedWidget->setCurrentIndex (ui.contentsWidget->row (current));
    ui.visibilityGroupBox->setEnabled(GeneralHandler::instance()->visibilityControl());
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
    /*
        load input plugins information
    */
    QList <DecoderFactory *> *decoders = 0;
    decoders = Decoder::factories();
    QStringList files = Decoder::files();
    ui.inputPluginTable->setColumnCount (3);
    ui.inputPluginTable->verticalHeader()->hide();
    ui.inputPluginTable->setHorizontalHeaderLabels (QStringList()
            << tr ("Enabled") << tr ("Description") << tr ("Filename"));
    ui.inputPluginTable->setRowCount (decoders->count ());
    for (int i = 0; i < decoders->count (); ++i)
    {
        InputPluginItem *item = new InputPluginItem(this, decoders->at(i));
        m_inputPluginItems.append(item);
        QCheckBox* checkBox = new QCheckBox (ui.inputPluginTable);
        checkBox->setFocusPolicy (Qt::NoFocus);
        ui.inputPluginTable->setCellWidget (i, 0, checkBox);
        checkBox->setChecked(item->isSelected());
        connect(checkBox, SIGNAL(toggled(bool)), item, SLOT(setSelected(bool)));
        ui.inputPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.inputPluginTable->setItem (i,2, new QTableWidgetItem (files.at (i).section('/',-1)));
        ui.inputPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.inputPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.inputPluginTable->item(i,2)->setToolTip(files.at (i));
    }
    /*
        load audio engines information
    */
    QList <EngineFactory *> *engines = AbstractEngine::factories();
    files = AbstractEngine::files();
    ui.inputPluginTable->setRowCount (decoders->count () + engines->count());
    for (int i = decoders->count (); i < decoders->count () + engines->count (); ++i)
    {
        QString filePath = files.at (i - decoders->count ());
        EnginePluginItem *item = new EnginePluginItem(this, engines->at(i - decoders->count ()));
        m_enginePluginItems.append(item);
        QCheckBox* checkBox = new QCheckBox (ui.inputPluginTable);
        checkBox->setFocusPolicy (Qt::NoFocus);
        ui.inputPluginTable->setCellWidget (i, 0, checkBox);
        checkBox->setChecked(item->isSelected());
        connect(checkBox, SIGNAL(toggled(bool)), item, SLOT(setSelected(bool)));
        ui.inputPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.inputPluginTable->setItem (i,2, new QTableWidgetItem (filePath.section('/',-1)));
        ui.inputPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.inputPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.inputPluginTable->item(i,2)->setToolTip(filePath);
    }
    ui.inputPluginTable->resizeColumnToContents (0);
    ui.inputPluginTable->resizeColumnToContents (1);
    ui.inputPluginTable->resizeRowsToContents ();
    /*
        load output plugins information
    */
    QList <OutputFactory *> *outputs = 0;
    outputs = Output::outputFactories();
    files = Output::outputFiles();
    ui.outputPluginTable->setColumnCount (3);
    ui.outputPluginTable->verticalHeader()->hide();
    ui.outputPluginTable->setHorizontalHeaderLabels (QStringList()
            << tr ("Enabled") << tr ("Description") << tr ("Filename"));
    ui.outputPluginTable->setRowCount (outputs->count ());

    for (int i = 0; i < outputs->count (); ++i)
    {
        OutputPluginItem *item = new OutputPluginItem(this,outputs->at(i));
        m_outputPluginItems.append(item);
        QRadioButton* button = new QRadioButton (ui.outputPluginTable);
        button->setFocusPolicy (Qt::NoFocus);
        ui.outputPluginTable->setCellWidget (i, 0, button);
        button->setChecked (item->isSelected());
        connect(button, SIGNAL(pressed ()), item, SLOT(select()));
        ui.outputPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.outputPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.outputPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.outputPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.outputPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.outputPluginTable->resizeColumnToContents (0);
    ui.outputPluginTable->resizeColumnToContents (1);
    ui.outputPluginTable->resizeRowsToContents ();
    /*
        load visual plugin information
    */
    QList <VisualFactory *> *visuals = 0;
    visuals = Visual::factories();
    files = Visual::files();
    ui.visualPluginTable->setColumnCount (3);
    ui.visualPluginTable->verticalHeader()->hide();
    ui.visualPluginTable->setHorizontalHeaderLabels (QStringList()
            << tr ("Enabled") << tr ("Description") << tr ("Filename"));
    ui.visualPluginTable->setRowCount (visuals->count ());

    for (int i = 0; i < visuals->count (); ++i)
    {
        VisualPluginItem *item = new VisualPluginItem(this,visuals->at(i));
        m_visualPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.visualPluginTable);
        button->setFocusPolicy (Qt::NoFocus);
        ui.visualPluginTable->setCellWidget (i, 0, button);
        button->setChecked (item->isSelected());
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        ui.visualPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.visualPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.visualPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.visualPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.visualPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.visualPluginTable->resizeColumnToContents (0);
    ui.visualPluginTable->resizeColumnToContents (1);
    ui.visualPluginTable->resizeRowsToContents ();

    /*
        load effect plugin information
    */
    QList <EffectFactory *> *effects = 0;
    effects = Effect::effectFactories();
    files = Effect::effectFiles();
    ui.effectPluginTable->setColumnCount (3);
    ui.effectPluginTable->verticalHeader()->hide();
    ui.effectPluginTable->setHorizontalHeaderLabels (QStringList()
            << tr ("Enabled") << tr ("Description") << tr ("Filename"));
    ui.effectPluginTable->setRowCount (effects->count ());

    for (int i = 0; i < effects->count (); ++i)
    {
        EffectPluginItem *item = new EffectPluginItem(this,effects->at(i));
        m_effectPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.effectPluginTable);
        button->setFocusPolicy (Qt::NoFocus);
        ui.effectPluginTable->setCellWidget (i, 0, button);
        button->setChecked (item->isSelected());
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        ui.effectPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.effectPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.effectPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.effectPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.effectPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.effectPluginTable->resizeColumnToContents (0);
    ui.effectPluginTable->resizeColumnToContents (1);
    ui.effectPluginTable->resizeRowsToContents ();

    /*
    load general plugin information
    */
    QList <GeneralFactory *> *generals = 0;
    generals = General::generalFactories();
    files = General::generalFiles();
    ui.generalPluginTable->setColumnCount (3);
    ui.generalPluginTable->verticalHeader()->hide();
    ui.generalPluginTable->setHorizontalHeaderLabels (QStringList()
            << tr ("Enabled") << tr ("Description") << tr ("Filename"));
    ui.generalPluginTable->setRowCount (generals->count ());

    for (int i = 0; i < generals->count (); ++i)
    {
        GeneralPluginItem *item = new GeneralPluginItem(this,generals->at(i));
        m_generalPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.generalPluginTable);
        button->setFocusPolicy (Qt::NoFocus);
        ui.generalPluginTable->setCellWidget (i, 0, button);
        button->setChecked (item->isSelected());
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        ui.generalPluginTable->setItem (i,1,
                                        new QTableWidgetItem (item->factory()->properties().name));
        ui.generalPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.generalPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.generalPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.generalPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.generalPluginTable->resizeColumnToContents (0);
    ui.generalPluginTable->resizeColumnToContents (1);
    ui.generalPluginTable->resizeRowsToContents ();

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
    QString fontname = settings.value ("PlayList/Font").toString();
    QFont font = QApplication::font();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.plFontLabel->setFont(font);

    font = QApplication::font ();
    fontname = settings.value ("MainWindow/Font").toString();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    ui.mainFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.mainFontLabel->setFont(font);
    ui.useBitmapCheckBox->setChecked(settings.value("MainWindow/bitmap_font", FALSE).toBool());
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
        settings.setValue ("PlayList/Font", font.toString());
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
        settings.setValue ("MainWindow/Font", font.toString());
    }
}

void ConfigDialog::showPluginSettings()
{
    switch ((int) ui.pluginsTab -> currentIndex ())
    {
    case 0:
    {
        int row = ui.inputPluginTable->currentRow ();
        if(row < 0)
            return;
        if(row < m_inputPluginItems.count() && !m_inputPluginItems.isEmpty())
        {
            m_inputPluginItems.at(row)->factory()->showSettings (this);
        }
        else if(row - m_inputPluginItems.count() < m_enginePluginItems.count() &&
                !m_enginePluginItems.isEmpty())
        {
            row -= m_inputPluginItems.count();
            m_enginePluginItems.at(row)->factory()->showSettings (this);
        }
        break;
    }
    case 1:
    {
        int row = ui.outputPluginTable->currentRow ();
        if (m_outputPluginItems.isEmpty() || row < 0)
            return;
        m_outputPluginItems.at(row)->factory()->showSettings (this);
        break;
    }
    case 2:
    {
        int row = ui.visualPluginTable->currentRow ();
        if (m_visualPluginItems.isEmpty() || row < 0)
            return;
        Visual::showSettings(m_visualPluginItems.at(row)->factory(), this);
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if (m_effectPluginItems.isEmpty() || row < 0)
            return;
        m_effectPluginItems.at(row)->factory()->showSettings (this);
        break;
    }
    case 4:
    {
        int row = ui.generalPluginTable->currentRow ();
        if (m_generalPluginItems.isEmpty() || row < 0)
            return;
        GeneralHandler::instance()->showSettings(m_generalPluginItems.at(row)->factory(), this);
        break;
    }
    }
}

void ConfigDialog::showPluginInfo()
{
    switch ((int) ui.pluginsTab -> currentIndex ())
    {
    case 0:
    {
        int row = ui.inputPluginTable->currentRow ();
        if(row < 0)
            return;
        if(row < m_inputPluginItems.count() && !m_inputPluginItems.isEmpty())
        {
            m_inputPluginItems.at(row)->factory()->showAbout (this);
        }
        else if(row - m_inputPluginItems.count() < m_enginePluginItems.count() &&
                !m_enginePluginItems.isEmpty())
        {
            row -= m_inputPluginItems.count();
            m_enginePluginItems.at(row)->factory()->showAbout (this);
        }
        break;
    }
    case 1:
    {
        int row = ui.outputPluginTable->currentRow ();
        if (m_outputPluginItems.isEmpty() || row < 0)
            return;
        m_outputPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    case 2:
    {
        int row = ui.visualPluginTable->currentRow ();
        if (m_visualPluginItems.isEmpty() || row < 0)
            return;
        m_visualPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if (m_effectPluginItems.isEmpty() || row < 0)
            return;
        m_effectPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    case 4:
    {
        int row = ui.generalPluginTable->currentRow ();
        if (m_generalPluginItems.isEmpty() || row < 0)
            return;
        m_generalPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    }
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
    if (MediaPlayer *player = MediaPlayer::instance())
    {
        player->playListManager()->setFormat(ui.formatLineEdit->text().trimmed());
        player->playListManager()->setUseMetadata(ui.metadataCheckBox->isChecked());
        player->playListManager()->setConvertUnderscore(ui.underscoresCheckBox->isChecked());
        player->playListManager()->setConvertTwenty(ui.per20CheckBox->isChecked());
    }
    settings.setValue ("PlayList/show_protocol", ui.protocolCheckBox->isChecked());
    settings.setValue ("PlayList/show_numbers", ui.numbersCheckBox->isChecked());
    settings.setValue ("PlayList/show_plalists", ui.playlistsCheckBox->isChecked());
    settings.setValue ("PlayList/show_popup", ui.popupCheckBox->isChecked());
    FileDialog::setEnabled(FileDialog::registeredFactories().at(ui.fileDialogComboBox->currentIndex()));

    Qmmp::setProxyEnabled(ui.enableProxyCheckBox->isChecked());
    Qmmp::setProxyAuthEnabled(ui.authProxyCheckBox->isChecked());
    QUrl proxyUrl;
    proxyUrl.setHost(ui.hostLineEdit->text());
    proxyUrl.setPort(ui.portLineEdit->text().toUInt());
    proxyUrl.setUserName(ui.proxyUserLineEdit->text());
    proxyUrl.setPassword(ui.proxyPasswLineEdit->text());
    Qmmp::setProxy(proxyUrl);
    settings.setValue ("MainWindow/start_hidden", ui.hiddenCheckBox->isChecked());
    settings.setValue ("MainWindow/hide_on_close", ui.hideOnCloseCheckBox->isChecked());
    settings.setValue ("MainWindow/opacity", 1.0 -  (double)ui.mwTransparencySlider->value()/100);
    settings.setValue ("Equalizer/opacity", 1.0 -  (double)ui.eqTransparencySlider->value()/100);
    settings.setValue ("PlayList/opacity", 1.0 -  (double)ui.plTransparencySlider->value()/100);
    settings.setValue ("General/openbox_compat", ui.openboxCheckBox->isChecked());
    settings.setValue ("General/metacity_compat", ui.metacityCheckBox->isChecked());
    settings.setValue ("MainWindow/bitmap_font", ui.useBitmapCheckBox->isChecked());
    settings.setValue ("General/skin_cursors", ui.skinCursorsCheckBox->isChecked());
    settings.setValue ("General/double_size", ui.doubleSizeCheckBox->isChecked());
    MetaDataManager::instance()->setCoverSearchSettings(ui.coverIncludeLineEdit->text().split(","),
                                                        ui.coverExcludeLineEdit->text().split(","),
                                                        ui.coverDepthSpinBox->value());
}

void ConfigDialog::updateButtons()
{
    bool preferences = FALSE;
    bool information = FALSE;
    switch ((int) ui.pluginsTab -> currentIndex ())
    {
    case 0:
    {
        int row = ui.inputPluginTable->currentRow ();
        if (row < 0)
            break;
        if(row < m_inputPluginItems.count() && !m_inputPluginItems.isEmpty())
        {
            DecoderFactory *factory = m_inputPluginItems.at(row)->factory();
            information = factory->properties().hasAbout;
            preferences = factory->properties().hasSettings;
        }
        else if(row - m_inputPluginItems.count() < m_enginePluginItems.count() &&
                !m_enginePluginItems.isEmpty())
        {
            row -= m_inputPluginItems.count();
            EngineFactory *factory = m_enginePluginItems.at(row)->factory();
            information = factory->properties().hasAbout;
            preferences = factory->properties().hasSettings;
        }
        break;
    }
    case 1:
    {
        int row = ui.outputPluginTable->currentRow ();
        if (m_outputPluginItems.isEmpty() || row < 0)
            break;
        OutputFactory *factory = m_outputPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 2:
    {
        int row = ui.visualPluginTable->currentRow ();
        if (m_visualPluginItems.isEmpty() || row < 0)
            break;
        VisualFactory *factory = m_visualPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if (m_effectPluginItems.isEmpty() || row < 0)
            break;
        EffectFactory *factory = m_effectPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 4:
    {
        int row = ui.generalPluginTable->currentRow ();
        if (m_generalPluginItems.isEmpty() || row < 0)
            break;
        GeneralFactory *factory = m_generalPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    }
    ui.preferencesButton->setEnabled(preferences);
    ui.informationButton->setEnabled(information);
}

void ConfigDialog::updateDialogButton(int index)
{
    ui.fdInformationButton->setEnabled(FileDialog::registeredFactories()[index]->properties().hasAbout);
}

void ConfigDialog::showFileDialogInfo()
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

void ConfigDialog::on_popupCustomizeButton_clicked()
{
    PopupSettings *p = new PopupSettings(this);
    p->exec();
    p->deleteLater();
}
