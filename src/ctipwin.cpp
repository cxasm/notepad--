#include "ctipwin.h"

#include <QTimer>

CTipWin::CTipWin(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::ToolTip);

	QPalette  palette(this->palette());
	palette.setColor(QPalette::Window, QColor(0xfff29d));
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


void CTipWin::showTips(QWidget* parent, QString text, int sec, bool isMousePos)
{

	if (parent != nullptr)
	{
		CTipWin* pWin = new CTipWin();
		pWin->setTipText(text);
		pWin->setAttribute(Qt::WA_DeleteOnClose);
		pWin->showMsg(sec);

		if (!isMousePos)
		{
		QPoint pos = parent->pos();
		QSize size = parent->size();

		QPoint newPos(pos.x() + 10, pos.y() + size.height() - 20);
		pWin->move(newPos);
	}
		else
		{
			pWin->move(parent->cursor().pos());
}

	}
}
