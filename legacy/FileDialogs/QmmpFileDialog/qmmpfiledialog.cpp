#include <QtPlugin>

#include "qmmpfiledialogimpl.h"
#include "qmmpfiledialog.h"



QmmpFileDialog::QmmpFileDialog()
{
    m_dialog = new QmmpFileDialogImpl();
    connect(m_dialog,SIGNAL(filesAdded(const QStringList&)),this,SIGNAL(filesAdded(const QStringList&)));
}

void QmmpFileDialog::handleSelected(/*const QStringList& s */)
{
}

bool QmmpFileDialog::modal()const
{
    return FALSE;
}

QmmpFileDialog::~QmmpFileDialog()
{
    qWarning("QmmpFileDialog::~QmmpFileDialog()");
    delete m_dialog;
}
void QmmpFileDialog::raise(const QString& d,Mode m,const QStringList& f)
{
    m_dialog->setModeAndMask(d,m,f);
    m_dialog->show();
    m_dialog->raise();
}


FileDialog* QmmpFileDialogFactory::create(){ return new QmmpFileDialog();}

QString QmmpFileDialogFactory::name(){return QmmpFileDialogFactoryName;}

QString QmmpFileDialogFactory::QmmpFileDialogFactoryName = "Qmmp File Dialog";


Q_EXPORT_PLUGIN(QmmpFileDialogFactory)

