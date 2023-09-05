#pragma once

#include <QWidget>
#include "ui_hexrulewin.h"

class HexRuleWin : public QWidget
{
	Q_OBJECT

public:
	HexRuleWin(int mode=0, int highlight=0, QWidget *parent = Q_NULLPTR);
	~HexRuleWin();

signals:
	void modeChange(int mode, int highlightBack);

private slots:
	void slot_okBt();

private:
	Ui::HexRuleWin ui;
	int m_mode;
	int m_isHighlightBackgroud;
};
