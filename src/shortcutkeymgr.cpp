#include "shortcutkeymgr.h"
#include "shortcutkeyeditwin.h"
#include "ccnotepad.h"

#include <QTableWidgetItem>
#include <QSettings>
#include <Qsci/qscicommandset.h>
#include <Qsci/qsciscintilla.h>
#include <QDebug>

QMap<QString, int>* ShortcutKeyMgr::s_shortcutKeysMap = nullptr;

struct ShortcutKeySt {
	QString iniTag;//保存在Ini文件中的名称
	QString name;//显示在表格中的名称
	QKeySequence key;

	bool canModify;//能否修改

	ShortcutKeySt() = default;
	ShortcutKeySt(QString name_, QString iniTag_, bool canMofidy=true) :name(name_), iniTag(iniTag_), canModify(canMofidy)
	{

	}
	ShortcutKeySt(QString name_, QString iniTag_, QString keySeq, bool canMofidy = true) :name(name_), iniTag(iniTag_), canModify(canMofidy)
	{
		key = QKeySequence(keySeq);
	}

};

QVector<ShortcutKeySt> shortCutTable;


ShortcutKeyMgr::ShortcutKeyMgr(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	initShortcutKeysMap();
	initNddShortcutTable();
	m_pNoteEdit = parent;
	initQscintShortcutTable();

	connect(ui.tableWidget, &QTableWidget::itemDoubleClicked, this, &ShortcutKeyMgr::slot_edit);

	ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	ui.qscintTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui.qscintTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui.qscintTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);


	QString tabQss = "QHeaderView::section{"
		"border-top:0px solid #E5E5E5;"
		"border-left:0px solid #E5E5E5;"
		"border-right:0.5px solid #E5E5E5;"
		"border-bottom: 0.5px solid #E5E5E5;"
		"background-color:white;"
		"padding:4px;"
		"}";

	ui.tableWidget->horizontalHeader()->setStyleSheet(tabQss);
	ui.tableWidget->verticalHeader()->setStyleSheet(tabQss);

	ui.qscintTableWidget->horizontalHeader()->setStyleSheet(tabQss);
	ui.qscintTableWidget->verticalHeader()->setStyleSheet(tabQss);

}

ShortcutKeyMgr::~ShortcutKeyMgr()
{}

void ShortcutKeyMgr::initShortcutKeysMap()
{
	if (s_shortcutKeysMap == nullptr)
	{
		s_shortcutKeysMap = new QMap<QString, int>();

		shortCutTable << ShortcutKeySt(tr("New File"), New_File, QString("Ctrl+T")) \
			<< ShortcutKeySt(tr("Open File"), Open_File, QString("Ctrl+O")) \
			<< ShortcutKeySt(tr("Save File"), Save_File, QString("Ctrl+S"), false) \
			<< ShortcutKeySt(tr("Save All File"), Save_All_File) \
			<< ShortcutKeySt(tr("Close"), Close, QString("Ctrl+W")) \
			<< ShortcutKeySt(tr("Close All"), Close_All, QString("Ctrl+Shift+W")) \
			<< ShortcutKeySt(tr("Cut"), Cut, QString("Ctrl+X"), false) \
			<< ShortcutKeySt(tr("Copy"), Copy, QString("Ctrl+C"), false) \
			<< ShortcutKeySt(tr("Paste"), Paste, QString("Ctrl+V"), false) \
			<< ShortcutKeySt(tr("Undo"), Undo, QString("Ctrl+Z"), false) \
			<< ShortcutKeySt(tr("Redo"), Redo, QString("Ctrl+Y"), false) \
			<< ShortcutKeySt(tr("Find"), Find, QString("Ctrl+F"), false) \
			<< ShortcutKeySt(tr("Replace"), Replace, QString("Ctrl+H")) \
			<< ShortcutKeySt(tr("Dir Find"), DirFind, QString("Ctrl+Shift+D")) \
			<< ShortcutKeySt(tr("Mark"), Mark) \
			<< ShortcutKeySt(tr("word highlight(F8)"), Word_highlight, QString("F8"))\
			<< ShortcutKeySt(tr("clear all highlight(F7)"), Clear_all_highlight, QString("F7")) \
			<< ShortcutKeySt(tr("Zoom In"), Zoom_In) \
			<< ShortcutKeySt(tr("Zoom Out"), Zoom_Out) \
			<< ShortcutKeySt(tr("Word Wrap"), Word_Wrap) \
			<< ShortcutKeySt(tr("Show Blank"), Show_Blank) \
			<< ShortcutKeySt(tr("Indent Guide"), Indent_Guide) \
			<< ShortcutKeySt(tr("Pre Hex Page"), Pre_Page) \
			<< ShortcutKeySt(tr("Next Hex Page"), Next_Page)\
			<< ShortcutKeySt(tr("Goto Hex Page"), Goto_Page, QString("Ctrl+G")) \
			<< ShortcutKeySt(tr("File Compare"), File_Compare) \
			<< ShortcutKeySt(tr("Dir Compare"), Dir_Compare) \
			<< ShortcutKeySt(tr("Bin Compare"), Bin_Compare) \
			<< ShortcutKeySt(tr("transform encoding"), Trans_code) \
			<< ShortcutKeySt(tr("batch rename file"), Batch_rename) \
			<< ShortcutKeySt(tr("Format Xml"), Format_Xml) \
			<< ShortcutKeySt(tr("Format Json"), Format_Json) \
			<< ShortcutKeySt(tr("Add/Del Line Comment"), ADD_DELETE_LINE_COMMENT, QString("Ctrl+Q")) \
			<< ShortcutKeySt(tr("Add Block Comment"), ADD_BLOCK_COMMENT, QString("Ctrl+K")) \
			<< ShortcutKeySt(tr("Del Block Comment"), CANCEL_BLOCK_COMMENT, QString("Ctrl+Shift+K")) \
			<< ShortcutKeySt(tr("Fold Level 1"), FOLD_1, QString("Alt+1")) \
			<< ShortcutKeySt(tr("Fold Level 2"), FOLD_2, QString("Alt+2")) \
			<< ShortcutKeySt(tr("Fold Level 3"), FOLD_3, QString("Alt+3")) \
			<< ShortcutKeySt(tr("Fold Level 4"), FOLD_4, QString("Alt+4")) \
			<< ShortcutKeySt(tr("Fold Level 5"), FOLD_5, QString("Alt+5")) \
			<< ShortcutKeySt(tr("Fold Level 6"), FOLD_6, QString("Alt+6")) \
			<< ShortcutKeySt(tr("Fold Level 7"), FOLD_7, QString("Alt+7")) \
			<< ShortcutKeySt(tr("Fold Level 8"), FOLD_8, QString("Alt+8")) \
			<< ShortcutKeySt(tr("UNFold Level 1"), UNFOLD_1, QString("Alt+Shift+1")) \
			<< ShortcutKeySt(tr("UNFold Level 2"), UNFOLD_2, QString("Alt+Shift+2")) \
			<< ShortcutKeySt(tr("UNFold Level 3"), UNFOLD_3, QString("Alt+Shift+3")) \
			<< ShortcutKeySt(tr("UNFold Level 4"), UNFOLD_4, QString("Alt+Shift+4")) \
			<< ShortcutKeySt(tr("UNFold Level 5"), UNFOLD_5, QString("Alt+Shift+5")) \
			<< ShortcutKeySt(tr("UNFold Level 6"), UNFOLD_6, QString("Alt+Shift+6")) \
			<< ShortcutKeySt(tr("UNFold Level 7"), UNFOLD_7, QString("Alt+Shift+7")) \
			<< ShortcutKeySt(tr("UNFold Level 8"), UNFOLD_8, QString("Alt+Shift+8"));

		for (int i = 0; i < shortCutTable.size(); ++i)
		{
			s_shortcutKeysMap->insert(shortCutTable.at(i).iniTag, i);
		}

		//从文件中读取用户修改过的配置表。
		loadUserDefSet();
	}
}

//务必先调用initShortcutKeysMap()
QKeySequence ShortcutKeyMgr::getUserDefShortcutKey(QString iniTag)
{
	if (s_shortcutKeysMap->contains(iniTag))
	{
		return shortCutTable.at(s_shortcutKeysMap->value(iniTag)).key;
	}
	return QKeySequence();
}

//从用户ini配置中加载快捷键配置
void ShortcutKeyMgr::loadUserDefSet()
{
	QString userDefFile = QString("notepad/shortcuttab");
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, userDefFile);
	qs.setIniCodec("UTF-8");

	QStringList keys = qs.allKeys();

	QString keySeqStr;

	for (int i = 0; i < keys.size(); ++i)
	{
		const QString & initTag = keys.at(i);

		keySeqStr = qs.value(initTag).toString();

		if (s_shortcutKeysMap->contains(initTag))
		{
			int index = s_shortcutKeysMap->value(initTag);
			shortCutTable[index].key = QKeySequence(keySeqStr);
		}
	}
}

//修改配置文件中的快捷键定义。inittag ini的key, keySeqStr ini的value。内存也修改了
bool ShortcutKeyMgr::ModifyShortCutKey(QString initTag, QString keySeqStr)
{
	QString userDefFile = QString("notepad/shortcuttab");
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, userDefFile);
	qs.setIniCodec("UTF-8");

	if (s_shortcutKeysMap->contains(initTag))
	{
		int index = s_shortcutKeysMap->value(initTag);
		if (!shortCutTable[index].canModify)
		{
			return false;
		}

		shortCutTable[index].key = QKeySequence(keySeqStr);
		qs.setValue(initTag, keySeqStr);
		qs.sync();
		return true;
	}
	return false;
}

void ShortcutKeyMgr::initNddShortcutTable()
{
	for (int i = 0; i < shortCutTable.size(); ++i)
	{
		ui.tableWidget->insertRow(i);

		QTableWidgetItem* item = new QTableWidgetItem(shortCutTable.at(i).name);
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		ui.tableWidget->setItem(i, 0, item);

		qDebug() << shortCutTable.at(i).key.toString();
		QTableWidgetItem* item1 = new QTableWidgetItem(shortCutTable.at(i).key.toString());
		ui.tableWidget->setItem(i, 1, item1);

		if (!shortCutTable.at(i).canModify)
		{
			QTableWidgetItem* item2 = new QTableWidgetItem(tr("Can't Modify"));
			ui.tableWidget->setItem(i, 2, item2);
		}
		else
		{
			QTableWidgetItem* item2 = new QTableWidgetItem(tr("Double Click To Modify"));
			ui.tableWidget->setItem(i, 2, item2);
		}
		
	}
}

//初始化qscint内部的快捷键。目前这部分不能修改。
void ShortcutKeyMgr::initQscintShortcutTable()
{
	QsciScintilla* pNote = new QsciScintilla(nullptr);
	QsciCommandSet* cmdSet = pNote->standardCommands();

	QList<QsciCommand*>& cmdList = cmdSet->commands();

	int rowNum = 0;
	for (int i = 0; i < cmdList.size(); ++i)
	{
		if (cmdList.at(i)->key() == 0)
		{
			continue;
		}
		ui.qscintTableWidget->insertRow(rowNum);

		QTableWidgetItem* item = new QTableWidgetItem(cmdList.at(i)->description());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		ui.qscintTableWidget->setItem(rowNum, 0, item);

		QTableWidgetItem* item1 = new QTableWidgetItem(QKeySequence(cmdList.at(i)->key()).toString());
		ui.qscintTableWidget->setItem(rowNum, 1, item1);

		QTableWidgetItem* item2 = new QTableWidgetItem(tr("Can't Modify"));
		ui.qscintTableWidget->setItem(rowNum, 2, item2);

		++rowNum;
		
	}
	delete pNote;
}

//type 0 ndd 1 qscint
int ShortcutKeyMgr::isKeySeqExist(int row, QString keySeq, int &type)
{
	int c = ui.tableWidget->rowCount();
	for (int i = 0; i < c; ++i)
	{
		if (i == row)
		{
			continue;
		}

		if (ui.tableWidget->item(i, 1)->text() == keySeq)
		{
			type = 0;
			return i;
		}
	}

	c = ui.qscintTableWidget->rowCount();

	for (int i = 0; i < c; ++i)
	{
		if (ui.qscintTableWidget->item(i, 1)->text() == keySeq)
		{
			type = 1;
			return i;
		}
	}
	return -1;
}
//双击修改槽函数
void ShortcutKeyMgr::slot_edit(QTableWidgetItem* item)
{
	int row = item->row();
	
	if (!shortCutTable.at(row).canModify)
	{
		ui.plainTextEdit->setPlainText(tr("row %1 shortcut key '%2' can't modify !").arg(row + 1).arg(shortCutTable.at(row).key.toString()));
		return;
	}

	ShortcutKeyEditWin* pWin = new ShortcutKeyEditWin(this);
	pWin->setTitle(shortCutTable.at(row).name);
	pWin->setCurKeyDesc(shortCutTable.at(row).key.toString());

	int ret = pWin->exec();
	if (1 == ret) //确定
	{
		QKeySequence newKeySeq = pWin->getNewKeySeq();

		QTableWidgetItem* item = ui.tableWidget->item(row, 1);
		if (item != nullptr)
		{
			//检查是否冲突，如果冲突，则不设置。先预留
			int conflictType = 0;
			int existId = isKeySeqExist(row, newKeySeq.toString(), conflictType);
			if (-1 == existId)
			{
				if (ModifyShortCutKey(shortCutTable.at(row).iniTag, newKeySeq.toString()))
				{
					CCNotePad* pNotePad = dynamic_cast<CCNotePad*>(m_pNoteEdit);
					if (pNotePad != nullptr)
					{
						pNotePad->setUserDefShortcutKey(row);
					}
					item->setText(newKeySeq.toString());
					ui.plainTextEdit->setPlainText(tr("modify row %1 to '%2' shortcut key success!").arg(row + 1).arg(newKeySeq.toString()));
				}
				else
				{
					ui.plainTextEdit->setPlainText(tr("error:modify row %1 to '%2' shortcut key failed !").arg(row + 1).arg(newKeySeq.toString()));
				}
			}
			else
			{
				if (conflictType == 0)
				{
					ui.plainTextEdit->setPlainText(tr("conflict error! '%1' Already exist at row %2").arg(newKeySeq.toString()).arg(existId + 1));
				}
				else
				{
					ui.plainTextEdit->setPlainText(tr("conflict error! '%1' Already exist at qscint row %2").arg(newKeySeq.toString()).arg(existId + 1));
				}
			}
		}
	}
	else if (2 == ret)//删除
	{
		QKeySequence newKeySeq(QKeySequence::UnknownKey);

		QTableWidgetItem* item = ui.tableWidget->item(row, 1);
		if (item != nullptr)
		{
			if (ModifyShortCutKey(shortCutTable.at(row).iniTag, newKeySeq.toString()))
			{
				CCNotePad* pNotePad = dynamic_cast<CCNotePad*>(m_pNoteEdit);
				if (pNotePad != nullptr)
				{
					pNotePad->setUserDefShortcutKey(row);
				}
				item->setText(newKeySeq.toString());
				ui.plainTextEdit->setPlainText(tr("modify row %1 to '%2' shortcut key success!").arg(row + 1).arg("NULL"));
			}
	else
	{
				ui.plainTextEdit->setPlainText(tr("error:modify row %1 to '%2' shortcut key failed !").arg(row + 1).arg("NULL"));
			}
		}
	}
	else
	{
		ui.plainTextEdit->setPlainText(tr("modify canceled !"));
	}
}
