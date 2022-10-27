#pragma once

#include <QWidget>
#include "ui_dirfindfile.h"

class DirFindFile : public QWidget
{
	Q_OBJECT

public:
	DirFindFile(int dire, QWidget* parent = Q_NULLPTR);
	virtual ~DirFindFile();

signals:
	void signFindFile(int dire, int prevOrNext, QString fileName, bool caseSenstive);

public slots:
	void slot_findPrev();
	void slot_findNext();


private:
	Ui::DirFindFile ui;
};
