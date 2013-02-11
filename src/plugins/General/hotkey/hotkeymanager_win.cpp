/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QtGlobal>
#ifdef Q_OS_WIN
#include <QSettings>
#include <QX11Info>
#include <QEvent>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <winuser.h>
#include <commctrl.h>
#include <qmmp/qmmp.h>
#include <qmmp/soundcore.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include "hotkeymanager.h"

quint32 Hotkey::defaultKey()
{
    return defaultKey(action);
}

quint32 Hotkey::defaultKey(int act)
{
    //default key bindings
    QMap<int, quint32> keyMap;
    keyMap[PLAY] = 0;
    keyMap[STOP] = VK_MEDIA_STOP;
    keyMap[PAUSE] = 0;
    keyMap[PLAY_PAUSE] = VK_MEDIA_PLAY_PAUSE;
    keyMap[NEXT] = VK_MEDIA_NEXT_TRACK;
    keyMap[PREVIOUS] = VK_MEDIA_PREV_TRACK;
    keyMap[SHOW_HIDE] = 0;
    keyMap[VOLUME_UP] = VK_VOLUME_UP;
    keyMap[VOLUME_DOWN] = VK_VOLUME_DOWN;
    keyMap[FORWARD] = 0;
    keyMap[REWIND] = 0;
    keyMap[JUMP_TO_TRACK] = 0;
    return keyMap[act];
}

WPARAM nextId132 = 1;


class KeyFilterWidget : public QWidget
{
public:
    KeyFilterWidget()
    {}

    ~KeyFilterWidget()
    {

    }

bool winEvent(MSG* m, long* result)
{
    if (m->message == WM_HOTKEY)
    {
        qDebug("HOT KEY PRESSED!!!");
        return true;
    }
    return QWidget::winEvent(m, result);
}
};


HotkeyManager::HotkeyManager(QObject *parent) : QObject(parent)
{
    QCoreApplication::instance()->installEventFilter(this);
    KeyFilterWidget *w = new KeyFilterWidget;

    //qDebug("+++%d++++", int(WId));
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat); //load settings
    settings.beginGroup("Hotkey");
    for (int i = Hotkey::PLAY; i <= Hotkey::JUMP_TO_TRACK; ++i)
    {
        qDebug("----");
        quint32 key = settings.value(QString("key_%1").arg(i), Hotkey::defaultKey(i)).toUInt();
        quint32 mod = settings.value(QString("modifiers_%1").arg(i), 0).toUInt();

        if (key)
        {

            Hotkey *hotkey = new Hotkey;
            hotkey->action = i;
            hotkey->key = key;
            //hotkey->code = keycodeToKeysym(hotkey->key);
            /*if(!hotkey->code)
                continue;*/

            qDebug("1");
            if(RegisterHotKey(w->winId(), MOD_CONTROL^hotkey->key,  MOD_CONTROL, hotkey->key))
            {
                qDebug("registered");
                nextId132++;
            }
            else
            {
                qDebug("failed");
            }
            qDebug("2");

            //hotkey->mod = mod | mask_mod;
            m_grabbedKeys << hotkey;
        }
    }
    settings.endGroup();
}

HotkeyManager::~HotkeyManager()
{
    /*foreach(Hotkey *key, m_grabbedKeys)
    {
        if(key->code)
            XUngrabKey(QX11Info::display(), key->code, key->mod, QX11Info::appRootWindow());
    }
    while (!m_grabbedKeys.isEmpty())
        delete m_grabbedKeys.takeFirst ();*/
}

const QString HotkeyManager::getKeyString(quint32 key, quint32 modifiers)
{
    qDebug("%ux++%ux", key, modifiers);
    QString strModList[] = { "Control", "Shift", "Alt"};
    quint32 modList[] = { HOTKEYF_CONTROL, HOTKEYF_SHIFT, HOTKEYF_ALT};

    MOD_CONTROL, MOD_ALT, HOTKEYF_EXT;

    QString keyStr;
    for (int j = 0; j < 3; j++)
    {
        if (modifiers & modList[j])
            keyStr.append(strModList[j] + "+");
    }

    LONG lScan = MapVirtualKey(key, 0) << 16;

    int nBufferLen = 64;
    std::wstring str;
    int nLen;
    do
    {
        nBufferLen *= 2;
        str.resize(nBufferLen);
        nLen = ::GetKeyNameTextW(lScan, &str[0], nBufferLen);
    }
    while (nLen == nBufferLen);

    keyStr += QString::fromWCharArray(str.c_str());

    return keyStr;
}

bool HotkeyManager::eventFilter(QObject* o, QEvent* e)
{
    if (e->type() == QEvent::KeyPress)
    {
        qDebug("KEY EVENT!!");
    }

    //receive events from active and root windows only
    /*if (e->type() == QEvent::KeyPress && (o == qApp->desktop () || o == qApp->activeWindow ()))
    {
        QKeyEvent* k = static_cast<QKeyEvent*>(e);
        quint32 key = keycodeToKeysym(k->nativeScanCode());
        quint32 mod = k->nativeModifiers ();
        SoundCore *core = SoundCore::instance();
        MediaPlayer *player = MediaPlayer::instance();
        foreach(Hotkey *hotkey, m_grabbedKeys)
        {
            if (hotkey->key != key || hotkey->mod != mod)
                continue;
            qDebug("HotkeyManager: [%s] pressed", qPrintable(getKeyString(key, mod)));

            switch (hotkey->action)
            {
            case Hotkey::PLAY:
                player->play();
                break;
            case Hotkey::STOP:
                player->stop();
                break;
            case Hotkey::PAUSE:
                core->pause();
                break;
            case Hotkey::PLAY_PAUSE:
                if (core->state() == Qmmp::Stopped)
                    MediaPlayer::instance()->play();
                else if (core->state() != Qmmp::FatalError)
                    core->pause();
                break;
            case Hotkey::NEXT:
                player->next();
                break;
            case Hotkey::PREVIOUS:
                player->previous();
                break;
            case Hotkey::SHOW_HIDE:
                UiHelper::instance()->toggleVisibility();
                break;
            case Hotkey::VOLUME_UP:
            case Hotkey::VOLUME_DOWN:
            {
                int volume = qMax(core->leftVolume(), core->rightVolume());
                int balance = 0;
                int left = core->leftVolume();
                int right = core->rightVolume();
                if (left || right)
                    balance = (right - left)*100/volume;
                if(hotkey->action == Hotkey::VOLUME_UP)
                    volume = qMin (100, volume + 5);
                else
                    volume = qMax (0, volume - 5);
                core->setVolume(volume-qMax(balance,0)*volume/100,
                                volume+qMin(balance,0)*volume/100);
            }
                break;
            case Hotkey::FORWARD:
                core->seek(core->elapsed() + 5000);
                break;
            case Hotkey::REWIND:
                core->seek(qMax(qint64(0), core->elapsed() - 5000));
                break;
            case Hotkey::JUMP_TO_TRACK:
                UiHelper::instance()->jumpToTrack();
                break;

            }
            qApp->processEvents();
        }
    }*/
    return QObject::eventFilter(o, e);
}
long HotkeyManager::m_alt_mask = 0;
long HotkeyManager::m_meta_mask = 0;
long HotkeyManager::m_super_mask = 0;
long HotkeyManager::m_hyper_mask = 0;
long HotkeyManager::m_numlock_mask = 0;
bool HotkeyManager::m_haveMods = false;


void HotkeyManager::ensureModifiers(){}

QList<long> HotkeyManager::ignModifiersList()
{
    ensureModifiers();
    QList<long> ret;
    ret << 0;
    return ret;
}

quint32 HotkeyManager::keycodeToKeysym(quint32 keycode)
{
    return MapVirtualKey(keycode, 1);
}
#endif

