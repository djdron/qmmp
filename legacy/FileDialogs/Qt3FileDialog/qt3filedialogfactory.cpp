#include <QtPlugin>
#include "qt3filedialogfactory.h"
#include "qt3filedialog.h"


FileDialog* Qt3SupportFileDialogFactory::create()
{
    return new Qt3SupportFileDialog();
}

QString Qt3SupportFileDialogFactory::name()
{
    return Qt3SupportFileDialogFactory::Qt3SupportFileDialogFactoryName;
}


QString Qt3SupportFileDialogFactory::Qt3SupportFileDialogFactoryName = "Qt3 File Dialog";

Q_EXPORT_PLUGIN(Qt3SupportFileDialogFactory)

