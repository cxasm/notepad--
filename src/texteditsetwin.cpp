#include "texteditsetwin.h"
#include "scintillaeditview.h"
#include "ccnotepad.h"
#include "qtlangset.h"
#include <QFontDialog>

TextEditSetWin::TextEditSetWin(QWidget *parent)
	: QWidget(parent), m_notepadWin(nullptr)
{
	ui.setupUi(this);

	if (ScintillaEditView::s_tabLens >= 0 && ScintillaEditView::s_tabLens <= 16)
	{
		ui.spinBoxTabLens->setValue(ScintillaEditView::s_tabLens);
	}

	ui.checkBoxReplaceTabToSpace->setChecked(ScintillaEditView::s_noUseTab);

	ui.BigTextSizeLimit->setValue(ScintillaEditView::s_bigTextSize);

	ui.restoreFile->setChecked((CCNotePad::s_restoreLastFile == 1));
}

TextEditSetWin::~TextEditSetWin()
{
	save();
}

void TextEditSetWin::setNotePadWin(QWidget *w)
{
	m_notepadWin = w;
}

#if 0
//弹出对话框时，默认初始化该值
void TextEditSetWin::setFont(QFont &font)
{
	if (m_curFont != font)
	{
		m_curFont = font;
	}
	ui.curTextFontEdit->setText(font.toString());
}
#endif
#if 0
//弹出对话框时，默认初始化该值
void TextEditSetWin::setProgramLangFont(QFont &font)
{
	if (m_curProLangFont != font)
	{
		m_curProLangFont = font;
	}
	ui.curProLangFontEdit->setText(font.toString());
}
#endif

void TextEditSetWin::save()
{
	bool lenChange = false;

	if (ui.spinBoxTabLens->value() != ScintillaEditView::s_tabLens)
	{
		ScintillaEditView::s_tabLens = ui.spinBoxTabLens->value();
		lenChange = true;
	}

	bool useChange = false;
	if (ui.checkBoxReplaceTabToSpace->isChecked() != ScintillaEditView::s_noUseTab)
	{
		ScintillaEditView::s_noUseTab = ui.checkBoxReplaceTabToSpace->isChecked();
		useChange = true;
	}

	if (lenChange || useChange)
	{
		emit sendTabFormatChange(lenChange, useChange);
	}

	if (ui.BigTextSizeLimit->value() != ScintillaEditView::s_bigTextSize)
	{
		ScintillaEditView::s_bigTextSize = ui.BigTextSizeLimit->value();
}

	int restoreFile = ui.restoreFile->isChecked() ? 1 : 0;

	if (restoreFile != CCNotePad::s_restoreLastFile)
	{
		CCNotePad::s_restoreLastFile = restoreFile;
}
}

void TextEditSetWin::slot_txtFontSet()
{
	CCNotePad* pMainWin = dynamic_cast<CCNotePad*>(m_notepadWin);
	if (pMainWin == nullptr)
	{
		//是从对比规则里面弹出来的，不进行文本的设置
		return;
	}

	QtLangSet* pWin = new QtLangSet(QString("txt"), this);
	pWin->setAttribute(Qt::WA_DeleteOnClose);

	connect(pWin, &QtLangSet::viewStyleChange, pMainWin, &CCNotePad::slot_viewStyleChange);
	connect(pWin, &QtLangSet::viewLexerChange, pMainWin, &CCNotePad::slot_viewLexerChange);
	pWin->show();
	pWin->selectInitLangTag("txt");
}
#if 0
void TextEditSetWin::slot_selectFont()
{
	QFont ft;
#if defined(Q_OS_WIN)
	ft.fromString(u8"宋体,14,-1,5,50,0,0,0,0,0,常规");
#elif defined(Q_OS_MAC)
    ft.fromString(u8"STSong,14,-1,5,50,0,0,0,0,0,Regular");
#else
    ft.fromString(u8"CESI宋体-GB2312,12,-1,5,50,0,0,0,0,0,Regular");
#endif

	bool ok = false;//定义bool型输出变量
	ft = QFontDialog::getFont(&ok, m_curFont, this,tr("User define Txt Font"));

	if (ok)
	{
		ui.curTextFontEdit->setText(ft.toString());

		if (m_curFont != ft)
		{
			m_curFont = ft;
			emit signTxtFontChange(ft);
		}
	}
	else
	{
		ui.curTextFontEdit->setText(ft.toString());
		m_curFont = ft;
	}

}
#endif
#if 0
void TextEditSetWin::slot_selectProLangFont()
{

#if defined(Q_OS_WIN)
	 QFont ft("Courier New", QsciLexer::s_defaultFontSize);
#elif defined(Q_OS_MAC)
	QFont ft("Menlo", s_defaultFontSize);
#else
	QFont ft("Bitstream Vera Sans", 9);
#endif

	bool ok = false;//定义bool型输出变量
	ft = QFontDialog::getFont(&ok, m_curProLangFont, this, tr("User define Txt Font"));

	if (ok)
	{
		ui.curProLangFontEdit->setText(ft.toString());

		if (m_curProLangFont != ft)
		{
			m_curProLangFont = ft;
			emit signProLangFontChange(ft);
		}
	}
	else
	{
		ui.curProLangFontEdit->setText(ft.toString());
		m_curProLangFont = ft;
	}

}
#endif
