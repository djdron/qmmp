#include "statusiconpopupwidget.h"
#include "qmmp/soundcore.h"

#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QFileInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QSpacerItem>

StatusIconPopupWidget::StatusIconPopupWidget(QWidget * parent)
        : QFrame(parent)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint |
                   Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::FramelessWindowHint);
    setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    setLineWidth(0);

    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    gLayout = new QGridLayout(this);
    m_lblTitle = new QLabel();
    m_lblArtist = new QLabel();
    m_lblAlbum = new QLabel();
    m_lblTime = new QLabel();
    m_spacer = new QSpacerItem(20,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_cover = new CoverWidget();

    m_cover->setMinimumSize(100,100);
    m_cover->setMaximumSize(100,100);

    gLayout->addWidget(m_cover,0,0,5,1);
    gLayout->addWidget(m_lblTitle,0,1);
    gLayout->addWidget(m_lblArtist,1,1);
    gLayout->addWidget(m_lblAlbum,2,1);
    gLayout->addWidget(m_lblTime,3,1);
    setLayout(gLayout);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(TRUE);
    m_timer->stop();

    gLayout->addItem(m_spacer,4,1,1,1);
    gLayout->setHorizontalSpacing(15);

    connect(m_timer,SIGNAL(timeout()),SLOT(deleteLater()));
    connect(SoundCore::instance(),SIGNAL(metaDataChanged()),this,SLOT(updateMetaData()));
    connect(SoundCore::instance(),SIGNAL(elapsedChanged(qint64)),this,SLOT(updateTime(qint64)));
    connect(SoundCore::instance(),SIGNAL(stateChanged(Qmmp::State)),this,SLOT(updateMetaData()));

    setVisible(FALSE);
}

StatusIconPopupWidget::~StatusIconPopupWidget()
{
}

void StatusIconPopupWidget::mousePressEvent(QMouseEvent *)
{
    deleteLater();
}

void StatusIconPopupWidget::updateMetaData()
{
    SoundCore *core = SoundCore::instance();
    if(core->state() == Qmmp::Playing)
    {
        QString title = core->metaData(Qmmp::TITLE);
        QString artist = core->metaData(Qmmp::ARTIST);
        QString album = core->metaData(Qmmp::ALBUM);

        if(title.isEmpty())
        {
            title = QFileInfo(core->metaData(Qmmp::URL)).completeBaseName();
            if(m_splitFileName && title.contains("-"))
            {
                artist = title.section('-',0,0).trimmed();
                title = title.section('-',1,1).trimmed();
            }
        }

        m_lblTitle->setText("<b>" + title + "</b>");

        if(!artist.isEmpty())
        {
            m_lblArtist->setText(artist);
            m_lblArtist->setVisible(TRUE);
        }
        else
        {
            m_lblArtist->setVisible(FALSE);
        }

        if(!album.isEmpty())
        {
            m_lblAlbum->setText(album);
            m_lblAlbum->setVisible(TRUE);
        }
        else
        {
            m_lblAlbum->setVisible(FALSE);
        }

        QPixmap cover = Decoder::findCover(core->metaData(Qmmp::URL));
        if(cover.isNull())
        {
            m_cover->setPixmap(QPixmap(":/empty_cover.png"));
            m_cover->setVisible(TRUE);
        }
        else
        {
            m_cover->setPixmap(cover);
            m_cover->setVisible(TRUE);
        }
        m_lblTime->setVisible(TRUE);
        m_totalTime = totalTimeString();
        setVisible(TRUE);
    }
    else
    {
        m_cover->setVisible(FALSE);
        m_lblAlbum->setVisible(FALSE);
        m_lblArtist->setVisible(FALSE);
        m_lblTime->setVisible(FALSE);
        m_lblTitle->setText("<b>Nothing is playing</b>");
        setVisible(FALSE); //
    }
    qApp->processEvents();
    resize(sizeHint());
    qApp->processEvents();
    if(isVisible())
        updatePosition(m_lastTrayX,m_lastTrayY);
}

void StatusIconPopupWidget::updateTime(qint64 elapsed)
{
    int second = elapsed / 1000;
    int minute = second / 60;
    int hour = minute / 60;

    SoundCore * core = SoundCore::instance();

    if(core->totalTime() > 3600000)
    {
        m_lblTime->setText(QString("%1:%2:%3").arg(hour,2,10,QChar('0')).arg(minute%60,2,10,QChar('0'))
                           .arg(second%60,2,10,QChar('0')) + "/" + m_totalTime);
        return;
    }
    m_lblTime->setText(QString("%1:%2").arg(minute%60,2,10,QChar('0')).arg(second%60,2,10,QChar('0')) +
                       "/" + m_totalTime);
}

QString StatusIconPopupWidget::totalTimeString()
{
    SoundCore * core = SoundCore::instance();

    int second = core->totalTime() / 1000;
    int minute = second / 60;
    int hour = minute / 60;

    if(core->totalTime() > 3600000)
    {
        return QString("%1:%2:%3").arg(hour,2,10,QChar('0')).arg(minute%60,2,10,QChar('0'))
                                  .arg(second%60,2,10,QChar('0'));
    }
    return QString("%1:%2").arg(minute%60,2,10,QChar('0')).arg(second%60,2,10,QChar('0'));
}

void StatusIconPopupWidget::updatePosition(int trayx, int trayy)
{
    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    int xpos = 0;
    int ypos = 0;

    xpos = screenGeometry.x() + trayx -5;
    if(xpos + width() > screenGeometry.width())
        xpos = screenGeometry.width() - width() -5;

    if(trayy < screenGeometry.y()) //tray on top of screen
    {
        ypos = screenGeometry.y() + 5;
    }

    if(trayy > screenGeometry.y()) //tray on bottom
    {
        ypos = screenGeometry.y() + screenGeometry.height() - height() -5;
    }
    move(xpos,ypos);
    return;
}

void StatusIconPopupWidget::showInfo(int x, int y, int delay, bool splitFileName)
{
    m_timer->stop();
    m_timer->setInterval(delay);
    m_lastTrayX = x;
    m_lastTrayY = y;
    m_splitFileName = splitFileName;
    updateMetaData();
    qApp->processEvents();
    updatePosition(x,y);
    qApp->processEvents();
    show();
    m_timer->start();
}


