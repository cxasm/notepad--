#include "findresultview.h"
#include "styleset.h"
#include "findresultwin.h"
#include "ctipwin.h"
#include "Scintilla.h"

#include <QClipboard>


const int MARGE_FOLDER = 1;
const int STYLE_COLOUR_TITLE = 1;
const int STYLE_COLOUR_DEST_FILE = 2;
const int STYLE_COLOUR_KEYWORD_HIGH = 3;
const int STYLE_COLOUR_KEYWORD_BACK_HIGH = 4;
const int STYLE_DEEP_COLOUR_KEYWORD_HIGH = 5; //深色模式下面的前景色
const int STYLE_DEEP_COLOUR_DEST_FILE = 6;

static void getFoldColor(QColor& fgColor, QColor& bgColor, QColor& activeFgColor)
{
	//这里看起来反了，但是实际代码就是如此
	fgColor = StyleSet::s_global_style->fold.bgColor;
	bgColor = StyleSet::s_global_style->fold.fgColor;

	activeFgColor = StyleSet::s_global_style->fold_active.fgColor;

}

void FindResultView::setFoldColor(int margin, QColor fgClack, QColor bkColor, QColor foreActive)
{
	SendScintilla(SCI_MARKERSETFORE, margin, fgClack);
	SendScintilla(SCI_MARKERSETBACK, margin, bkColor);
	SendScintilla(SCI_MARKERSETBACKSELECTED, margin, foreActive);
}

FindResultView::FindResultView(QWidget *parent)
	: QsciScintilla(parent)
{
	//通过fold发现，尽量使用qscint的功能，因为他做了大量封装和简化
	setFolding(BoxedTreeFoldStyle, MARGE_FOLDER);

	SendScintilla(SCI_SETMARGINWIDTHN, MARGE_FOLDER, 14);

	SendScintilla(SCI_MARKERSETFORE, SC_MARKNUM_FOLDERSUB, QColor(0xff,0,0));

	setCaretLineVisible(true);
	SendScintilla(SCI_SETSCROLLWIDTHTRACKING, true);
	SendScintilla(SCI_SETCARETLINEBACK, StyleSet::s_global_style->current_line_background_color.bgColor);

	QColor foldfgColor = Qt::white, foldbgColor = Qt::gray, activeFoldFgColor = Qt::red;
	getFoldColor(foldfgColor, foldbgColor, activeFoldFgColor);
	setFoldMarginColors(StyleSet::s_global_style->fold_margin.fgColor, StyleSet::s_global_style->fold_margin.bgColor);
	setFoldColor(SC_MARKNUM_FOLDEROPEN, foldfgColor, foldbgColor, activeFoldFgColor);
	setFoldColor(SC_MARKNUM_FOLDER, foldfgColor, foldbgColor, activeFoldFgColor);
	setFoldColor(SC_MARKNUM_FOLDERSUB, foldfgColor, foldbgColor, activeFoldFgColor);
	setFoldColor(SC_MARKNUM_FOLDERTAIL, foldfgColor, foldbgColor, activeFoldFgColor);
	setFoldColor(SC_MARKNUM_FOLDEREND, foldfgColor, foldbgColor, activeFoldFgColor);
	setFoldColor(SC_MARKNUM_FOLDEROPENMID, foldfgColor, foldbgColor, activeFoldFgColor);
	setFoldColor(SC_MARKNUM_FOLDERMIDTAIL, foldfgColor, foldbgColor, activeFoldFgColor);

	SendScintilla(SCI_STYLESETSIZE, STYLE_DEFAULT, 12);

	if (!StyleSet::isCurrentDeepStyle())
	{
		this->setPaper(StyleSet::s_global_style->default_style.bgColor);

		SendScintilla(SCI_STYLESETBACK, STYLE_COLOUR_TITLE, 0xffbbbb);
		SendScintilla(SCI_STYLESETBACK, STYLE_COLOUR_DEST_FILE, 0xd5ffd5);
		SendScintilla(SCI_STYLESETEOLFILLED, STYLE_COLOUR_TITLE, true);
		SendScintilla(SCI_STYLESETEOLFILLED, STYLE_COLOUR_DEST_FILE, true);
		SendScintilla(SCI_STYLESETFORE, STYLE_COLOUR_KEYWORD_HIGH, 0x4080ff);
		SendScintilla(SCI_STYLESETBACK, STYLE_COLOUR_KEYWORD_BACK_HIGH, 0xbfffff);
	
	}
	else
	{
		this->setColor(StyleSet::s_global_style->default_style.fgColor);
		this->setPaper(StyleSet::s_global_style->default_style.bgColor);

		//QColor& col = StyleSet::s_global_style->default_style.fgColor;
		//sptr_t fgLParam = (col.blue() << 16) | (col.green() << 8) | col.red();

		QColor& col2 = StyleSet::s_global_style->default_style.bgColor;
		sptr_t bklParam = (col2.blue() << 16) | (col2.green() << 8) | col2.red();

		SendScintilla(SCI_STYLESETBACK, STYLE_COLOUR_TITLE, 0xffbbbb); 
		SendScintilla(SCI_STYLESETEOLFILLED, STYLE_COLOUR_TITLE, true);
		//SendScintilla(SCI_STYLESETFORE, STYLE_COLOUR_TITLE, fgLParam);

		SendScintilla(SCI_STYLESETBACK, STYLE_DEEP_COLOUR_DEST_FILE, bklParam);
		SendScintilla(SCI_STYLESETFORE, STYLE_DEEP_COLOUR_DEST_FILE, 0x99cc99);
		//SendScintilla(SCI_STYLESETFORE, STYLE_DEEP_COLOUR_DEST_FILE, fgLParam);
		//SendScintilla(SCI_STYLESETBOLD, STYLE_DEEP_COLOUR_DEST_FILE, 1);//这个无法生效。可能是qscint bug

		//文字要大一号，然后颜色是绿色
		SendScintilla(SCI_STYLESETSIZE, STYLE_DEEP_COLOUR_DEST_FILE, 14);

		SendScintilla(SCI_STYLESETFORE, STYLE_DEEP_COLOUR_KEYWORD_HIGH, 0x00aaff);
		SendScintilla(SCI_STYLESETBACK, STYLE_DEEP_COLOUR_KEYWORD_HIGH, bklParam);


		//给行号使用
		SendScintilla(SCI_STYLESETFORE, STYLE_COLOUR_KEYWORD_HIGH, 0x4080ff);
		SendScintilla(SCI_STYLESETBACK, STYLE_COLOUR_KEYWORD_HIGH, bklParam);
	}


	this->setReadOnly(true);

	m_resultWin = dynamic_cast<FindResultWin*>(parent);
}

FindResultView::~FindResultView()
{}

//设置行背景色
void FindResultView::setLineBackColorStyle(int line, int style)
{
	int startPos = SendScintilla(SCI_POSITIONFROMLINE, line);
	int len = SendScintilla(SCI_LINELENGTH, line);

	SendScintilla(SCI_STARTSTYLING, startPos);
	SendScintilla(SCI_SETSTYLING, len, style);
}

//设置行前景色。line 行号，从相对行pos的位置开始
void FindResultView::setLineColorStyle(int line, int offsetPos, int length, int style)
{
	int startPos = SendScintilla(SCI_POSITIONFROMLINE, line);
	SendScintilla(SCI_STARTSTYLING, startPos + offsetPos);
	SendScintilla(SCI_SETSTYLING, length, style);
}


void FindResultView::mouseDoubleClickEvent(QMouseEvent* /*e*/)
{
	//QsciScintilla::mouseDoubleClickEvent(e);

	int line = 0;
	int index = 0;

	this->getCursorPosition(&line,&index);

	emit lineDoubleClick(line);
}

void FindResultView::contextUserDefineMenuEvent(QMenu* menu)
{
	if (menu != nullptr)
	{
		//移动一下位置

		QAction* pCopy =  menu->findChild<QAction*>("copy");
		if (pCopy != nullptr)
		{
			menu->removeAction(pCopy);
		}

		menu->addAction(tr("Fold All"), this, &FindResultView::on_foldAll);
		menu->addAction(tr("Expand All"), this, &FindResultView::on_expandAll);

		menu->addSeparator();
	
		if (pCopy != nullptr)
		{
			menu->addAction(pCopy);
		}
		menu->addAction(tr("copy select line"), this, &FindResultView::on_copySelectLine);
		menu->addAction(tr("warp"), this, &FindResultView::on_warp);

		menu->addSeparator();

		menu->addAction(tr("clear"), this, &FindResultView::on_clear);
		menu->addAction(tr("close"), this, &FindResultView::on_close);


	}
	menu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
	menu->show();
}

void FindResultView::on_foldAll()
{
	for (int i = 0; i < m_resultWin->m_resultLineInfo.size(); ++i)
	{
		const ResultLineInfo& lineInfo = m_resultWin->m_resultLineInfo.at(i);

		//如果是1级别的行，则进行一个收起操作
		if ((lineInfo.level == 1) || (lineInfo.level == 0))
		{
			SendScintilla(SCI_FOLDLINE, i, (long)SC_FOLDACTION_CONTRACT);
		}
	}
}

void FindResultView::on_expandAll()
{
	for (int i = 0; i < m_resultWin->m_resultLineInfo.size(); ++i)
	{
		const ResultLineInfo& lineInfo = m_resultWin->m_resultLineInfo.at(i);

		//如果是1级别的行，则进行一个收起操作
		if ((lineInfo.level == 1) || (lineInfo.level == 0))
		{
			SendScintilla(SCI_FOLDLINE, i, SC_FOLDACTION_EXPAND);
		}
	}
}

//复制选中的内容
//void FindResultView::on_copySelect()
//{
//	QString word = selectedText();
//	if (!word.isEmpty())
//	{
//		QClipboard* clipboard = QApplication::clipboard();
//		clipboard->setText(word);
//
//		CTipWin::showTips(this, tr("Copy to clipboard Finished!"), 1200,true);
//	}
//}

//复制选中行
void FindResultView::on_copySelectLine()
{
	int startPos = SendScintilla(SCI_GETSELECTIONSTART);
	int endPos = SendScintilla(SCI_GETSELECTIONEND);

	int startLine = SendScintilla(SCI_LINEFROMPOSITION, startPos);
	int endLine = SendScintilla(SCI_LINEFROMPOSITION, endPos);

	int start = SendScintilla(SCI_POSITIONFROMLINE, startLine);
	int end = SendScintilla(SCI_POSITIONFROMLINE, endLine+1);

	char *dest = new char[end - start + 1];
	dest[end - start] = '\0';

	Sci_TextRange tr;
	tr.chrg.cpMin = static_cast<Sci_PositionCR>(start);
	tr.chrg.cpMax = static_cast<Sci_PositionCR>(end);
	tr.lpstrText = dest;

	this->SendScintilla(SCI_GETTEXTRANGE, 0, &tr);

	QString text(dest);

	QStringList lineText = text.split("\n");
	QString selectConnect;

	for (int i = 0; i < lineText.size(); ++i)
	{
		const QString& t = lineText.at(i);
		int pos = t.indexOf(": ");
		if (pos != -1)
		{
			QString v = t.mid(pos + 2);
			selectConnect.append(v);
			selectConnect.append("\n");
		}
	}

	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(selectConnect);

	//CTipWin::showTips(this, tr("Copy to clipboard Finished!"), 1200, true);
}

void  FindResultView::on_clear()
{
	this->clear();
	m_resultWin->clear();
}

void  FindResultView::on_close()
{
	m_resultWin->m_parent->close();
}

void  FindResultView::on_warp()
{
	if (SC_WRAP_CHAR != wrapMode())
	{
		this->setWrapMode(QsciScintilla::WrapCharacter);
	}
	else
	{
		this->setWrapMode(QsciScintilla::WrapNone);
	}
}