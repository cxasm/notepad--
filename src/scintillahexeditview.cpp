#include "scintillahexeditview.h"
#include "styleset.h"

#include "ccnotepad.h"
#include <stdexcept>
#include <QMimeData>
#include <QScrollBar>

// initialize the static variable
bool ScintillaHexEditView::_SciInit = false;
#define DEFAULT_FONT_NAME "Courier New"

ScintillaHexEditView::ScintillaHexEditView(QWidget *parent):QsciScintilla(parent), m_NoteWin(nullptr)
{
	init();
}

ScintillaHexEditView::~ScintillaHexEditView()
{
}

void ScintillaHexEditView::setNoteWidget(QWidget * win)
{
	CCNotePad* pv = dynamic_cast<CCNotePad*>(win);
	if (pv != nullptr)
	{
		m_NoteWin = pv;
}
}

sptr_t ScintillaHexEditView::execute(quint32 Msg, uptr_t wParam, sptr_t lParam) const {
	try {
		return (m_pScintillaFunc) ? m_pScintillaFunc(m_pScintillaPtr, Msg, wParam, lParam) : -1;
	}
	catch (...)
	{
		return -1;
	}
};

void ScintillaHexEditView::init()
{
	if (!_SciInit)
	{
		_SciInit = true;
	}

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


	//设置字体
#if defined (Q_OS_MAC)
    QFont font(DEFAULT_FONT_NAME, 12, QFont::Normal);
#else
	QFont font(DEFAULT_FONT_NAME, 11, QFont::Normal);
#endif
	setFont(font);
	setMarginsFont(font);
	execute(SCI_SETTABWIDTH, 4);

	updateThemes();

	connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &ScintillaHexEditView::slot_scrollYValueChange);

}

//Y方向滚动条值变化后的槽函数
void ScintillaHexEditView::slot_scrollYValueChange(int value)
{
	if (value >= this->verticalScrollBar()->maximum())
	{
		if (m_NoteWin != nullptr)
		{
			m_NoteWin->showChangePageTips(this);
		}
	}
	else if (value == this->verticalScrollBar()->minimum())
	{
		if (m_NoteWin != nullptr)
		{
			m_NoteWin->showChangePageTips(this);
		}
	}
}


void ScintillaHexEditView::dragEnterEvent(QDragEnterEvent* event)
{
	//if (event->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
	//{
	//	event->accept(); //可以在这个窗口部件上拖放对象
	//}
	//else
	//{
	//	event->ignore();
	//}
	event->accept();
	}

void ScintillaHexEditView::dropEvent(QDropEvent* e)
{
	QList<QUrl> urls = e->mimeData()->urls();
	if (urls.isEmpty())
		return;

	CCNotePad* pv = dynamic_cast<CCNotePad*>(m_NoteWin);
	if (pv != nullptr)
		pv->receiveEditDrop(e);

	//qDebug() << ui.leftSrc->geometry() << ui.rightSrc->geometry() << QCursor::pos() << this->mapFromGlobal(QCursor::pos());
}


void ScintillaHexEditView::updateThemes()
{
	//如果是黑色主题，则单独做一些风格设置
	setColor(StyleSet::s_global_style->default_style.fgColor);
	setMarginsBackgroundColor(StyleSet::s_global_style->line_number_margin.bgColor);
	setPaper(StyleSet::s_global_style->default_style.bgColor);
}
