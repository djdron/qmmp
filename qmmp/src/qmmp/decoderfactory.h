
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

#ifndef DECODERFACTORY_H
#define DECODERFACTORY_H

class QObject;
class QString;
class QIODevice;
class QWidget;
class QTranslator;

class Decoder;
class Output;
class FileInfo;

class DecoderProperties
{
public:
    DecoderProperties()
    {
        hasAbout = FALSE;
        hasSettings = FALSE;
        noInput = FALSE;
        noOutput = FALSE;

    }
    QString name;
    QString shortName;
    QString filter;
    QString description;
    QString contentType;
    QString protocols;
    bool hasAbout;
    bool hasSettings;
    bool noInput;
    bool noOutput;
};

class DecoderFactory
{
public:
    virtual ~DecoderFactory() {}
    virtual bool supports(const QString &source) const = 0;
    virtual bool canDecode(QIODevice *) const = 0;
    virtual const DecoderProperties properties() const = 0;
    virtual Decoder *create(QObject *, QIODevice *input = 0,
                            Output *output = 0, const QString &path = QString()) = 0;
    //virtual FileInfo *createFileInfo(const QString &source) = 0;
    virtual QList<FileInfo *> createPlayList(const QString &fileName) = 0;
    virtual QObject* showDetails(QWidget *parent, const QString &path) = 0;
    virtual void showSettings(QWidget *parent) = 0;
    virtual void showAbout(QWidget *parent) = 0;
    virtual QTranslator *createTranslator(QObject *parent) = 0;
};

Q_DECLARE_INTERFACE(DecoderFactory, "DecoderFactory/1.0");

#endif
