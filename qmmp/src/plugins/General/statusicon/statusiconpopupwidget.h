#ifndef STATUSICONPOPUPWIDGET_H
#define STATUSICONPOPUPWIDGET_H

#include <QFrame>
#include <QWidget>
#include <QGridLayout>
#include "plugins/General/covermanager/coverwidget.h"

class QLabel;
class QTimer;


class StatusIconPopupWidget : public QFrame
{
    Q_OBJECT
public:

    StatusIconPopupWidget(QWidget * parent = 0);
    ~StatusIconPopupWidget();

    void showInfo(int x, int y, int delay, bool splitFileName); //x,y are tray icon position

protected:

    virtual void mousePressEvent(QMouseEvent *);

private:

    QString totalTimeString();
    QLabel *m_lblTitle;
    QLabel *m_lblArtist;
    QLabel *m_lblAlbum;
    QLabel *m_lblTime;
    QGridLayout * gLayout;
    QTimer *m_timer;
    CoverWidget * m_cover;
    QString m_totalTime;
    QSpacerItem *m_spacer;

    int m_lastTrayX;
    int m_lastTrayY;
    int m_splitFileName;

private slots:

    void updatePosition(int trayx, int trayy);
    void updateMetaData();
    void updateTime(qint64 elapsed);

};

#endif // STATUSICONPOPUPWIDGET_H
