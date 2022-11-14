#include "textfind.h"

TextFind::TextFind(int dire, QWidget *parent): QWidget(parent)
{
	ui.setupUi(this);
}


TextFind::~TextFind()
{
}


void TextFind::slot_findPrev()
{
	QString text = ui.textLineEdit->text();
	text = text.trimmed();


	emit signFindFile(0, text);
}

void TextFind::slot_findNext()
{
	QString text = ui.textLineEdit->text();
	text = text.trimmed();

	emit signFindFile(1, text);
}