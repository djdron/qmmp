/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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

#include <QFile>
#include <QDBusMetaType>
#include <QDBusArgument>
#include <QDBusMessage>
#include <QDBusConnection>
#include <qmmp/soundcore.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistmanager.h>
#include "player2object.h"

Player2Object::Player2Object(QObject *parent) : QObject(parent)
{
    m_prev_item = 0;
    m_previous_pos = 0;
    m_core = SoundCore::instance();
    m_player = MediaPlayer::instance();
    m_pl_manager =  m_player->playListManager();
    connect(m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(emitPropertiesChanged()));
    connect(m_core, SIGNAL(metaDataChanged ()), SLOT(updateId()));
    connect(m_core, SIGNAL(metaDataChanged ()), SLOT(emitPropertiesChanged()));
    connect(m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(emitPropertiesChanged()));
    connect(m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(checkState(Qmmp::State)));
    connect(m_core, SIGNAL(volumeChanged(int,int)), SLOT(emitPropertiesChanged()));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(checkSeeking(qint64)));
    connect(m_pl_manager, SIGNAL(repeatableListChanged(bool)), SLOT(emitPropertiesChanged()));
    connect(m_pl_manager, SIGNAL(shuffleChanged(bool)), SLOT(emitPropertiesChanged()));
    connect(m_player, SIGNAL(repeatableChanged(bool)), SLOT(emitPropertiesChanged()));
    syncProperties();
}

Player2Object::~Player2Object()
{}

bool Player2Object::canControl() const
{
    return true;
}

bool Player2Object::canGoNext() const
{
    return m_pl_manager->currentPlayList()->nextItem() != 0;
}

bool Player2Object::canGoPrevious() const
{
    return m_pl_manager->currentPlayList()->currentRow() > 0;
}

bool Player2Object::canPause() const
{
    return (m_core->state() == Qmmp::Paused || m_core->state() == Qmmp::Playing);
}
bool Player2Object::canPlay() const
{
    return m_pl_manager->currentPlayList()->count() != 0;
}

bool Player2Object::canSeek() const
{
    return m_core->totalTime() > 0;
}

QString Player2Object::loopStatus() const
{
    if(m_player->isRepeatable())
        return "Track";
    else if(m_pl_manager->isRepeatableList())
        return "Playlist";
    else
        return "None";
}

void Player2Object::setLoopStatus(const QString &value)
{
    if(value == "Track")
    {
        m_player->setRepeatable(true);
    }
    else if(value == "Playlist")
    {
        m_pl_manager->setRepeatableList(true);
        m_player->setRepeatable(false);
    }
    else
    {
        m_pl_manager->setRepeatableList(false);
        m_player->setRepeatable(false);
    }
}

double Player2Object::maximumRate() const
{
    return 1.0;
}

QVariantMap Player2Object::metadata() const
{
    PlayListItem *item = m_pl_manager->currentPlayList()->currentItem();
    if(!item || m_core->metaData(Qmmp::URL).isEmpty())
        return QVariantMap();
    QVariantMap map;
    map["mpris:length"] = m_core->totalTime() * 1000;
    if(!MetaDataManager::instance()->getCoverPath(m_core->metaData(Qmmp::URL)).isEmpty())
        map["mpris:artUrl"] = MetaDataManager::instance()->getCoverPath(m_core->metaData(Qmmp::URL));
    if(!m_core->metaData(Qmmp::ALBUM).isEmpty())
        map["xesam:album"] = m_core->metaData(Qmmp::ALBUM);
    if(!m_core->metaData(Qmmp::ARTIST).isEmpty())
        map["xesam:artist"] = QStringList() << m_core->metaData(Qmmp::ARTIST);
    if(!m_core->metaData(Qmmp::COMMENT).isEmpty())
        map["xseam:comment"] = QStringList() << m_core->metaData(Qmmp::COMMENT);
    if(!m_core->metaData(Qmmp::COMPOSER).isEmpty())
        map["xesam:composer"] = QStringList() << m_core->metaData(Qmmp::COMPOSER);
    if(!m_core->metaData(Qmmp::DISCNUMBER).isEmpty())
        map["xesam:discNumber"] = m_core->metaData(Qmmp::DISCNUMBER).toInt();
    if(!m_core->metaData(Qmmp::GENRE).isEmpty())
        map["xesam:genre"] = QStringList() << m_core->metaData(Qmmp::GENRE);
    if(!m_core->metaData(Qmmp::TITLE).isEmpty())
        map["xesam:title"] = QStringList() << m_core->metaData(Qmmp::TITLE);
    if(!m_core->metaData(Qmmp::TRACK).isEmpty())
        map["xesam:trackNumber"] = m_core->metaData(Qmmp::TRACK);
    map["mpris:trackid"] = m_trackID;
    map["xesam:url"] = m_core->metaData(Qmmp::URL);
    return map;
}

double Player2Object::minimumRate() const
{
    return 1.0;
}

QString Player2Object::playbackStatus() const
{
    if(m_core->state() == Qmmp::Playing)
        return "Playing";
    else if (m_core->state() == Qmmp::Paused)
        return "Paused";
    return "Stopped";
}

qlonglong Player2Object::position() const
{
    return m_core->elapsed() * 1000;
}

double Player2Object::rate() const
{
    return 1.0;
}

void Player2Object::setRate(double value)
{
    Q_UNUSED(value)
}

bool Player2Object::shuffle() const
{
    return m_pl_manager->isShuffle();
}

void Player2Object::setShuffle(bool value)
{
    m_pl_manager->setShuffle(value);
}

double Player2Object::volume() const
{
    return qMax(m_core->leftVolume(), m_core->rightVolume())/100.0;
}

void Player2Object::Player2Object::setVolume(double value)
{
    value = qBound(0.0, value ,1.0);
    int balance = (volume() > 0) ? (m_core->rightVolume() - m_core->leftVolume())/volume() : 0;
    m_core->setVolume(value*100 - qMax(balance,0)*value,
                      value*100 + qMin(balance,0)*value);
}

void Player2Object::Next()
{
    m_player->next();
}

void Player2Object::OpenUri(const QString &in0)
{
    QString path = in0;
    if(in0.startsWith("file://"))
    {
        path = QUrl(in0).toLocalFile ();
        if(!QFile::exists(path))
            return; //error
    }
    if(!m_pl_manager->currentPlayList()->isLoaderRunning())
    {
        m_pl_manager->selectPlayList(m_pl_manager->currentPlayList());
        connect(m_pl_manager->currentPlayList(), SIGNAL(itemAdded(PlayListItem*)),
                SLOT(playItem(PlayListItem*)));
        connect(m_pl_manager->currentPlayList(), SIGNAL(loaderFinished()), this, SLOT(disconnectPl()));
    }
    m_pl_manager->currentPlayList()->add(path);
}

void Player2Object::Pause()
{
    m_core->pause();
}

void Player2Object::Play()
{
    m_player->play();
}

void Player2Object::PlayPause()
{
    if(m_core->state() == Qmmp::Stopped)
        m_player->play();
    else if(m_core->state() == Qmmp::Paused || m_core->state() == Qmmp::Playing)
        m_core->pause();
}

void Player2Object::Previous()
{
    m_player->previous();
}

void Player2Object::Seek(qlonglong Offset)
{
    m_core->seek(qMin((qint64)0, m_core->elapsed() +  Offset/1000));
}
void Player2Object::SetPosition(const QDBusObjectPath &TrackId, qlonglong Position)
{
    if(m_trackID == TrackId.path())
        m_core->seek(Position/1000);
    else
        qWarning("Player2Object: SetPosition() called with a invalid trackId");
}

void Player2Object::Stop()
{
    m_core->stop();
}

void Player2Object::emitPropertiesChanged()
{
    QList<QByteArray> changedProps;
    if(m_props["canGoNext"] != canGoNext())
        changedProps << "canGoNext";
    if(m_props["canGoPrevious"] != canGoPrevious())
        changedProps << "canGoPrevious";
    if(m_props["canPause"] != canPause())
        changedProps << "canPause";
    if(m_props["canPlay"] != canPlay())
        changedProps << "canPlay";
    if(m_props["canSeek"] != canSeek())
        changedProps << "canSeek";
    if(m_props["loopStatus"] != loopStatus())
        changedProps << "loopStatus";
    if(m_props["maximumRate"] != maximumRate())
        changedProps << "maximumRate";
    if(m_props["minimumRate"] != minimumRate())
        changedProps << "minimumRate";
    if(m_props["playbackStatus"] != playbackStatus())
        changedProps << "playbackStatus";
    if(m_props["position"] != position())
        changedProps << "position";
    if(m_props["rate"] != rate())
        changedProps << "rate";
    if(m_props["shuffle"] != shuffle())
        changedProps << "shuffle";
    if(m_props["volume"] != volume())
        changedProps << "volume";
    if(m_props["metadata"] != metadata())
        changedProps << "metadata";

    syncProperties();

    if(changedProps.isEmpty())
        return;

    QVariantMap map;
    foreach(QByteArray name, changedProps)
        map.insert(name, m_props.value(name));

    QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
                                                  "org.freedesktop.DBus.Properties", "PropertiesChanged");
    QVariantList args = QVariantList()
            << "org.mpris.MediaPlayer2.Player"
            << map
            << QStringList();
    msg.setArguments(args);
    QDBusConnection::sessionBus().send(msg);
}

void Player2Object::updateId()
{
    if(m_prev_item != m_pl_manager->currentPlayList()->currentItem())
    {
        m_trackID = QString("%1/Track/%2").arg("/org/mpris/MediaPlayer2").arg(qrand());
        m_prev_item = m_pl_manager->currentPlayList()->currentItem();
    }
}

void Player2Object::checkState(Qmmp::State state)
{
    if(state == Qmmp::Playing)
        m_previous_pos = 0;
}

void Player2Object::checkSeeking(qint64 elapsed)
{
    if(abs(elapsed - m_previous_pos) > 2000)
    {
        emit Seeked(elapsed * 1000);
    }
    m_previous_pos = elapsed;
}

void Player2Object::playItem(PlayListItem *item)
{
    m_pl_manager->selectPlayList((PlayListModel*)sender());
    m_pl_manager->activatePlayList((PlayListModel*)sender());
    disconnect(sender(), SIGNAL(itemAddded(itemAdded(PlayListItem*))), this, SLOT(playItem(PlayListItem*)));
    if(!m_pl_manager->currentPlayList()->setCurrent(item))
        return;
    m_core->stop();
    m_player->play();
}

void Player2Object::disconnectPl()
{
    disconnect(sender(), SIGNAL(itemAddded(itemAdded(PlayListItem*))), this, SLOT(playItem(PlayListItem*)));
}

void Player2Object::syncProperties()
{
    m_props["canGoNext"] = canGoNext();
    m_props["canGoPrevious"] = canGoPrevious();
    m_props["canPause"] = canPause();
    m_props["canPlay"] = canPlay();
    m_props["canSeek"] = canSeek();
    m_props["loopStatus"] = loopStatus();
    m_props["maximumRate"] = maximumRate();
    m_props["minimumRate"] = minimumRate();
    m_props["playbackStatus"] = playbackStatus();
    m_props["position"] = position();
    m_props["rate"] = rate();
    m_props["shuffle"] = shuffle();
    m_props["volume"] = volume();
    m_props["metadata"] = metadata();
}
