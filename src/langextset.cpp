#include "langextset.h"
#include "scintillaeditview.h"
#include "extlexermanager.h"
#include "ccnotepad.h"

#include <qtablewidget.h>
#include <QHeaderView>
#include <QSettings>
#include <QMessageBox>

int ITEM_CHANGED = Qt::UserRole; 
int ITEM_LEX_ID = Qt::UserRole + 1; //对应的语法lexer的 ID
int ITEM_LEX_EXT_OLD_VALUE = Qt::UserRole + 2; //对应的语法关联EXT文件的旧值

LangExtSet::LangExtSet(QWidget *parent)
	: QMainWindow(parent), m_isChanged(false)
{
	ui.setupUi(this);

	ui.langTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	initLangName();

	connect(ui.langTableWidget, &QTableWidget::itemChanged, this, &LangExtSet::slot_itemChanged);
	connect(ui.langTableWidget, &QTableWidget::currentItemChanged, this, &LangExtSet::slot_currentItemChanged);
}

LangExtSet::~LangExtSet()
{}

void LangExtSet::initLangName()
{
	int langId = 0;

	QMap<QString, QStringList> extLangMap;

	ExtLexerManager::getInstance()->getExtlistByLangTag(extLangMap);

	for (int i = 0; i <= L_TXT; ++i)
	{
		if (i == L_GLOBAL)
		{
			continue;
		}

		QsciLexer* pLexer = ScintillaEditView::createLexer(i);
		if (nullptr != pLexer)
		{
			QString langName = pLexer->lexerTag();
			QTableWidgetItem* item = new QTableWidgetItem(langName);
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			item->setData(ITEM_LEX_ID, QVariant(i));

			ui.langTableWidget->insertRow(langId);
			ui.langTableWidget->setItem(langId, 0, item);

			QStringList extList;
			if (extLangMap.contains(langName))
			{
				extList = extLangMap.value(langName);
			}

			QTableWidgetItem* item1 = new QTableWidgetItem(extList.join(','));
			item1->setData(ITEM_CHANGED, QVariant(false)); 

			//把旧值记起来，后面修改后对比实时修改内存中的改动
			item1->setData(ITEM_LEX_EXT_OLD_VALUE, QVariant(extList));
			
			ui.langTableWidget->setItem(langId, 1, item1);
			delete pLexer;

			++langId;
		}
	}
	ui.langTableWidget->sortItems(0, Qt::AscendingOrder);
}

void LangExtSet::slot_itemChanged(QTableWidgetItem* item)
{
	item->setData(ITEM_CHANGED, QVariant(true));

	slot_currentItemChanged(item, nullptr);

	if (!m_isChanged)
	{
		m_isChanged = true;
	}
}

void LangExtSet::slot_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	if (current != nullptr)
	{
		int row = current->row();
		QTableWidgetItem* tagItem = ui.langTableWidget->item(row, 0);
		QTableWidgetItem* extItem = ui.langTableWidget->item(row, 1);

		if (tagItem != nullptr && extItem != nullptr)
		{
			ui.plainTextEdit->setPlainText(tr("current lang: %1 \next file suffix is : %2\nDouble-click a column item to modify the syntax association file.").arg(tagItem->text()).arg(extItem->text()));
		}
	}
}

void LangExtSet::slot_save()
{
	if (!m_isChanged)
	{
		ui.statusBar->showMessage(tr("Not change, no need save !"), 10000);
		return;
	}

	int rowNums = ui.langTableWidget->rowCount();

	QString userLangFile = QString("notepad/tag_ext");
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, userLangFile);
	qs.setIniCodec("UTF-8");

	for (int i = 0; i < rowNums; ++i)
	{
		QTableWidgetItem* item = ui.langTableWidget->item(i, 1);
		if (item != nullptr && item->data(ITEM_CHANGED).toBool())
		{
			QTableWidgetItem* langItem = ui.langTableWidget->item(i, 0);
			if (langItem != nullptr)
			{
				QString langTag = langItem->text();
				int lexId = langItem->data(ITEM_LEX_ID).toInt();

				QStringList extList = item->text().split(',');

				for (int i = extList.size() - 1; i >= 0; --i)
				{
					if (extList.at(i).isEmpty())
					{
						extList.removeAt(i);
					}
				}
				
				qs.setValue(langTag, extList);

				QString langLexerTag = QString("%1_lexId").arg(langTag);
				qs.setValue(langLexerTag, lexId);

				//上面是更新文件，下面是更新内存，让实时生效
				QStringList oldExtList = item->data(ITEM_LEX_EXT_OLD_VALUE).toStringList();

				updataExtLexerManager(langTag, lexId, oldExtList, extList);

				//更新新值。因为可能不关闭时再次修改
				item->setData(ITEM_LEX_EXT_OLD_VALUE, QVariant(extList));
			}
		}
	}
	qs.sync();
	m_isChanged = false;
	ui.statusBar->showMessage(tr("Save Finished !"), 10000);
}

//更新ExtLexerManager中tag关联的值
void LangExtSet::updataExtLexerManager(QString tag, int lexId, QStringList & oldExtList, QStringList & newExtList)
{
	//把旧的直接删除
	for (int i = 0; i < oldExtList.size(); ++i)
	{
		ExtLexerManager::getInstance()->remove(oldExtList.at(i));
	}
	//把新的重新加入一遍
	for (int i = 0; i < newExtList.size(); ++i)
	{
		FileExtLexer v;
		v.ext = newExtList.at(i);
		v.id = (LangType)lexId;

		ExtLexerManager::getInstance()->addNewExtType(v.ext, v.id, tag);
	}
}

//加载文件的关联后缀到语法中
void LangExtSet::loadExtRelevanceToMagr()
{
	QString userLangFile = QString("notepad/tag_ext");//自定义语言中不能有.字符，否则可能有错，后续要检查
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, userLangFile);
	qs.setIniCodec("UTF-8");

	if (!QFile::exists(qs.fileName()))
	{
		return;
	}

	QStringList keylist = qs.allKeys();
	QString langTag;
	QStringList extList;
	QString key_id;
	int lexid = 0;

	for (int i = 0; i < keylist.size(); ++i)
	{
		langTag = keylist.at(i);
		if (langTag.endsWith("_lexId"))
		{
			continue;
		}
		key_id = QString("%1_lexId").arg(langTag);
		extList = qs.value(langTag).toStringList();
		lexid = qs.value(key_id).toInt();

		for (int j = 0; j < extList.size(); ++j)
		{
			FileExtLexer v;
			v.ext = extList.at(j);
			v.id = (LangType)lexid;

			ExtLexerManager::getInstance()->addNewExtType(v.ext, v.id, langTag);

		}
	}
}

void LangExtSet::closeEvent(QCloseEvent* e)
{
	if (m_isChanged)
	{
		if (QMessageBox::Yes == QMessageBox::question(this, tr("Save Change"), tr("Configuration has been modified. Do you want to save it?")))
		{
			slot_save();
		}
	}
}