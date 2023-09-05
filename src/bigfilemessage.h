#pragma once
#include "ccnotepad.h"

#include <QWidget>
#include <QDialog>
#include "ui_bigfilemessage.h"

class BigFileMessage : public QDialog
{
	Q_OBJECT

public:
	BigFileMessage(QWidget *parent = nullptr);
	~BigFileMessage();
	void setTip(QString msg);
	void setDefOpenMode(NddDocType defMode);

private slots:
	void slot_okBt();
	void slot_cancelBt();

private:
	Ui::BigFileMessageClass ui;
	int m_result;
};
