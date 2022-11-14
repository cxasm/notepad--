#pragma once

#include <QWidget>
#include "ui_ctipwin.h"

class CTipWin : public QWidget
{
	Q_OBJECT

public:
	CTipWin(QWidget *parent = Q_NULLPTR);
	~CTipWin();
	void setTipText(QString text);
	void showMsg(int sec = 2000);

private slots:
	void slot_delayClose();

private:
	Ui::CTipWin ui;
};
