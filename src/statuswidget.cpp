#include "statuswidget.h"

StatusWidget::StatusWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

StatusWidget::~StatusWidget()
{
}

void StatusWidget::setTipMsg(QString msg)
{
	ui.msgLable->setText(msg);
}

