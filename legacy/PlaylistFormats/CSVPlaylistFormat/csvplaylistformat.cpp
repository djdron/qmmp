#include <QtPlugin>
#include  <QFileInfo>

#include "csvplaylistformat.h"
#include <mediafile.h>


bool CSVPlaylistFormat::hasFormat(const QString & f)
{
	foreach(QString s,m_supported_formats)
		if(f == s)
			return true;

	return false;
}

QStringList CSVPlaylistFormat::getExtensions() const
{
	return m_supported_formats;
}

CSVPlaylistFormat::CSVPlaylistFormat()
{
	m_supported_formats << "csv";
}

QString CSVPlaylistFormat::name() const
{
	return "CSVPlaylistFormat";
}


QStringList CSVPlaylistFormat::decode(const QString & contents)
{
    qWarning("CONTENTS: %s",qPrintable(contents));
	QStringList out;
	QStringList splitted = contents.split("\n");
	if(!splitted.isEmpty())
	{
        foreach(QString str, splitted)
        {
            QStringList song = str.split(";");
            qWarning("SONG: %s",qPrintable(song[0]));
            if(song.count() > 1)
            {
                QString unverified = song[1];
                if(QFileInfo(unverified).exists())
                    out << QFileInfo(unverified).absoluteFilePath();
                else
                    qWarning("File %s does not exist",unverified.toLocal8Bit().data());
            }

		}
        return out;
	}
	else
		qWarning("Error parsing CSV playlist format");

	return QStringList();
}

QString CSVPlaylistFormat::encode(const QList< MediaFile * > & contents)
{
	QStringList out;
	foreach(MediaFile* f,contents)
		out.append(f->title() + ";" + f->path() + ";" + QString::number(f->length()));

	return out.join("\n");
}

Q_EXPORT_PLUGIN(CSVPlaylistFormat)

