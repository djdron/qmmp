#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include "ui_renamedialog.h"

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class RenameDialog : public QDialog
{
	Q_OBJECT
public:
	RenameDialog(QWidget *parent = 0);
	~RenameDialog();
	Ui::RenameDialog ui;
};

#endif
