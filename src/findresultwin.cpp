#include "findresultwin.h"
#include "findwin.h"
#include "common.h"
#include "styleset.h"
#include "nddsetting.h"
#include "findresultview.h"

#include <qsciscintilla.h>
#include <Scintilla.h>

//#include <QTreeWidgetItem>
//#include <QStyleFactory>
//#include <QToolButton>
//#include <qtreeview.h>
//#include <QStandardItem> 
//#include <QStandardItemModel>
#include <QClipboard>
#include <QTextEdit>
#include <qscrollbar.h>
//目前可以高亮，使用富文本进行了高亮设置。但是有个问题：富文本与html有一些冲突，在<>存在时，可能导致乱。这是一个问题。20220609
//使用Html的转义解决了该问题

FindResultWin::FindResultWin(QWidget *parent)
	: QWidget(parent), m_menu(nullptr), m_parent(parent),m_defaultFontSize(14), m_defFontSizeChange(false)
{
	ui.setupUi(this);
	connect(ui.displayView, &FindResultView::lineDoubleClick, this, &FindResultWin::on_lineDoubleClick);
	
}

FindResultWin::~FindResultWin()
{
	if (m_defFontSizeChange)
	{
		NddSetting::updataKeyValueFromNumSets(FIND_RESULT_FONT_SIZE, m_defaultFontSize);
	}

	clear();
}

void FindResultWin::clear()
{
	for (int i = 0; i < m_resultLineFilePath.size(); ++i)
	{
		delete m_resultLineFilePath.at(i);
	}
	m_resultLineFilePath.clear();
	m_resultLineInfo.clear();
}

void FindResultWin::slot_clearAllContents()
{
	clear();
	ui.displayView->clear();
}

#if 0 //老的机制，暂时屏蔽，后续可删除
//高亮查找的关键字文本。Index表示是第几次出现，前面的要跳过
void FindResultWin::highlightFindText(int index, QString &srcText, QString &findText, Qt::CaseSensitivity cs)
{
#if 0
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
	srcText.replace(pos, lens, QString("<font style='font-size:14px;background-color:#ffffbf'>%1</font>").arg(srcText.mid(pos,lens)));
#endif
}
#endif

const int MAX_HEAD_LENTGH = 20;
const int MAX_TAIL_LENGTH = 80;

#if 0
//更复杂的高亮：在全词语匹配，大小写敏感，甚至正则表达式情况下，上面的highlightFindText是不够的。需要精确定位
QString FindResultWin::highlightFindText(FindRecord& record)
{
#if 0
	QByteArray utf8bytes = record.lineContents.toUtf8();

	int lineLens = utf8bytes.length();

	bool isNeedCut = false;

	//行太长的进行缩短显示
	if (lineLens > 300)
	{
		isNeedCut = true;
	}

	//高亮的开始、结束位置
	int targetStart = record.pos - record.lineStartPos;
	int targetLens = record.end - record.pos;
	int tailStart = record.end - record.lineStartPos;

	QString head; 
	QString src;
	QString tail;
	if (!StyleSet::isCurrentDeepStyle())
	{
		if (!isNeedCut)
		{
			head = QString("<font style='font-size:14px;'>%1</font>").arg(QString(utf8bytes.mid(0, targetStart)).toHtmlEscaped());
			src = QString("<font style='font-size:14px;background-color:#ffffbf'>%1</font>").arg(QString(utf8bytes.mid(targetStart, targetLens)).toHtmlEscaped());
			tail = QString("<font style='font-size:14px;'>%1</font>").arg(QString(utf8bytes.mid(tailStart)).toHtmlEscaped());
		}
		else
		{
			head = QString(utf8bytes.mid(0, targetStart));
			if (head.size() > MAX_HEAD_LENTGH)
			{
				head = (head.mid(0, MAX_HEAD_LENTGH) + "...").toHtmlEscaped();
			}
			else
			{
				head = head.toHtmlEscaped();
			}
			head = QString("<font style='font-size:14px;'>%1</font>").arg(head);
			src = QString("<font style='font-size:14px;background-color:#ffffbf'>%1</font>").arg(QString(utf8bytes.mid(targetStart, targetLens)).toHtmlEscaped());
			tail = QString(utf8bytes.mid(tailStart));
			if (tail > MAX_TAIL_LENGTH)
			{
				tail = (tail.mid(0, MAX_TAIL_LENGTH) + "...").toHtmlEscaped();
			}
			else
			{
				tail = tail.toHtmlEscaped();
			}
			tail = QString("<font style='font-size:14px;'>%1</font>").arg(tail);
		}
	}
	else
	{
		if (!isNeedCut)
		{
			head = QString("<font style='font-size:14px;color:#dcdcdc'>%1</font>").arg(QString(utf8bytes.mid(0, targetStart)).toHtmlEscaped());
			src = QString("<font style='font-size:14px;font-weight:bold;color:#ffaa00'>%1</font>").arg(QString(utf8bytes.mid(targetStart, targetLens)).toHtmlEscaped());
			tail = QString("<font style='font-size:14px;color:#dcdcdc'>%1</font>").arg(QString(utf8bytes.mid(tailStart)).toHtmlEscaped());
		}
		else
		{
			QString headContens = QString(utf8bytes.mid(0, targetStart));
			if (headContens.size() > MAX_HEAD_LENTGH)
			{
				headContens = (headContens.mid(0, MAX_HEAD_LENTGH) + "...").toHtmlEscaped();
			}
			else
			{
				headContens = headContens.toHtmlEscaped();
			}

			head = QString("<font style='font-size:14px;color:#dcdcdc'>%1</font>").arg(headContens);
			src = QString("<font style='font-size:14px;font-weight:bold;color:#ffaa00'>%1</font>").arg(QString(utf8bytes.mid(targetStart, targetLens)).toHtmlEscaped());

			QString tailContens = QString(utf8bytes.mid(tailStart));
			if (tailContens > MAX_TAIL_LENGTH)
			{
				tailContens = (tailContens.mid(0, MAX_TAIL_LENGTH) + "...").toHtmlEscaped();
			}
			else
			{
				tailContens = tailContens.toHtmlEscaped();
			}
			tail = QString("<font style='font-size:14px;color:#dcdcdc'>%1</font>").arg(tailContens);
		}
	}

	return QString("%1%2%3").arg(head).arg(src).arg(tail);
#endif
	return "";
}


//在当前文件查找字段，结果是一个单一的FindRecords
void FindResultWin::appendResultsToShow(FindRecords* record)
{
#if 0
	if (record == nullptr)
	{
		return;
	}
	FindResultView* pDisplay = ui.displayView;

	QString findTitle = tr("Search \"%1\" (%2 hits)\n").arg(record->findText).arg(record->records.size());

	//pDisplay->append(findTitle);

	pDisplay->insertAt(findTitle,0,0);

	pDisplay->SendScintilla(SCI_SETFOLDLEVEL, 0, (long)(0|SC_FOLDLEVELHEADERFLAG));

	QStringList contents;

	for (int i = 0; i < record->records.size(); ++i)
	{
		FindRecord v = record->records.at(i);

		const QString & richText = v.lineContents;

		QString text;
		if (!StyleSet::isCurrentDeepStyle())
		{
			text = tr("Line %1 : %2\n").arg(v.lineNum + 1).arg(richText);
		}
		else
		{
			text = tr("Line %1 : %2\n").arg(v.lineNum + 1).arg(richText);
		}

		contents.append(text);
	}

	pDisplay->insertAt(contents.join(""), 1, 0);

	for (int i = 0; i < record->records.size(); ++i)
	{
		pDisplay->SendScintilla(SCI_SETFOLDLEVEL, i+1, 1);
	}
#endif

#if 0
	

	QString findTitle;

	findTitle = tr("<font style='font-size:14px;font-weight:bold;color:#343497'>Search \"%1\" (%2 hits)</font>").arg(record->findText.toHtmlEscaped()).arg(record->records.size());

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

	QString desc;
	if (!StyleSet::isCurrentDeepStyle())
	{
		desc = tr("<font style='font-size:14px;font-weight:bold;color:#309730'>%1 (%2 hits)</font>").arg(record->findFilePath.toHtmlEscaped()).arg(record->records.size());
	}
	else
	{
		desc = tr("<font style='font-size:14px;color:#99cc99'>%1 (%2 hits)</font>").arg(record->findFilePath.toHtmlEscaped()).arg(record->records.size());
	}

	QStandardItem* descItem = new QStandardItem(desc);

	if (!StyleSet::isCurrentDeepStyle())
	{
	setItemBackground(descItem, QColor(0xd5ffd5));
	}
	else
	{
		setItemBackground(descItem, QColor(0x484848));
	}

	titleItem->appendRow(descItem);
	

	descItem->setData(QVariant((qlonglong)record->pEdit), ResultItemEditor);
	descItem->setData(QVariant(record->findFilePath), ResultItemEditorFilePath);
	descItem->setData(QVariant(record->findText), ResultWhatFind);
	//描述行双击不响应
	descItem->setData(QVariant(true), ResultItemDesc);


	for (int i =0 ; i < record->records.size(); ++i)
	{
		FindRecord v = record->records.at(i);

		QString richText = highlightFindText(v);

		QString text;
		if (!StyleSet::isCurrentDeepStyle())
		{
			text = tr("<font style='font-size:14px;'>Line </font><font style='font-size:14px;color:#ff8040'>%1</font> : %2").arg(v.lineNum + 1).arg(richText);
		}
		else
		{
			text = tr("<font style='font-size:14px;color:#ffffff'>Line </font><font style='font-size:14px;color:#ff8040'>%1</font> : %2").arg(v.lineNum + 1).arg(richText);
		}
		QStandardItem* childItem = new QStandardItem(text);
		childItem->setData(QVariant(v.pos), ResultItemPos);
		childItem->setData(QVariant(v.end - v.pos), ResultItemLen);
#if defined(Q_OS_MAC)
        childItem->setTextAlignment(Qt::AlignVCenter);
#endif
		descItem->appendRow(childItem);
	}
	if (!record->records.isEmpty())
	{
		ui.resultTreeView->expand(m_model->index(0, 0, m_model->index(0, 0)));
	}
#endif
}
#endif

void FindResultWin::appendResultsToShow(QVector<FindRecords*>* record, int hits, QString whatFind)
{
	if (record == nullptr)
	{
		return;
	}

	if (this->isHidden())
	{
		this->setVisible(true);
	}

	ResultLineInfo lineInfo;

	QString findTitle = tr("Search \"%1\" (%2 hits in %3 files)\n").arg(whatFind).arg(hits).arg(record->size());

	FindResultView* pDisplay = ui.displayView;
	pDisplay->on_foldAll();

	pDisplay->insertAt(findTitle, 0, 0);

	lineInfo.level = 0;
	m_resultLineInfo.insert(0,lineInfo);

	pDisplay->SendScintilla(SCI_SETFOLDLEVEL, 0, (long)(0 | SC_FOLDLEVELHEADERFLAG));

	if (record->size() == 0)
	{
		return;
	}

	QStringList contents;

	QList<int> keyworkOffsetPos;
	QString text;
	QString linePrefix;


	int insertIndex = 1;
	for (int i = 0, count = record->size(); i < count; ++i)
	{
		FindRecords* pr = record->at(i);

		QString* pFilePath = new QString(pr->findFilePath);
		QString desc;
		if (!StyleSet::isCurrentDeepStyle())
		{
			desc = tr(" %1 (%2 hits)\n").arg(pr->findFilePath).arg(pr->records.size());
		}
		else
		{
			desc = tr(" %1 (%2 hits)\n").arg(pr->findFilePath).arg(pr->records.size());
		}
		contents.append(desc);
		m_resultLineFilePath.append(pFilePath);

		lineInfo.level = 1;
		m_resultLineInfo.insert(insertIndex, lineInfo);
		++insertIndex;

		for (int j = 0; j < pr->records.size(); ++j)
		{
			FindRecord  v = pr->records.at(j);
			QString richText = v.lineContents;

			linePrefix = tr("    Line %1: ").arg(v.lineNum + 1);

			if (!StyleSet::isCurrentDeepStyle())
			{
				text = tr("%1%2\n").arg(linePrefix).arg(richText);
			}
			else
			{
				text = tr("%1%2\n").arg(linePrefix).arg(richText);
			}
			contents.append(text);
			keyworkOffsetPos.append(linePrefix.toUtf8().size());
			lineInfo.level = 2;
			lineInfo.resultPos = v.pos;
			lineInfo.resultEnd = v.end;
			lineInfo.pFilePath = pFilePath;
			m_resultLineInfo.insert(insertIndex, lineInfo);
			++insertIndex;

			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
	}

	pDisplay->insertAt(contents.join(""), 1, 0);
	
	int lineNum = 1;

	QList<int> destFileLineNum;

	for (int i = 0, count = record->size(); i < count; ++i)
	{
		FindRecords* pr = record->at(i);

		pDisplay->SendScintilla(SCI_SETFOLDLEVEL, lineNum, (long)(1 | SC_FOLDLEVELHEADERFLAG));
		destFileLineNum.append(lineNum);
		++lineNum;

		for (int j = 0; j < pr->records.size(); ++j)
		{
			pDisplay->SendScintilla(SCI_SETFOLDLEVEL, lineNum, (long)2| SC_FOLDLEVELBASE);

			++lineNum;
		}
	}

	//着色
	pDisplay->setLineBackColorStyle(0, STYLE_COLOUR_TITLE);

	for (int i = 0, count = destFileLineNum.size(); i < count; ++i)
	{
		pDisplay->setLineBackColorStyle(destFileLineNum.at(i), (StyleSet::isCurrentDeepStyle()? STYLE_DEEP_COLOUR_DEST_FILE:STYLE_COLOUR_DEST_FILE));
	}
	
	//关键字高亮
	//高亮的开始、结束位置
	int targetStart = 0;
	int targetLens = 0;
	lineNum = 1;

	int lineOffsetPosIndex = 0;
	int lineOffsetPos = 0;

	QString lineNumStr = tr("    Line ");
	int skipLineNumOffset = lineNumStr.toUtf8().size();

	for (int i = 0, count = record->size(); i < count; ++i)
	{
		FindRecords* pr = record->at(i);
		++lineNum;
		for (int j = 0; j < pr->records.size(); ++j)
		{
			lineOffsetPos = keyworkOffsetPos.at(lineOffsetPosIndex);
			const FindRecord &v = pr->records.at(j);
			targetStart = v.pos - v.lineStartPos + lineOffsetPos;
			targetLens = v.end - v.pos;
			pDisplay->setLineColorStyle(lineNum, skipLineNumOffset, lineOffsetPos-skipLineNumOffset-2, STYLE_COLOUR_KEYWORD_HIGH);
			pDisplay->setLineColorStyle(lineNum, targetStart, targetLens, (StyleSet::isCurrentDeepStyle()? STYLE_DEEP_COLOUR_KEYWORD_HIGH:STYLE_COLOUR_KEYWORD_BACK_HIGH));
			++lineNum;
			++lineOffsetPosIndex;
		}
	}
	pDisplay->SendScintilla(SCI_GOTOLINE, 0);
}


int FindResultWin::getDefaultFontSize()
{
	return m_defaultFontSize;
}

void FindResultWin::setDefaultFontSize(int defSize)
{
	m_defaultFontSize = defSize;
}

void FindResultWin::on_lineDoubleClick(int lineNum)
{
	if (lineNum < m_resultLineInfo.size())
	{
		const ResultLineInfo& lineInfo = m_resultLineInfo.at(lineNum);

		if (lineInfo.level == 2)
		{
			//文件定位到行
			emit lineDoubleClicked(lineInfo.pFilePath, lineInfo.resultPos, lineInfo.resultEnd);
		}
		else if ((lineInfo.level == 0) || (lineInfo.level == 1))
		{
			ui.displayView->SendScintilla(SCI_FOLDLINE, lineNum, SC_FOLDACTION_TOGGLE);
		}
	}
}