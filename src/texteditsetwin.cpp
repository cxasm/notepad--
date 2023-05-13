#include "texteditsetwin.h"
#include "scintillaeditview.h"
#include "ccnotepad.h"
#include "qtlangset.h"
#include "nddsetting.h"
#include <QFontDialog>
#include <QColorDialog>

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

	int clearOpenfilelist = NddSetting::getKeyValueFromDelayNumSets(CLEAR_OPENFILE_ON_CLOSE);

	ui.openfileRecord->setChecked((clearOpenfilelist ==1));

	QPalette pal = QApplication::palette();

	QPixmap f(32, 32);
	f.fill(pal.text().color());
	ui.appFontColorLabel->setPixmap(f);

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

	//注意这里是禁用，和启用是相反的意思
	int clearOpenfilelist = (ui.openfileRecord->isChecked() ? 1 : 0);

	NddSetting::updataKeyValueFromDelayNumSets(CLEAR_OPENFILE_ON_CLOSE, clearOpenfilelist);
}

void TextEditSetWin::slot_txtFontSet()
{
	CCNotePad* pMainWin = dynamic_cast<CCNotePad*>(m_notepadWin);
	if (pMainWin == nullptr)
	{
		//是从对比规则里面弹出来的，不进行文本的设置
		return;
	}
	QtLangSet* pWin = pMainWin->getLangSet();

#if 0
	QtLangSet* pWin = new QtLangSet(QString("txt"), this);
	pWin->setAttribute(Qt::WA_DeleteOnClose);

	connect(pWin, &QtLangSet::viewStyleChange, pMainWin, &CCNotePad::slot_viewStyleChange);
	connect(pWin, &QtLangSet::viewLexerChange, pMainWin, &CCNotePad::slot_viewLexerChange);
	pWin->show();
#endif
	if (pWin != nullptr)
	{
	pWin->selectInitLangTag("txt");
}
}

#if 0
//app字体颜色设置
void TextEditSetWin::slot_appFontColor()
{
	QPalette pal = qApp->palette();

	QColor oldColor = pal.text().color();
	QColor color = QColorDialog::getColor(pal.text().color(), this, tr("App Font Foreground Color"));
	if (color.isValid() && color != oldColor)
	{
		pal.setColor(QPalette::WindowText, color);//设置颜色
		pal.setColor(QPalette::Text, color);//设置颜色
		pal.setColor(QPalette::ButtonText, color);//设置颜色
		pal.setColor(QPalette::ToolTipText, color);
		qApp->setPalette(pal);

		QPixmap f(32, 32);
		f.fill(pal.text().color());
		ui.appFontColorLabel->setPixmap(f);

		//发现如果修改APP字体颜色后，必须要把存在的窗口关闭一下，否则存在的窗口的字体颜色无法生效。
		CCNotePad* pMainWin = dynamic_cast<CCNotePad*>(m_notepadWin);
		if (pMainWin != nullptr)
		{
			//是从主界面调用的，执行一下颜色的更新
			pMainWin->changeAppFontColor(color);
			return;
		}
	}
}
#endif
#if 0
//不能整体修改QApplication::font()，会引发语法里面的文字重叠破坏。
//只针对菜单和状态栏，查找框字体进行修改。
void TextEditSetWin::slot_selectAppFont()
{
	QFont ft;
	QFont curAppFont = QApplication::font();

	bool ok = false;//定义bool型输出变量
	ft = QFontDialog::getFont(&ok, curAppFont, this,tr("The App Font"));

	if (ok)
	{
		ui.appFontEdit->setText(ft.toString());

		if (curAppFont != ft)
		{
			//emit signAppFontChange(ft);
			QApplication::setFont(ft,"FindResultWin");
		}
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
