#include <QtPlugin>
#include <QTranslator>
#include <QLocale>

#include "incdecvolumeoption.h"

bool IncDecVolumeCommandLineOption::identify(const QString & str) const
{
    if (
        str == QString("--volume-inc") ||
        str == QString("--volume-dec")
    )
    {
        return TRUE;
    }

    return FALSE;
}

const QString IncDecVolumeCommandLineOption::helpString() const
{
    return  QString(
                "--volume-inc         " + tr("Increase volume with step 5")+"\n"
                "--volume-dec         " + tr("Decrease volume with step 5")+"\n"
            );
}


void IncDecVolumeCommandLineOption::executeCommand(const QString& opt_str, CommandLineManager* clm, Control* ctrl)
{
    int volume = qMax(clm->leftVolume(), clm->rightVolume());
    int balance = 0;
    int left = clm->leftVolume();
    int right = clm->rightVolume();
    if (left || right)
        balance = (right - left)*100/volume;

    if (opt_str == "--volume-inc")
    {
        volume = qMin (100, volume + 5);
    }
    else if (opt_str == "--volume-dec")
    {
        volume = qMax (0, volume - 5);
    }
    ctrl->setVolume(volume-qMax(balance,0)*volume/100,
                    volume+qMin(balance,0)*volume/100);
}

const QString IncDecVolumeCommandLineOption::name() const
{
    return "IncDecVolumeCommandLineOption";
}

QTranslator *IncDecVolumeCommandLineOption::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/incdecvolume_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(IncDecVolumeCommandLineOption)
