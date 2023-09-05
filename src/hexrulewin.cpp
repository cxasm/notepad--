#include "hexrulewin.h"

HexRuleWin::HexRuleWin(int mode, int highlight, QWidget *parent): QWidget(parent), m_mode(mode), m_isHighlightBackgroud(highlight)
{
	ui.setupUi(this);

	if (mode == 0)
	{
		ui.maxLcsMode->setChecked(true);
	}
	else
	{
		ui.byteCmpMode->setChecked(true);
	}

	ui.hightlightBack->setChecked((highlight == 0) ? false : true);
}

HexRuleWin::~HexRuleWin()
{
}


void HexRuleWin::slot_okBt()
{
	int nowMode = (ui.maxLcsMode->isChecked() ? 0 : 1);

	int highLight = (ui.hightlightBack->isChecked() ? 1 : 0);

	if ((nowMode != m_mode) || (highLight != m_isHighlightBackgroud))
	{
		m_mode = nowMode;
		m_isHighlightBackgroud = highLight;
		emit modeChange(m_mode, m_isHighlightBackgroud);
	}
	close();
}
