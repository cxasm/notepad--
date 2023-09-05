#pragma once

#include <QWidget>
#include <QTabWidget>
#include "ui_columnedit.h"

class ColumnEdit : public QWidget
{
	Q_OBJECT

public:
	ColumnEdit(QWidget *parent = Q_NULLPTR);
	~ColumnEdit();


	void setTabWidget(QTabWidget * editTabWidget);

private:

	QWidget * autoAdjustCurrentEditWin();


private slots:
	void slot_insertNumEnable(bool check);
	void slot_insertTextEnable(bool check);
	void slot_addPrefix(int s);
	void slot_bigChar(bool isCheck);
	void slot_ok();

private:
	Ui::ColumnEdit ui;
	QWidget* m_curEditWin;
	QTabWidget *m_editTabWidget;
};
