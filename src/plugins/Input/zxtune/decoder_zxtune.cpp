/***************************************************************************
 *   Copyright (C) 2015 by Andrey Dj                                       *
 *   djdron@gmail.com                                                      *
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

#include <qmmp/statehandler.h>
#include <QFile>
#include "decoder_zxtune.h"


// Decoder class
DecoderZXTune::DecoderZXTune(const QString& path) : eos(false)
{
	m_path = path;
	if(m_path.startsWith("zxtune://"))
	{
		m_path.remove("zxtune://");
	}
	if(m_path.contains('|'))
	{
		m_subname = m_path.section('|', 1);
		m_path = m_path.section('|', 0, 0);
	}
	qInfo("DecoderZXTune: opening file=%s, subname=%s", m_path.toStdString().c_str(), m_subname.toStdString().c_str());
}

DecoderZXTune::~DecoderZXTune()
{}

bool DecoderZXTune::initialize()
{
	QFile file(m_path);
	if(!file.open(QFile::ReadOnly))
	{
		qWarning("DecoderZXTune: unable to open file");
		return false;
	}
	qint64 size = file.size();
	std::auto_ptr<Dump> data(new Dump(size));
	qint64 readed = file.read((char*)&data->front(), size);
	if(readed != size)
	{
		qWarning("DecoderZXTune: unable to read file");
		return false;
	}

	Binary::Container::Ptr input_file = Binary::CreateContainer(data);
	if(!input_file)
	{
		qWarning("DecoderZXTune: unsupported format");
		return false;
	}

	Parameters::Container::Ptr params = Parameters::Container::Create();
	m_module = Module::Open(*params, ZXTune::OpenLocation(*params, input_file, m_subname.toStdString()));
	if(!m_module)
	{
		qWarning("DecoderZXTune: invalid module");
		return false;
	}

	m_player = PlayerWrapper::Create(m_module);
	if(!m_player)
	{
		qWarning("DecoderZXTune: unable to create player");
		return false;
	}

//	addMetaData(GetMetadata(m_module, m_subname, 0));

	Module::Information::Ptr mi = m_module->GetModuleInformation();
	Parameters::Accessor::Ptr props = m_module->GetModuleProperties();
	double len = mi->FramesCount() * FrameDuration(props);

	m_totalTime = len*1000;
	configure(44100, 2);
	qDebug("DecoderZXTune: module opened successfully");
	return true;
}

qint64 DecoderZXTune::totalTime()
{
    return m_totalTime;
}

void DecoderZXTune::seek(qint64 pos)
{
	m_player->Seek(pos*44100/1000);
}

int DecoderZXTune::bitrate()
{
    return 8;
}

qint64 DecoderZXTune::read(unsigned char* data, qint64 size)
{
	if(eos)
		return 0;
	std::size_t samples = size/sizeof(Sound::Sample); // convert bytes to samples & clip to player internal buffer size
	if(samples > 16384)
		samples = 16384;
	std::size_t rendered = m_player->RenderSound(reinterpret_cast<Sound::Sample*>(data), samples);
	if(rendered != samples)
		eos = true;
	return rendered*sizeof(Sound::Sample);
}

double DecoderZXTune::FrameDuration(Parameters::Accessor::Ptr props)
{
	Parameters::IntType frameDuration = Parameters::ZXTune::Sound::FRAMEDURATION_DEFAULT;
	props->FindValue(Parameters::ZXTune::Sound::FRAMEDURATION, frameDuration);
	return double(frameDuration) / Time::Microseconds::PER_SECOND;
}

QMap<Qmmp::MetaData, QString> DecoderZXTune::GetMetadata(Module::Holder::Ptr module, const QString& subname, int track_idx)
{
	QMap<Qmmp::MetaData, QString> metadata;

	Module::Information::Ptr mi = module->GetModuleInformation();
	Parameters::Accessor::Ptr props = module->GetModuleProperties();

	String author;
	if(props->FindValue(Module::ATTR_AUTHOR, author) && !author.empty())
		metadata.insert(Qmmp::ARTIST, author.c_str());
	String title;
	if(props->FindValue(Module::ATTR_TITLE, title) && !title.empty())
		metadata.insert(Qmmp::TITLE, title.c_str());
	String comment;
	if(props->FindValue(Module::ATTR_COMMENT, comment) && !comment.empty())
		metadata.insert(Qmmp::COMMENT, comment.c_str());
	String program;
	if(props->FindValue(Module::ATTR_PROGRAM, program) && !program.empty())
		metadata.insert(Qmmp::COMPOSER, program.c_str());
	if(track_idx > 0)
	{
		metadata.insert(Qmmp::TRACK, QString::number(track_idx));
	}
	metadata.insert(Qmmp::URL, subname);
	return metadata;
}
