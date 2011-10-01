/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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

#ifndef CONVERTER_H
#define CONVERTER_H

#include <QThread>
#include <QQueue>
#include <QHash>
#include <stdio.h>
#include <qmmp/decoder.h>
#include <qmmp/inputsource.h>

class Converter : public QThread
{
    Q_OBJECT
public:
    explicit Converter(QObject *parent = 0);

    void add(const QStringList &urls);
    void add(const QString &url);

signals:
    void progress(int percent);
    void desriptionChanged(QString text);
    void error(QString text);

private:
    void run();
    bool convert(Decoder *decoder, FILE *file);
    QQueue <Decoder*> m_decoders;
    QHash <Decoder*, InputSource*> m_inputs;

};

#endif // CONVERTER_H
