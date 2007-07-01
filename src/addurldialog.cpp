#include "addurldialog.h"

#include "playlistmodel.h"

AddUrlDialog::AddUrlDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
}

AddUrlDialog::~AddUrlDialog()
{}



QPointer<AddUrlDialog> AddUrlDialog::instance = 0;

void AddUrlDialog::popup(QWidget* parent,PlayListModel* model )
{
    if(!instance) 
    {
        instance = new AddUrlDialog(parent);
        instance->setModel(model);
    }
    
    instance->show();
    instance->raise();
}

void AddUrlDialog::accept( )
{
    if(!urlComboBox->currentText().isEmpty())
    {
        QString s = urlComboBox->currentText();
        if(!s.startsWith("http://"))
            s.prepend("http://");
        
        m_model->addFile(s);
    }
    
    QDialog::accept();
}

void AddUrlDialog::setModel( PlayListModel *m )
{
    m_model = m;
}
