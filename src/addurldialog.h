#ifndef ADDURLDIALOG_H
#define ADDURLDIALOG_H

#include "ui_addurldialog.h"
#include <QDialog>
#include <QPointer>


class PlayListModel;

class AddUrlDialog : public QDialog , private Ui::AddUrlDialog
{
  Q_OBJECT
public:
    static void popup(QWidget* parent ,PlayListModel*);
protected:
  AddUrlDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
  ~AddUrlDialog();
protected slots:
  virtual void accept();
private:
    void setModel(PlayListModel*);
    static QPointer<AddUrlDialog> instance;
    PlayListModel* m_model;

};
#endif //ADDURLDIALOG_H
