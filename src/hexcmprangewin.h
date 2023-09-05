#pragma once

#include <QDialog>
#include "ui_hexcmprangewin.h"

class HexCmpRangeWin : public QDialog
{
	Q_OBJECT

public:
	HexCmpRangeWin(QWidget *parent = Q_NULLPTR);
	~HexCmpRangeWin();
	void getRange(bool & isCancel, qint64 & leftStartPos, int & leftCmpLen, qint64 & rightStartPos, int & rightCmpLen);

private slots:
	void slot_ok();
	void slot_asyncStartPos(const QString & text);
	void slot_asyncLenPos(const QString & text);


private:
	Ui::HexCmpRangeWin ui;

	bool m_isCancel;
};
