#include "qscidisplaywindow.h"
#include "MediatorDisplay.h"
#include "textfind.h"
#include "common.h"
#include "styleset.h"

#include <QScrollBar>
#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>
#include <stdexcept>

QsciDisplayWindow::QsciDisplayWindow(QWidget *parent):QsciScintilla(parent), m_textFindWin(nullptr), m_preFirstLineNum(0), m_isShowFindItem(true)
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
	execute(SCI_SETMULTIPLESELECTION, false);
	execute(SCI_SETMULTIPASTE, 1);
	execute(SCI_SETADDITIONALCARETSVISIBLE, false);
	execute(SCI_SETSELFORE, true, 0x0);
	execute(SCI_SETSELBACK, true, 0x00ffff);

	QColor foldfgColor(StyleSet::foldfgColor);
	QColor foldbgColor(StyleSet::foldbgColor);//默认0xff,0xff,0xff

	//通过fold发现，尽量使用qscint的功能，因为他做了大量封装和简化
	setFolding(BoxedTreeFoldStyle, 2);
	setFoldMarginColors(foldfgColor, foldbgColor);
	setMarginsBackgroundColor(StyleSet::marginsBackgroundColor); //0xea, 0xf7, 0xff //默认0xf0f0f0


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

const int MAXLINEHIGHLIGHT = 400;

void QsciDisplayWindow::slot_delayWork()
{
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
//
//int nbDigitsFromNbLines(size_t nbLines)
//{
//	int nbDigits = 0; // minimum number of digit should be 4
//	if (nbLines < 10) nbDigits = 1;
//	else if (nbLines < 100) nbDigits = 2;
//	else if (nbLines < 1000) nbDigits = 3;
//	else if (nbLines < 10000) nbDigits = 4;
//	else if (nbLines < 100000) nbDigits = 5;
//	else if (nbLines < 1000000) nbDigits = 6;
//	else // rare case
//	{
//		nbDigits = 7;
//		nbLines /= 1000000;
//
//		while (nbLines)
//		{
//			nbLines /= 10;
//			++nbDigits;
//		}
//	}
//	return nbDigits;
//}

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
		nbDigits = nbDigits < 3 ? 3 : nbDigits;
		
		auto pixelWidth = 8 + nbDigits * execute(SCI_TEXTWIDTH, STYLE_LINENUMBER, reinterpret_cast<sptr_t>("8"));
		execute(SCI_SETMARGINWIDTHN, 3, pixelWidth);
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

void QsciDisplayWindow::travEveryBlockToSave(std::function<void(QString&, int)> savefun, QList<BlockUserData*>* externLineInfo)
{

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
	QString path, cmd;
#ifdef _WIN32
	path = m_filePath.replace("/", "\\");
	cmd = QString("explorer.exe /select,%1").arg(path);
#else
	path = m_filePath.replace("\\", "/");
	cmd = QString("open -R %1").arg(path);
#endif
	QProcess process;
	process.startDetached(cmd);
}

