#pragma once

#include <QDialog>
#include <QCloseEvent>
#include "ui_progresswin.h"

class ProgressWin : public QDialog
{
	Q_OBJECT

public:
	ProgressWin(QWidget *parent = Q_NULLPTR);
	virtual ~ProgressWin();


	void info(QString text);
	void setTotalSteps(int step);
	void moveStep();

	int getTotalStep();
	void setStep(int step);

	bool isCancel();

	void setCancel();

protected:
	void closeEvent(QCloseEvent* e) override;

public slots:
	void slot_quitBt();

signals:
	void quitClick();

private:
	Ui::ProgressWin ui;
	int m_curStep;

	bool m_isCancel;
};
