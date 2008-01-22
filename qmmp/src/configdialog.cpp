/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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

#include <decoder.h>
#include <output.h>
#include <decoderfactory.h>
#include <outputfactory.h>
#include <visualfactory.h>
#include <effectfactory.h>
#include <effect.h>

#include "skin.h"
#include "filedialog.h"
#include "pluginitem.h"
#include "configdialog.h"
#include "skinreader.h"

ConfigDialog::ConfigDialog ( QWidget *parent )
        : QDialog ( parent )
{
    ui.setupUi ( this );
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    setAttribute(Qt::WA_DeleteOnClose, FALSE);
    connect ( ui. contentsWidget,
              SIGNAL ( currentItemChanged ( QListWidgetItem *, QListWidgetItem * ) ),
              this, SLOT ( changePage ( QListWidgetItem *, QListWidgetItem* ) ) );
    connect ( ui.mainFontButton, SIGNAL ( clicked() ), SLOT ( setMainFont() ) );
    connect ( ui.plFontButton, SIGNAL ( clicked() ), SLOT ( setPlFont() ) );
    connect ( ui.preferencesButton, SIGNAL ( clicked() ), SLOT (showPluginSettings()));
    connect ( ui.informationButton, SIGNAL ( clicked() ), SLOT (showPluginInfo()));
    connect ( this, SIGNAL(accepted()),SLOT(saveSettings()));
    ui.listWidget->setIconSize ( QSize ( 69,29 ) );
    m_skin = Skin::getPointer();
    ui.fileDialogComboBox->insertItems(0,FileDialog::registeredFactories());
    readSettings();
    SkinReader reader;
    reader.updateCache();
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
        delete m_outputPluginItems.takeFirst();
    while (!m_visualPluginItems.isEmpty())
        delete m_visualPluginItems.takeFirst();
    while (!m_effectPluginItems.isEmpty())
        delete m_effectPluginItems.takeFirst();
}

void ConfigDialog::readSettings()
{
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    ui.formatLineEdit->setText(
        settings.value ( "PlayList/title_format", "%p - %t").toString());
    ui.metadataCheckBox->setChecked(
        settings.value ( "PlayList/load_metadata", TRUE).toBool());
    ui.trayCheckBox->setChecked(
        settings.value("Tray/enabled",TRUE).toBool());
    ui.messageCheckBox->setChecked(
        settings.value("Tray/show_message",TRUE).toBool());
    ui.messageDelaySpinBox->setValue(settings.value("Tray/message_delay",
                                     2000).toInt());
    ui.messageCheckBox->setEnabled(ui.trayCheckBox->isChecked());
    ui.messageDelaySpinBox->setEnabled(ui.trayCheckBox->isChecked() ||
                                       ui.messageCheckBox->isChecked() );
    ui.toolTipCheckBox->setEnabled(ui.trayCheckBox->isChecked());
    ui.toolTipCheckBox->setChecked(
        settings.value("Tray/show_tooltip",FALSE).toBool());

    ui.hideToTrayRadioButton->setChecked(settings.value("Tray/hide_on_close", FALSE).toBool());
    ui.closeGroupBox->setEnabled(ui.trayCheckBox->isChecked());

    QString f_dialogName =
        settings.value("FileDialog",QtFileDialogFactory::QtFileDialogFactoryName).toString();

    int ind = FileDialog::registeredFactories().indexOf(f_dialogName);
    if (ind != -1)
        ui.fileDialogComboBox->setCurrentIndex(ind);
    else
        ui.fileDialogComboBox->setCurrentIndex(0);

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

    ui.softVolumeCheckBox->setChecked(settings.value("Volume/software_volume", FALSE).toBool());
}

void ConfigDialog::changePage ( QListWidgetItem *current, QListWidgetItem *previous )
{
    if ( !current )
        current = previous;
    ui.stackedWidget->setCurrentIndex ( ui.contentsWidget->row ( current ) );
}

void ConfigDialog::changeSkin()
{
    int row = ui.listWidget->currentRow();
    QString path = m_skinList.at ( row ).canonicalFilePath();
    m_skin->setSkin ( path );
}

void ConfigDialog::loadSkins()
{
    m_skinList.clear();
    //findSkins(":/");

    QFileInfo fileInfo (":/default");
    QPixmap preview = Skin::getPixmap ("main", QDir (fileInfo.filePath()));
    QListWidgetItem *item = new QListWidgetItem (fileInfo.fileName ());
    item->setIcon ( preview );
    ui.listWidget->addItem ( item );
    m_skinList << fileInfo;

    findSkins(QDir::homePath() +"/.qmmp/skins");
    findSkins(QDir::homePath() +"/.qmmp/cache/skins");
    findSkins(qApp->applicationDirPath()+"/../share/qmmp/skins");
    connect ( ui.listWidget, SIGNAL ( itemClicked ( QListWidgetItem* ) ),
              this, SLOT ( changeSkin() ) );
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
        QPixmap preview = Skin::getPixmap ( "main", QDir ( fileInfo.filePath() ) );
        if ( !preview.isNull() )
        {
            QListWidgetItem *item = new QListWidgetItem ( fileInfo.fileName () );
            item->setIcon ( preview );
            ui.listWidget->addItem ( item );
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
    decoders = Decoder::decoderFactories();
    QStringList files = Decoder::decoderFiles();
    ui.inputPluginTable->setColumnCount ( 3 );
    ui.inputPluginTable->verticalHeader()->hide();
    ui.inputPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.inputPluginTable->setRowCount ( decoders->count () );
    for ( int i = 0; i < decoders->count (); ++i )
    {
        InputPluginItem *item = new InputPluginItem(this,decoders->at(i),files.at(i));
        QCheckBox* checkBox = new QCheckBox ( ui.inputPluginTable );
        connect(checkBox, SIGNAL(toggled(bool)), item, SLOT(setSelected(bool)));
        checkBox->setChecked(item->isSelected());
        ui.inputPluginTable->setCellWidget ( i, 0, checkBox );
        ui.inputPluginTable->setItem ( i,1,
                                       new QTableWidgetItem (item->factory()->properties().name) );
        ui.inputPluginTable->setItem ( i,2, new QTableWidgetItem (files.at (i)) );
        ui.inputPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.inputPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
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
        OutputPluginItem *item = new OutputPluginItem(this,outputs->at(i),files.at(i));
        m_outputPluginItems.append(item);
        QRadioButton* button = new QRadioButton ( ui.outputPluginTable );
        connect(button, SIGNAL(pressed ()), item, SLOT(select()));
        button->setChecked ( item->isSelected() );
        ui.outputPluginTable->setCellWidget ( i, 0, button );
        ui.outputPluginTable->setItem (i,1,
                                       new QTableWidgetItem (item->factory()->properties().name));
        ui.outputPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i)));
        ui.outputPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.outputPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    ui.outputPluginTable->resizeColumnToContents ( 0 );
    ui.outputPluginTable->resizeColumnToContents ( 1 );
    ui.outputPluginTable->resizeRowsToContents ();
    /*
        load visual plugin information
    */
    QList <VisualFactory *> *visuals = 0;
    visuals = Visual::visualFactories();
    files = Visual::visualFiles();
    ui.visualPluginTable->setColumnCount ( 3 );
    ui.visualPluginTable->verticalHeader()->hide();
    ui.visualPluginTable->setHorizontalHeaderLabels ( QStringList()
            << tr ( "Enabled" ) << tr ( "Description" ) << tr ( "Filename" ) );
    ui.visualPluginTable->setRowCount ( visuals->count () );

    for ( int i = 0; i < visuals->count (); ++i )
    {
        VisualPluginItem *item = new VisualPluginItem(this,visuals->at(i),files.at(i));
        m_visualPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.visualPluginTable);
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        button->setChecked (item->isSelected());
        ui.visualPluginTable->setCellWidget ( i, 0, button );
        ui.visualPluginTable->setItem (i,1,
                            new QTableWidgetItem (item->factory()->properties().name));
        ui.visualPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i)));
        ui.visualPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.visualPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
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
    ui.effectPluginTable->setRowCount ( visuals->count () );

    for ( int i = 0; i < effects->count (); ++i )
    {
        EffectPluginItem *item = new EffectPluginItem(this,effects->at(i),files.at(i));
        m_effectPluginItems.append(item);
        QCheckBox* button = new QCheckBox (ui.effectPluginTable);
        connect(button, SIGNAL(clicked (bool)), item, SLOT(select(bool)));
        button->setChecked (item->isSelected());
        ui.effectPluginTable->setCellWidget ( i, 0, button );
        ui.effectPluginTable->setItem (i,1,
                            new QTableWidgetItem (item->factory()->properties().name));
        ui.effectPluginTable->setItem (i,2, new QTableWidgetItem (files.at(i)));
        ui.effectPluginTable->item(i,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.effectPluginTable->item(i,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    ui.effectPluginTable->resizeColumnToContents ( 0 );
    ui.effectPluginTable->resizeColumnToContents ( 1 );
    ui.effectPluginTable->resizeRowsToContents ();
}


void ConfigDialog::loadFonts()
{
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QString fontname = settings.value ( "PlayList/Font","" ).toString();
    if ( fontname.isEmpty () )
        fontname = QFont ( "Helvetica [Cronyx]", 10 ).toString();
    ui.plFontLabel -> setText ( fontname );

    fontname = settings.value ( "MainWindow/Font","" ).toString();
    if ( fontname.isEmpty () )
        fontname = QFont ( "Helvetica [Cronyx]", 9 ).toString();
    ui.mainFontLabel -> setText ( fontname );
}

void ConfigDialog::setPlFont()
{
    bool ok;
    QFont font;
    font.fromString ( ui.plFontLabel->text() );
    font = QFontDialog::getFont ( &ok, font, this );
    if ( ok )
    {
        ui.plFontLabel -> setText ( font.toString () );
        QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
        settings.setValue ( "PlayList/Font", font.toString() );
    }
}

void ConfigDialog::setMainFont()
{
    bool ok;
    QFont font;
    font.fromString ( ui.plFontLabel->text() );
    font = QFontDialog::getFont ( &ok, font, this );
    if ( ok )
    {
        ui.mainFontLabel -> setText ( font.toString () );
        QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
        settings.setValue ( "MainWindow/Font", font.toString() );
    }
}

void ConfigDialog::showPluginSettings()
{
    switch ( ( int ) ui.pluginsTab -> currentIndex () )
    {
    case 0:
    {
        QList <DecoderFactory *> *decoders = 0;
        decoders = Decoder::decoderFactories();
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
        m_visualPluginItems.at(row)->factory()->showSettings ( this );
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
    }
}

void ConfigDialog::showPluginInfo()
{
    switch ( ( int ) ui.pluginsTab -> currentIndex () )
    {
    case 0:
    {
        QList <DecoderFactory *> *decoders = 0;
        decoders = Decoder::decoderFactories();
        int row = ui.inputPluginTable->currentRow ();
        if ( !decoders || row<0 )
            return;

        decoders->at ( row )->showAbout ( this );
        break;
    }
    case 1:
    {
        int row = ui.outputPluginTable->currentRow ();
        if ( m_outputPluginItems.isEmpty() || row < 0 )
            return;
        m_outputPluginItems.at(row)->factory()->showAbout ( this );
        break;
    }
    case 2:
    {
        int row = ui.visualPluginTable->currentRow ();
        if ( m_visualPluginItems.isEmpty() || row < 0 )
            return;
        m_visualPluginItems.at(row)->factory()->showAbout ( this );
        break;
    }
    case 3:
    {
        int row = ui.effectPluginTable->currentRow ();
        if ( m_effectPluginItems.isEmpty() || row < 0 )
            return;
        m_effectPluginItems.at(row)->factory()->showAbout ( this );
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
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.setValue ("PlayList/title_format", ui.formatLineEdit->text());
    settings.setValue ("PlayList/load_metadata", ui.metadataCheckBox->isChecked());
    settings.setValue ("MainWindow/tray_enabled", ui.trayCheckBox->isChecked());
    settings.setValue ("Tray/enabled", ui.trayCheckBox->isChecked());
    settings.setValue ("Tray/show_message", ui.messageCheckBox->isChecked());
    settings.setValue ("Tray/message_delay", ui.messageDelaySpinBox->value());
    settings.setValue ("Tray/show_tooltip", ui.toolTipCheckBox->isChecked());
    settings.setValue ("Tray/hide_on_close",ui.hideToTrayRadioButton->isChecked());
    settings.setValue ("FileDialog", ui.fileDialogComboBox->currentText());
    settings.setValue ("Proxy/use_proxy", ui.enableProxyCheckBox->isChecked());
    settings.setValue ("Proxy/authentication", ui.authProxyCheckBox->isChecked());
    settings.setValue ("Proxy/host",ui.hostLineEdit->text());
    settings.setValue ("Proxy/port",ui.portLineEdit->text());
    settings.setValue ("Proxy/user",ui.proxyUserLineEdit->text());
    settings.setValue ("Proxy/passw",ui.proxyPasswLineEdit->text());
    settings.setValue ("Volume/software_volume", ui.softVolumeCheckBox->isChecked());
}

