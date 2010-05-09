/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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
#include <QPainter>
#include <QResizeEvent>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QSignalMapper>
#include <QHBoxLayout>
#include <QCloseEvent>

#include "dock.h"
#include "playlist.h"
#include "skin.h"
#include "listwidget.h"
#include "button.h"

#include "playlisttitlebar.h"
#include "playlistslider.h"
#include "pixmapwidget.h"
#include "symboldisplay.h"
#include "playlistcontrol.h"
#include "keyboardmanager.h"
#include "playlistbrowser.h"
#include "playlistselector.h"
#include <qmmpui/playlistitem.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/fileloader.h>
#include <qmmpui/generalhandler.h>
#include <qmmp/soundcore.h>

PlayList::PlayList (PlayListManager *manager, QWidget *parent)
        : QWidget (parent)
{
    setAttribute(Qt::WA_AlwaysShowToolTips,true);
    m_pl_manager = manager;
    m_update = false;
    m_resize = false;
    m_skin = Skin::instance();
    m_ratio = m_skin->ratio();
    m_shaded = false;
    m_pl_browser = 0;
    m_pl_selector = 0;

    resize (275*m_ratio, 116*m_ratio);
    setSizeIncrement (25*m_ratio, 29*m_ratio);
    setMinimumSize(275*m_ratio, 116*m_ratio);

    m_listWidget = new ListWidget (this);
    m_plslider = new PlayListSlider (this);
    m_buttonAdd = new Button (this,Skin::PL_BT_ADD,Skin::PL_BT_ADD, Skin::CUR_PNORMAL);
    m_buttonSub = new Button (this,Skin::PL_BT_SUB,Skin::PL_BT_SUB, Skin::CUR_PNORMAL);
    m_selectButton  = new Button (this,Skin::PL_BT_SEL,Skin::PL_BT_SEL, Skin::CUR_PNORMAL);
    m_sortButton= new Button (this,Skin::PL_BT_SORT,Skin::PL_BT_SORT, Skin::CUR_PNORMAL);
    m_playlistButton = new Button (this,Skin::PL_BT_LST,Skin::PL_BT_LST, Skin::CUR_PNORMAL);
    m_resizeWidget = new QWidget(this);
    m_resizeWidget->resize(25,25);
    m_resizeWidget->setCursor(m_skin->getCursor (Skin::CUR_PSIZE));
    m_pl_control = new PlaylistControl (this);

    m_length_totalLength = new SymbolDisplay (this,14);
    m_length_totalLength->setAlignment (Qt::AlignLeft);

    m_current_time = new SymbolDisplay (this,6);
    m_keyboardManager = new KeyboardManager (this);

    connect (m_listWidget, SIGNAL (selectionChanged()), parent, SLOT (replay()));

    connect (m_plslider, SIGNAL (sliderMoved (int)), m_listWidget, SLOT (scroll (int)));
    connect (m_listWidget, SIGNAL (positionChanged (int, int)), m_plslider,
             SLOT (setPos (int, int)));
    connect (m_skin, SIGNAL (skinChanged()), this, SLOT (updateSkin()));
    connect (m_buttonAdd, SIGNAL (clicked()), SLOT (showAddMenu()));
    connect (m_buttonSub, SIGNAL (clicked()), SLOT (showSubMenu()));
    connect (m_selectButton, SIGNAL (clicked()), SLOT (showSelectMenu()));
    connect (m_sortButton, SIGNAL (clicked()), SLOT (showSortMenu()));
    connect (m_playlistButton, SIGNAL (clicked()), SLOT (showPlaylistMenu()));

    connect (m_pl_control, SIGNAL (nextClicked()), SIGNAL (next()));
    connect (m_pl_control, SIGNAL (previousClicked()), SIGNAL (prev()));
    connect (m_pl_control, SIGNAL (playClicked()), SIGNAL (play()));
    connect (m_pl_control, SIGNAL (pauseClicked()), SIGNAL (pause()));
    connect (m_pl_control, SIGNAL (stopClicked()), SIGNAL (stop()));
    connect (m_pl_control, SIGNAL (ejectClicked()), SIGNAL (eject()));

    connect (m_pl_manager, SIGNAL (selectedPlayListChanged(PlayListModel *, PlayListModel *)),
             m_listWidget, SLOT(setModel(PlayListModel*)));
    m_listWidget->setModel(m_pl_manager->selectedPlayList());

    m_titleBar = new PlayListTitleBar (this);
    m_titleBar->move (0,0);
    connect (m_pl_manager, SIGNAL (currentPlayListChanged(PlayListModel *, PlayListModel *)),
             m_titleBar, SLOT(setModel(PlayListModel*)));
    m_titleBar->setModel(m_pl_manager->currentPlayList());

    createMenus();
    createActions();

    readSettings();
    setCursor(m_skin->getCursor(Skin::CUR_PNORMAL));
    updatePositions();
}

PlayList::~PlayList()
{
    delete m_keyboardManager;
}

void PlayList::updatePositions()
{
    int sx = (width()-275*m_ratio)/25;
    int sy = (height()-116*m_ratio)/29;
    if (sx < 0 || sy < 0 || m_shaded) //skip shaded mode
        return;

    m_titleBar->resize (275*m_ratio+25*sx, 20*m_ratio);
    m_plslider->resize (20*m_ratio, 58*m_ratio+sy*29);

    if(m_pl_selector)
    {
        m_listWidget->resize (243*m_ratio+25*sx, 58*m_ratio+29*sy - m_pl_selector->height());
        m_pl_selector->resize(243*m_ratio+25*sx, m_pl_selector->height());
        m_pl_selector->move(12*m_ratio, 20*m_ratio + 58*m_ratio+29*sy - m_pl_selector->height());
    }
    else
        m_listWidget->resize (243*m_ratio+25*sx, 58*m_ratio+29*sy);
    m_listWidget->move (12*m_ratio,20*m_ratio);

    m_buttonAdd->move (11*m_ratio, 86*m_ratio+29*sy);
    m_buttonSub->move (40*m_ratio, 86*m_ratio+29*sy);
    m_selectButton->move (70*m_ratio, 86*m_ratio+29*sy);
    m_sortButton->move (99*m_ratio, 86*m_ratio+29*sy);

    m_pl_control->move (128*m_ratio+sx*25, 100*m_ratio+29*sy);
    m_playlistButton->move (228*m_ratio+sx*25,86*m_ratio+29*sy);

    m_length_totalLength -> move (131*m_ratio+sx*25,88*m_ratio+29*sy);
    m_current_time->move (190*m_ratio+sx*25,101*m_ratio+29*sy);

    m_plslider->move (255*m_ratio+sx*25,20*m_ratio);
    m_resizeWidget->move(width() - 25, height() - 29);
}

void PlayList::createMenus()
{
    m_addMenu = new QMenu (this);
    m_subMenu = new QMenu (this);
    m_selectMenu = new QMenu (this);
    m_sortMenu = new QMenu (this);
    m_playlistMenu = new QMenu (this);
}

void PlayList::createActions()
{
    //add menu
    QAction *addFileAct = new QAction (QIcon::fromTheme("audio-x-generic"), tr("&Add File"), this);
    addFileAct->setShortcut (tr ("F"));
    m_addMenu->addAction (addFileAct);
    connect (addFileAct, SIGNAL (triggered()), parent(), SLOT (addFile ()));
    m_actions << addFileAct;

    QAction *addDirAct = new QAction (QIcon::fromTheme("folder"), tr("&Add Directory"),this);
    addDirAct->setShortcut (tr ("D"));
    m_addMenu->addAction (addDirAct);
    connect (addDirAct, SIGNAL (triggered()), parent(), SLOT (addDir ()));
    m_actions << addDirAct;

    QAction *addUrlAct = new QAction (QIcon::fromTheme("network-server"), tr("&Add Url"),this);
    addUrlAct->setShortcut (tr ("U"));
    m_addMenu->addAction (addUrlAct);
    connect (addUrlAct, SIGNAL (triggered()), parent(), SLOT (addUrl ()));
    m_actions << addUrlAct;

    //remove menu
    QAction *remSelAct = new QAction (QIcon::fromTheme("edit-delete"), tr("&Remove Selected"),this);
    remSelAct->setShortcut (tr ("Del"));
    m_subMenu->addAction (remSelAct);
    connect (remSelAct, SIGNAL (triggered()), m_pl_manager, SLOT (removeSelected ()));
    addAction (remSelAct);

    QAction *remAllAct = new QAction (QIcon::fromTheme("edit-clear"), tr("&Remove All"),this);
    //remAllAct->setShortcut(tr("D")); FIXME: add correct shortcat
    m_subMenu->addAction (remAllAct);
    connect (remAllAct, SIGNAL (triggered()), m_pl_manager, SLOT (clear ()));
    m_actions << remAllAct;

    QAction *remUnselAct = new QAction (QIcon::fromTheme("edit-delete"), tr("&Remove Unselected"),this);
    m_subMenu->addAction (remUnselAct);
    connect (remUnselAct, SIGNAL (triggered()), m_pl_manager, SLOT (removeUnselected ()));

    m_subMenu->addSeparator();
    m_subMenu->addAction (QIcon::fromTheme("dialog-error"), tr("Remove unavailable files"),
                          m_pl_manager, SLOT(removeInvalidItems()));
    m_subMenu->addAction (tr("Remove duplicates"), m_pl_manager, SLOT(removeDuplicates()));

    //listwidget menu
    QAction *detailsAct = new QAction (QIcon::fromTheme("dialog-information"),
                                       tr("&View Track Details"),this);
    detailsAct->setShortcut (tr ("Alt+I"));
    m_listWidget->menu()->addAction (detailsAct);
    connect (detailsAct, SIGNAL (triggered()), m_pl_manager, SLOT (showDetails ()));

    // sort menu
    m_sortMenu->addAction (detailsAct);
    m_sortMenu->addSeparator();

    QMenu* sort_mode_menu = new QMenu (tr("Sort List"), m_sortMenu);
    sort_mode_menu->setIcon(QIcon::fromTheme("view-sort-ascending"));
    QSignalMapper* signalMapper = new QSignalMapper (this);
    QAction* titleAct = sort_mode_menu->addAction (tr ("By Title"));
    connect (titleAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (titleAct, PlayListModel::TITLE);

    QAction* albumAct = sort_mode_menu->addAction (tr ("By Album"));
    connect (albumAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (albumAct, PlayListModel::ALBUM);

    QAction* artistAct = sort_mode_menu->addAction (tr ("By Artist"));
    connect (artistAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (artistAct, PlayListModel::ARTIST);

    QAction* nameAct = sort_mode_menu->addAction (tr ("By Filename"));
    connect (nameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (nameAct, PlayListModel::FILENAME);

    QAction* pathnameAct = sort_mode_menu->addAction (tr ("By Path + Filename"));
    connect (pathnameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (pathnameAct, PlayListModel::PATH_AND_FILENAME);

    QAction* dateAct = sort_mode_menu->addAction (tr ("By Date"));
    connect (dateAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (dateAct, PlayListModel::DATE);

    QAction* trackAct = sort_mode_menu->addAction (tr("By Track Number"));
    connect (trackAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (trackAct, PlayListModel::TRACK);

    connect (signalMapper, SIGNAL (mapped (int)), m_pl_manager, SLOT (sort (int)));

    m_sortMenu->addMenu (sort_mode_menu);

    sort_mode_menu = new QMenu (tr("Sort Selection"), m_sortMenu);
    sort_mode_menu->setIcon(QIcon::fromTheme("view-sort-ascending"));
    signalMapper = new QSignalMapper (this);
    titleAct = sort_mode_menu->addAction (tr ("By Title"));
    connect (titleAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (titleAct, PlayListModel::TITLE);

    albumAct = sort_mode_menu->addAction (tr ("By Album"));
    connect (albumAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (albumAct, PlayListModel::ALBUM);

    artistAct = sort_mode_menu->addAction (tr ("By Artist"));
    connect (artistAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (artistAct, PlayListModel::ARTIST);

    nameAct = sort_mode_menu->addAction (tr ("By Filename"));
    connect (nameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (nameAct, PlayListModel::FILENAME);

    pathnameAct = sort_mode_menu->addAction (tr ("By Path + Filename"));
    connect (pathnameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (pathnameAct, PlayListModel::PATH_AND_FILENAME);

    dateAct = sort_mode_menu->addAction (tr ("By Date"));
    connect (dateAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (dateAct, PlayListModel::DATE);

    trackAct = sort_mode_menu->addAction (tr ("By Track Number"));
    connect (trackAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (trackAct, PlayListModel::TRACK);

    connect (signalMapper, SIGNAL (mapped (int)), m_pl_manager, SLOT (sortSelection (int)));

    m_sortMenu->addMenu (sort_mode_menu);

    m_sortMenu->addSeparator();
    m_sortMenu->addAction (QIcon::fromTheme("media-playlist-shuffle"), tr("Randomize List"),
                           m_pl_manager, SLOT(randomizeList()));
    m_sortMenu->addAction (QIcon::fromTheme("view-sort-descending"), tr("Reverse List"),
                           m_pl_manager, SLOT(reverseList()));

    m_listWidget->menu()->addSeparator();
    m_listWidget->menu()->addActions (m_subMenu->actions().mid(0,3)); //use 3 first actions
    m_listWidget->menu()->addMenu(GeneralHandler::instance()->createMenu(GeneralHandler::PLAYLIST_MENU,
                                  tr("Actions"), this));

    m_listWidget->menu()->addSeparator();
    m_listWidget->menu()->addAction(tr("&Queue"),m_pl_manager, SLOT(addToQueue()), tr("Q"));
    m_actions << m_listWidget->menu()->actions();

    //select menu
    QAction *invSelAct = new QAction (tr ("Invert Selection"),this);
    m_selectMenu->addAction (invSelAct);
    connect (invSelAct, SIGNAL (triggered()), m_pl_manager, SLOT (invertSelection ()));

    m_selectMenu->addSeparator();

    QAction *selNoneAct = new QAction (tr ("&Select None"),this);
    selNoneAct->setShortcut(tr("Shift+A"));
    m_selectMenu->addAction (selNoneAct);
    connect (selNoneAct, SIGNAL (triggered()), m_pl_manager, SLOT (clearSelection ()));
    this->addAction (selNoneAct);

    QAction *selAllAct = new QAction (QIcon::fromTheme("edit-select-all"), tr ("&Select All"),this);
    selAllAct->setShortcut (tr ("Ctrl+A"));
    m_actions << selAllAct;
    m_selectMenu->addAction (selAllAct);
    connect (selAllAct, SIGNAL (triggered()), m_pl_manager, SLOT (selectAll ()));

    //  Playlist Menu
    QAction *newListAct = new QAction (QIcon::fromTheme("document-new"), tr("&New List"), this);
    newListAct->setShortcut (tr ("Ctrl+T"));
    m_actions << newListAct;
    m_playlistMenu->addAction (newListAct);
    connect (newListAct, SIGNAL (triggered()), m_pl_manager, SLOT (createPlayList()));
    QAction *deleteListAct = new QAction (QIcon::fromTheme("window-close"), tr ("&Delete List"),this);
    deleteListAct->setShortcut (tr ("Ctrl+W"));
    m_actions << deleteListAct;
    m_playlistMenu->addAction (deleteListAct);
    connect (deleteListAct, SIGNAL (triggered()), SLOT (deletePlaylist()));
    m_playlistMenu->addSeparator();

    QAction *loadListAct = new QAction (QIcon::fromTheme("document-open"), tr("&Load List"), this);
    loadListAct->setShortcut (tr ("O"));
    m_actions << loadListAct;
    m_playlistMenu->addAction (loadListAct);
    connect (loadListAct, SIGNAL (triggered()), this, SIGNAL (loadPlaylist()));

    QAction *saveListAct = new QAction (QIcon::fromTheme("document-save-as"), tr("&Save List"), this);
    saveListAct->setShortcut (tr ("Shift+S"));
    m_actions << saveListAct;
    m_playlistMenu->addAction (saveListAct);
    connect (saveListAct, SIGNAL (triggered()), this, SIGNAL (savePlaylist()));

    m_playlistMenu->addSeparator();
    QAction *nextListAct = new QAction (QIcon::fromTheme("go-next"),
                                        tr("&Select Next Playlist"), this);
    nextListAct->setShortcut (tr ("Ctrl+PgDown"));
    m_actions << nextListAct;
    m_playlistMenu->addAction (nextListAct);
    connect (nextListAct, SIGNAL (triggered()), m_pl_manager, SLOT (selectNextPlayList()));

    QAction *previousListAct = new QAction (QIcon::fromTheme("go-previous"),
                                            tr("&Select Previous Playlist"),this);
    previousListAct->setShortcut (tr ("Ctrl+PgUp"));
    m_actions << previousListAct;
    m_playlistMenu->addAction (previousListAct);
    connect (previousListAct, SIGNAL (triggered()), m_pl_manager, SLOT (selectPreviousPlayList()));

    QAction *browseListsAct = new QAction (QIcon::fromTheme("view-list-details"),
                                           tr("&Show Playlists"), this);
    browseListsAct->setShortcut (tr ("P"));
    m_actions << browseListsAct;
    m_playlistMenu->addAction (browseListsAct);
    connect (browseListsAct, SIGNAL (triggered()), SLOT (showPlayLists()));

    Dock::instance()->addActions (m_actions);
}

void PlayList::closeEvent (QCloseEvent *e)
{
    if (e->spontaneous ())
        emit closed();
    writeSettings();
}

void PlayList::paintEvent (QPaintEvent *)
{
    int m_sx = (width()-275*m_ratio) /25;
    int m_sy = (height()-116*m_ratio) /29;
    drawPixmap (m_sx, m_sy);
}

void PlayList::drawPixmap (int sx, int sy)
{
    QPainter paint;
    paint.begin (this);
    paint.drawPixmap (0,20*m_ratio, m_skin->getPlPart (Skin::PL_LFILL));
    for (int i = 1; i<sy+2*m_ratio; i++)
    {
        paint.drawPixmap (0,20*m_ratio+29*i,m_skin->getPlPart (Skin::PL_LFILL));
    }
    paint.drawPixmap (0,78*m_ratio+29*sy,m_skin->getPlPart (Skin::PL_LSBAR));
    for (int i = 0; i<sx; i++)
    {
        paint.drawPixmap (125*m_ratio+i*25,78*m_ratio+sy*29,m_skin->getPlPart (Skin::PL_SFILL1));
    }
    paint.drawPixmap (125*m_ratio+sx*25,78*m_ratio+sy*29,m_skin->getPlPart (Skin::PL_RSBAR));
    paint.end();
}

void PlayList::resizeEvent (QResizeEvent *)
{
    updatePositions();
}

void PlayList::mousePressEvent (QMouseEvent *e)
{
    m_pos = e->pos ();
    if (m_resizeWidget->underMouse())
    {
        m_resize = true;
        setCursor (m_skin->getCursor (Skin::CUR_PSIZE));
    }
    else
        m_resize = false;
}

void PlayList::mouseMoveEvent (QMouseEvent *e)
{
    if (m_resize)
    {
#ifdef Q_OS_WIN32
        int sx = (width()-275) /25;
        int sy = (height()-116) /29;
	if(width() < e->x() - 14)
           sx++;
	else if(width() > e->x() + 14)
           sx--;
        if(height() < e->y() - 14)
           sy++;	
        else if(height() > e->y() + 14)
           sy--;
        resize (275+25*sx,116+29*sy);
#else
        resize (e->x() +25, e->y() +25);
#endif
    }
}

void PlayList::mouseReleaseEvent (QMouseEvent *)
{
    setCursor (m_skin->getCursor (Skin::CUR_PNORMAL));
    /*if (m_resize)
        m_listWidget->updateList();*/
    m_resize = false;
    Dock::instance()->updateDock();
}

void PlayList::changeEvent (QEvent * event)
{
    if (event->type() == QEvent::ActivationChange)
    {
        m_titleBar->setActive (isActiveWindow());
    }
}

void PlayList::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    if (settings.value("PlayList/show_plalists", false).toBool())
    {
        if(!m_pl_selector)
            m_pl_selector = new PlayListSelector(m_pl_manager, this);
        m_pl_selector->show();
    }
    else
    {
        if(m_pl_selector)
            m_pl_selector->deleteLater();
        m_pl_selector = 0;
    }
    if (m_update)
    {
        m_listWidget->readSettings();
        m_titleBar->readSettings();
        if(m_pl_selector)
            m_pl_selector->readSettings();
        updatePositions();
    }
    else
    {
        if (settings.value("General/openbox_compat", false).toBool() ||
            settings.value("General/metacity_compat", false).toBool())
            setWindowFlags (Qt::Tool | Qt::FramelessWindowHint);
        else
            setWindowFlags (Qt::Dialog | Qt::FramelessWindowHint); 
        move (settings.value ("PlayList/pos", QPoint (100, 332)).toPoint());  //position
        m_update = true;
    }
}

void PlayList::writeSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup ("PlayList");
    //position
    settings.setValue ("pos", this->pos());
    settings.endGroup();
}

void PlayList::showAddMenu()
{
    m_addMenu->exec (m_buttonAdd->mapToGlobal (QPoint (0,0)));
}

void PlayList::showSubMenu()
{
    m_subMenu->exec (m_buttonSub->mapToGlobal (QPoint (0,0)));
}

void PlayList::showSelectMenu()
{
    m_selectMenu->exec (m_selectButton->mapToGlobal (QPoint (0,0)));
}

void PlayList::showSortMenu()
{
    m_sortMenu->exec (m_sortButton->mapToGlobal (QPoint (0,0)));
}

QString PlayList::formatTime (int sec)
{
    if(sec > 3600)
        sec /= 60;
    return QString("%1:%2").arg(sec/60, 2, 10, QChar('0')).arg(sec%60, 2, 10, QChar('0'));
}

void PlayList::setTime(qint64 time)
{
    if (time < 0)
        m_current_time->display ("--:--");
    else
        m_current_time->display (formatTime (time/1000));
    m_current_time->update();

    if (SoundCore::instance())
    {
        QString str_length = formatTime (m_pl_manager->currentPlayList()->totalLength()) +
                             "/" + formatTime (SoundCore::instance()->totalTime()/1000);
        m_length_totalLength->display (str_length);
        m_length_totalLength->update();
    }
}

void PlayList::updateList()
{
    m_listWidget->updateList();
    m_titleBar->showCurrent();
}

PlayListItem *PlayList::currentItem()
{
    return m_pl_manager->currentPlayList()->currentItem();
}

void PlayList::showPlaylistMenu()
{
    m_playlistMenu->exec (m_playlistButton->mapToGlobal (QPoint (0,0)));
}

void PlayList::keyPressEvent (QKeyEvent *ke)
{
    if (m_keyboardManager->handleKeyPress (ke))
        update();
}

void PlayList::updateSkin()
{
    setCursor(m_skin->getCursor(Skin::CUR_PNORMAL)); // TODO shaded
    m_resizeWidget->setCursor(m_skin->getCursor (Skin::CUR_PSIZE));
    m_ratio = m_skin->ratio();
    setMinimalMode(m_shaded);
}

void PlayList::deletePlaylist()
{
    m_pl_manager->removePlayList(m_pl_manager->selectedPlayList());
}

void PlayList::showPlayLists()
{
    if(!m_pl_browser)
    {
        m_pl_browser = new PlayListBrowser(m_pl_manager, this);
        m_pl_browser->show();
    }
    else
        m_pl_browser->show();
}

void PlayList::setMinimalMode(bool b)
{
    if(!m_shaded)
        m_height = height();
    m_shaded = b;
    if(m_shaded)
    {
        m_height = height();
        setSizeIncrement (25*m_ratio, 1);
        setMinimumSize (275*m_ratio, 14*m_ratio);
        resize(width(), 14*m_ratio);
    }
    else
    {
        setMinimumSize(275*m_ratio, 116*m_ratio);
        resize (width(), m_height);
        setSizeIncrement (25*m_ratio, 29*m_ratio);

    }
    updatePositions();
    update();
}
