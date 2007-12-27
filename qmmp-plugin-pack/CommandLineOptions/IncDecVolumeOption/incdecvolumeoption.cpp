#include <QtPlugin>

#include "incdecvolumeoption.h"
#include <soundcore.h>
#include "mainwindow.h"

bool IncDecVolumeCommandLineOption::identify(const QString & str) const
{
    if(
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
            "--volume-inc         Increase volume with step 10\n"
            "--volume-dec         Decrease volume with step 10\n"
                   );
}


void IncDecVolumeCommandLineOption::executeCommand(const QString & option_string, MainWindow *mw)
{
    if (option_string == "--volume-inc")
    {
	int l = 0;
	int r = 0;
	mw->soundCore()->volume(&l,&r);
        mw->soundCore()->setVolume(l+10,r+10);
    }
    else if (option_string == "--volume-dec")
    {
	int l = 0;
	int r = 0;
	mw->soundCore()->volume(&l,&r);
        mw->soundCore()->setVolume(l-10,r-10);    
    }
}

const QString IncDecVolumeCommandLineOption::name() const
{
    return "IncDecVolumeCommandLineOption";
}

Q_EXPORT_PLUGIN(IncDecVolumeCommandLineOption)

