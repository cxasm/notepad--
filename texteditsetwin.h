#pragma once

#include <QWidget>
#include "ui_texteditsetwin.h"

class TextEditSetWin : public QWidget
{
	Q_OBJECT

public:
	TextEditSetWin(QWidget *parent = Q_NULLPTR);
	~TextEditSetWin();
	//void setFont(QFont & font);
#if 0
	void setProgramLangFont(QFont & font);
#endif
signals:
	void sendTabFormatChange(bool, bool);
	//void signTxtFontChange(QFont& font);
	void signProLangFontChange(QFont& font);
private:
	
	void save();

private slots:
#if 0
	void slot_selectFont();

	void slot_selectProLangFont();
#endif
private:
	Ui::TextEditSetWin ui;

	//QFont m_curFont;//text的font

	//QFont m_curProLangFont; //编程语言的font
};
