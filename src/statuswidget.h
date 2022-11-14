#pragma once

#include <QWidget>
#include "ui_statuswidget.h"

class StatusWidget : public QWidget
{
	Q_OBJECT

public:
	StatusWidget(QWidget *parent = Q_NULLPTR);
	~StatusWidget();

	void setTipMsg(QString msg);

private:
	Ui::StatusWidget ui;
};
