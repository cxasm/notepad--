#pragma once

#include <QWidget>
#include "ui_gotolinewin.h"

class GoToLineWin : public QWidget
{
	Q_OBJECT

public:
	GoToLineWin(QWidget *parent = Q_NULLPTR);
	~GoToLineWin();

signals:
	void sign_gotoLine(int dire, int lineNum);

private slots:
	void slot_ok();

private:
	Ui::GoToLineWin ui;
};
