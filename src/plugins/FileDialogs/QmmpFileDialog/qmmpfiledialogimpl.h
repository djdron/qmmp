#ifndef QMMPFILEDIALOGIMPL_H
#define QMMPFILEDIALOGIMPL_H

#include "ui_qmmpfiledialog.h"
#include <QDialog>

#include <qmmpui/filedialog.h>

class QDirModel;


class QmmpFileDialogImpl : public QDialog , private Ui::QmmpFileDialog
{
    Q_OBJECT
public:
    QmmpFileDialogImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~QmmpFileDialogImpl();
    void setModeAndMask(const QString&,FileDialog::Mode m,const QStringList& mask);
protected slots:
    void on_lookInComboBox_activated(const QString&);
    void on_upToolButton_clicked();
    void on_fileListView_doubleClicked(const QModelIndex&);
    void on_fileNameLineEdit_returnPressed();
    void on_addPushButton_clicked();
    void on_listToolButton_toggled(bool);
    void on_iconToolButton_toggled(bool);
signals:
    void filesAdded(const QStringList&);
protected:
    QDirModel* m_model;

};
#endif //QMMPFILEDIALOGIMPL_H
