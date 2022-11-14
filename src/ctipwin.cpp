#include "ctipwin.h"

#include <QTimer>

CTipWin::CTipWin(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::ToolTip);

	QPalette  palette(this->palette());
	palette.setColor(QPalette::Background, QColor(0xfff29d));
	this->setPalette(palette);
}

CTipWin::~CTipWin()
{
}

void CTipWin::setTipText(QString text)
{
	ui.labelInfo->setText(text);
}

void CTipWin::showMsg(int sec)
{
	show();

	QTimer::singleShot(sec, this, SLOT(slot_delayClose()));
}

void CTipWin::slot_delayClose()
{
	close();
}

