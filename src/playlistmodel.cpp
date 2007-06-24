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
#include <QWidget>
#include <QFile>
#include <QDir>
#include <QtAlgorithms>
#include <QFileInfo>
#include <QTextStream>
#include <QPluginLoader>
#include <QApplication>
#include <QTimer>

#include <time.h>

#include <decoder.h>
#include <decoderfactory.h>

#include "fileloader.h"
#include "playlistmodel.h"
#include "mediafile.h"
#include "playlistformat.h"
#include "playstate.h"

#include <QMetaType>

#define INVALID_ROW -1

PlayListModel::PlayListModel ( QObject *parent )
        : QObject ( parent ) , m_selection()
{
    qsrand(time(0));
    m_total_length = 0;
    m_current = 0;
    m_block_update_signals = false;
    is_repeatable_list = false;
    m_play_state = new NormalPlayState(this);
    //readSettings();

    registerPlaylistFormat( new PLSPlaylistFormat);
    registerPlaylistFormat( new M3UPlaylistFormat);
#ifndef XSPF_PLUGIN
    registerPlaylistFormat( new XSPFPlaylistFormat);
#endif
    loadExternalPlaylistFormats();

    //qRegisterMetaType<MediaFile*>("MediaFileStar");
}

PlayListModel::~PlayListModel()
{
    writeSettings();
    clear();
    delete m_play_state;
    qDeleteAll(m_registered_pl_formats);

    foreach(GuardedFileLoader l,m_running_loaders)
    {
        if (!l.isNull())
        {
            l->finish();
            l->wait();
        }
    }
}

void PlayListModel::load ( MediaFile *file )
{
    if (m_files.isEmpty())
        m_currentItem = file;

    m_total_length += file->length();
    m_files << file;

    //if (!m_block_update_signals)
    emit listChanged();
}

int PlayListModel::count()
{
    return m_files.size();
}

MediaFile* PlayListModel::currentItem()
{
    if ( m_files.isEmpty() )
        return 0;
    else
        return m_files.at ( m_current );
}

int PlayListModel::currentRow()
{
    return m_current;
}

bool PlayListModel::setCurrent ( int c )
{
    if ( c > count()-1 || c < 0)
        return FALSE;
    m_current = c;
    m_currentItem = m_files.at(c);
    emit currentChanged();
    emit listChanged();
    return TRUE;
}


bool PlayListModel::next()
{
    if (isFileLoaderRunning())
        m_play_state->prepare();

    return m_play_state->next();
}

bool PlayListModel::previous()
{
    if (isFileLoaderRunning())
        m_play_state->prepare();

    return m_play_state->previous();//)
}

void PlayListModel::clear()
{
    foreach(GuardedFileLoader l,m_running_loaders)
    {
        if (!l.isNull())
        {
            qWarning("void PlayListModel::clear()");
            l->finish();
            l->wait();
        }
    }

    m_running_loaders.clear();

    m_current = 0;
    while ( !m_files.isEmpty() )
        delete m_files.takeFirst();

    m_total_length = 0;
    m_play_state->resetState();
    emit listChanged();
}

void PlayListModel::clearSelection()
{
    for ( int i = 0; i<m_files.size(); ++i )
        m_files.at ( i )->setSelected ( FALSE );
    emit listChanged();
}

QList <QString> PlayListModel::getTitles ( int b,int l )
{
    QList <QString> m_titles;
    for ( int i = b; ( i < b + l ) && ( i < m_files.size() ); ++i )
        m_titles << m_files.at ( i )->title();
    return m_titles;
}

QList <QString> PlayListModel::getTimes ( int b,int l )
{
    QList <QString> m_times;
    for ( int i = b; ( i < b + l ) && ( i < m_files.size() ); ++i )
        m_times << QString ( "%1" ).arg ( m_files.at ( i )->length() /60 ) +":"
        +QString ( "%1" ).arg ( m_files.at ( i )->length() %60/10 ) +
        QString ( "%1" ).arg ( m_files.at ( i )->length() %60%10 );
    return m_times;
}

bool PlayListModel::isSelected ( int row )
{
	if(m_files.count() > row && row >= 0)
      return m_files.at ( row )->isSelected();
	
	return false;
}

void PlayListModel::setSelected ( int row, bool yes )
{
	if(m_files.count() > row && row >= 0)
    	m_files.at ( row )->setSelected ( yes );
}

void PlayListModel::removeSelected()
{
    removeSelection(false);
}

void PlayListModel::removeUnselected()
{
    removeSelection(true);
}

void PlayListModel::removeSelection(bool inverted)
{
    int i = 0;

    int select_after_delete = -1;

    while ( !m_files.isEmpty() && i<m_files.size() )
    {
        if ( m_files.at ( i )->isSelected() ^ inverted )
        {
            MediaFile* f = m_files.takeAt ( i );
            m_total_length -= f->length();
            if (m_total_length < 0)
                m_total_length = 0;
            delete f;

            select_after_delete = i;

            if ( m_current >= i && m_current!=0 )
                m_current--;
        }
        else
            i++;
    }

    if (!m_files.isEmpty())
        m_currentItem = m_files.at(m_current);

    if(select_after_delete >= m_files.count())
        select_after_delete = m_files.count() - 1;

    setSelected(select_after_delete,true);

    m_play_state->prepare();

    emit listChanged();
}

void PlayListModel::invertSelection()
{
    for ( int i = 0; i<m_files.size(); ++i )
        m_files.at ( i )->setSelected ( !m_files.at ( i )->isSelected() );
    emit listChanged();
}

void PlayListModel::selectAll()
{
    for ( int i = 0; i<m_files.size(); ++i )
        m_files.at ( i )->setSelected ( TRUE );
    emit listChanged();
}

void PlayListModel::showDetails()
{
    for ( int i = 0; i<m_files.size(); ++i )
    {
        if ( m_files.at ( i )->isSelected() )
        {
            DecoderFactory *fact = Decoder::findFactory ( m_files.at ( i )->path() );
            if ( fact )
                fact->showDetails ( 0, m_files.at ( i )->path() );

            return;
        }
    }

}


void PlayListModel::readSettings()
{
    QFile file ( QDir::homePath() +"/.qmmp/playlist.txt" );
    file.open ( QIODevice::ReadOnly );

    QStringList files;
    QByteArray line;
    m_files.clear();

    while ( !file.atEnd () )
    {
        line = file.readLine();
        files <<  QString::fromUtf8 ( line ).trimmed ();
    }

    file.close ();

    int preload = (files.count() < 100) ? files.count() : 100;

    for (int i = 0;i < preload;i++)
	 {
        load(new MediaFile(files.takeAt(0)));
	 }


    if (files.isEmpty())
       return;

	 FileLoader* f_loader = createFileLoader();

    f_loader->setFilesToLoad(files);
    //f_loader->start(QThread::IdlePriority);
    QTimer::singleShot(1000,f_loader,SLOT(start()));
    //m_play_state->prepare();
}

void PlayListModel::writeSettings()
{
    QFile file ( QDir::homePath() +"/.qmmp/playlist.txt" );
    file.open ( QIODevice::WriteOnly );
    foreach ( MediaFile* m, m_files )
    file.write ( m->path().toUtf8 () +"\n" );
    file.close ();
}

void PlayListModel::addFile(const QString& path)
{
    if (path.isEmpty ())
        return;
    if (Decoder::supports(path))
        load(new MediaFile(path));

    m_play_state->prepare();
}

FileLoader * PlayListModel::createFileLoader()
{
    FileLoader* f_loader = new FileLoader(this);
// f_loader->setStackSize(20 * 1024 * 1024);
    m_running_loaders << f_loader;
    connect(f_loader,SIGNAL(newMediaFile(MediaFile*)),this,SLOT(load(MediaFile*)),Qt::QueuedConnection);
    connect(f_loader,SIGNAL(finished()),this,SLOT(preparePlayState()));
    connect(f_loader,SIGNAL(finished()),f_loader,SLOT(deleteLater()));
    return f_loader;
}

void PlayListModel::addFiles(const QStringList &files)
{
    FileLoader* f_loader = createFileLoader();
    f_loader->setFilesToLoad(files);
    f_loader->start(QThread::IdlePriority);
}

void PlayListModel::addDirectory(const QString& s)
{
    FileLoader* f_loader = createFileLoader();
    f_loader->setDirectoryToLoad(s);
    f_loader->start(QThread::IdlePriority);
}

bool PlayListModel::setFileList(const QStringList & l)
{
    bool model_cleared = FALSE;
    foreach(QString str,l)
    {
        QFileInfo f_info(str);
        if (f_info.exists())
        {
            if (!model_cleared)
            {
                clear();
                model_cleared = TRUE;
            }
            if (f_info.isDir())
                addDirectory(str);
            else
                addFile(str);
        }
        // Do the processing the rest of events to avoid GUI freezing
        QApplication::processEvents(QEventLoop::AllEvents,10);
    }

    return model_cleared;
}

int PlayListModel::firstSelectedUpper(int row)
{
    for (int i = row - 1;i >= 0;i--)
    {
        if (isSelected(i))
            return i;
    }
    return -1;
}

int PlayListModel::firstSelectedLower(int row)
{
    for (int i = row + 1;i < count() ;i++)
    {
        if (isSelected(i))
            return i;
    }
    return -1;
}

void PlayListModel::moveItems( int from, int to )
{
    // Get rid of useless work
    if (from == to)
        return;

    QList<int> selected_rows = getSelectedRows();

    if (! (bottommostInSelection(from) == INVALID_ROW ||
            from == INVALID_ROW ||
            topmostInSelection(from) == INVALID_ROW)
       )
    {
        if (from > to)
            foreach(int i, selected_rows)
            if (i + to - from < 0)
                break;
            else
                m_files.move(i,i + to - from);
        else
            for (int i = selected_rows.count() - 1; i >= 0; i--)
                if (selected_rows[i] + to -from >= m_files.count())
                    break;
                else
                    m_files.move(selected_rows[i],selected_rows[i] + to - from);

        m_current = m_files.indexOf(m_currentItem);

        emit listChanged();
    }
}



int PlayListModel::topmostInSelection( int row)
{
    if ( row == 0)
        return 0;

    for (int i = row - 1;i >= 0;i--)
    {
        if (isSelected(i))
            continue;
        else
            return i + 1;
    }
    return 0;
}

int PlayListModel::bottommostInSelection( int row )
{
    if (row >= m_files.count() - 1)
        return row;

    for (int i = row + 1;i < count() ;i++)
    {
        if (isSelected(i))
            continue;
        else
            return i - 1;
    }
    return count() - 1;
}

const SimpleSelection& PlayListModel::getSelection(int row )
{
    m_selection.m_top = topmostInSelection( row );
    m_selection.m_anchor = row;
    m_selection.m_bottom = bottommostInSelection( row );
    m_selection.m_selected_rows = getSelectedRows();
    return m_selection;
}

QList<int> PlayListModel::getSelectedRows() const
{
    QList<int>selected_rows;
    for (int i = 0;i<m_files.count();i++)
    {
        if (m_files[i]->isSelected())
        {
            selected_rows.append(i);
        }
    }
    return selected_rows;
}

QList< MediaFile * > PlayListModel::getSelectedItems() const
{
    QList<MediaFile*>selected_items;
    for (int i = 0;i<m_files.count();i++)
    {
        if (m_files[i]->isSelected())
        {
            selected_items.append(m_files[i]);
        }
    }
    return selected_items;
}

void PlayListModel::addToQueue()
{
    QList<MediaFile*> selected_items = getSelectedItems();
    foreach(MediaFile* file,selected_items)
    {/*
        if (isQueued(file))
            m_queued_songs.removeAt(m_queued_songs.indexOf(file));
        else
            m_queued_songs.append(file);
		 */
		 setQueued(file);
    }
    emit listChanged();
}

void PlayListModel::setQueued(MediaFile* file)
{
	if (isQueued(file))
		m_queued_songs.removeAt(m_queued_songs.indexOf(file));
	else
		m_queued_songs.append(file);
	
	emit listChanged();
}

bool PlayListModel::isQueued(MediaFile* f) const
{
    return m_queued_songs.contains(f);
}

void PlayListModel::setCurrentToQueued()
{
    setCurrent(row(m_queued_songs.at(0)));
    m_queued_songs.pop_front();
}

bool PlayListModel::isEmptyQueue() const
{
    return m_queued_songs.isEmpty();
}

void PlayListModel::randomizeList()
{
    for (int i = 0;i < m_files.size();i++)
        m_files.swap(qrand()%m_files.size(),qrand()%m_files.size());

    m_current = m_files.indexOf(m_currentItem);
    emit listChanged();
}

void PlayListModel::reverseList()
{
    for (int i = 0;i < m_files.size()/2;i++)
        m_files.swap(i,m_files.size() - i - 1);

    m_current = m_files.indexOf(m_currentItem);
    emit listChanged();
}

////===============THE BEGINNING OF SORT IMPLEMENTATION =======================////

// First we'll implement bundle of static compare procedures
// to sort items in different ways
static bool _titleLessComparator(MediaFile* s1,MediaFile* s2)
{
    return s1->title() < s2->title();
}

static bool _titleGreaterComparator(MediaFile* s1,MediaFile* s2)
{
    return s1->title() > s2->title();
}

static bool _pathAndFilenameLessComparator(MediaFile* s1,MediaFile* s2)
{
    return s1->path() < s2->path();
}

static bool _pathAndFilenameGreaterComparator(MediaFile* s1,MediaFile* s2)
{
    return s1->path() > s2->path();
}

static bool _filenameLessComparator(MediaFile* s1,MediaFile* s2)
{
    QFileInfo i_s1(s1->path());
    QFileInfo i_s2(s2->path());
    return i_s1.baseName() < i_s2.baseName();
}

static bool _filenameGreaterComparator(MediaFile* s1,MediaFile* s2)
{
    QFileInfo i_s1(s1->path());
    QFileInfo i_s2(s2->path());
    return i_s1.baseName() > i_s2.baseName();
}

static bool _dateLessComparator(MediaFile* s1,MediaFile* s2)
{
    return s1->year() < s2->year();
}

static bool _dateGreaterComparator(MediaFile* s1,MediaFile* s2)
{
    return s1->year() > s2->year();
}

// This is main sort method
void PlayListModel::doSort(int sort_mode,QList<MediaFile*>& list_to_sort)
{
    QList<MediaFile*>::iterator begin;
    QList<MediaFile*>::iterator end;

    begin = list_to_sort.begin();
    end = list_to_sort.end();

    bool (*compareLessFunc)(MediaFile*,MediaFile*) = 0;
    bool (*compareGreaterFunc)(MediaFile*,MediaFile*) = 0;

    switch (sort_mode)
    {
    case TITLE:
        compareLessFunc = _titleLessComparator;
        compareGreaterFunc = _titleGreaterComparator;
        break;
    case FILENAME:
        compareLessFunc = _filenameLessComparator;
        compareGreaterFunc = _filenameGreaterComparator;
        break;
    case PATH_AND_FILENAME:
        compareLessFunc = _pathAndFilenameLessComparator;
        compareGreaterFunc = _pathAndFilenameGreaterComparator;
        break;
    case DATE:
        compareLessFunc = _dateLessComparator;
        compareGreaterFunc = _dateGreaterComparator;
        break;
        //qWarning("TODO Sort by Date: %s\t%d",__FILE__,__LINE__);
    default:
        compareLessFunc = _titleLessComparator;
        compareGreaterFunc = _titleGreaterComparator;
    }

    static bool sorted_asc = false;
    if (!sorted_asc)
    {
        qSort(begin,end,compareLessFunc);
        sorted_asc = true;
    }
    else
    {
        qSort(begin,end,compareGreaterFunc);
        sorted_asc = false;
    }

    m_current = m_files.indexOf(m_currentItem);
}

void PlayListModel::sortSelection(int mode)
{
    QList<MediaFile*>selected_items = getSelectedItems();
    QList<int>selected_rows = getSelectedRows();

    doSort(mode,selected_items);

    for (int i = 0;i < selected_rows.count();i++)
        m_files.replace(selected_rows[i],selected_items[i]);

    m_current = m_files.indexOf(m_currentItem);
    emit listChanged();
}

void PlayListModel::sort(int mode)
{
    doSort(mode,m_files);
    emit listChanged();
}

////=============== THE END OF SORT IMPLEMENTATION =======================////

void PlayListModel::prepareForShufflePlaying(bool val)
{
    if (m_play_state)
        delete m_play_state;

    if (val)
        m_play_state = new ShufflePlayState(this);
    else
        m_play_state = new NormalPlayState(this);

}

void PlayListModel::prepareForRepeatablePlaying(bool val)
{
    is_repeatable_list = val;
}

void PlayListModel::doCurrentVisibleRequest()
{
    emit currentChanged();
    emit listChanged();
}

void PlayListModel::setUpdatesEnabled(bool yes)
{
    if (yes)
    {
        m_block_update_signals = false;
        emit listChanged();
    }
    else
    {
        m_block_update_signals = true;
    }
}

void PlayListModel::loadPlaylist(const QString & f_name)
{

    foreach(PlaylistFormat* prs,m_registered_pl_formats.values())
    {
        if (prs->hasFormat(QFileInfo(f_name).completeSuffix().toLower()))
        {
            QFile file(f_name);
            if (file.open(QIODevice::ReadOnly))
            {
                clear();
                addFiles(prs->decode(QTextStream(&file).readAll()));
                file.close();
            }
            else
                qWarning("Error opening %s",f_name.toLocal8Bit().data());
        }
    }
}

void PlayListModel::savePlaylist(const QString & f_name)
{
    foreach(PlaylistFormat* prs,m_registered_pl_formats.values())
    {
        if (prs->hasFormat(QFileInfo(f_name).completeSuffix().toLower()))
        {
            QFile file(f_name);
            if (file.open(QIODevice::WriteOnly))
            {
                QTextStream ts(&file);
                ts << prs->encode(m_files);
                file.close();
            }
            else
                qWarning("Error opening %s",f_name.toLocal8Bit().data());
        }
    }
}


void PlayListModel::loadExternalPlaylistFormats()
{
    QDir pluginsDir (qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd("Plugins/PlaylistFormats");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
            qDebug("PlaylistFormat: plugin loaded - %s", qPrintable(fileName));

        PlaylistFormat *fmt = 0;
        if (plugin)
            fmt = qobject_cast<PlaylistFormat *>(plugin);

        if (fmt)
            if (!registerPlaylistFormat(fmt))
                qDebug("Warning: Plugin with name %s is already registered...",
                       qPrintable(fmt->name()));
    }
}

bool PlayListModel::registerPlaylistFormat(PlaylistFormat* p)
{
    QString name = p->name();
    if (!m_registered_pl_formats.contains(name))
    {
        m_registered_pl_formats.insert(name,p);
        return true;
    }
    return false;
}

bool PlayListModel::isFileLoaderRunning() const
{
    foreach(FileLoader* l,m_running_loaders)
    if (l && l->isRunning())
        return TRUE;

    return FALSE;
}

void PlayListModel::preparePlayState()
{
    m_play_state->prepare();
}








