#pragma once

#include <QDialog>
#include "ui_closeDlg.h"

class closeDlg : public QDialog
{
	Q_OBJECT

public:
	closeDlg(QWidget *parent = Q_NULLPTR);
	virtual ~closeDlg();

	

public slots:
	void save();
	void discard();
	void cancel();

private:
	Ui::closeDlg ui;
};
