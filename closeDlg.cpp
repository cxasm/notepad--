#include "closeDlg.h"

closeDlg::closeDlg(QWidget *parent): QDialog(parent)
{
	ui.setupUi(this);
}

closeDlg::~closeDlg()
{
}

void closeDlg::save()
{
	if (ui.leftBox->isChecked() && ui.rightBox->isChecked())
	{
		done(3);
	}
	else if (ui.leftBox->isChecked())
	{
		done(1);
	}
	else if (ui.rightBox->isChecked())
	{
		done(2);
	}
	else
	{
		done(0);
	}
}

void closeDlg::discard()
{
	done(-1);
}

void closeDlg::cancel()
{
	done(-2);
}