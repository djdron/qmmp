#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <qmmp/qmmp.h>

#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QSettings settings(Qmmp::configFile(),QSettings::IniFormat);
    settings.beginGroup("Kde_Notifier");
    ui->kNotifyDelaySpinBox->setValue(settings.value("notify_delay",10000).toInt());
    settings.endGroup();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(),QSettings::IniFormat);
    settings.beginGroup("Kde_Notifier");
    settings.setValue("notify_delay",ui->kNotifyDelaySpinBox->value());
    settings.endGroup();
    QDialog::accept();
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
