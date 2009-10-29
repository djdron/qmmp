#include "kdenotifyfactory.h"
#include "kdenotify.h"
#include "settingsdialog.h"

#include <QtGui>

const GeneralProperties KdeNotifyFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("KDE 4 notification plugin");
    properties.shortName = "kdenotify_icon";
    properties.hasAbout = TRUE;
    properties.hasSettings = TRUE;
    properties.visibilityControl = FALSE;
    return properties;
}

General *KdeNotifyFactory::create(QObject *parent)
{
    return new KdeNotify(parent);
}

QDialog *KdeNotifyFactory::createConfigDialog(QWidget *parent)
{
    return new SettingsDialog(parent);
}

void KdeNotifyFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About KDE Notification Plugin"),
                        tr("KDE 4 notification plugin for Qmmp \n") +
                        "Artur Guzik <a.guzik88@gmail.com>");
}

QTranslator *KdeNotifyFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/kdenotify_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(KdeNotifyFactory)
