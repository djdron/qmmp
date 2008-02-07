#ifndef IncDecVolumeCommandLineOption_H
#define IncDecVolumeCommandLineOption_H 

#include <commandlineoption.h>

#include <QString>
#include <QObject>

class MainWindow;

class IncDecVolumeCommandLineOption : public QObject, public CommandLineOption
{
Q_OBJECT
Q_INTERFACES(CommandLineOption)
public:
    virtual bool identify(const QString& opt_str)const;
    virtual const QString name()const;
    virtual const QString helpString()const;
    virtual void executeCommand(const QString& opt_str,MainWindow* mw);
};

#endif

