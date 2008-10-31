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
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/mpcfile.h>

#include <QFile>
#include <QFileInfo>

#include "aacfile.h"
#include "detailsdialog.h"

#define QStringToTString_qt4(s) TagLib::String(s.toUtf8().constData(), TagLib::String::UTF8)

DetailsDialog::DetailsDialog(QWidget *parent, const QString &path)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_path = path;
    setWindowTitle (path.section('/',-1));
    path.section('/',-1);
    ui.pathLineEdit->setText(m_path);
    if (QFile::exists(m_path))
        loadAACInfo();
}


DetailsDialog::~DetailsDialog()
{}

void DetailsDialog::loadAACInfo()
{
    QFile input(m_path);
    if (!input.open(QIODevice::ReadOnly))
        return;

    AACFile f(&input);

    QString text;
    text = QString("%1").arg(f.length()/60);
    text +=":"+QString("%1").arg(f.length()%60,2,10,QChar('0'));
    ui.lengthLabel->setText(text);
    /*text = QString("%1").arg(f.audioProperties()->sampleRate());
    ui.sampleRateLabel->setText(text+" "+tr("Hz"));*/
    /*text = QString("%1").arg(f.audioProperties()->channels());
    ui.channelsLabel->setText(text);*/
    text = QString("%1").arg(f.bitrate());
    ui.bitrateLabel->setText(text+" "+tr("kbps"));
    /*text = QString("%1").arg(f.audioProperties()->mpcVersion());
    ui.versionLabel->setText(text);*/
    text = QString("%1 "+tr("KB")).arg(input.size()/1024);
    ui.fileSizeLabel->setText(text);

    //show metadata
    ui.titleLineEdit->setText(f.metaData().value(Qmmp::TITLE));
    ui.artistLineEdit->setText(f.metaData().value(Qmmp::ARTIST));
    ui.albumLineEdit->setText(f.metaData().value(Qmmp::ALBUM));
    ui.commentLineEdit->setText(f.metaData().value(Qmmp::COMMENT));
    ui.yearLineEdit->setText(f.metaData().value(Qmmp::YEAR));
    ui.trackLineEdit->setText(f.metaData().value(Qmmp::TRACK));
    ui.genreLineEdit->setText(f.metaData().value(Qmmp::GENRE));
    input.close();
}
