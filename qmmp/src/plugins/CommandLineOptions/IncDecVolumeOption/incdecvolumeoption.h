#ifndef IncDecVolumeCommandLineOption_H
#define IncDecVolumeCommandLineOption_H

#include <QString>
#include <QObject>

#include <qmmpui/commandlineoption.h>
#include <qmmpui/control.h>
#include <qmmpui/commandlinemanager.h>


class IncDecVolumeCommandLineOption : public QObject, public CommandLineOption
{
Q_OBJECT
Q_INTERFACES(CommandLineOption)
public:
    virtual bool identify(const QString& opt_str)const;
    virtual const QString name()const;
    virtual const QString helpString()const;
    virtual void executeCommand(const QString& opt_str, CommandLineManager* clm, Control* ctrl);
    virtual QTranslator *createTranslator(QObject *parent);
};

#endif

