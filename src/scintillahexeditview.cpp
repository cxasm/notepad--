#include "scintillahexeditview.h"
#include "styleset.h"

#include "ccnotepad.h"
#include <stdexcept>
#include <QMimeData>

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
	m_NoteWin = win;
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
	setMarginsBackgroundColor(StyleSet::marginsBackgroundColor);


	execute(SCI_SETTABWIDTH, 4);
	setPaper(QColor(0xfc, 0xfc, 0xfc));

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
