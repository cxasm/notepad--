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

#if 0
	void setProgramLangFont(QFont & font);
#endif
signals:
	void sendTabFormatChange(bool, bool);
	//void signAppFontChange(QFont& font);
	void signProLangFontChange(QFont& font);
private:
	
	void save();



private slots:
#if 0
	void slot_selectAppFont();

	void slot_selectProLangFont();

	void slot_appFontColor();
#endif
	void slot_txtFontSet();


private:
	Ui::TextEditSetWin ui;

	QWidget* m_notepadWin;

	//QFont m_curProLangFont; //编程语言的font
};
