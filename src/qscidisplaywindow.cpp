#include "qscidisplaywindow.h"
#include "MediatorDisplay.h"
#include "textfind.h"
#include "common.h"
#include "styleset.h"
#include "rcglobal.h"

#include <QScrollBar>
#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>
#include <stdexcept>
#include <SciLexer.h>

QsciDisplayWindow::QsciDisplayWindow(QWidget *parent):QsciScintilla(parent), m_textFindWin(nullptr), m_preFirstLineNum(0), m_isShowFindItem(true), m_hasHighlight(false)
{
	//20210815 左右行同步还有问题，暂时不屏蔽，不实现
	connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &QsciDisplayWindow::slot_scrollYValueChange);
	connect(this->horizontalScrollBar(), &QScrollBar::valueChanged, this, &QsciDisplayWindow::slot_scrollXValueChange);

	setAcceptDrops(false);

	m_findStartPos = 0;
	m_findEndPos = 0;
	m_findCurPos = 0;

	m_pScintillaFunc = (SCINTILLA_FUNC)this->SendScintillaPtrResult(SCI_GETDIRECTFUNCTION);
	m_pScintillaPtr = (SCINTILLA_PTR)this->SendScintillaPtrResult(SCI_GETDIRECTPOINTER);


	if (!m_pScintillaFunc)
	{
		throw std::runtime_error("ScintillaEditView::init : SCI_GETDIRECTFUNCTION message failed");
}

	if (!m_pScintillaPtr)
	{
		throw std::runtime_error("ScintillaEditView::init : SCI_GETDIRECTPOINTER message failed");
	}

	//这个无比要设置false，否则双击后高亮单词，拷贝时会拷贝多个选择。
	execute(SCI_SETMULTIPLESELECTION, true);
	execute(SCI_SETMULTIPASTE, 1);
	execute(SCI_SETADDITIONALCARETSVISIBLE, false);
	execute(SCI_SETSELFORE, true, 0x0);
	execute(SCI_SETSELBACK, true, 0x00ffff);

	//QColor foldfgColor(StyleSet::foldfgColor);
	//QColor foldbgColor(StyleSet::foldbgColor);//默认0xff,0xff,0xff

	////通过fold发现，尽量使用qscint的功能，因为他做了大量封装和简化
	//setFolding(BoxedTreeFoldStyle, 2);
	//setFoldMarginColors(foldfgColor, foldbgColor);
	//setMarginsBackgroundColor(StyleSet::marginsBackgroundColor); //0xea, 0xf7, 0xff //默认0xf0f0f0

		//双击后同样的字段进行高亮
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE_SMART, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE_SMART, 100);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_SMART, false);
	execute(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_SMART, 0x00ff00);

	setStyleOptions();

	//开启后可以保证长行在滚动条下完整显示
	execute(SCI_SETSCROLLWIDTHTRACKING, true);
	connect(this, &QsciScintilla::selectionChanged, this, &QsciDisplayWindow::slot_clearHightWord, Qt::QueuedConnection);
	connect(this, &QsciDisplayWindow::delayWork, this, &QsciDisplayWindow::slot_delayWork, Qt::QueuedConnection);
}

QsciDisplayWindow::~QsciDisplayWindow()
{
	if (m_textFindWin != nullptr)
	{
		delete m_textFindWin;
		m_textFindWin = nullptr;
	}
}

void QsciDisplayWindow::setFoldColor(int margin, QColor fgClack, QColor bkColor)
{
	SendScintilla(SCI_MARKERSETFORE, margin, fgClack);
	SendScintilla(SCI_MARKERSETBACK, margin, bkColor);
}

void QsciDisplayWindow::setStyleOptions()
{
	if (StyleSet::m_curStyleId != BLACK_SE)
	{
		setMarginsForegroundColor(QColor(0x80, 0x80, 0x80)); //默认0x80, 0x80, 0x80
	}
	else
	setMarginsBackgroundColor(0xf0f0f0);
	setFoldMarginColors(0xf0f0f0, 0xf0f0f0);
	{
		//setCaretLineBackgroundColor(QColor(0xe8e8ff));
		setCaretLineBackgroundColor(QColor(0xFAF9DE));
		setMatchedBraceForegroundColor(QColor(191, 141, 255));
		setMatchedBraceBackgroundColor(QColor(222, 222, 222));
		setCaretForegroundColor(QColor(0, 0, 0));
		setFoldColor(SC_MARKNUM_FOLDEROPEN, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDER, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDERSUB, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDERTAIL, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDEREND, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDEROPENMID, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDERMIDTAIL, QColor(Qt::white), QColor(128, 128, 128));
	}
}

void QsciDisplayWindow::setIsShowFindItem(bool v)
{
	m_isShowFindItem = v;
}


sptr_t QsciDisplayWindow::execute(quint32 Msg, uptr_t wParam, sptr_t lParam) const {
	try {
		return (m_pScintillaFunc) ? m_pScintillaFunc(m_pScintillaPtr, Msg, wParam, lParam) : -1;
	}
	catch (...)
	{
		return -1;
	}
};

void QsciDisplayWindow::mouseDoubleClickEvent(QMouseEvent * e)
{
	QsciScintilla::mouseDoubleClickEvent(e);

	if (hasSelectedText())
	{
		emit delayWork();
	}
}

void QsciDisplayWindow::clearIndicator(int indicatorNumber) {
	size_t docStart = 0;
	size_t docEnd = length();
	execute(SCI_SETINDICATORCURRENT, indicatorNumber);
	execute(SCI_INDICATORCLEARRANGE, docStart, docEnd - docStart);
};

const int MAXLINEHIGHLIGHT = 400;

void QsciDisplayWindow::slot_clearHightWord()
{
	if (m_hasHighlight)
	{
		m_hasHighlight = false;
		clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);
	}
}


void QsciDisplayWindow::highlightViewWithWord(QString & word2Hilite)
{
	int originalStartPos = execute(SCI_GETTARGETSTART);
	int originalEndPos = execute(SCI_GETTARGETEND);

	int firstLine = static_cast<int>(this->execute(SCI_GETFIRSTVISIBLELINE));
	int nbLineOnScreen = this->execute(SCI_LINESONSCREEN);
	int nbLines = std::min(nbLineOnScreen, MAXLINEHIGHLIGHT) + 1;
	int lastLine = firstLine + nbLines;
	int startPos = 0;
	int endPos = 0;
	auto currentLine = firstLine;
	int prevDocLineChecked = -1;	//invalid start


	auto searchMark = [this](int &startPos, int &endPos, QByteArray &word2Mark) {

		int targetStart = 0;
		int targetEnd = 0;

		long lens = word2Mark.length();

		while (targetStart >= 0)
		{
			execute(SCI_SETTARGETRANGE, startPos, endPos);

			targetStart = SendScintilla(SCI_SEARCHINTARGET, lens, word2Mark.data());

			if (targetStart == -1 || targetStart == -2)
				break;

			targetEnd = int(this->execute(SCI_GETTARGETEND));

			if (targetEnd > endPos)
			{
				//we found a result but outside our range, therefore do not process it
				break;
			}

			int foundTextLen = targetEnd - targetStart;

			if (foundTextLen > 0)
			{
				this->execute(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE_SMART);
				this->execute(SCI_INDICATORFILLRANGE, targetStart, foundTextLen);
			}

			if (targetStart + foundTextLen == endPos)
				break;

			startPos = targetStart + foundTextLen;

		}
	};


	QByteArray whatMark = word2Hilite.toUtf8();

	SendScintilla(SCI_SETSEARCHFLAGS, SCFIND_REGEXP | SCFIND_MATCHCASE | SCFIND_WHOLEWORD | SCFIND_REGEXP_SKIPCRLFASONE);

	for (; currentLine < lastLine; ++currentLine)
	{
		int docLine = static_cast<int>(this->execute(SCI_DOCLINEFROMVISIBLE, currentLine));
		if (docLine == prevDocLineChecked)
			continue;	//still on same line (wordwrap)
		prevDocLineChecked = docLine;
		startPos = static_cast<int>(this->execute(SCI_POSITIONFROMLINE, docLine));
		endPos = static_cast<int>(this->execute(SCI_POSITIONFROMLINE, docLine + 1));

		if (endPos == -1)
		{	//past EOF
			endPos = this->length() - 1;
			searchMark(startPos, endPos, whatMark);
			break;
		}
		else
		{
			searchMark(startPos, endPos, whatMark);
		}
	}

	m_hasHighlight = true;

	// restore the original targets to avoid conflicts with the search/replace functions
	this->execute(SCI_SETTARGETRANGE, originalStartPos, originalEndPos);
}



void QsciDisplayWindow::slot_delayWork()
{

	if (!hasSelectedText())
	{
		return;
	}

	QString word = selectedText();
	if (!word.isEmpty())
	{
		highlightViewWithWord(word);
	}

#if 0
	if (!hasSelectedText())
	{
		return;
	}

	QString word = selectedText();
	if (!word.isEmpty())
	{

		QVector<int>resultPos;
		resultPos.reserve(50);


		int firstLine = execute(SCI_GETFIRSTVISIBLELINE);
		int nbLineOnScreen = execute(SCI_LINESONSCREEN);
		int nbLines = std::min(nbLineOnScreen, MAXLINEHIGHLIGHT) + 1;
		int lastLine = firstLine + nbLines;


		long startPos = execute(SCI_POSITIONFROMLINE, firstLine);
		long endPos = execute(SCI_POSITIONFROMLINE, lastLine);

		if (endPos == -1)
		{
			endPos = execute(SCI_GETLENGTH);
		}

		int curpos = execute(SCI_GETCURRENTPOS);
		int mainSelect = 1;

		struct Sci_TextToFind findOptions;
		findOptions.chrg.cpMin = startPos;
		findOptions.chrg.cpMax = endPos;

		std::string wordStr = word.toStdString();
		findOptions.lpstrText = wordStr.c_str();

		int pos = execute(SCI_FINDTEXT, SCFIND_MATCHCASE | SCFIND_WHOLEWORD, reinterpret_cast<sptr_t>(&findOptions));

		while (pos != -1)
		{
			resultPos.append(pos);

			if (pos <= curpos)
			{
				mainSelect = resultPos.size();
			}
			findOptions.chrg.cpMin = findOptions.chrgText.cpMax;
			pos = execute(SCI_FINDTEXT, SCFIND_MATCHCASE | SCFIND_WHOLEWORD, reinterpret_cast<sptr_t>(&findOptions));
		}

		for (int i = 0, size = resultPos.size(); i < size; ++i)
		{
			execute(SCI_ADDSELECTION, resultPos.at(i), resultPos.at(i) + word.size());

		}

		if (!resultPos.isEmpty())
		{
			execute(SCI_SETMAINSELECTION, mainSelect - 1);
		}
	}
#endif
}

void QsciDisplayWindow::setMediator(MediatorDisplay* mediator)
{
	m_mediator = mediator;
}

//滚动条值变化后的槽函数。一旦滚动则会出发这里，发送消息给中介，让中介去同步另外一方
void QsciDisplayWindow::slot_scrollYValueChange(int value)
{
	if (m_direction == RC_LEFT)
	{
		if (m_mediator->getLeftScrollValue() != value)
		{
			m_mediator->setLeftScrollValue(value);
		}
	}
	else
	{
		if (m_mediator->getRightScrollValue() != value)
		{
			m_mediator->setRightScrollValue(value);
		}
	}
	autoAdjustLineWidth(value);
	slot_delayWork();
	//qDebug("-- dir s n %d %d", m_direction, contentY());
}

//根据现有滚动条来决定是否更新屏幕线宽长度。每滚动2000个单位必须调整line宽
void QsciDisplayWindow::autoAdjustLineWidth(int xScrollValue)
{

	if (std::abs(xScrollValue - m_preFirstLineNum) > 400)
	{
		m_preFirstLineNum = xScrollValue;

		updateLineNumberWidth();
	}
}

void QsciDisplayWindow::updateLineNumberWidth()
{
	auto linesVisible = execute(SCI_LINESONSCREEN);
	if (linesVisible)
	{
		int nbDigits = 0;

		auto firstVisibleLineVis = execute(SCI_GETFIRSTVISIBLELINE);
		auto lastVisibleLineVis = linesVisible + firstVisibleLineVis + 1;
		auto lastVisibleLineDoc = execute(SCI_DOCLINEFROMVISIBLE, lastVisibleLineVis);

		nbDigits = nbDigitsFromNbLines(lastVisibleLineDoc);
		nbDigits = nbDigits < 4 ? 4 : nbDigits;
		
		auto pixelWidth = 8 + nbDigits * execute(SCI_TEXTWIDTH, STYLE_LINENUMBER, reinterpret_cast<sptr_t>("8"));
		execute(SCI_SETMARGINWIDTHN, MARGIN_LINE_NUM, pixelWidth);
	}
}

//X方向滚动条值变化后的槽函数。一旦滚动则会出发这里，发送消息给中介，让中介去同步另外一方
void QsciDisplayWindow::slot_scrollXValueChange(int value)
{
	if (m_direction == RC_LEFT)
	{
		if (m_mediator->getLeftScrollXValue() != value)
		{
			m_mediator->setLeftScrollXValue(value);
		}
	}
	else
	{
		if (m_mediator->getRightScrollXValue() != value)
		{
			m_mediator->setRightScrollXValue(value);
		}
	}

	//qDebug("-- dir s n %d %d", m_direction, contentY());
}



void QsciDisplayWindow::setDirection(RC_DIRECTION direction)
{
	m_direction = direction;
}

int QsciDisplayWindow::getCurVerticalScrollValue()
{
	return this->verticalScrollBar()->value();
}

void QsciDisplayWindow::contextUserDefineMenuEvent(QMenu* menu)
{
	//QAction* action;
	if (menu != nullptr)
	{
		menu->addAction(tr("Find Text"), this, SLOT(slot_findText()));
		menu->addAction(tr("Show File in Explorer"), this, SLOT(slot_showFileInExplorer()));
		menu->addAction(tr("Save As ..."), this, &QsciDisplayWindow::sign_saveAsFile);
	}
	menu->show();
}

void QsciDisplayWindow::inputMethodEvent(QInputMethodEvent* event)
{
	if (!event->preeditString().isEmpty())
	{
		return;
	}

	QsciScintilla::inputMethodEvent(event);
}


void QsciDisplayWindow::slot_findText()
{
	if (m_isShowFindItem)
	{
	if (m_textFindWin == nullptr)
	{
		m_textFindWin = new TextFind(m_direction);
		connect(m_textFindWin, &TextFind::signFindFile, this, &QsciDisplayWindow::slot_FindTextWithPara);

		m_textFindWin->activateWindow();
		m_textFindWin->show();
	}
	else
	{
		m_textFindWin->activateWindow();
		m_textFindWin->showNormal();
	}

	m_findCurPos = 0;
}
	else
	{
		//不使用这里的查找，直接发信号到外面
		emit sign_find();
	}
}

void QsciDisplayWindow::slot_FindTextWithPara(int prevOrNext, QString text)
{
	std::string str = text.toStdString();

	int length = SendScintilla(SCI_GETLENGTH);

	if (length > 0)
	{
		if (prevOrNext == 1)
		{
			SendScintilla(SCI_SETTARGETSTART, m_findCurPos);
			SendScintilla(SCI_SETTARGETEND, length);


			int ret = SendScintilla(SCI_SEARCHINTARGET, str.length(), str.c_str());
			if (ret >= 0)
			{
				m_findCurPos = ret + str.length();
				SendScintilla(SCI_GOTOPOS, ret);
				SendScintilla(SCI_SETSELECTION, ret, ret + str.length());
			}
			else
			{
				QMessageBox::information(this, tr("Not Find"), tr("Not Find Next!"));
				m_textFindWin->activateWindow();
			}
		}
		else if (prevOrNext == 0)
		{
			SendScintilla(SCI_SETTARGETSTART, m_findCurPos);
			SendScintilla(SCI_SETTARGETEND, 0);


			int ret = SendScintilla(SCI_SEARCHINTARGET, str.length(), str.c_str());
			if (ret >= 0)
			{
				m_findCurPos = ret - 1;

				SendScintilla(SCI_GOTOPOS, ret);
				SendScintilla(SCI_SETSELECTION, ret, ret + str.length());
			}
			else
			{
				QMessageBox::information(this, tr("Not Find"), tr("Not Find Prev!"));
				m_textFindWin->activateWindow();
			}
		}
	}
}

//定位到文件夹
void QsciDisplayWindow::slot_showFileInExplorer()
{
	showFileInExplorer(m_filePath);
}

