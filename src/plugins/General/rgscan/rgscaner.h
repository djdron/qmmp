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

#ifndef RGSCANER_H
#define RGSCANER_H

#include <QRunnable>
#include <QObject>
#include <QMutex>
#include <stdio.h>
#include <qmmp/decoder.h>
#include <qmmp/inputsource.h>
#include "gain_analysis.h"


/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class RGScaner : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit RGScaner();
    ~RGScaner();

    bool prepare(const QString &url);
    void stop();
    bool isRunning();
    double gain();
    GainHandle_t *handle();

signals:
    void progress(int percent);
    void finished(const QString &url);

private:
    void run();
    InputSource *m_source;
    Decoder *m_decoder;
    QMutex m_mutex;
    bool m_user_stop;
    bool m_is_running;
    QString m_url;
    double m_gain;
    GainHandle_t *m_handle;

};

#endif // RGSCANER_H
