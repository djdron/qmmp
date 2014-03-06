#ifndef MEDIAKEYS_H
#define MEDIAKEYS_H

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class MediaKeys : public QObject
{
    Q_OBJECT
public:
    explicit MediaKeys(QObject *parent = 0);
    virtual ~MediaKeys();

private slots:
    QDBusPendingReply<> grabMediaPlayerKeys(const QString &application, uint time);
    QDBusPendingReply<> releaseMediaPlayerKeys(const QString &application);
    void onRegisterFinished(QDBusPendingCallWatcher *watcher);
    void onKeyPressed(const QString &in0, const QString &in1);

private:
    QDBusInterface *m_interface;
    bool m_isRegistered;


};

#endif // MEDIAKEYS_H
