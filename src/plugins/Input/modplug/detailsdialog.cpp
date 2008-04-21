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
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <stdint.h>
#include <libmodplug/stdafx.h>
#include <libmodplug/it_defs.h>
#include <libmodplug/sndfile.h>
#include <libmodplug/modplug.h>

#define MAX_MESSAGE_LENGTH 4000

#include "archivereader.h"
#include "detailsdialog.h"

DetailsDialog::DetailsDialog(QWidget *parent, const QString &path)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_path = path;
    setWindowTitle (path.section('/',-1));
    ui.pathLineEdit->setText(m_path);
    ui.nameLabel->setText(tr("File name:")+'\n'+
                          tr("Title:")+'\n'+
                          tr("Type:")+'\n'+
                          tr("Length:")+'\n'+
                          tr("Speed:")+'\n'+
                          tr("Tempo:")+'\n'+
                          tr("Samples:")+'\n'+
                          tr("Instruments:")+'\n'+
                          tr("Patterns:")+'\n'+
                          tr("Channels:"));

    loadInfo();
}


DetailsDialog::~DetailsDialog()
{}

void DetailsDialog::loadInfo()
{
    quint32 lSongTime, lNumSamples, lNumInstruments, i;
    char lBuffer[33];
    ArchiveReader reader(this);
    QByteArray buffer;
    if(reader.isSupported(m_path))
    {
        buffer = reader.unpack(m_path);
    }
    else
    {
        QFile file(m_path);
        if(!file.open(QIODevice::ReadOnly))
        {
            qWarning("DetailsDialog: error: %s", qPrintable(file.errorString ()));
            return;
        }
        buffer = file.readAll();
        file.close();
    }

    CSoundFile* soundFile = new CSoundFile();
    soundFile->Create((uchar*) buffer.data(), buffer.size());

    QString lInfo = m_path.section('/',-1);
    lInfo += '\n';
    lInfo += QString::fromUtf8(soundFile->GetTitle());
    lInfo += '\n';

    switch(soundFile->GetType())
    {
    case MOD_TYPE_MOD:
        lInfo+= "ProTracker";
        break;
    case MOD_TYPE_S3M:
        lInfo+= "Scream Tracker 3";
        break;
    case MOD_TYPE_XM:
        lInfo+= "Fast Tracker 2";
        break;
    case MOD_TYPE_IT:
        lInfo+= "Impulse Tracker";
        break;
    case MOD_TYPE_MED:
        lInfo+= "OctaMed";
        break;
    case MOD_TYPE_MTM:
        lInfo+= "MTM";
        break;
    case MOD_TYPE_669:
        lInfo+= "669 Composer / UNIS 669";
        break;
    case MOD_TYPE_ULT:
        lInfo+= "ULT";
        break;
    case MOD_TYPE_STM:
        lInfo+= "Scream Tracker";
        break;
    case MOD_TYPE_FAR:
        lInfo+= "Farandole";
        break;
    case MOD_TYPE_AMF:
        lInfo+= "ASYLUM Music Format";
        break;
    case MOD_TYPE_AMS:
        lInfo+= "AMS module";
        break;
    case MOD_TYPE_DSM:
        lInfo+= "DSIK Internal Format";
        break;
    case MOD_TYPE_MDL:
        lInfo+= "DigiTracker";
        break;
    case MOD_TYPE_OKT:
        lInfo+= "Oktalyzer";
        break;
    case MOD_TYPE_DMF:
        lInfo+= "Delusion Digital Music Fileformat (X-Tracker)";
        break;
    case MOD_TYPE_PTM:
        lInfo+= "PolyTracker";
        break;
    case MOD_TYPE_DBM:
        lInfo+= "DigiBooster Pro";
        break;
    case MOD_TYPE_MT2:
        lInfo+= "MT2";
        break;
    case MOD_TYPE_AMF0:
        lInfo+= "AMF0";
        break;
    case MOD_TYPE_PSM:
        lInfo+= "PSM";
        break;
    default:
        lInfo+= "Unknown";
        break;
    }
    lInfo += '\n';

    lSongTime = soundFile->GetSongTime();
    QString text = QString("%1").arg(lSongTime/60);
    text +=":"+QString("%1").arg(lSongTime%60,2,10,QChar('0'));
    lInfo += text;
    lInfo += '\n';

    QTextStream lStrStream(&lInfo);
    lStrStream << (int)soundFile->GetMusicSpeed() << '\n';
    lStrStream << (int)soundFile->GetMusicTempo() << '\n';
    lStrStream << (int)(lNumSamples = soundFile->GetNumSamples()) << '\n';
    lStrStream << (int)(lNumInstruments = soundFile->GetNumInstruments());
    lStrStream << '\n';
    lStrStream << (int)(soundFile->GetNumPatterns()) << '\n';
    lStrStream << (int)soundFile->GetNumChannels();
    ui.valueLabel->setText(lInfo);
    lInfo.clear();
    for(i = 0; i < lNumSamples; i++)
    {
        soundFile->GetSampleName(i, lBuffer);
        lInfo += QString::fromUtf8(lBuffer);
        lInfo += '\n';
    }
    ui.samplesTextEdit->setText(lInfo.trimmed ());
    lInfo.clear();
    for(i = 0; i < lNumInstruments; i++)
    {
        soundFile->GetInstrumentName(i, lBuffer);
        lInfo += QString::fromUtf8(lBuffer);
        lInfo += '\n';
    }
    ui.instrumentsTextEdit->setText(lInfo.trimmed ());
    lInfo.clear();
    char message[MAX_MESSAGE_LENGTH];
    int length = soundFile->GetSongComments(message, MAX_MESSAGE_LENGTH, 80);
    if (length != 0)
        ui.instrumentsTextEdit->setText(QString::fromUtf8(message).trimmed ());

    soundFile->Destroy();
    delete soundFile;
}

