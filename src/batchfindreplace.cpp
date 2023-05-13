#include "batchfindreplace.h"
#include "scintillaeditview.h"
#include "ccnotepad.h"
#include "progresswin.h"
#include "nddsetting.h"
#include "ctipwin.h"

#include <QTableWidgetItem>
#include <QFileDialog>

BatchFindReplace::BatchFindReplace(QWidget *parent)
	: QMainWindow(parent), m_curEditWin(nullptr), m_editTabWidget(nullptr)
{
	ui.setupUi(this);

	m_mainNotepad = dynamic_cast<CCNotePad*>(parent);
}

BatchFindReplace::~BatchFindReplace()
{

}

void BatchFindReplace::closeEvent(QCloseEvent* event)
{
	QByteArray curGeo = this->saveGeometry();
	NddSetting::updataWinPos(BATCH_FIND_REPLACE_POS, curGeo);
}

void BatchFindReplace::setTabWidget(QTabWidget* editTabWidget)
{
	m_editTabWidget = editTabWidget;
}

//自动调整当前窗口的状态，如果发生了变化，则需要认定为是首次查找
QWidget* BatchFindReplace::autoAdjustCurrentEditWin()
{
	QWidget* pw = m_editTabWidget->currentWidget();

	if (m_curEditWin != pw)
	{
		m_curEditWin = pw;
	}
	return pw;
}

void BatchFindReplace::appendToFindTable(QString findKeyword)
{
	int rNum = ui.findReplaceTable->rowCount();
	ui.findReplaceTable->insertRow(rNum);

	QTableWidgetItem* itemFind = new QTableWidgetItem(findKeyword);
	ui.findReplaceTable->setItem(rNum, 0, itemFind);
	ui.findReplaceTable->setItem(rNum, 1, new QTableWidgetItem());
}

//这里不能追加，而是插入，即对应的Item必须已经存在。否则不插入
void BatchFindReplace::insertToReplaceTable(int row, QString replaceKeyword)
{
	QTableWidgetItem* item = ui.findReplaceTable->item(row, 1);
	if (item == nullptr)
	{
		ui.statusBar->showMessage(tr("$1 has no find match work item").arg(replaceKeyword));
		return;
	}

	item->setText(replaceKeyword);
}

void BatchFindReplace::insertToFindReplaceTable(QStringList& replaceKeyword)
{
	for (int i = 0; i < replaceKeyword.size(); ++i)
	{
		insertToReplaceTable(i, replaceKeyword.at(i));
	}
}


void BatchFindReplace::appendToFindReplaceTable(QStringList& findKeyword)
{
	if (findKeyword.isEmpty())
	{
		return;
	}

	int rNum = ui.findReplaceTable->rowCount();

	for (int i = 0; i < findKeyword.size(); ++i)
	{
		int curRow = rNum + i;
		ui.findReplaceTable->insertRow(curRow);

		QTableWidgetItem* itemFind = new QTableWidgetItem(findKeyword.at(i));
		ui.findReplaceTable->setItem(curRow, 0, itemFind);
		ui.findReplaceTable->setItem(curRow, 1, new QTableWidgetItem());
	}
}

bool BatchFindReplace::tranInputKeyword(QString& findKeyWord, QStringList& outputKeyWordList)
{
	//把空白字符，空格或者\t \r\n 等字符进行替换为空格
	QRegExp re("\\s");
	findKeyWord.replace(re, QString(" "));

	//再进行空格分隔处理
	outputKeyWordList = findKeyWord.split(" ");

	if (outputKeyWordList.size() > 20000)
	{
		ui.statusBar->showMessage(tr("Max find key word 20000 !"), 10000);
		return false;
	}

	//删除每一个空的元素
	for (int i = outputKeyWordList.size() - 1; i >= 0; --i)
	{
		if (outputKeyWordList[i].trimmed().isEmpty())
		{
			outputKeyWordList.removeAt(i);
		}
	}

	if (outputKeyWordList.isEmpty())
	{
		return false;
	}

	return true;
}

void BatchFindReplace::on_freshBtClick()
{
	QStringList findWordList;
	QString findKeyWord = ui.findKeywordEdit->toPlainText();
	if (findKeyWord.isEmpty())
	{
		ui.statusBar->showMessage(tr("Please input find keyword !"),10000);

		if (ui.findReplaceTable->rowCount() > 0)
		{
			ui.findReplaceTable->clearContents();
			ui.findReplaceTable->setRowCount(0);
		}
		return;
	}

	if (!tranInputKeyword(findKeyWord, findWordList))
	{
		return;
	}
	else
	{
		ui.findReplaceTable->clearContents();
		ui.findReplaceTable->setRowCount(0);
		appendToFindReplaceTable(findWordList);
	}

	QStringList replaceWordList;
	QString replaceKeyWord = ui.replaceKeywordEdit->toPlainText();
	if (!tranInputKeyword(replaceKeyWord, replaceWordList))
	{
		return;
	}
	else
	{
		insertToFindReplaceTable(replaceWordList);
	}
}

//进行批量查找工作
void BatchFindReplace::on_findBtClick()
{
	if (m_mainNotepad != nullptr && m_mainNotepad)
	{
		int rowNums = ui.findReplaceTable->rowCount();
		if (rowNums == 0)
		{
			CTipWin::showTips(this, tr("Please fresh first !"), 1200);
			return;
		}
		int foundTimes = 0;

		QStringList findKeyList;
	
		for (int i = 0; i < rowNums; ++i)
		{
			QTableWidgetItem* item = ui.findReplaceTable->item(i, 0);
			if (item != nullptr && !item->text().isEmpty())
			{
				findKeyList.append(item->text());
			}
		}

		foundTimes = m_mainNotepad->findAtBack(findKeyList);

		ui.statusBar->showMessage(tr("Batch Find Finished! total %1 found.").arg(foundTimes),10000);
	}
}

//进行批量替换工作
void BatchFindReplace::on_replaceBtClick()
{
	if (m_mainNotepad != nullptr)
	{
		int rowNums = ui.findReplaceTable->rowCount();
		if (rowNums == 0)
		{
			CTipWin::showTips(this, tr("Please fresh first !"), 1200);
			return;
		}


		QStringList findKeyList;
		QStringList replaceKeyList;

		for (int i = 0; i < rowNums; ++i)
		{
			QTableWidgetItem* item = ui.findReplaceTable->item(i, 0);
			if (item != nullptr && !item->text().isEmpty())
			{
				QTableWidgetItem* replaceItem = ui.findReplaceTable->item(i, 1);
				if (replaceItem != nullptr)
				{
					if (item->text() != replaceItem->text())
					{
						findKeyList.append(item->text());
						replaceKeyList.append(replaceItem->text());
					}
				}
			}
		}
		m_mainNotepad->replaceAtBack(findKeyList, replaceKeyList);

		ui.statusBar->showMessage(tr("Batch Replace Finished, total Replace %1 times !").arg(findKeyList.size()), 10000);
	}
}

void BatchFindReplace::on_swapFindReplace()
{
	QString findText = ui.findKeywordEdit->toPlainText();
	QString replaceText = ui.replaceKeywordEdit->toPlainText();

	ui.findKeywordEdit->setPlainText(replaceText);
	ui.replaceKeywordEdit->setPlainText(findText);

	on_freshBtClick();
}

void BatchFindReplace::on_export()
{
	QString filter("Text files (*.txt);;All types(*.*)");
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As ..."), QString(), filter);

	if (!fileName.isEmpty())
	{
		QSettings setting(fileName, QSettings::IniFormat);
		setting.setIniCodec("UTF-8");

		int rowNums = ui.findReplaceTable->rowCount();

		QStringList findList;
		QStringList replaceList;

		for (int i = 0; i < rowNums; ++i)
		{
			QTableWidgetItem* item = ui.findReplaceTable->item(i, 0);
			if (item != nullptr && !item->text().isEmpty())
			{
				QTableWidgetItem* replaceItem = ui.findReplaceTable->item(i, 1);
				if (replaceItem != nullptr)
				{
					findList.append(item->text());
					replaceList.append(replaceItem->text());
				}
			}
		}

		if (!findList.isEmpty())
		{
			setting.setValue("find", findList);
			setting.setValue("replace", replaceList);

			ui.statusBar->showMessage(tr("Export File finished !"), 10000);
		}
		else
		{
			ui.statusBar->showMessage(tr("No Content to Export !"), 10000);
		}
	}
}

void BatchFindReplace::on_import()
{
	QFileDialog fd(this, QString(), CCNotePad::s_lastOpenDirPath);
	fd.setFileMode(QFileDialog::ExistingFile);

	if (fd.exec() == QDialog::Accepted)   //如果成功的执行
	{
		QStringList fileNameList = fd.selectedFiles();      //返回文件列表的名称
		QFileInfo fi(fileNameList[0]);

		QSettings setting(fi.filePath(), QSettings::IniFormat);
		setting.setIniCodec("UTF-8");

		ui.findKeywordEdit->setPlainText(setting.value("find").toStringList().join(" "));
		ui.replaceKeywordEdit->setPlainText(setting.value("replace").toStringList().join(" "));

		on_freshBtClick();
	}
	else
	{
		fd.close();
	}
}

void  BatchFindReplace::on_mark()
{
	if (m_mainNotepad != nullptr)
	{
		int rowNums = ui.findReplaceTable->rowCount();
		if (rowNums == 0)
		{
			CTipWin::showTips(this, tr("Please fresh first !"), 1200);
			return;
		}
		int markTimes = 0;

		QStringList findKeyList;

		for (int i = 0; i < rowNums; ++i)
		{
			QTableWidgetItem* item = ui.findReplaceTable->item(i, 0);
			if (item != nullptr && !item->text().isEmpty())
			{
				findKeyList.append(item->text());
			}
		}

		markTimes = m_mainNotepad->markAtBack(findKeyList);

		ui.statusBar->showMessage(tr("Batch Mark Finished, total Mark %1 times !").arg(markTimes), 10000);
	}
}

void BatchFindReplace::on_clearMark()
{
	if (m_mainNotepad != nullptr)
	{
		m_mainNotepad->slot_clearMark();
	}
}
