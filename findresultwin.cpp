#include "findresultwin.h"
#include "findwin.h"
#include "common.h"

#include <QTreeWidgetItem>
#include <QStyleFactory>
#include <QToolButton>
#include <qtreeview.h>
#include <QStandardItem> 
#include <QStandardItemModel>
#include <QClipboard>
#include <QTextEdit>

#include "ndstyleditemdelegate.h"

//目前可以高亮，使用富文本进行了高亮设置。但是有个问题：富文本与html有一些冲突，在<>存在时，可能导致乱。这是一个问题。20220609
//使用Html的转义解决了该问题

FindResultWin::FindResultWin(QWidget *parent)
	: QWidget(parent), m_menu(nullptr)
{
	ui.setupUi(this);

	//设置左边的缩起来按钮为加号，而不是箭头
	ui.resultTreeView->setStyle(QStyleFactory::create("windows"));
	ui.resultTreeView->header()->hide();
	
	m_model = new QStandardItemModel(ui.resultTreeView);
	m_delegate = new NdStyledItemDelegate(ui.resultTreeView);
	ui.resultTreeView->setModel(m_model);
	ui.resultTreeView->setItemDelegate(m_delegate);
	ui.resultTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.resultTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

#if defined (Q_OS_MAC)
	QString qss = "QTreeView::item:selected{ \
	   background:#e8e8ff; \
    } \
    QTreeView::item{ \
        height:18px; \
    }";
#else
    QString qss = "QTreeView::item:selected{ \
       background:#e8e8ff; \
    }";
#endif
	ui.resultTreeView->setStyleSheet(qss);

	connect(ui.resultTreeView, &QTreeView::doubleClicked, this, &FindResultWin::itemDoubleClicked);
}

FindResultWin::~FindResultWin()
{
}

void FindResultWin::contextMenuEvent(QContextMenuEvent *)
{
	if (m_menu == nullptr)
	{
		m_menu = new QMenu(this);
		m_menu->addAction(tr("clear this find result"), this, &FindResultWin::slot_clearContents);
		m_menu->addAction(tr("clear all find result"), this, &FindResultWin::slot_clearAllContents);

		m_menu->addSeparator();
		m_pSelectSectonAct = m_menu->addAction(tr("select section"), this, &FindResultWin::slot_selectSection);
		m_menu->addAction(tr("select all item"), this, &FindResultWin::slot_selectAll);

		m_menu->addSeparator();

		m_menu->addAction(tr("copy select item (Ctrl Muli)"), this, &FindResultWin::slot_copyItemContents);
		m_menu->addAction(tr("copy select Line (Ctrl Muli)"), this, &FindResultWin::slot_copyContents);


	}
	m_menu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
	m_menu->show();

	QModelIndex curSelItem = ui.resultTreeView->currentIndex();

	if (!curSelItem.data(ResultItemRoot).isNull())
	{
		//是根不能选择section
		m_pSelectSectonAct->setEnabled(false);
}
	else if (!curSelItem.data(ResultItemEditor).isNull())
	{
		m_pSelectSectonAct->setEnabled(true);
	}
	else if (!curSelItem.data(ResultItemPos).isNull())
	{
		m_pSelectSectonAct->setEnabled(true);
	}
}

void FindResultWin::slot_clearContents()
{
	QModelIndex curSelItem = ui.resultTreeView->currentIndex();

	if (!curSelItem.data(ResultItemRoot).isNull())
	{
		//是根可以直接删除
		m_model->removeRow(curSelItem.row());
	}
	else if (!curSelItem.data(ResultItemEditor).isNull())
	{
		//需要找到上一个父节点，即根
		QModelIndex rootItem = curSelItem.parent();

		if (rootItem.isValid())
		{
			m_model->removeRow(curSelItem.row(), rootItem);

			if (0 == m_model->rowCount(rootItem))
			{
				m_model->removeRow(rootItem.row());
			}
		}
	}
	else if (!curSelItem.data(ResultItemPos).isNull())
	{
		//需要找到上两个父节点，即根
		QModelIndex itemEditor = curSelItem.parent();
		if (itemEditor.isValid())
		{
			QModelIndex rootItem = itemEditor.parent();
			if (rootItem.isValid())
			{
				m_model->removeRow(itemEditor.row(), rootItem);
				if (0 == m_model->rowCount(rootItem))
				{
					m_model->removeRow(rootItem.row());
				}
			}
		}
	}
}

//全选
void FindResultWin::slot_selectAll()
{

	QModelIndex root = ui.resultTreeView->rootIndex();

	QModelIndex curSelItem = ui.resultTreeView->currentIndex();
	QModelIndex firstRootItem;
	//先找到根节点
	while (curSelItem.isValid())
	{
		QModelIndex pMi = curSelItem.parent();
		if (pMi.isValid())
		{
			curSelItem = pMi;
}
		else
		{
			break;
		}
	}

	//找到第一个兄弟根节点
	if (curSelItem.row() != 0)
	{
		firstRootItem = curSelItem.siblingAtRow(0);
	}
	else
	{
		firstRootItem = curSelItem;
	}

	auto selectSection = [this](QModelIndex& sectionItem)->int{
		//遍历其下面的所有子节点
		ui.resultTreeView->selectionModel()->select(sectionItem, QItemSelectionModel::Select);

		//遍历下面的子节点
		int i = 0;
		QModelIndex childMi;
		childMi = sectionItem.child(i, 0);
		while (childMi.isValid())
		{
			++i;
			ui.resultTreeView->selectionModel()->select(childMi, QItemSelectionModel::Select);
			childMi = sectionItem.child(i, 0);
		}
		return i+1;
	};

	QModelIndex rootItem = firstRootItem;
	int j = 0;
	int selectCount = 0;
	while (rootItem.isValid())
	{
		//遍历根节点下面每一个section
		{
			int i = 0;
			QModelIndex section = rootItem.child(i, 0);
			while (section.isValid() && !section.data(ResultItemEditor).isNull())
			{
				++i;
				selectCount += selectSection(section);
				section = firstRootItem.child(i, 0);
			}
		}

		//切换到下一个查找的根节点
		j++;
		rootItem = firstRootItem.siblingAtRow(j);
	}

	QString msg = tr("%1 rows selected !").arg(selectCount);
	emit showMsg(msg);
}

void FindResultWin::slot_selectSection()
{
	QModelIndex curSelItem = ui.resultTreeView->currentIndex();

	auto selectSection = [this](QModelIndex& sectionItem)->int {
		//遍历其下面的所有子节点
		ui.resultTreeView->selectionModel()->select(sectionItem, QItemSelectionModel::Select);

		//遍历下面的子节点
		int i = 0;
		QModelIndex childMi;
		childMi = sectionItem.child(i, 0);
		while (childMi.isValid())
		{
			++i;
			ui.resultTreeView->selectionModel()->select(childMi, QItemSelectionModel::Select);
			childMi = sectionItem.child(i, 0);
		}
		return i+1;
	};

	int selectCount = 0;

	if (!curSelItem.data(ResultItemRoot).isNull())
	{
		//啥也不做。不能选择多个section
	}
	else if (!curSelItem.data(ResultItemEditor).isNull())
	{
		selectCount = selectSection(curSelItem);
		
	}
	else if (!curSelItem.data(ResultItemPos).isNull())
	{
		//其父节点
		QModelIndex sectionItem = curSelItem.parent();
		if (!sectionItem.data(ResultItemEditor).isNull())
		{
			selectCount = selectSection(sectionItem);
		}
	}

	QString msg = tr("%1 rows selected !").arg(selectCount);
	emit showMsg(msg);
}

//拷贝item的行，不进行过滤，全部拷贝
void FindResultWin::slot_copyItemContents()
{
	QModelIndexList selectList;

	ui.resultTreeView->getSelectedIndexes(selectList);

	QString selectConnect;

	for (int i = 0, s = selectList.size(); i < s; ++i)
	{

		QModelIndex curSelItem = selectList.at(i);

		QString text = m_model->itemData(curSelItem).values()[0].toString();

		QTextEdit t(this);
		t.setAcceptRichText(true);
		t.setText(text);
		text = t.toPlainText();

		selectConnect.append(text);
		selectConnect.append("\n");
	}

	QClipboard *clip = QApplication::clipboard();
	clip->setText(selectConnect);

	QString msg = tr("%1 items have been copied to the clipboard !").arg(selectList.size());
	emit showMsg(msg);
}

void FindResultWin::slot_copyContents()
{
	QModelIndexList selectList;

	//std::sort(selectList.begin(), selectList.end(), [](QModelIndex& a, QModelIndex& b) {
	//	return a.row() < b.row();
	//});
	
	ui.resultTreeView->getSelectedIndexes(selectList);

	QString selectConnect;

	int copyTimes = 0;

	for (int i = 0, s = selectList.size(); i < s; ++i)
	{
		QModelIndex curSelItem = selectList.at(i);

		//只拷贝真正的行内容
		if (curSelItem.data(ResultItemPos).isNull())
		{
			continue;
		}

		copyTimes++;

	QString text = m_model->itemData(curSelItem).values()[0].toString();

	QTextEdit t(this);
	t.setAcceptRichText(true);
	t.setText(text);
	text = t.toPlainText();

		int pos = text.indexOf(": ");
		if (pos != -1)
		{
			text = text.mid(pos + 2);
		}

		selectConnect.append(text);
		selectConnect.append("\n");
	}

	QClipboard *clip = QApplication::clipboard();
	clip->setText(selectConnect);

	QString msg = tr("%1 lines have been copied to the clipboard !").arg(copyTimes);
	emit showMsg(msg);
}

void FindResultWin::slot_clearAllContents()
{
	m_model->clear();
}

//高亮查找的关键字文本。Index表示是第几次出现，前面的要跳过
void FindResultWin::highlightFindText(int index, QString &srcText, QString &findText, Qt::CaseSensitivity cs)
{
	int pos = 0;
	int findPos = 0;

	//先把< > 转义为因为会与原来的html标签冲突。这是一个很厉害的方法，如果不转义，会导致显示丢失
	srcText = srcText.toHtmlEscaped();
	findText = findText.toHtmlEscaped();
	int lens = findText.size();

	while (index > 0)
	{
		pos = srcText.indexOf(findText, findPos, cs);
		if (pos == -1)
		{
			//错误，不替换
			return;
		}
		else
		{
			findPos = pos + lens;
		}
		index--;
	}
	srcText.replace(pos, lens, QString("<font style='background-color:#ffffbf'>%1</font>").arg(srcText.mid(pos,lens)));
}

//更复杂的高亮：在全词语匹配，大小写敏感，甚至正则表达式情况下，上面的highlightFindText是不够的。需要精确定位
QString FindResultWin::highlightFindText(FindRecord& record)
{
	QByteArray utf8bytes = record.lineContents.toUtf8();

	//高亮的开始、结束位置
	int targetStart = record.pos - record.lineStartPos;
	int targetLens = record.end - record.pos;
	int tailStart = record.end - record.lineStartPos;

	QString head = QString(utf8bytes.mid(0, targetStart)).toHtmlEscaped();
	QString src = QString("<font style='background-color:#ffffbf'>%1</font>").arg(QString(utf8bytes.mid(targetStart, targetLens)).toHtmlEscaped());
	QString tail = QString(utf8bytes.mid(tailStart)).toHtmlEscaped();

	return QString("%1%2%3").arg(head).arg(src).arg(tail);
}

void FindResultWin::appendResultsToShow(FindRecords* record)
{
	if (record == nullptr)
	{
		return;
	}
	QString findTitle = tr("<font style='font-weight:bold;color:#343497'>Search \"%1\" (%2 hits)</font>").arg(record->findText.toHtmlEscaped()).arg(record->records.size());

	QStandardItem* titleItem = new QStandardItem(findTitle);
	setItemBackground(titleItem, QColor(0xbbbbff));
	m_model->insertRow(0, titleItem);
	titleItem->setData(QVariant(true), ResultItemRoot);

	int rowNum = m_model->rowCount();
	//把其余的行收起来。把第一行张开
	for (int i = 1; i < rowNum; ++i)
	{
		ui.resultTreeView->collapse(m_model->index(i, 0));
	}

	ui.resultTreeView->expand(m_model->index(0, 0));

	if (record->records.size() == 0)
	{
		return;
	}

	QString desc = tr("<font style='font-weight:bold;color:#309730'>%1 (%2 hits)</font>").arg(record->findFilePath.toHtmlEscaped()).arg(record->records.size());
	QStandardItem* descItem = new QStandardItem(desc);
	setItemBackground(descItem, QColor(0xd5ffd5));
	titleItem->appendRow(descItem);
	

	descItem->setData(QVariant((qlonglong)record->pEdit), ResultItemEditor);
	descItem->setData(QVariant(record->findFilePath), ResultItemEditorFilePath);
	descItem->setData(QVariant(record->findText), ResultWhatFind);
	//描述行双击不响应
	descItem->setData(QVariant(true), ResultItemDesc);

	//int lastLineNum = -1;
	//int occurTimes = 0;

	for (int i =0 ; i < record->records.size(); ++i)
	{
		FindRecord v = record->records.at(i);

		////找到是第几次出现关键字
		//if (lastLineNum != v.lineNum)
		//{
		//	lastLineNum = v.lineNum;
		//	occurTimes = 1;
		//}
		//else
		//{
		//	occurTimes++;
		//}

		//highlightFindText(occurTimes, v.lineContents, record->findText, (Qt::CaseSensitivity)record->caseSensitivity);

		QString richText = highlightFindText(v);

		QString text = tr("Line <font style='color:#ff8040'>%1</font> : %2").arg(v.lineNum + 1).arg(richText);

		QStandardItem* childItem = new QStandardItem(text);
		childItem->setData(QVariant(v.pos), ResultItemPos);
		childItem->setData(QVariant(v.end - v.pos), ResultItemLen);
		descItem->appendRow(childItem);
	}
	if (!record->records.isEmpty())
	{
		ui.resultTreeView->expand(m_model->index(0, 0, m_model->index(0, 0)));
	}

}

void FindResultWin::appendResultsToShow(QVector<FindRecords*>* record, int hits, QString whatFind)
{
	if (record == nullptr)
	{
		return;
	}

	QString findTitle = tr("<font style='font-weight:bold;color:#343497'>Search \"%1\" (%2 hits in %3 files)</font>").arg(whatFind.toHtmlEscaped()).arg(hits).arg(record->size());
	QStandardItem* titleItem = new QStandardItem(findTitle);
	setItemBackground(titleItem, QColor(0xbbbbff));
	titleItem->setData(QVariant(true), ResultItemRoot);

	//总是把结果插在最上面一行
	m_model->insertRow(0, titleItem);

	int rowNum = m_model->rowCount();

	//把其余的行收起来
	for (int i = 1; i < rowNum; ++i)
	{
		ui.resultTreeView->collapse(m_model->index(i, 0));
	}
	ui.resultTreeView->expand(m_model->index(0, 0));

	if (record->size() == 0)
	{
		return;
	}

	for (int i = 0,count= record->size(); i < count; ++i)
	{
		FindRecords* pr = record->at(i);

		QString desc = tr("<font style='font-weight:bold;color:#309730'>%1 (%2 hits)</font>").arg(pr->findFilePath.toHtmlEscaped()).arg(pr->records.size());

		QStandardItem* descItem = new QStandardItem(desc);
		setItemBackground(descItem, QColor(0xd5ffd5));
		titleItem->insertRow(0,descItem);

		//默认全部收起来
		if (count > 10)
		{
			ui.resultTreeView->collapse(m_model->index(0, 0, m_model->index(0, 0)));
		}
	
		descItem->setData(QVariant((qlonglong)pr->pEdit), ResultItemEditor);
		descItem->setData(QVariant(pr->findFilePath), ResultItemEditorFilePath);
		descItem->setData(QVariant(pr->findText), ResultWhatFind);

		//描述行双击不响应
		descItem->setData(QVariant(true), ResultItemDesc);

		for (int i = 0; i < pr->records.size(); ++i)
		{
			FindRecord  v = pr->records.at(i);
			QString richText = highlightFindText(v);

			QString text = QString("Line <font style='color:#ff8040'>%1</font> : %2").arg(v.lineNum + 1).arg(richText);

			QStandardItem* childItem = new QStandardItem(text);
			childItem->setData(QVariant(v.pos), ResultItemPos);
			childItem->setData(QVariant(v.end - v.pos), ResultItemLen);
			descItem->appendRow(childItem);

			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
		if (count <= 10 && !pr->records.isEmpty())
		{
			ui.resultTreeView->expand(m_model->index(0, 0, m_model->index(0, 0)));
		}
	}
	ui.resultTreeView->expand(m_model->index(0, 0, m_model->index(0, 0)));
}

void FindResultWin::setItemBackground(QStandardItem* item, const QColor& color)
{
	QBrush b(color);
	item->setBackground(b);
}

void FindResultWin::setItemForeground(QStandardItem* item, const QColor& color)
{
	QBrush b(color);
	item->setForeground(b);
}
