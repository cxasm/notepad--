#include "filecmprulewin.h"

FileCmpRuleWin::FileCmpRuleWin(int mode, bool blankMath, int equalRato,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	if (mode == 0)
	{
		ui.radioButtonDefault->setChecked(true);
	}
	else if(mode == 1)
	{
		ui.radioButtonIgnoreBackWhite->setChecked(true);
	}
	else if (mode == 2)
	{
		ui.radioButtonIgnoreAllWhite->setChecked(true);
	}

	ui.checkBoxBlankMatch->setChecked(blankMath);

	int index = 0;

	if (equalRato == 70)
	{
		index = 1;
	}
	else if (equalRato == 90)
	{
		index = 2;
	}
	
	ui.comboBoxEqualRato->setCurrentIndex(index);
}

FileCmpRuleWin::~FileCmpRuleWin()
{
}

void FileCmpRuleWin::slot_apply()
{
	int status = 0;
	
	if (ui.radioButtonDefault->isChecked())
	{
		status = 0;
	}
	else if (ui.radioButtonIgnoreBackWhite->isChecked())
	{
		status = 1;
	}
	else if (ui.radioButtonIgnoreAllWhite->isChecked())
	{
		status = 2;
	}

	bool blankMatch = (ui.checkBoxBlankMatch->isChecked() ? true : false);
	int equalRato = 50;

	if (ui.comboBoxEqualRato->currentIndex() == 1)
	{
		equalRato = 70;
	}
	else if (ui.comboBoxEqualRato->currentIndex() == 2)
	{
		equalRato = 90;
	}

	emit sign_cmpModeChange(status, blankMatch, equalRato);
	close();
}
