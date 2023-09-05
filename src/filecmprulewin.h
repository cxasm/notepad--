#pragma once

#include <QWidget>
#include "ui_filecmprulewin.h"

class FileCmpRuleWin : public QWidget
{
	Q_OBJECT

public:
	FileCmpRuleWin(int mode, bool blankMath, int equalRato, QWidget *parent = Q_NULLPTR);
	virtual ~FileCmpRuleWin();

signals:
	void sign_cmpModeChange(int mode,bool blankMatch,int equalRato);

private slots:
	void slot_apply();

private:
	Ui::FileCmpRuleWin ui;
};
