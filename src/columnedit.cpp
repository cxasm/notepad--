#include "columnedit.h"
#include "scintillaeditview.h"

ColumnEdit::ColumnEdit(QWidget *parent)
	: QWidget(parent), m_curEditWin(nullptr), m_editTabWidget(nullptr)
{
	ui.setupUi(this);

	connect(ui.addPrefix, &QCheckBox::stateChanged, this, &ColumnEdit::slot_addPrefix);
	connect(ui.is16, &QRadioButton::clicked, this, &ColumnEdit::slot_bigChar);
}

ColumnEdit::~ColumnEdit()
{
}


void ColumnEdit::slot_insertTextEnable(bool check)
{
	if (check)
	{
		if (ui.numGroupBox->isChecked())
		{
			ui.numGroupBox->setChecked(!check);
		}
	}
}

void ColumnEdit::slot_insertNumEnable(bool check)
{
	if (check)
	{
		if (ui.textGroupBox->isChecked())
		{
			ui.textGroupBox->setChecked(!check);
		}
	}
}

void ColumnEdit::slot_addPrefix(int s)
{
	if (s == Qt::Checked)
	{
		ui.prefix->setEnabled(true);
	}
	else
	{
		ui.prefix->setEnabled(false);
	}
}

void ColumnEdit::slot_bigChar(bool isCheck)
{
	ui.capital->setEnabled(isCheck);
}

//自动调整当前窗口的状态
QWidget* ColumnEdit::autoAdjustCurrentEditWin()
{
	QWidget* pw = m_editTabWidget->currentWidget();

	if (m_curEditWin != pw)
	{
		m_curEditWin = pw;
	}
	return pw;
}

void ColumnEdit::setTabWidget(QTabWidget *editTabWidget)
{
	m_editTabWidget = editTabWidget;
}

void ColumnEdit::slot_ok()
{
	autoAdjustCurrentEditWin();

	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(m_curEditWin);
	if (pEdit == nullptr || pEdit->isReadOnly())
	{
		QApplication::beep();
		return;
	}

	QString text;

	bool isNum = false;
	int numType = 10;
	int initNum = 0;
	int inc = ui.incNum->value();
	int repeNum = ui.repeNum->value();
	bool isAddPrefix = ui.addPrefix->isChecked();
	QString prefix = ui.prefix->text();
	bool isCapital = ui.capital->isChecked();

	//是插入文本模式
	if (ui.textGroupBox->isChecked())
	{

		text = ui.inputText->text();
	}
	else
	{
		isNum = true;

		text = ui.initNum->text();

		bool ok = false;

		if (ui.is10->isChecked())
		{
			numType = 10;
		}
		else if (ui.is16->isChecked())
		{
			numType = 16;
		}
		else if (ui.is8->isChecked())
		{
			numType = 8;
		}
		else if (ui.is2->isChecked())
		{
			numType = 2;
		}
		int num = text.toInt(&ok, 10);

		initNum = num;

		if (ok)
		{
			text = QString::number(num, numType);

			if (isAddPrefix)
			{
				text = prefix + text;
			}
		}
		else
		{
			//这里要加个提示
			QApplication::beep();
			return;
		}
	}

	pEdit->execute(SCI_BEGINUNDOACTION);

	if (ui.textGroupBox->isChecked())
	{
		if (pEdit->execute(SCI_SELECTIONISRECTANGLE) || pEdit->execute(SCI_GETSELECTIONS) > 1)
		{
			ColumnModeInfos colInfos = pEdit->getColumnModeSelectInfo();
			std::sort(colInfos.begin(), colInfos.end(), SortInPositionOrder());

			QByteArray bytes = text.toUtf8();
			pEdit->columnReplace(colInfos, bytes);
			std::sort(colInfos.begin(), colInfos.end(), SortInSelectOrder());
			pEdit->setMultiSelections(colInfos);

			return;
		}
	}
	else
	{
		if (pEdit->execute(SCI_SELECTIONISRECTANGLE) || pEdit->execute(SCI_GETSELECTIONS) > 1)
		{
			ColumnModeInfos colInfos = pEdit->getColumnModeSelectInfo();

			// If there is no column mode info available, no need to do anything
			// If required a message can be shown to user, that select column properly or something similar
			if (colInfos.size() > 0)
			{
				std::sort(colInfos.begin(), colInfos.end(), SortInPositionOrder());
				QByteArray bytes;
				if (isAddPrefix)
				{
					bytes = prefix.toUtf8();
				}
				pEdit->columnReplace(colInfos, initNum, inc, repeNum, numType, isCapital, bytes);
				std::sort(colInfos.begin(), colInfos.end(), SortInSelectOrder());
				pEdit->setMultiSelections(colInfos);
			}
			return;
		}
	}

	auto cursorPos = pEdit->execute(SCI_GETCURRENTPOS);
	auto cursorCol = pEdit->execute(SCI_GETCOLUMN, cursorPos);
	auto cursorLine = pEdit->execute(SCI_LINEFROMPOSITION, cursorPos);
	auto endPos = pEdit->execute(SCI_GETLENGTH);
	auto endLine = pEdit->execute(SCI_LINEFROMPOSITION, endPos);
	
	QByteArray lineData;

	int rn = repeNum;

	for (size_t i = cursorLine; i <= static_cast<size_t>(endLine); ++i)
	{
		auto lineBegin = pEdit->execute(SCI_POSITIONFROMLINE, i);
		auto lineEnd = pEdit->execute(SCI_GETLINEENDPOSITION, i);

		auto lineEndCol = pEdit->execute(SCI_GETCOLUMN, lineEnd);
		auto lineLen = lineEnd - lineBegin;
		lineData.resize(lineLen);
			

		Sci_TextRange  lineText;
		lineText.chrg.cpMin = static_cast<Sci_Position>(lineBegin);
		lineText.chrg.cpMax = static_cast<Sci_Position>(lineEnd);
		lineText.lpstrText = lineData.data();
		//获取原始行的内容
		pEdit->SendScintilla(SCI_GETTEXTRANGE, 0, &lineText);

		if (lineEndCol < cursorCol)
		{
			QByteArray s_space(cursorCol - lineEndCol, ' ');
			lineData.append(s_space);
			lineData.append(text.toUtf8());
		}
		else
		{
			int posAbs2Start = pEdit->execute(SCI_FINDCOLUMN, i, cursorCol);
			int posRelative2Start = posAbs2Start - lineBegin;
			lineData.insert(posRelative2Start, text.toUtf8());
		}

		pEdit->SendScintilla(SCI_SETTARGETRANGE, lineBegin, lineEnd);
		pEdit->SendScintilla(SCI_REPLACETARGET, lineData.size(), lineData.data());

		if (isNum)
		{
			--rn;

			if (rn > 0)
			{

			}
			else
			{
				rn = repeNum;
				initNum += inc;

			}
			

			if (numType != 16)
			{
			text = QString::number(initNum, numType);
			}
			else
			{
				if (isCapital)
				{
					text = QString::number(initNum, numType).toUpper();
				}
				else
				{
					text = QString::number(initNum, numType);
				}
			}


			if (isAddPrefix)
			{
				text = prefix + text;
			}
		}
	}

	pEdit->execute(SCI_ENDUNDOACTION);
}
