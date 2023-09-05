#include "findcmpwin.h"
#include "qscidisplaywindow.h"
#include <QRadioButton>
#include <QMessageBox>

FindCmpWin::FindCmpWin(RC_DIRECTION dir, QWidget *parent):QMainWindow(parent), m_editWidget(nullptr), m_isFindFirst(true), m_findHistory(nullptr)
{
	ui.setupUi(this);

	m_BackwardDir = false;
	m_matchWhole = false;
	m_matchCase = false;
	m_matchWrap = true;
	m_searchMode = 1;

	if (dir == RC_LEFT)
	{
		ui.leftSearch->setChecked(true);
	}
	else
	{
		ui.rightSearch->setChecked(true);
	}

	connect(ui.leftSearch, &QRadioButton::toggled, this, &FindCmpWin::slot_isSearchLeft);

	connect(ui.findModeRegularBt, &QRadioButton::toggled, this, &FindCmpWin::slot_findModeRegularBtChange);
}

FindCmpWin::~FindCmpWin()
{
}

void FindCmpWin::slot_isSearchLeft(bool checked)
{
	emit sgin_searchDirectionChange(checked ? RC_LEFT : RC_RIGHT);
}

void FindCmpWin::setCurrentTab(FindTabIndex index)
{
	ui.findinfilesTab->setCurrentIndex(index);

	if (FIND_TAB == index)
	{
		ui.findComboBox->setFocus();
	}
}

void FindCmpWin::setWorkEdit(QsciDisplayWindow *editWidget)
{
	m_editWidget = editWidget;
	m_isFindFirst = true;
}


void FindCmpWin::setFindText(QString &text)
{
	ui.findComboBox->setEditText(text);
}


void FindCmpWin::setFindHistory(QList<QString>* findHistory)
{
	m_findHistory = findHistory;

	if ((m_findHistory != nullptr) && !m_findHistory->isEmpty())
	{
		ui.findComboBox->addItems(*m_findHistory);
		ui.findComboBox->clearEditText();
	}
}

//从ui读取参数配置到成员变量
void FindCmpWin::updateParameterFromUI()
{
	if (ui.findinfilesTab->currentIndex() == 0)
	{
		int searchMode = 0;

		if (ui.findModeNormalBt->isChecked())
		{
			searchMode = 1;
		}
		else if (ui.findModeRegularBt->isChecked())
		{
			searchMode = 2;
		}

		if (m_searchMode != searchMode)
		{
			m_searchMode = searchMode;
			m_isFindFirst = true;
		}


		if (m_expr != ui.findComboBox->currentText())
		{
			m_expr = ui.findComboBox->currentText();
			m_isFindFirst = true;
		}

		if (m_BackwardDir != ui.findBackwardBox->isChecked())
		{
			m_BackwardDir = ui.findBackwardBox->isChecked();
			m_isFindFirst = true;
		}

		if (m_matchWhole != ui.findMatchWholeBox->isChecked())
		{
			m_matchWhole = ui.findMatchWholeBox->isChecked();
			m_isFindFirst = true;
		}

		if (m_matchCase != ui.findMatchCaseBox->isChecked())
		{
			m_matchCase = ui.findMatchCaseBox->isChecked();
			m_isFindFirst = true;
		}

		if (m_matchWrap != ui.findWrapBox->isChecked())
		{
			m_matchWrap = ui.findWrapBox->isChecked();
			m_isFindFirst = true;
		}
	}

	m_re = ((m_searchMode == 2) ? true : false);

	if (m_cs != m_matchCase)
	{
		m_cs = m_matchCase;
	}

	if (m_wo != m_matchWhole)
	{
		m_wo = m_matchWhole;
	}

	if (m_wrap != m_matchWrap)
	{
		m_wrap = m_matchWrap;
	}

	m_forward = !m_BackwardDir;
}

void FindCmpWin::addFindHistory(QString &text)
{
	if ((m_findHistory != nullptr) && (-1 == m_findHistory->indexOf(text)))
	{
		m_findHistory->push_front(text);
		ui.findComboBox->insertItem(0, text);
	}
}

//检查是否是第一次查找，凡是参数变化了，则认定为是第一次查找。
//因为查找分firstFirst和firstNext，则是qscint特性决定的。所以正确识别第一次查找是必要的
bool FindCmpWin::isFirstFind()
{
	return m_isFindFirst;
}

//一旦修改条件发生变化，则认定为第一次查找
void FindCmpWin::slot_findNext()
{
	if (ui.findComboBox->currentText().isEmpty())
	{
		ui.statusbar->showMessage(tr("what find is null !"), 3000);
		return;
	}

	updateParameterFromUI();


	QsciDisplayWindow* pEdit = m_editWidget;

	//第一次查找
	if (isFirstFind())
	{
		if (pEdit != nullptr)
		{
			QString whatFind = ui.findComboBox->currentText();

			if (!pEdit->findFirst(whatFind, m_re, m_cs, m_wo, m_wrap, m_forward))
			{
				ui.statusbar->showMessage(tr("cant't find text \'%1\'").arg(m_expr),3000);
			}

			m_isFindFirst = false;

			//加入历史列表
			addFindHistory(whatFind);
		}
	}
	else
	{
		//查找下一个
		if (pEdit != nullptr)
		{
			if (!pEdit->findNext())
			{
				ui.statusbar->showMessage(tr("no more find text \'%1\'").arg(m_expr),3000);
			}
		}
	}
}

//
//void FindCmpWin::addCurFindRecord(QsciDisplayWindow* pEdit, FindCmpRecords& recordRet)
//{
//	int pos = pEdit->execute(SCI_GETCURRENTPOS);
//	int lineNum = pEdit->execute(SCI_LINEFROMPOSITION, pos);
//	int lineLens = pEdit->execute(SCI_LINELENGTH, lineNum);
//
//	char* lineText = new char[lineLens + 1];
//	memset(lineText, 0, lineLens + 1);
//	pEdit->execute(SCI_GETCURLINE, lineLens, reinterpret_cast<sptr_t>(lineText));
//
//	FindCmpRecord aRecord;
//	aRecord.lineNum = lineNum;
//	aRecord.pos = pos;
//	aRecord.lineContents = QString(lineText);
//
//	delete[]lineText;
//	
//	recordRet.records.append(aRecord);
//}

void FindCmpWin::slot_findModeRegularBtChange(bool checked)
{
	if (checked)
	{
		ui.findBackwardBox->setEnabled(false);
		ui.findBackwardBox->setChecked(false);
		ui.findMatchWholeBox->setEnabled(false);
		ui.findMatchWholeBox->setChecked(false);
	}
	else
	{
		ui.findBackwardBox->setEnabled(true);
		ui.findMatchWholeBox->setEnabled(true);
	}
	m_isFindFirst = true;
}
