#ifndef CSVPLAYLISTFORMAT_H
#define CSVPLAYLISTFORMAT_H

#include <playlistformat.h>

#include <QStringList>

class MediaFile;

/*!
 * Example of custom playlist CSV semicolon separated format.
 * Each line represents a song in the next format:
 * TITLE;FILEPATH;DURATION
 */

class CSVPlaylistFormat : public QObject, public PlaylistFormat
{
Q_OBJECT
Q_INTERFACES(PlaylistFormat)
public:
    CSVPlaylistFormat();
    virtual ~CSVPlaylistFormat(){;}
    virtual QStringList decode(const QString& contents);
    virtual QString encode(const QList<MediaFile*>& contents);
    virtual QStringList getExtensions()const;
    virtual bool hasFormat(const QString& ext);
    virtual QString name()const;
protected:
    QStringList m_supported_formats;
};

#endif //CSVPLAYLISTFORMAT_H

