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
#ifndef SKIN_H
#define SKIN_H

#include <QObject>
#include <QMap>
#include <QPixmap>
#include <QDir>
#include <QRegion>

/*
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/


class Skin : public QObject
{
    Q_OBJECT
public:
    Skin(QObject *parent = 0);

    ~Skin();

    static Skin *getPointer();
    static QPixmap getPixmap(const QString&, QDir);
    void setSkin(const QString& path);
    const QPixmap getMain() const
    {
        return m_main;
    };
    const QPixmap getButton(uint bt) const
    {
        return buttons[bt];
    };
    const QPixmap getTitleBar(uint tb) const
    {
        return titlebar[tb];
    };
    const QPixmap getPosBar() const
    {
        return posbar;
    };
    const QPixmap getNumber(uint n) const
    {
        return m_numbers[n];
    };
    /*!
     * Returns count of numbers in number list.
     * We need this to check if we have "-" in pixmaps.
     * if no we should draw it manually.
     */
    const uint getNumCount(void) const
    {
        return m_numbers.count();
    }
    const QPixmap getPlPart(uint p) const
    {
        return m_pl_parts[p];
    };
    const QPixmap getEqPart(uint p) const
    {
        return m_eq_parts[p];
    };
    const QPixmap getEqSlider(uint n) const
    {
        return m_eq_bar[n];
    };
    const QPixmap getEqSpline(uint n) const
    {
        return m_eq_spline[n];
    };
    const QPixmap getMSPart(uint n) const
    {
        return m_ms_parts[n];
    };
    const QPixmap getLetter(const QChar& ch)
    {
        return m_letters[ch];
    };
    const QPixmap getItem(uint n) const
    {
        return m_parts[n];
    };
    const QPixmap getVolumeBar(int n) const
    {
        return m_volume[n];
    };
    const QPixmap getBalanceBar(int n) const
    {
        return m_balance[n];
    };
    const QByteArray getPLValue (QByteArray c) const
    {
        return m_pledit_txt[c];
    };
    const QColor getVisBarColor(int n) const
    {
        return m_vis_bars[n];
    };
    const QRegion getMWRegion() const
    {
        return m_mwRegion;
    };
    const QRegion getPLRegion() const
    {
        return m_plRegion;
    };

    enum Buttons
    {
        BT_PREVIOUS_N,
        BT_PREVIOUS_P,
        BT_PLAY_N,
        BT_PLAY_P,
        BT_PAUSE_N,
        BT_PAUSE_P,
        BT_STOP_N,
        BT_STOP_P,
        BT_NEXT_N,
        BT_NEXT_P,
        BT_EJECT_N,
        BT_EJECT_P,
        /*titlebar.* */
        BT_MENU_N,
        BT_MENU_P,
        BT_MINIMIZE_N,
        BT_MINIMIZE_P,
        BT_CLOSE_N,
        BT_CLOSE_P,
        BT_SHADE1_N,
        BT_SHADE1_P,
        BT_SHADE2_N,
        BT_SHADE2_P,
        /* posbar.* */
        BT_POSBAR_N,
        BT_POSBAR_P,
        /* pledit.* */
        PL_BT_ADD,
        PL_BT_SUB,
        PL_BT_SEL,
        PL_BT_SORT,
        PL_BT_LST,
        PL_BT_SCROLL_N,
        PL_BT_SCROLL_P,
        /* eqmain.* */
        EQ_BT_BAR_N,
        EQ_BT_BAR_P,
        EQ_BT_ON_N,
        EQ_BT_ON_P,
        EQ_BT_OFF_N,
        EQ_BT_OFF_P,
        EQ_BT_PRESETS_N,
        EQ_BT_PRESETS_P,
        EQ_BT_AUTO_1_N,
        EQ_BT_AUTO_1_P,
        EQ_BT_AUTO_0_N,
        EQ_BT_AUTO_0_P,
        /* shufrep.* */
        BT_EQ_ON_N,
        BT_EQ_ON_P,
        BT_EQ_OFF_N,
        BT_EQ_OFF_P,
        BT_PL_ON_N,
        BT_PL_ON_P,
        BT_PL_OFF_N,
        BT_PL_OFF_P,
        REPEAT_ON_N,
        REPEAT_ON_P,
        REPEAT_OFF_N,
        REPEAT_OFF_P,
        SHUFFLE_ON_N,
        SHUFFLE_ON_P,
        SHUFFLE_OFF_N,
        SHUFFLE_OFF_P,
        /* volume.* */
        BT_VOL_N,
        BT_VOL_P,
        /* balance.* */
        BT_BAL_N,
        BT_BAL_P,
        // Playlist play/stop buttons
        /*BT_PL_PLAY,
         BT_PL_STOP,
          BT_PL_PAUSE,
          BT_PL_NEXT,
        BT_PL_PREV,
          BT_PL_EJECT*/
    };
    enum TitleBar
    {
        TITLEBAR_A,
        TITLEBAR_I,
        STATUSBAR_A,
        STATUSBAR_I
    };
    enum PlayList
    {
        PL_CORNER_UL_A,
        PL_CORNER_UL_I,
        PL_CORNER_UR_A,
        PL_CORNER_UR_I,
        PL_TITLEBAR_A,
        PL_TITLEBAR_I,
        PL_TFILL1_A,
        PL_TFILL1_I,
        PL_TFILL2_A,
        PL_TFILL2_I,
        PL_LFILL,
        PL_RFILL,
        PL_LSBAR,
        PL_RSBAR,
        PL_SFILL1,
        PL_SFILL2,
        PL_CONTROL
    };
    enum Equalizer
    {
        EQ_MAIN,
        EQ_TITLEBAR_A,
        EQ_TITLEBAR_I,
        EQ_GRAPH,
    };
    enum MonoSter
    {
        MONO_A,
        MONO_I,
        STEREO_A,
        STEREO_I,
    };
    enum OtherParts
    {
        PLAY,
        PAUSE,
        STOP,
    };

signals:
    void skinChanged();

private:
    QPixmap *getPixmap(const QString&);

    /*!
     * As far as there is no standard in skin making we cannot be sure
     * that all needful images we can find in skin :( This will cause 
     * segfaults and asserts. So to prevent this we need such method
     * to load pixmap from default skin.
     */
    QPixmap *getDummyPixmap(const QString&);
    static Skin *pointer;
    QDir m_skin_dir;
    QMap<uint, QPixmap> buttons;
    QMap<uint, QPixmap> titlebar;
    QMap<uint, QPixmap> m_pl_parts;
    QMap<uint, QPixmap> m_eq_parts;
    QMap<uint, QPixmap> m_ms_parts;
    QMap<uint, QPixmap> m_parts;
    QMap<QChar, QPixmap> m_letters;
    QMap<QByteArray, QByteArray> m_pledit_txt;
    QPixmap m_main;
    QPixmap posbar;
    QList<QPixmap> m_numbers;
    QList<QPixmap> m_eq_bar;
    QList<QPixmap> m_eq_spline;
    QList<QPixmap> m_volume;
    QList<QPixmap> m_balance;
    QList<QColor> m_vis_bars;
    QRegion m_mwRegion;
    QRegion m_plRegion;

    void loadMain();
    void loadButtons();
    void loadTitleBar();
    void loadPosBar();
    void loadNumbers();
    void loadPlayList();
    void loadPLEdit();
    void loadEqMain();
    void loadVisColor();
    void loadShufRep();
    void loadLetters();
    void loadMonoSter();
    void loadVolume();
    void loadBalance();
    void loadRegion();
    QRegion createRegion(const QString &path, const QString &key);

};

#endif
