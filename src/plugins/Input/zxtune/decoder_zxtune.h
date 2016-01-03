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

#ifndef DECODER_ZXTUNE_H
#define DECODER_ZXTUNE_H

#include <qmmp/decoder.h>

//common includes
#include <contract.h>
#include <cycle_buffer.h>
#include <error_tools.h>
#include <progress_callback.h>
//library includes
#include <binary/container.h>
#include <binary/container_factories.h>
#include <time/stamp.h>
#include <core/core_parameters.h>
#include <core/module_open.h>
#include <core/module_holder.h>
#include <core/module_player.h>
#include <core/module_detect.h>
#include <core/module_attrs.h>
#include <core/data_location.h>
#include <parameters/container.h>
#include <sound/sound_parameters.h>
#include <platform/version/api.h>

//std includes
#include <vector>

//boost
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/static_assert.hpp>
#include <boost/range/end.hpp>
#include <boost/type_traits/is_signed.hpp>

#include "player.h"

/**
   @author Andrey Dj <djdron@gmail.com>
*/
class DecoderZXTune : public Decoder
{
public:
    DecoderZXTune(const QString &path);
    virtual ~DecoderZXTune();

    // Standard Decoder API
    bool initialize();
    qint64 totalTime();
    int bitrate();
    qint64 read(unsigned char *data, qint64 size);
    void seek(qint64);

	static double FrameDuration(Parameters::Accessor::Ptr props);
	static QMap<Qmmp::MetaData, QString> GetMetadata(Module::Holder::Ptr module, const QString& subname, int track_idx);

private:
	bool	eos;
    qint64	m_totalTime;
    QString	m_path;
	QString	m_subname;
	Module::Holder::Ptr	m_module;
	PlayerWrapper::Ptr	m_player;
};

#endif // DECODER_ZXTUNE_H
