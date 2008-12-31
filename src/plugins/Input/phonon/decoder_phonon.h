/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

#ifndef DECODER_PHONON_H
#define DECODER_PHONON_H

#include <phonon/phononnamespace.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <phonon/videowidget.h>
#include <phonon/effect.h>
#include <phonon/effectparameter.h>

#include <qmmp/decoder.h>
#include <qmmp/statehandler.h>

class Output;
class QIDevice;
class DecoderPhonon;
class QMenu;

class VideoWindow : public Phonon::VideoWidget
{
Q_OBJECT
public:
    VideoWindow(DecoderPhonon *decoder, QWidget *parent = 0);

protected:
    void mouseDoubleClickEvent(QMouseEvent *e);
    void closeEvent (QCloseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void changeScreen(bool fullScreen);

private:
    DecoderPhonon *m_decoder;
    QAction *m_fullScreenAction;
    QPoint m_pos;
    QMenu *m_menu;
    QByteArray m_geometry;
};


class DecoderPhonon : public Decoder
{
    Q_OBJECT
public:
    DecoderPhonon(QObject *, DecoderFactory *, const QString &url);
    virtual ~DecoderPhonon();

    // Standard Decoder API
    bool initialize();
    qint64 lengthInSeconds();
    void seek(qint64);
    void stop();
    void pause();

    // Equalizer
    void setEQ(double bands[10], double preamp);
    void setEQEnabled(bool on);

private slots:
    void updateState(Phonon::State newstate, Phonon::State oldstate);
    void updateTime(qint64 time);

private:
    // thread run function
    void run();
    // phonon object
    Phonon::MediaObject *m_mediaObject;
    Phonon::AudioOutput *m_audioOutput;
    Phonon::VideoWidget *m_videoWidget;
    Phonon::Path m_audioOutputPath;
    bool m_stop;

    QString m_url;
};


#endif // DECODER_PHONON_H
