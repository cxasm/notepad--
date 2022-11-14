#pragma once

#include <QWidget>
#include "ui_optionsview.h"

class OptionsView : public QWidget
{
	Q_OBJECT

public:
	OptionsView(QWidget* pNotepadWin, QWidget *parent = Q_NULLPTR);
	~OptionsView();

signals:
	void sendTabFormatChange(bool, bool);
	//void signTxtFontChange(QFont& font);
	void signProLangFontChange(QFont& font);

private slots:
	void slot_curRowChanged(int row);
	void slot_ok();

private:
	Ui::OptionsView ui;

	QWidget* m_pNotepadWin;
};
