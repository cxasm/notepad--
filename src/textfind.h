#pragma once

#include <QWidget>
#include "ui_textfind.h"

class TextFind : public QWidget
{
	Q_OBJECT

public:
	TextFind(int dire, QWidget *parent = Q_NULLPTR);
	virtual ~TextFind();

signals:
	void signFindFile(int prevOrNext, QString text);

public slots:
	void slot_findPrev();
	void slot_findNext();

private:
	Ui::TextFind ui;
};
