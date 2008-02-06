/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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
#ifndef PLUGINITEM_H
#define PLUGINITEM_H

#include <QObject>

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class DecoderFactory;
class OutputFactory;
class VisualFactory;
class EffectFactory;
class GeneralFactory;

class InputPluginItem : public QObject
{
    Q_OBJECT
public:
    InputPluginItem(QObject *parent, DecoderFactory *fact, const QString &filePath);

    ~InputPluginItem();

    bool isSelected();
    DecoderFactory * factory();

public slots:
    void setSelected(bool);

private:
    QString m_fileName;
    DecoderFactory *m_factory;

};

class OutputPluginItem : public QObject
{
    Q_OBJECT
public:
    OutputPluginItem(QObject *parent, OutputFactory *fact, const QString &filePath);

    ~OutputPluginItem();

    bool isSelected();
    OutputFactory * factory();

public slots:
    void select();

private:
    QString m_fileName;
    OutputFactory *m_factory;

};

class VisualPluginItem : public QObject
{
    Q_OBJECT
public:
    VisualPluginItem(QObject *parent, VisualFactory *fact, const QString &filePath);

    ~VisualPluginItem();

    bool isSelected();
    VisualFactory * factory();

public slots:
    void select(bool);

private:
    QString m_fileName;
    VisualFactory *m_factory;
};

class EffectPluginItem : public QObject
{
    Q_OBJECT
public:
    EffectPluginItem(QObject *parent, EffectFactory *fact, const QString &filePath);

    ~EffectPluginItem();

    bool isSelected();
    EffectFactory * factory();

public slots:
    void select(bool);

private:
    QString m_fileName;
    EffectFactory *m_factory;
};

class GeneralPluginItem : public QObject
{
    Q_OBJECT
public:
    GeneralPluginItem(QObject *parent, GeneralFactory *fact, const QString &filePath);

    ~GeneralPluginItem();

    bool isSelected();
    GeneralFactory * factory();

public slots:
    void select(bool);

private:
    QString m_fileName;
    GeneralFactory *m_factory;
};

#endif
