#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include "ui_dialog.h"

class renameDialog : public QDialog
{
    Q_OBJECT
public:
	renameDialog(QWidget *parent = 0);
	~renameDialog();
	Ui::Dialog ui;
};

#endif
