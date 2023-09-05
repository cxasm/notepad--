#pragma once

#include <QWidget>
#include "ui_hexfilegoto.h"

class HexFileGoto : public QWidget
{
	Q_OBJECT

public:
	HexFileGoto(QWidget *parent = Q_NULLPTR);
	~HexFileGoto();
	qint64 getFileAddr();

signals:
	void gotoClick(qint64 addr);

private slots:
	void slot_goto();
	void slot_showDecInfo(const QString & text);
	void slot_showHexInfo(const QString & text);

private:
	Ui::HexFileGoto ui;
};
