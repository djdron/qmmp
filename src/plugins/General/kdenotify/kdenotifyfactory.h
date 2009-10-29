#ifndef KDENOTIFYFACTORY_H
#define KDENOTIFYFACTORY_H

#include <QObject>
#include <QTranslator>
#include <QDialog>

#include <qmmpui/general.h>
#include <qmmpui/generalfactory.h>

class KdeNotifyFactory : public QObject, public GeneralFactory
{
    Q_OBJECT;
    Q_INTERFACES(GeneralFactory)
public:
    const GeneralProperties properties() const;
    General *create(QObject *parent);
    QDialog *createConfigDialog(QWidget *parent);
    void showAbout(QWidget *parent);
    QTranslator *createTranslator(QObject *parent);
};

#endif // KDENOTIFYFACTORY_H
