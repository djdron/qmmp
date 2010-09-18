/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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

#include <QtPlugin>
#include <QTranslator>
#include <QLocale>
#include <qmmp/soundcore.h>

#include "incdecvolumeoption.h"

bool IncDecVolumeCommandLineOption::identify(const QString & str) const
{
    if (
        str == QString("--volume-inc") ||
        str == QString("--volume-dec")
    )
    {
        return true;
    }

    return false;
}

const QString IncDecVolumeCommandLineOption::helpString() const
{
    return  QString(
                "--volume-inc         " + tr("Increase volume with step 5")+"\n"
                "--volume-dec         " + tr("Decrease volume with step 5")+"\n"
            );
}

QString IncDecVolumeCommandLineOption::executeCommand(const QString& opt_str, const QStringList &args)
{
    Q_UNUSED(args);
    SoundCore *core = SoundCore::instance();
    int volume = qMax(core->leftVolume(), core->rightVolume());
    int balance = 0;
    int left = core->leftVolume();
    int right = core->rightVolume();
    if (left || right)
        balance = (right - left)*100/volume;

    if (opt_str == "--volume-inc")
    {
        volume = qMin (100, volume + 5);
    }
    else if (opt_str == "--volume-dec")
    {
        volume = qMax (0, volume - 5);
    }
    core->setVolume(volume-qMax(balance,0)*volume/100,
                    volume+qMin(balance,0)*volume/100);
    return QString();
}

const QString IncDecVolumeCommandLineOption::name() const
{
    return "IncDecVolumeCommandLineOption";
}

QTranslator *IncDecVolumeCommandLineOption::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/incdecvolume_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(incdecvolumeoption, IncDecVolumeCommandLineOption)
