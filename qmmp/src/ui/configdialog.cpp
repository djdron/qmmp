/***************************************************************************
 *   Copyright (C) 2007-2008 by Ilya Kotov                                 *
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

#include <qmmp/decoder.h>
#include <qmmp/output.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/outputfactory.h>
#include <qmmp/visualfactory.h>
#include <qmmp/effectfactory.h>
#include <qmmp/effect.h>
#include <qmmp/soundcore.h>
#include <qmmpui/generalfactory.h>
#include <qmmpui/general.h>
#include <qmmpui/generalhandler.h>
#include <qmmpui/filedialog.h>


#include "skin.h"
#include "pluginitem.h"
#include "configdialog.h"
#include "skinreader.h"

ConfigDialog::ConfigDialog ( QWidget *parent )
        : QDialog ( parent )
{
    ui.setupUi ( this );
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    setAttribute(Qt::WA_DeleteOnClose, FALSE);
    ui.preferencesButton->setEnabled(FALSE);
    ui.informationButton->setEnabled(FALSE);
    connect (ui. contentsWidget,
             SIGNAL (currentItemChanged (QListWidgetItem *, QListWidgetItem * )),
             this, SLOT (changePage (QListWidgetItem *, QListWidgetItem* )));
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
    ui.listWidget->setIconSize (QSize (69,29));
    m_skin = Skin::getPointer();
    readSettings();
    m_reader = new SkinReader(this);
    loadSkins();
    loadPluginsInfo();
    loadFonts();
    createMenus();
}

ConfigDialog::~ConfigDialog()
{
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
    ui.formatLineEdit->setText(
        settings.value ("PlayList/title_format", "%p - %t").toString());
    ui.metadataCheckBox->setChecked(
        settings.value ("PlayList/load_metadata", TRUE).toBool());
    ui.underscoresCheckBox->setChecked(settings.value ("PlayList/convert_underscore", TRUE).toBool());
    ui.per20CheckBox->setChecked(settings.value ("PlayList/convert_twenty", TRUE).toBool());
    ui.fullPathCheckBox->setChecked(settings.value ("PlayList/full_stream_path", FALSE).toBool());
    ui.protocolCheckBox->setChecked(settings.value ("PlayList/show_protocol", FALSE).toBool());

    //proxy settings
    ui.enableProxyCheckBox->setChecked(
        settings.value ("Proxy/use_proxy", FALSE).toBool());
    ui.authProxyCheckBox->setChecked(
        settings.value ("Proxy/authentication", FALSE).toBool());

    ui.hostLineEdit->setText(settings.value("Proxy/host").toString());
    ui.portLineEdit->setText(settings.value("Proxy/port").toString());
    ui.proxyUserLineEdit->setText(settings.value("Proxy/user").toString());
    ui.proxyPasswLineEdit->setText(settings.value("Proxy/passw").toString());

    ui.hostLineEdit->setEnabled(ui.enableProxyCheckBox->isChecked());
    ui.portLineEdit->setEnabled(ui.enableProxyCheckBox->isChecked());
    ui.proxyUserLineEdit->setEnabled(ui.authProxyCheckBox->isChecked());
    ui.proxyPasswLineEdit->setEnabled(ui.authProxyCheckBox->isChecked());

    ui.hiddenCheckBox->setChecked(settings.value("MainWindow/start_hidden", FALSE).toBool());
    ui.hideOnCloseCheckBox->setChecked(settings.value("MainWindow/hide_on_close", FALSE).toBool());
    //volume
    ui.softVolumeCheckBox->setChecked(SoundCore::instance()->softwareVolume());
    connect(ui.softVolumeCheckBox, SIGNAL(clicked(bool)), SoundCore::instance(), SLOT(setSoftwareVolume(bool)));
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
    findSkins(qApp->applicationDirPath()+"/../share/qmmp/skins");
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
    dir.setFilter ( QDir::Dirs | QDir::NoDotAndDotDot);
    QList <QFileInfo> fileList = dir.entryInfoList();
    if ( fileList.count() == 0 )
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
    ui.inputPluginTable->setColumnCount ( 3 );
    ui.inputPluginTable->verticalHeader()->hide();
    ui.inputPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.inputPluginTable->setRowCount ( decoders->count () );
    for ( int i = 0; i < decoders->count (); ++i )
    {
        InputPluginItem *item = new InputPluginItem(this, decoders->at(i));
        m_inputPluginItems.append(item);
        QCheckBox* checkBox = new QCheckBox ( ui.inputPluginTable );
        ui.inputPluginTable->setCellWidget ( i, 0, checkBox );
        checkBox->setChecked(item->isSelected());
        connect(checkBox, SIGNAL(toggled(bool)), item, SLOT(setSelected(bool)));
        ui.inputPluginTable->setItem ( i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.inputPluginTable->setItem ( i,2, new QTableWidgetItem (files.at (i).section('/',-1)));
        ui.inputPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.inputPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.inputPluginTable->item(i,2)->setToolTip(files.at (i));
    }
    ui.inputPluginTable->resizeColumnToContents ( 0 );
    ui.inputPluginTable->resizeColumnToContents ( 1 );
    ui.inputPluginTable->resizeRowsToContents ();
    /*
        load output plugins information
    */
    QList <OutputFactory *> *outputs = 0;
    outputs = Output::outputFactories();
    files = Output::outputFiles();
    ui.outputPluginTable->setColumnCount ( 3 );
    ui.outputPluginTable->verticalHeader()->hide();
    ui.outputPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.outputPluginTable->setRowCount ( outputs->count () );

    for ( int i = 0; i < outputs->count (); ++i )
    {
        OutputPluginItem *item = new OutputPluginItem(this,outputs->at(i));
        m_outputPluginItems.append(item);
        QRadioButton* button = new QRadioButton ( ui.outputPluginTable );
        ui.outputPluginTable->setCellWidget ( i, 0, button );
        button->setChecked ( item->isSelected() );
        connect(button, SIGNAL(pressed ()), item, SLOT(select()));
        ui.outputPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.outputPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.outputPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.outputPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.outputPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.outputPluginTable->resizeColumnToContents ( 0 );
    ui.outputPluginTable->resizeColumnToContents ( 1 );
    ui.outputPluginTable->resizeRowsToContents ();
    /*
        load visual plugin information
    */
    QList <VisualFactory *> *visuals = 0;
    visuals = Visual::factories();
    files = Visual::files();
    ui.visualPluginTable->setColumnCount ( 3 );
    ui.visualPluginTable->verticalHeader()->hide();
    ui.visualPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.visualPluginTable->setRowCount ( visuals->count () );

    for ( int i = 0; i < visuals->count (); ++i )
    {
        VisualPluginItem *item = new VisualPluginItem(this,visuals->at(i));
        m_visualPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.visualPluginTable);
        ui.visualPluginTable->setCellWidget ( i, 0, button );
        button->setChecked (item->isSelected());
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        ui.visualPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.visualPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.visualPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.visualPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.visualPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.visualPluginTable->resizeColumnToContents ( 0 );
    ui.visualPluginTable->resizeColumnToContents ( 1 );
    ui.visualPluginTable->resizeRowsToContents ();

    /*
        load effect plugin information
    */
    QList <EffectFactory *> *effects = 0;
    effects = Effect::effectFactories();
    files = Effect::effectFiles();
    ui.effectPluginTable->setColumnCount ( 3 );
    ui.effectPluginTable->verticalHeader()->hide();
    ui.effectPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.effectPluginTable->setRowCount ( effects->count () );

    for ( int i = 0; i < effects->count (); ++i )
    {
        EffectPluginItem *item = new EffectPluginItem(this,effects->at(i));
        m_effectPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.effectPluginTable);
        ui.effectPluginTable->setCellWidget ( i, 0, button );
        button->setChecked (item->isSelected());
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        ui.effectPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.effectPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i).section('/',-1)));
        ui.effectPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.effectPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.effectPluginTable->item(i,2)->setToolTip(files.at (i));
    }

    ui.effectPluginTable->resizeColumnToContents ( 0 );
    ui.effectPluginTable->resizeColumnToContents ( 1 );
    ui.effectPluginTable->resizeRowsToContents ();

    /*
    load general plugin information
    */
    QList <GeneralFactory *> *generals = 0;
    generals = General::generalFactories();
    files = General::generalFiles();
    ui.generalPluginTable->setColumnCount ( 3 );
    ui.generalPluginTable->verticalHeader()->hide();
    ui.generalPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.generalPluginTable->setRowCount ( generals->count () );

    for ( int i = 0; i < generals->count (); ++i )
    {
        GeneralPluginItem *item = new GeneralPluginItem(this,generals->at(i));
        m_generalPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.generalPluginTable);
        ui.generalPluginTable->setCellWidget ( i, 0, button );
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
    QString fontname = settings.value ( "PlayList/Font","" ).toString();
    QFont font;
    font.fromString(fontname);
    ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.plFontLabel->setFont(font);

    fontname = settings.value ("MainWindow/Font","").toString();
    font.fromString(fontname);
    ui.mainFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.mainFontLabel->setFont(font);
}

void ConfigDialog::setPlFont()
{
    bool ok;
    QFont font = ui.plFontLabel->font();
    font = QFontDialog::getFont ( &ok, font, this );
    if (ok)
    {
        ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        ui.plFontLabel->setFont(font);
        QSettings settings ( Qmmp::configFile(), QSettings::IniFormat );
        settings.setValue ( "PlayList/Font", font.toString() );
    }
}

void ConfigDialog::setMainFont()
{
    bool ok;
    QFont font = ui.mainFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if ( ok )
    {
        ui.mainFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        ui.mainFontLabel->setFont(font);
        QSettings settings ( Qmmp::configFile(), QSettings::IniFormat );
        settings.setValue ( "MainWindow/Font", font.toString() );
    }
}

void ConfigDialog::showPluginSettings()
{
    switch ((int) ui.pluginsTab -> currentIndex ())
    {
    case 0:
    {
        QList <DecoderFactory *> *decoders = 0;
        decoders = Decoder::factories();
        int row = ui.inputPluginTable->currentRow ();
        if ( !decoders || row<0 )
            return;

        decoders->at ( row )->showSettings ( this );
        break;
    }
    case 1:
    {
        int row = ui.outputPluginTable->currentRow ();
        if ( m_outputPluginItems.isEmpty() || row < 0 )
            return;
        m_outputPluginItems.at(row)->factory()->showSettings ( this );
        break;
    }
    case 2:
    {
        int row = ui.visualPluginTable->currentRow ();
        if ( m_visualPluginItems.isEmpty() || row < 0 )
            return;
        Visual::showSettings(m_visualPluginItems.at(row)->factory(), this);
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if ( m_effectPluginItems.isEmpty() || row < 0 )
            return;
        m_effectPluginItems.at(row)->factory()->showSettings ( this );
        break;
    }
    case 4:
    {
        int row = ui.generalPluginTable->currentRow ();
        if ( m_generalPluginItems.isEmpty() || row < 0 )
            return;
        GeneralHandler::instance()->showSettings(m_generalPluginItems.at(row)->factory(), this);
        break;
    }
    }
}

void ConfigDialog::showPluginInfo()
{
    switch ( ( int ) ui.pluginsTab -> currentIndex () )
    {
    case 0:
    {
        int row = ui.inputPluginTable->currentRow ();
        if (m_inputPluginItems.isEmpty() || row < 0)
            return;
        m_inputPluginItems.at(row)->factory()->showAbout (this);
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
        if ( m_visualPluginItems.isEmpty() || row < 0 )
            return;
        m_visualPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if ( m_effectPluginItems.isEmpty() || row < 0 )
            return;
        m_effectPluginItems.at(row)->factory()->showAbout (this);
        break;
    }
    case 4:
    {
        int row = ui.generalPluginTable->currentRow ();
        if ( m_generalPluginItems.isEmpty() || row < 0 )
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
    menu->addAction(tr("Tracknumber"))->setData("%n");
    menu->addAction(tr("Genre"))->setData("%g");
    menu->addAction(tr("Filename"))->setData("%f");
    menu->addAction(tr("Filepath"))->setData("%F");
    menu->addAction(tr("Date"))->setData("%d");
    menu->addAction(tr("Year"))->setData("%y");
    menu->addAction(tr("Comment"))->setData("%c");
    ui.titleButton->setMenu(menu);
    ui.titleButton->setPopupMode(QToolButton::InstantPopup);
    connect( menu, SIGNAL(triggered ( QAction * )), SLOT(addTitleString( QAction * )));
}

void ConfigDialog::addTitleString( QAction * a)
{
    if (ui.formatLineEdit->cursorPosition () < 1)
        ui.formatLineEdit->insert(a->data().toString());
    else
        ui.formatLineEdit->insert(" - "+a->data().toString());
}

void ConfigDialog::saveSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue ("PlayList/title_format", ui.formatLineEdit->text().trimmed());
    settings.setValue ("PlayList/load_metadata", ui.metadataCheckBox->isChecked());
    settings.setValue ("PlayList/convert_underscore", ui.underscoresCheckBox->isChecked());
    settings.setValue ("PlayList/convert_twenty", ui.per20CheckBox->isChecked());
    settings.setValue ("PlayList/full_stream_path", ui.fullPathCheckBox->isChecked());
    settings.setValue ("PlayList/show_protocol", ui.protocolCheckBox->isChecked());
    FileDialog::setEnabled(FileDialog::registeredFactories().at(ui.fileDialogComboBox->currentIndex()));
    settings.setValue ("Proxy/use_proxy", ui.enableProxyCheckBox->isChecked());
    settings.setValue ("Proxy/authentication", ui.authProxyCheckBox->isChecked());
    settings.setValue ("Proxy/host",ui.hostLineEdit->text());
    settings.setValue ("Proxy/port",ui.portLineEdit->text());
    settings.setValue ("Proxy/user",ui.proxyUserLineEdit->text());
    settings.setValue ("Proxy/passw",ui.proxyPasswLineEdit->text());
    settings.setValue ("MainWindow/start_hidden", ui.hiddenCheckBox->isChecked());
    settings.setValue ("MainWindow/hide_on_close", ui.hideOnCloseCheckBox->isChecked());
}

void ConfigDialog::updateButtons()
{

    bool preferences = FALSE;
    bool information = FALSE;
    switch ( ( int ) ui.pluginsTab -> currentIndex () )
    {
    case 0:
    {
        int row = ui.inputPluginTable->currentRow ();
        if (m_inputPluginItems.isEmpty() || row < 0)
            break;
        DecoderFactory *factory = m_inputPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 1:
    {
        int row = ui.outputPluginTable->currentRow ();
        if (m_outputPluginItems.isEmpty() || row < 0 )
            break;
        OutputFactory *factory = m_outputPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 2:
    {
        int row = ui.visualPluginTable->currentRow ();
        if (m_visualPluginItems.isEmpty() || row < 0 )
            break;
        VisualFactory *factory = m_visualPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if ( m_effectPluginItems.isEmpty() || row < 0 )
            break;
        EffectFactory *factory = m_effectPluginItems.at(row)->factory();
        information = factory->properties().hasAbout;
        preferences = factory->properties().hasSettings;
        break;
    }
    case 4:
    {
        int row = ui.generalPluginTable->currentRow ();
        if ( m_generalPluginItems.isEmpty() || row < 0 )
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
