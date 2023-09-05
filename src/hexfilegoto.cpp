#include "hexfilegoto.h"

HexFileGoto::HexFileGoto(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.lineEditDecAddr, &QLineEdit::textChanged, this, &HexFileGoto::slot_showDecInfo);
	connect(ui.lineEditHexAddr, &QLineEdit::textChanged, this, &HexFileGoto::slot_showHexInfo);
}

HexFileGoto::~HexFileGoto()
{
}

//显示为
void HexFileGoto::slot_showDecInfo(const QString &text)
{
	bool ok;
	qlonglong num = text.toLongLong(&ok, 10);

	if (ok)
	{
		QString v = QString("Dec : %1 \nHex : 0x%2 ").arg(QString::number(num, 10)).arg(QString::number(num, 16));
		ui.textBrowser->setText(v);
	}
	else
	{
		QString v = QString("Dec : error \nHex : error ");
		ui.textBrowser->setText(v);
	}
}

void HexFileGoto::slot_showHexInfo(const QString &text)
{
	bool ok;
	qlonglong num = text.toLongLong(&ok, 16);

	if (ok)
	{
		QString v = QString("Dec : %1 \nHex : 0x%2 ").arg(QString::number(num, 10)).arg(QString::number(num, 16));
		ui.textBrowser->setText(v);
	}
	else
	{
		QString v = QString("Dec : error \nHex : error ");
		ui.textBrowser->setText(v);
	}
}

qint64 HexFileGoto::getFileAddr()
{
	bool ok;
	qint64 ret = 0;

	if (ui.radioButtonHex->isChecked())
	{
		ret = ui.lineEditHexAddr->text().toLongLong(&ok,16);

		ui.lineEditDecAddr->setText(QString::number(ret));
	}
	else
	{
		ret = ui.lineEditDecAddr->text().toLongLong(&ok);

		ui.lineEditHexAddr->setText(QString::number(ret,16));
	}

	return (ok ? ret: -1);
}

void HexFileGoto::slot_goto()
{
	emit gotoClick(getFileAddr());
}
