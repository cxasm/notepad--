#pragma once

#include <QWidget>
#include "ui_texteditsetwin.h"

class TextEditSetWin : public QWidget
{
	Q_OBJECT

public:
	TextEditSetWin(QWidget *parent = Q_NULLPTR);
	~TextEditSetWin();
	void setNotePadWin(QWidget * w);
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
	void slot_txtFontSet();
private:
	Ui::TextEditSetWin ui;

	QWidget* m_notepadWin;
	//QFont m_curFont;//text的font

	//QFont m_curProLangFont; //编程语言的font
};
