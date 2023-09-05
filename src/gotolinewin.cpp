#include "gotolinewin.h"

GoToLineWin::GoToLineWin(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

GoToLineWin::~GoToLineWin()
{
}

void GoToLineWin::slot_ok()
{
	bool ok;
	int lineNums = ui.lineEditNum->text().toInt(&ok);
	if (ok && lineNums>=1)
	{
		int dire = (ui.radioButton->isChecked() ? 0 : 1);
		emit sign_gotoLine(dire, lineNums-1);
		close();
	}
}
