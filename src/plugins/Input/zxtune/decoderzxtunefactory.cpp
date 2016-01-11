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

#include <QTranslator>
#include <QMessageBox>
#include <QRegExp>
#include <QFile>
#include "decoderzxtunefactory.h"
#include "decoder_zxtune.h"

static const char* plugin_info =
"ZXTune Player (C) 2008 - 2015 by Vitamin/CAIG.\n"
"based on r3500 oct 30 2015\n"
"Qmmp plugin by djdron (C) 2015.\n\n"

"Used source codes from:\n"
"AYEmul from S.Bulba\n"
"AYFly from Ander\n"
"xPlugins from elf/2\n"
"Pusher from Himik/ZxZ\n\n"

"Used 3rdparty libraries:\n"
"boost C++ library\n"
"zlib from Jean-loup Gailly and Mark Adler\n"
"z80ex from Boo-boo\n"
"lhasa from Simon Howard\n"
"libxmp from Claudio Matsuoka\n"
"libsidplayfp from Simon White, Antti Lankila and Leandro Nini\n"
"snes_spc from Shay Green\n"
"Game Music Emu from Shay Green and Chris Moeller\n";

static const char* file_types[] =
{
	// AY/YM
	"as0", "asc", "ay", "ayc", "ftc", "gtr", "psc", "psg", "psm", "pt1", "pt2", "pt3", "sqt", "st1", "st3", "stc", "stp", "ts", "vtx", "ym",
	// dac
	"ahx", "pdt", "chi", "str", "dst", "sqd", "et1", "dmm", "669", "amf", "dbm", "dmf", "dtm", "dtt", "emod", "far", "fnk", "gdm", "gtk", "mod", "mtn", "imf", "ims", "it", "liq", "mdl", "med", "mtm", "okt", "pt36", "ptm", "rtm", "s3m", "sfx", "stim", "stm", "stx", "tcb", "ult", "xm",
	// fm
	"tfc", "tfd", "tf0", "tfe",
	// Sam Coupe
	"cop",
	// C64
	"sid",
	// NES/SNES
	"spc", "nsf", "nsfe",
	// Game Boy
	"gbs",
	// Atari
	"sap",
	// TurboGrafX
	"hes",
	// Multidevice
	"mtc", "vgm", "gym",
	// arch
	"hrp", "scl", "szx", "trd", "cc3", "dsq", "esv", "fdi", "gam", "gamplus", "logo1", "$b", "$c", "$m", "hrm", "bin", "p", "lzs", "msp", "pcd", "td0", "tlz", "tlzp", "trs",
	"7z", "rar", "zip", "lha", "umx",
	// end
	NULL
};

// DecoderZXTuneFactory

bool DecoderZXTuneFactory::supports(const QString &source) const
{
    foreach(QString filter, properties().filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (regexp.exactMatch(source))
            return true;
    }
    return false;
}

bool DecoderZXTuneFactory::canDecode(QIODevice *) const
{
    return false;
}

const DecoderProperties DecoderZXTuneFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("ZXTune Plugin");
	for(const char** ft = file_types; *ft; ++ft)
	{
		properties.filters << QString("*.").append(*ft);
	}
    properties.description = tr("ZXTune Files");
    //properties.contentType = ;
    properties.shortName = "zxtune";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.noInput = true;
    properties.protocols << "zxtune";
    return properties;
}

Decoder *DecoderZXTuneFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderZXTune(path);
}

struct ModuleDesc
{
	Module::Holder::Ptr module;
	std::string subname;
};
typedef std::vector<ModuleDesc> Modules;


QList<FileInfo *> DecoderZXTuneFactory::createPlayList(const QString &fileName, bool useMetaData, QStringList *ignoredFiles)
{
	QList <FileInfo*> list;
	//is it one track?
	QString path = fileName;
	if(fileName.startsWith("zxtune://"))
	{
		path.remove("zxtune://");
		// @todo: return list with one item with this path
//		FileInfo *info = new FileInfo();
//		list << info;
		return list;
	}

	QFile file(fileName);
	if(!file.open(QFile::ReadOnly))
	{
		qWarning("DecoderZXTuneFactory: unable to open file");
		return list;
	}
	qint64 size = file.size();
	std::auto_ptr<Dump> data(new Dump(size));
	qint64 readed = file.read((char*)&data->front(), size);
	if(readed != size)
	{
		qWarning("DecoderZXTuneFactory: unable to read file");
		return list;
	}

	Binary::Container::Ptr input_file = Binary::CreateContainer(data);
	if(!input_file)
	{
		qWarning("DecoderZXTuneFactory: unsupported format");
		return list;
	}

	Modules	input_modules;

	struct ModuleDetector : public Module::DetectCallback
	{
		ModuleDetector(Modules* _mods) : modules(_mods) {}
		virtual void ProcessModule(ZXTune::DataLocation::Ptr location, ZXTune::Plugin::Ptr, Module::Holder::Ptr holder) const
		{
			ModuleDesc m;
			m.module = holder;
			m.subname = location->GetPath()->AsString();
			modules->push_back(m);
		}
		virtual Log::ProgressCallback* GetProgress() const { return NULL; }
		Modules* modules;
	};

	ModuleDetector md(&input_modules);
	Parameters::Container::Ptr params = Parameters::Container::Create();
	Module::Detect(*params, ZXTune::CreateLocation(input_file), md);
	if(input_modules.empty())
	{
		qWarning("DecoderZXTuneFactory: unsupported format");
		return list;
	}
	int m_idx = 1;
	for(Modules::iterator it = input_modules.begin(); it != input_modules.end(); ++it)
	{
		ModuleDesc& m = *it;
		FileInfo* info = new FileInfo();

		QMap<Qmmp::MetaData, QString> meta = DecoderZXTune::GetMetadata(m.module, QString::fromStdString(m.subname), input_modules.size() > 1 ? m_idx++ : 0);
		info->setMetaData(meta);

		Module::Information::Ptr mi = m.module->GetModuleInformation();
		Parameters::Accessor::Ptr props = m.module->GetModuleProperties();
		double len = mi->FramesCount() * DecoderZXTune::FrameDuration(props);
		QString fullPath = "zxtune://" + path;
		if(!m.subname.empty())
			fullPath += "|" + QString::fromStdString(m.subname);
		info->setPath(fullPath);
		info->setLength(len);
		list << info;
	}
    return list;
}

MetaDataModel* DecoderZXTuneFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    Q_UNUSED(path);
    Q_UNUSED(parent);
    return 0;
}

void DecoderZXTuneFactory::showSettings(QWidget *parent)
{
    Q_UNUSED(parent);
}

void DecoderZXTuneFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About ZXTune Audio Plugin"),
                        tr(plugin_info));
}

QTranslator *DecoderZXTuneFactory::createTranslator(QObject *parent)
{
	return NULL;
/*    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/zxtune_plugin_") + locale);
    return translator;*/
}
