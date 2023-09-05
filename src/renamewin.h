#pragma once

#include <QWidget>
#include "ui_renamewin.h"

class ReNameWin : public QWidget
{
	Q_OBJECT

public:
	ReNameWin(QWidget *parent = Q_NULLPTR);
	~ReNameWin();

	

private slots:
	void slot_selectDir();
	void slot_renameOptionsChange(int id, bool status);
	void slot_userDefineExt();
	void slot_startRename();

private:
	void changeFileExt();
	void changeFileName();

private:
	Ui::ReNameWin ui;
	int m_extComBoxNum;
};
