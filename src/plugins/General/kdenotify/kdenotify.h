#ifndef KDENOTIFY_H
#define KDENOTIFY_H

#include "qmmpui/general.h"
#include "qmmp/qmmp.h"

class QDBusInterface;

class KdeNotify : public General
{
    Q_OBJECT;
public:
    KdeNotify(QObject *parent = 0);
    ~KdeNotify();

private:
    QDBusInterface *notifier;
    int m_NotifyDelay;

private slots:
    void showMetaData();
    QString totalTimeString();


};

#endif // KDENOTIFY_H
