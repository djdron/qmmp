/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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

#include <qmmpui/playlistitem.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/fileloader.h>
#include <qmmpui/generalhandler.h>
#include <qmmp/soundcore.h>

PlayList::PlayList (QWidget *parent)
        : QWidget (parent)
{
    setWindowFlags (Qt::Dialog | Qt::FramelessWindowHint);

    m_update = FALSE;
    m_resize = FALSE;
    m_anchor_row = -1;

    createMenus();


    resize (275,116);
    setMinimumSize (275,116);
    setBaseSize (275,116);
    m_listWidget = new ListWidget (this);
    m_listWidget->show();
    m_listWidget->setGeometry (12,20,243,58);

    m_plslider = new PlayListSlider (this);
    m_plslider->show();

    setSizeIncrement (25,29);
    m_skin = Skin::getPointer();

    m_buttonAdd = new Button (this,Skin::PL_BT_ADD,Skin::PL_BT_ADD);
    m_buttonAdd->move (11,86);
    m_buttonSub = new Button (this,Skin::PL_BT_SUB,Skin::PL_BT_SUB);
    m_buttonSub->move (40,86);
    m_selectButton  = new Button (this,Skin::PL_BT_SEL,Skin::PL_BT_SEL);
    m_selectButton->move (70,86);
    m_sortButton= new Button (this,Skin::PL_BT_SORT,Skin::PL_BT_SORT);
    m_sortButton->move (99,86);
    m_playlistButton = new Button (this,Skin::PL_BT_LST,Skin::PL_BT_LST);

    m_pl_control = new PlaylistControl (this);
    m_pl_control->move (0,0);
    m_pl_control->show();

    m_length_totalLength = new SymbolDisplay (this,14);
    m_length_totalLength->setAlignment (Qt::AlignLeft);
    m_length_totalLength -> show();

    m_current_time = new SymbolDisplay (this,6);
    m_current_time->show();

    m_keyboardManager = new KeyboardManager (this);

    connect (m_listWidget, SIGNAL (selectionChanged()), parent, SLOT (replay()));

    connect (m_plslider, SIGNAL (sliderMoved (int)), m_listWidget, SLOT (scroll (int)));
    connect (m_listWidget, SIGNAL (positionChanged (int, int)), m_plslider,
             SLOT (setPos (int, int)));
    connect (m_skin, SIGNAL (skinChanged()), this, SLOT (update()));
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
    m_titleBar = new PlayListTitleBar (this);
    m_titleBar->move (0,0);
    readSettings();
}


PlayList::~PlayList()
{}

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
    QAction *addFileAct = new QAction (tr ("&Add File"),this);
    addFileAct->setShortcut (tr ("F"));
    m_addMenu->addAction (addFileAct);
    connect (addFileAct, SIGNAL (triggered()), parent(), SLOT (addFile ()));
    m_actions << addFileAct;

    QAction *addDirAct = new QAction (tr ("&Add Directory"),this);
    addDirAct->setShortcut (tr ("D"));
    m_addMenu->addAction (addDirAct);
    connect (addDirAct, SIGNAL (triggered()), parent(), SLOT (addDir ()));
    m_actions << addDirAct;

    QAction *addUrlAct = new QAction (tr ("&Add Url"),this);
    addUrlAct->setShortcut (tr ("U"));
    m_addMenu->addAction (addUrlAct);
    connect (addUrlAct, SIGNAL (triggered()), parent(), SLOT (addUrl ()));
    m_actions << addUrlAct;

    //remove menu
    QAction *remSelAct = new QAction (tr ("&Remove Selected"),this);
    remSelAct->setShortcut (tr ("Del"));
    m_subMenu->addAction (remSelAct);
    connect (remSelAct, SIGNAL (triggered()),
             m_playListModel, SLOT (removeSelected ()));
    this->addAction (remSelAct);

    QAction *remAllAct = new QAction (tr ("&Remove All"),this);
    //remAllAct->setShortcut(tr("D")); FIXME: add correct shortcat
    m_subMenu->addAction (remAllAct);
    connect (remAllAct, SIGNAL (triggered()), m_playListModel, SLOT (clear ()));
    m_actions << remAllAct;

    QAction *remUnselAct = new QAction (tr ("&Remove Unselected"),this);
    m_subMenu->addAction (remUnselAct);
    connect (remUnselAct, SIGNAL (triggered()),
             m_playListModel, SLOT (removeUnselected ()));

    //listwidget menu
    QAction *detailsAct = new QAction (tr ("&View Track Details"),this);
    detailsAct->setShortcut (tr ("Alt+I"));
    m_listWidget->menu()->addAction (detailsAct);
    connect (detailsAct, SIGNAL (triggered()), m_playListModel, SLOT (showDetails ()));

    // sort menu
    m_sortMenu->addAction (detailsAct);
    m_sortMenu->addSeparator();

    QMenu* sort_mode_menu = new QMenu (tr ("Sort List"),m_sortMenu);

    QSignalMapper* signalMapper = new QSignalMapper (this);
    QAction* titleAct = sort_mode_menu->addAction (tr ("By Title"));
    connect (titleAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (titleAct, PlayListModel::TITLE);

    QAction* nameAct = sort_mode_menu->addAction (tr ("By Filename"));
    connect (nameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (nameAct, PlayListModel::FILENAME);

    QAction* pathnameAct = sort_mode_menu->addAction (tr ("By Path + Filename"));
    connect (pathnameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (pathnameAct, PlayListModel::PATH_AND_FILENAME);

    QAction* dateAct = sort_mode_menu->addAction (tr ("By Date"));
    connect (dateAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (dateAct, PlayListModel::DATE);

    QAction* trackAct = sort_mode_menu->addAction (tr ("By Track Number"));
    connect (trackAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (trackAct, PlayListModel::TRACK);

    connect (signalMapper, SIGNAL (mapped (int)),
             m_playListModel, SLOT (sort (int)));

    m_sortMenu->addMenu (sort_mode_menu);

    sort_mode_menu = new QMenu (tr ("Sort Selection"),m_sortMenu);
    signalMapper = new QSignalMapper (this);
    titleAct = sort_mode_menu->addAction (tr ("By Title"));
    connect (titleAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (titleAct, PlayListModel::TITLE);

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

    connect (signalMapper, SIGNAL (mapped (int)),
             m_playListModel, SLOT (sortSelection (int)));

    m_sortMenu->addMenu (sort_mode_menu);

    m_sortMenu->addSeparator();
    m_sortMenu->addAction (tr ("Randomize List"),m_playListModel,SLOT (randomizeList()));
    m_sortMenu->addAction (tr ("Reverse List"),m_playListModel,SLOT (reverseList()));

    m_listWidget->menu()->addSeparator();
    m_listWidget->menu()->addActions (m_subMenu->actions());
    m_listWidget->menu()->addMenu(GeneralHandler::instance()->createMenu(GeneralHandler::PLAYLIST_MENU,
                                  tr("Actions"), this));

    m_listWidget->menu()->addSeparator();
    m_listWidget->menu()->addAction(tr("&Queue"),m_playListModel, SLOT(addToQueue()), tr("Q"));
    m_actions << m_listWidget->menu()->actions();

    //select menu
    QAction *invSelAct = new QAction (tr ("Invert Selection"),this);
    m_selectMenu->addAction (invSelAct);
    connect (invSelAct, SIGNAL (triggered()),
             m_playListModel, SLOT (invertSelection ()));

    m_selectMenu->addSeparator();

    QAction *selNoneAct = new QAction (tr ("&Select None"),this);
    //selNoneAct->setShortcut(tr("Ctrl+Shift+A"));
    m_selectMenu->addAction (selNoneAct);
    connect (selNoneAct, SIGNAL (triggered()),
             m_playListModel, SLOT (clearSelection ()));
    this->addAction (selNoneAct);

    QAction *selAllAct = new QAction (tr ("&Select All"),this);
    selAllAct->setShortcut (tr ("Ctrl+A"));
    m_selectMenu->addAction (selAllAct);
    connect (selAllAct, SIGNAL (triggered()),
             m_playListModel, SLOT (selectAll ()));
    this->addAction (selAllAct);

//  Playlist Menu
    QAction *newListAct = new QAction (tr ("&New List"),this);
    newListAct->setShortcut (tr ("Shift+N"));
    m_actions << newListAct;
    m_playlistMenu->addAction (newListAct);
    connect (newListAct, SIGNAL (triggered()), this, SIGNAL (newPlaylist()));
    m_playlistMenu->addSeparator();

    QAction *loadListAct = new QAction (tr ("&Load List"),this);
    loadListAct->setShortcut (tr ("O"));
    m_playlistMenu->addAction (loadListAct);
    connect (loadListAct, SIGNAL (triggered()), this, SIGNAL (loadPlaylist()));

    QAction *saveListAct = new QAction (tr ("&Save List"),this);
    saveListAct->setShortcut (tr ("Shift+S"));
    m_playlistMenu->addAction (saveListAct);
    connect (saveListAct, SIGNAL (triggered()), this, SIGNAL (savePlaylist()));
    this->addActions (m_playlistMenu->actions());

    Dock::getPointer()->addActions (m_actions);
}

void PlayList::closeEvent (QCloseEvent *e)
{
    if (e->spontaneous ())
        emit closed();
    writeSettings();
}

void PlayList::paintEvent (QPaintEvent *)
{
    int m_sx = (width()-275) /25;
    int m_sy = (height()-116) /29;
    drawPixmap (m_sx, m_sy);
}

void PlayList::drawPixmap (int sx, int sy)
{
    QPainter paint;
    paint.begin (this);
    paint.drawPixmap (0,20,m_skin->getPlPart (Skin::PL_LFILL));
    for (int i = 1; i<sy+2; i++)
    {
        paint.drawPixmap (0,20+29*i,m_skin->getPlPart (Skin::PL_LFILL));
    }
    paint.drawPixmap (0,78+29*sy,m_skin->getPlPart (Skin::PL_LSBAR));
    for (int i = 0; i<sx; i++)
    {
        paint.drawPixmap (125+i*25,78+sy*29,m_skin->getPlPart (Skin::PL_SFILL1));
    }

    paint.drawPixmap (125+sx*25,78+sy*29,m_skin->getPlPart (Skin::PL_RSBAR));
    paint.end();

}

void PlayList::resizeEvent (QResizeEvent *e)
{
    int sx = (e->size().width()-275) /25;
    int sy = (e->size().height()-116) /29;
    if (sx < 0 || sy < 0)
        return;

    m_titleBar->resize (275+25*sx,20);
    m_plslider->resize (20,58+sy*29);

    m_listWidget->resize (243+25*sx,58+29*sy);

    m_buttonAdd->move (11,86+29*sy);
    m_buttonSub->move (40,86+29*sy);
    m_selectButton->move (70,86+29*sy);
    m_sortButton->move (99,86+29*sy);

    m_pl_control->move (128+sx*25,100+29*sy);
    m_playlistButton->move (228+sx*25,86+29*sy);

    m_length_totalLength -> move (131+sx*25,88+29*sy);
    m_current_time->move (190+sx*25,101+29*sy);

    m_plslider->move (255+sx*25,20);
}
void PlayList::mousePressEvent (QMouseEvent *e)
{
    m_pos = e->pos ();
    if ((m_pos.x() > width()-25) && (m_pos.y() > height()-25))
    {
        m_resize = TRUE;
        setCursor (Qt::SizeFDiagCursor);
    }
    else
        m_resize = FALSE;
}
void PlayList::mouseMoveEvent (QMouseEvent *e)
{
    if (m_resize)
    {
        resize (e->x() +25, e->y() +25);
        //usleep(32000);
    }
}
void PlayList::mouseReleaseEvent (QMouseEvent *)
{
    setCursor (Qt::ArrowCursor);
    /*if (m_resize)
        m_listWidget->updateList();*/
    m_resize = FALSE;
    Dock::getPointer()->updateDock();
}
void PlayList::setModel (PlayListModel *model)
{
    m_playListModel = model;
    m_listWidget->setModel (model);
    m_keyboardManager->setModel (model);
    m_titleBar->setModel (model);
    createActions();
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
    if (m_update)
    {
        m_listWidget->readSettings();
        m_titleBar->readSettings();
    }
    else
    {
        QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
        settings.beginGroup ("PlayList");
        //position
        move (settings.value ("pos", QPoint (100, 332)).toPoint());
        settings.endGroup();
        m_update = TRUE;
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
    int minutes = sec / 60;
    int seconds = sec % 60;

    QString str_minutes = QString::number (minutes);
    QString str_seconds = QString::number (seconds);

    if (minutes < 10) str_minutes.prepend ("0");
    if (seconds < 10) str_seconds.prepend ("0");

    return str_minutes + ":" + str_seconds;
}

void PlayList::setTime(qint64 time)
{
    if (time < 0)
        m_current_time->display ("--:--");
    else
        m_current_time->display (formatTime (time));
    m_current_time->update();

    if (m_playListModel && SoundCore::instance())
    {
        m_playListModel->totalLength();
        QString str_length = formatTime (m_playListModel->totalLength()) +
                             "/" + formatTime (SoundCore::instance()->length());
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
    return m_playListModel ? m_playListModel->currentItem() : 0;
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
