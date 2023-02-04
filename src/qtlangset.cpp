#include "qtlangset.h"
#include "scintillaeditview.h"
#include "nddsetting.h"
#include "rcglobal.h"
#include "ccnotepad.h"
#include "styleset.h"
#include "extlexermanager.h"

#include <SciLexer.h>
#include <qscilexer.h>
#include <QSettings>
#include <QColorDialog> 
#include <QMessageBox>
#include <QSpinBox>
#include <Qsci/qscilexerglobal.h>

#include <QDebug>
#include <QDir>

#if 0
//要注意与common.h的保持一致。
enum LangType {
	L_TEXT = -1, L_PHP, L_C, L_CPP, L_CS, L_OBJC, L_JAVA, L_RC, \
	L_HTML, L_XML, L_MAKEFILE, L_PASCAL, L_BATCH, L_INI, L_ASCII, L_USER, \
	L_ASP, L_SQL, L_VB, L_JS, L_CSS, L_PERL, L_PYTHON, L_LUA, \
	L_TEX, L_FORTRAN, L_BASH, L_FLASH, L_NSIS, L_TCL, L_LISP, L_SCHEME, \
	L_ASM, L_DIFF, L_PROPS, L_PS, L_RUBY, L_SMALLTALK, L_VHDL, L_KIX, L_AU3, \
	L_CAML, L_ADA, L_VERILOG, L_MATLAB, L_HASKELL, L_INNO, L_SEARCHRESULT, \
	L_CMAKE, L_YAML, L_COBOL, L_GUI4CLI, L_D, L_POWERSHELL, L_R, L_JSP, \
	L_COFFEESCRIPT, L_JSON, L_JAVASCRIPT, L_FORTRAN_77, L_BAANC, L_SREC, \
	L_IHEX, L_TEHEX, L_SWIFT, \
	L_ASN1, L_AVS, L_BLITZBASIC, L_PUREBASIC, L_FREEBASIC, \
	L_CSOUND, L_ERLANG, L_ESCRIPT, L_FORTH, L_LATEX, \
	L_MMIXAL, L_NIM, L_NNCRONTAB, L_OSCRIPT, L_REBOL, \
	L_REGISTRY, L_RUST, L_SPICE, L_TXT2TAGS, L_VISUALPROLOG, L_TYPESCRIPT, \
	L_EDIFACT, L_MARKDOWN, L_OCTAVE, L_PO, L_POV, L_IDL, L_GO, L_TXT, \
	// Don't use L_JS, use L_JAVASCRIPT instead
	// The end of enumated language type, so it should be always at the end
	L_EXTERNAL = 100, L_USER_DEFINE = 200
};
#endif

//static const QColor blackColor(Qt::black);
//static const QColor lightColor(0xdedede);
//
//static const QColor blackColor1(0x0000ff);
//static const QColor lightColor1(0xffaa00);
//
////C++注释的默认颜色，不显眼
//static const QColor blackColor2(0x007f00);
//static const QColor lightColor2(0xaaff7f);
//
//static const QColor blackColor3(0x7f7f00);
//static const QColor lightColor3(0xfca287);
//
//static const QColor blackColor4(0x8000ff);
//static const QColor lightColor4(0xffaa00);
//
//static const QColor blackColor5(0x007f7f);
//static const QColor lightColor5(0xaaff7f);
//
//static const QColor blackColor6(0x7f007f);
//static const QColor lightColor6(0x00ffff);

QtLangSet::QtLangSet(QString initTag, QWidget *parent)
	: QMainWindow(parent), m_selectLexer(nullptr), m_selectStyleId(0), m_isStyleChange(false),m_isStyleChildChange(false), m_initShowLexerTag(initTag), m_previousSysLangItem(nullptr),m_isGlobelItem(false)
{
	ui.setupUi(this);
	initLangList();
	initUserDefineLangList();

	startSignSlot();
	//initDarkColorMap();

	m_themesId = StyleSet::getCurrentSytleId();
	m_lastThemesId = -1;
	ui.mainThemesCbox->setCurrentIndex(m_themesId);
}

QtLangSet::~QtLangSet()
{
	if (nullptr != m_selectLexer)
	{
		delete m_selectLexer;
		m_selectLexer = nullptr;
	}
}


//在同步字体时，务必先关闭关联槽函数，避免循环触发。务必配对使用
void QtLangSet::enableFontChangeSensitive(bool isSensitive)
{
	if (isSensitive)
	{
		connect(ui.boldCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontBoldChange);
		connect(ui.italicCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontItalicChange);
		connect(ui.underlineCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontUnderlineChange);
		connect(ui.fontSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slot_fontSizeChange(int)));
		connect(ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &QtLangSet::slot_fontChange);
	}
	else
	{
		disconnect(ui.boldCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontBoldChange);
		disconnect(ui.italicCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontItalicChange);
		disconnect(ui.underlineCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontUnderlineChange);
		disconnect(ui.fontSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slot_fontSizeChange(int)));
		disconnect(ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &QtLangSet::slot_fontChange);
	}
}

void QtLangSet::startSignSlot()
{
	//开始本来使用的是QListWidget::currentItemChanged信号，但是发现该信号存在莫名其妙的触发机制。
	//估计是QT5.12的bug。所以换成itemClicked信号
	connect(ui.langListWidget, &QListWidget::itemClicked, this, &QtLangSet::slot_itemSelect);
	connect(ui.langListWidget, &QListWidget::currentRowChanged, this, &QtLangSet::slot_langListCurRowChanged);

	connect(ui.userLangListWidget, &QListWidget::itemClicked, this, &QtLangSet::slot_userLangItemSelect);


	connect(ui.styleListWidget, &QListWidget::itemClicked, this, &QtLangSet::slot_styleItemSelect);
	connect(ui.styleListWidget, &QListWidget::currentRowChanged, this, &QtLangSet::slot_styleListCurRowChanged);

	connect(ui.boldCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontBoldChange);
	connect(ui.italicCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontItalicChange);
	connect(ui.underlineCheckBox, &QCheckBox::stateChanged, this, &QtLangSet::slot_fontUnderlineChange);
	connect(ui.fontSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slot_fontSizeChange(int)));
	connect(ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &QtLangSet::slot_fontChange);
}

//让初始化时选择当前编辑器的语言
void QtLangSet::selectInitLangTag(QString initLangTag)
{

	//第一次必须以初始化lexid显示。这样做实在是因为初始化list启动后，会自动把第一个设置为当前。
	//这里还是有问题，是qt5.12.4的bug机制导致:如果没有选择QListWidget，会总是自动选择第一个
	if (!initLangTag.isEmpty())
	{
		QList<QListWidgetItem*> destItems = ui.langListWidget->findItems(initLangTag,Qt::MatchExactly);
		if (!destItems.isEmpty())
		{
			QListWidgetItem* newItem = destItems.first();

			ui.langListWidget->scrollToItem(newItem);
			ui.langListWidget->setCurrentItem(newItem);
			ui.userLangListWidget->setCurrentItem(nullptr);

			slot_itemSelect(newItem);
		}
		else
		{
			destItems = ui.userLangListWidget->findItems(initLangTag, Qt::MatchExactly);
			if (!destItems.isEmpty())
			{
				QListWidgetItem* userItem = destItems.first();
				ui.userLangListWidget->scrollToItem(userItem);
				ui.userLangListWidget->setCurrentItem(userItem);
				ui.langListWidget->setCurrentItem(nullptr);

				slot_userLangItemSelect(userItem);

			}
		}
	}
}

void QtLangSet::slot_fontBoldChange(int state)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		if (m_curStyleData.font.bold() != ((state == Qt::Unchecked) ? false : true))
		{
			m_curStyleData.font.setBold((state == Qt::Unchecked) ? false : true);
			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;

			if (m_isGlobelItem)
			{
				if (ui.useGlobalFont->isVisible() && ui.useGbFontBold->isChecked())
				{
					slot_useAlobalFontBold(true);
					saveLangeSet(m_selectLexer);
				}
				else if (ui.useGlobalFont->isVisible() && !ui.useGbFontBold->isChecked())
				{
					//保存全局样式第一条样式本身
					saveLangeSet(m_selectLexer);
			}
				else if (!ui.useGlobalFont->isVisible())
				{
					//全局非第一个样式，修改当前全局字体大小
						//全局样式的非第一个
					setGlobalFont(m_selectStyleId, m_curStyleData.font);

					//保存全局样式第一条样式本身
					saveLangeSet(m_selectLexer);

					//这里就是全局的样式。通知当前所有的编辑框，去修改他们本身的全局样式
					CCNotePad* pMainNote = dynamic_cast<CCNotePad*>(parent());
					if (pMainNote != nullptr)
					{
						pMainNote->setGlobalFont(m_selectStyleId);
					}
				}
			}
			else
			{
				emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
			}
		}
	}
}

void QtLangSet::slot_fontItalicChange(int state)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		if (m_curStyleData.font.italic() != ((state == Qt::Unchecked) ? false : true))
		{
			m_curStyleData.font.setItalic((state == Qt::Unchecked) ? false : true);
			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;
			if (m_isGlobelItem)
			{
				if (ui.useGlobalFont->isVisible() && ui.useGbFontItalic->isChecked())
				{
					slot_useAlobalFontItalic(true);
					saveLangeSet(m_selectLexer);
				}
				else if (ui.useGlobalFont->isVisible() && !ui.useGbFontItalic->isChecked())
				{
					//保存全局样式第一条样式本身
					saveLangeSet(m_selectLexer);
			}
				else if (!ui.useGlobalFont->isVisible())
				{
					//全局非第一个样式，修改当前全局字体大小
						//全局样式的非第一个
					setGlobalFont(m_selectStyleId, m_curStyleData.font);
					saveLangeSet(m_selectLexer);

					//这里就是全局的样式。通知当前所有的编辑框，去修改他们本身的全局样式
					CCNotePad* pMainNote = dynamic_cast<CCNotePad*>(parent());
					if (pMainNote != nullptr)
					{
						pMainNote->setGlobalFont(m_selectStyleId);
					}
				}
			}
			else
			{
				emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
			}
		}
	}
}

void QtLangSet::slot_fontUnderlineChange(int state)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		if (m_curStyleData.font.underline() != ((state == Qt::Unchecked) ? false : true))
		{
			m_curStyleData.font.setUnderline((state == Qt::Unchecked) ? false : true);
			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;
			if (m_isGlobelItem)
			{
				if (ui.useGlobalFont->isVisible() && ui.useGbFontUnderline->isChecked())
				{
					slot_useAlobalFontUnderline(true);
					saveLangeSet(m_selectLexer);
				}
				else if (ui.useGlobalFont->isVisible() && !ui.useGbFontUnderline->isChecked())
				{
					//保存全局样式第一条样式本身
					saveLangeSet(m_selectLexer);
			}
				else if (!ui.useGlobalFont->isVisible())
				{
					//全局非第一个样式，修改当前全局字体大小
						//全局样式的非第一个
					setGlobalFont(m_selectStyleId, m_curStyleData.font);
					saveLangeSet(m_selectLexer);

					//这里就是全局的样式。通知当前所有的编辑框，去修改他们本身的全局样式
					CCNotePad* pMainNote = dynamic_cast<CCNotePad*>(parent());
					if (pMainNote != nullptr)
					{
						pMainNote->setGlobalFont(m_selectStyleId);
					}
				}
			}
			else
			{
				emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
			}
		}
	}
}

//发现一个qt现象：在debug断住该slot_fontSizeChange函数，会导致槽触发2次；不断住或者release模式直接跑，不会触发2次
void QtLangSet::slot_fontSizeChange(int v)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		//全局修改，把所有语言的所有风格都设置
		//全局修改，把所有语言的所有风格都设置
		if (m_isGlobelItem)
		{
			if (m_curStyleData.font.pointSize() != v)
			{
				m_curStyleData.font.setPointSize(v);
				//qDebug() << m_curStyleData.font.family();
				m_isStyleChange = true;
				m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);

				//全局第一个样式，修改所有语法样式
				if (ui.useGlobalFont->isVisible() && ui.useGbFontSize->isChecked())
				{
					slot_useAlobalFontSize(true);
					saveLangeSet(m_selectLexer);
				}
				else if (ui.useGlobalFont->isVisible() && !ui.useGbFontSize->isChecked())
				{
					//保存全局样式第一条样式本身
					saveLangeSet(m_selectLexer);
			}
				else if (!ui.useGlobalFont->isVisible())
				{
					//全局非第一个样式，修改当前全局字体大小
					//全局样式的非第一个
					setGlobalFont(m_selectStyleId, m_curStyleData.font);
					saveLangeSet(m_selectLexer);

					//这里就是全局的样式。通知当前所有的编辑框，去修改他们本身的全局样式
					CCNotePad* pMainNote = dynamic_cast<CCNotePad*>(parent());
					if (pMainNote != nullptr)
					{
						pMainNote->setGlobalFont(m_selectStyleId);
		}
				}
			}
		}
		else
		{
			if (m_curStyleData.font.pointSize() != v)
			{
				m_curStyleData.font.setPointSize(v);
				m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
				m_isStyleChange = true;
				emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
			}
		}
		/*else if (!ui.modifyAllFont->isChecked())
		{
			if (m_curStyleData.font.pointSize() != v)
			{
				m_curStyleData.font.setPointSize(v);
				m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
				m_isStyleChange = true;
				emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
			}
		}
		else
		{
			m_curStyleData.font.setPointSize(v);
			m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
			m_isStyleChange = true;
			//即时设置风格
			if (m_selectLexer != nullptr)
			{
				int row = ui.styleListWidget->count();

				QListWidgetItem* item = nullptr;

				for (int i = 0; i < row; ++i)
				{
					item = ui.styleListWidget->item(i);
					int styleId = item->data(Qt::UserRole).toInt();

					//所有字体大小修改为一致
					QFont f = m_selectLexer->font(styleId);
					f.setPointSize(v);
					m_selectLexer->setFont(f, styleId);
				}

				saveCurLangSettings();
				emit viewLexerChange(m_selectLexer->lexerTag());
			}
		}*/
		}
	}


void QtLangSet::getCurUseLexerTags(QVector<QString>& tag)
{
	CCNotePad* pMainNote = dynamic_cast<CCNotePad*>(parent());
	if (pMainNote != nullptr)
	{
		pMainNote->getCurUseLexerTags(tag);
	}
}

//使用全局设置设定所有语言风格
void QtLangSet::updateAllLangeStyleWithGlobal(GLOBAL_STYLE_SET flag)
{
	QFont oldfont;

	for (int index = 0; index <= L_TXT; ++index)
	{
		//GLOBAL本身不保存，因为GLOBAL不是语法样式，而是全局的属性风格
		if (index == L_GLOBAL)
		{
			continue;
		}

		QsciLexer *pLexer = ScintillaEditView::createLexer(index);
		if (nullptr != pLexer)
		{

			switch (flag)
			{
			case GLOBAL_FONT:
			{
				for (int i = 0; i <= 255; ++i)
				{
					if (!pLexer->description(i).isEmpty())
					{
						oldfont = pLexer->font(i);
						oldfont.setFamily(m_curStyleData.font.family());
						pLexer->setFont(oldfont, i);
					}
				}
			}
			break;
			case GLOBAL_FONT_SIZE:
			{
				for (int i = 0; i <= 255; ++i)
				{
					if (!pLexer->description(i).isEmpty())
					{
						oldfont = pLexer->font(i);
						oldfont.setPointSize(m_curStyleData.font.pointSize());
						pLexer->setFont(oldfont, i);
					}
				}
			}
			break;
			case GLOBAL_FONT_BOLD:
			{
				for (int i = 0; i <= 255; ++i)
				{
					if (!pLexer->description(i).isEmpty())
					{
						oldfont = pLexer->font(i);
						oldfont.setBold(m_curStyleData.font.bold());
						pLexer->setFont(oldfont, i);
					}
				}
			}
			break;
			case GLOBAL_FONT_UNDERLINE:
			{
				for (int i = 0; i <= 255; ++i)
				{
					if (!pLexer->description(i).isEmpty())
					{
						oldfont = pLexer->font(i);
						oldfont.setUnderline(m_curStyleData.font.underline());
						pLexer->setFont(oldfont, i);
					}
				}
			}
			break;
			case GLOBAL_FONT_ITALIC:
			{
				for (int i = 0; i <= 255; ++i)
				{
					if (!pLexer->description(i).isEmpty())
					{
						oldfont = pLexer->font(i);
						oldfont.setItalic(m_curStyleData.font.italic());
						pLexer->setFont(oldfont, i);
					}
				}
			}
			break;
			case GLOBAL_FG_COLOR:
			{
				pLexer->setColor(m_curStyleData.color, -1);
			}
			break;
			case GLOBAL_BK_COLOR:
			{
				pLexer->setPaper(m_curStyleData.paper, -1);
				//默认纸背景色会和QPalette保持一致。单独需要单独设置一下
				pLexer->setDefaultPaper(m_curStyleData.paper);
			}
			break;
			default:
				break;
			}
			
			saveLangeSet(pLexer);
		}
		delete pLexer;
	}
}

//恢复所有语言的初始配置。与restoreOriginLangOneStyle类似，但是粒度更大
void  QtLangSet::restoreOriginLangAllStyle()
{
	QsciLexer *pLexer = nullptr;

	//一旦重置，当前修改无条件不保存。否则避免当前的刚刚重置，又被保存
	m_isStyleChange = false;

	for (int index = 0; index <= L_TXT; ++index)
	{

		pLexer = ScintillaEditView::createLexer(index);

		if (pLexer == nullptr)
		{
			continue;
		}

		//如果存在自定义的配置，直接全部删除掉
		QString cfgPath = QString("notepad/userstyle/%1/%2").arg(StyleSet::getCurrentStyle()).arg(pLexer->lexerTag());
		QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);

		if (QFile::exists(qs.fileName()))
		{
			QFile::remove(qs.fileName());
		}

		delete pLexer;
		pLexer = nullptr;
	}
}

//只在restoreOriginLangOneStyle中调用
static void restoreLangFontFgColorToDarkStyle(QsciLexer *pLexer, int i)
{
	//if (QtLangSet::s_darkColorMap.contains(pLexer->color(i).name()))
	//{
	//	pLexer->setColor(QtLangSet::s_darkColorMap.value(pLexer->color(i).name()), i);
	//}
	}

//只在restoreOriginLangOneStyle中调用
static void restoreLangPaperColorToDarkStyle(QsciLexer *pLexer, int i)
{
	//注意下面这个颜色和void StyleSet::setBlackStyle()中保持一致
	//背景也变成暗黑色
	if (StyleSet::foldbgColor != pLexer->paper(i))
	{
		pLexer->setPaper(StyleSet::foldbgColor, i);
		pLexer->setDefaultPaper(StyleSet::foldbgColor);
	}
}

//恢复所有语言的初始配置,只恢复GLOBAL_STYLE_SET指定的风格
void QtLangSet::restoreOriginLangOneStyle(GLOBAL_STYLE_SET flag)
{

	QFont oldfont;
	QFont curfont;

	QColor oldClor;

	for (int index = 0; index <= L_TXT; ++index)
	{
		QsciLexer *pLexer = ScintillaEditView::createLexer(index);
		QsciLexer *pOriginLexer = ScintillaEditView::createLexer(index,"",true);
		if (nullptr != pLexer)
		{
			for (int i = 0; i <= 255; ++i)
			{
				if (!pLexer->description(i).isEmpty())
				{

					switch (flag)
					{
					case GLOBAL_FONT:
					{
						oldfont = pOriginLexer->font(i);
						pLexer->setFont(oldfont, i);
					}
					break;
					case GLOBAL_FONT_SIZE:
					{
						oldfont = pOriginLexer->font(i);
						curfont = pLexer->font(i);
						curfont.setPointSize(oldfont.pointSize());
						pLexer->setFont(curfont, i);
					}
					break;
					case GLOBAL_FONT_BOLD:
					{
						oldfont = pOriginLexer->font(i);
						curfont = pLexer->font(i);
						curfont.setBold(oldfont.bold());
						pLexer->setFont(curfont, i);
					}
					break;
					case GLOBAL_FONT_UNDERLINE:
					{
						oldfont = pOriginLexer->font(i);
						curfont = pLexer->font(i);
						curfont.setUnderline(oldfont.underline());
						pLexer->setFont(curfont, i);
					}
					break;
					case GLOBAL_FONT_ITALIC:
					{
						oldfont = pOriginLexer->font(i);
						curfont = pLexer->font(i);
						curfont.setItalic(oldfont.italic());
						pLexer->setFont(curfont, i);
					}
					break;
					case GLOBAL_FG_COLOR:
					{
						oldClor = pOriginLexer->color(i);
						pLexer->setColor(oldClor, i);

						/*if (BLACK_SE == StyleSet::getCurrentSytleId())
						{
							restoreLangFontFgColorToDarkStyle(pLexer, i);
						}*/

					}
					break;
					case GLOBAL_BK_COLOR:
					{
						oldClor = pOriginLexer->paper(i);
						pLexer->setPaper(oldClor, i);

						/*if (BLACK_SE == StyleSet::getCurrentSytleId())
						{
						restoreLangPaperColorToDarkStyle(pLexer, i);
						}*/
					}
					break;
					default:
						break;
					}
				}
			}
			saveLangeSet(pLexer);
		}
		delete pLexer;
		delete pOriginLexer;
	}
}
//预览全局修改字体效果。把当前所有的语法，风格字体都修改一遍
void QtLangSet::previewAllGoblalChange()
{
	QVector<QString> tags;

	getCurUseLexerTags(tags);

	for (int i = 0, s = tags.size(); i < s; ++i)
	{
		emit viewLexerChange(tags.at(i));
	}
}

void QtLangSet::slot_fontChange(const QFont &font)
{
	//即时设置风格
	if (m_selectLexer != nullptr)
	{
		//全局修改
		if (m_isGlobelItem)
	  {
			if (m_curStyleData.font != font)
			{
				m_curStyleData.font.setFamily(font.family());
				m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
				m_isStyleChange = true;

				//修改所有字体
				if (ui.useGlobalFont->isVisible() && ui.useGbFont->isChecked())
				{
						slot_useAlobalFont(true);
					saveLangeSet(m_selectLexer);
					}
				else if (ui.useGlobalFont->isVisible() && !ui.useGbFont->isChecked())
				{
					//保存全局样式第一条样式本身
					saveLangeSet(m_selectLexer);
				}
				else if(!ui.useGlobalFont->isVisible())
				{
					//全局样式的非第一个
					setGlobalFont(m_selectStyleId, m_curStyleData.font);

					//保存全局样式第一条样式本身
					saveLangeSet(m_selectLexer);

					//这里就是全局的样式。通知当前所有的编辑框，去修改他们本身的全局样式
					CCNotePad* pMainNote = dynamic_cast<CCNotePad*>(parent());
					if (pMainNote != nullptr)
					{
						pMainNote->setGlobalFont(m_selectStyleId);
				}
			}
		}
		}
		else
		{
#if 0
			//统一修改字体，大小和风格保存不变
			QFont oldf = m_curStyleData.font;
			m_curStyleData.font = font;
			m_curStyleData.font.setBold(oldf.bold());
			m_curStyleData.font.setItalic(oldf.italic());
			m_curStyleData.font.setUnderline(oldf.underline());

			m_isStyleChange = true;
			//即时设置风格
			if (m_selectLexer != nullptr)
			{
				int row = ui.styleListWidget->count();

				QListWidgetItem* item = nullptr;

				for (int i = 0; i < row; ++i)
				{
					item = ui.styleListWidget->item(i);
					int styleId = item->data(Qt::UserRole).toInt();
					QFont f = m_selectLexer->font(styleId);

					//新字体的名统一设置，但是风格字体大小保存老的一致
					QFont newFont = m_curStyleData.font;
					newFont.setBold(f.bold());
					newFont.setItalic(f.italic());
					newFont.setUnderline(f.underline());
					newFont.setPointSize(f.pointSize());

					m_selectLexer->setFont(newFont, styleId);
				}

				saveCurLangSettings();
				emit viewLexerChange(m_selectLexer->lexerTag());
			}
#endif
			if (m_curStyleData.font != font)
			{
				m_curStyleData.font.setFamily(font.family());
				m_selectLexer->setFont(m_curStyleData.font, m_selectStyleId);
				m_isStyleChange = true;

				emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, m_curStyleData.color, m_curStyleData.paper, m_curStyleData.font, true);
				
		}
	}
}
}

void QtLangSet::initLangList()
{

	for (int i = 0; i <= L_TXT; ++i)
	{
		QsciLexer *pLexer = ScintillaEditView::createLexer(i);
		if (nullptr != pLexer)
		{
			QString langName = pLexer->lexerTag();
			QListWidgetItem *item = new QListWidgetItem(langName);
			item->setData(Qt::UserRole,i);
			ui.langListWidget->addItem(item);
			delete pLexer;
		}
	}

	//怀疑这个是队列链接的，会延迟执行。故总是导致选择的选不中，而是第一个语言asp。
	ui.langListWidget->sortItems(Qt::AscendingOrder);

	//手动修改第一个名称为中文。
	if (ui.langListWidget->item(0)->text() == QString("AllGlobal"))
	{
		ui.langListWidget->item(0)->setText(tr("AllGlobal"));
}
}

//isLoadToUI是否加载显示到当前UI界面
bool QtLangSet::readLangSetFile(QString langName,QString &keyword, QString &motherLang, QString & extTypes)
{
	QString userLangFile = QString("notepad/userlang/%1").arg(langName);//自定义语言中不能有.字符，否则可能有错，后续要检查
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, userLangFile);
	qs.setIniCodec("UTF-8");
	//qDebug() << qs.fileName();

	if (!qs.contains("mz"))
	{
		return false;
	}

	//自定义语言格式。
	//mz:ndd
	//name:xxx
	//mother:xxx none/cpp/html 就三种
	//ext:xx xx xx 文件关联后缀名
	//keword:xxx
	keyword = qs.value("keyword").toString();
	extTypes = qs.value("ext").toString();
	motherLang = qs.value("mother").toString();

	return true;
}

//初始化用户自定义的语言配置
void QtLangSet::initUserDefineLangList()
{
	QString userLangDirPath = getUserLangDirPath();

	//遍历文件夹
	QDir dir_file(userLangDirPath);
	QFileInfoList list_file = dir_file.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name);

	for (int i = 0; i < list_file.size(); ++i)
	{  //将当前目录中所有文件添加到treewidget中
		QFileInfo fileInfo = list_file.at(i);

		//这个文件是ext和tag的映射文件，不做配置解析
		if (fileInfo.baseName() == "ext_tag")
		{
			continue;
		}

		ui.userLangListWidget->addItem(fileInfo.baseName());
	}
	ui.userLangListWidget->sortItems(Qt::AscendingOrder);
}


//切换当前语言。
//QListWidget::currentItemChanged这个信号qt5.12.10中应该是存在bug,总是莫名其妙的要发一次currentItemChanged信号出来。导致第一次该选的项被覆盖。
//换成itemClicked信号
void QtLangSet::slot_itemSelect(QListWidgetItem *item)
{
	if (item == nullptr)
	{
		return;
	}
	//qDebug() << item->text() << (previous ? previous->text() : 0);

	//如果前后主题发生了变化，则依旧要走更新，防止主题切换后没及时更新
	if (m_previousSysLangItem == item && (m_lastThemesId == m_themesId))
	{
		return;
	}

	ui.curThemes->setText(tr("Current themes : %1, language : %2").arg(StyleSet::getCurrentStyle()).arg(item->text()));


	ui.langListWidget->scrollToItem(item);

	ui.userLangListWidget->setCurrentItem(nullptr);

	//保存前一个已经修改的。询问用户是否保存
	if (m_isStyleChange && m_previousSysLangItem != nullptr && m_selectLexer !=nullptr)
	{
		//只有当前是非全局，才需要提示用户是否要保持。
		//全局的不需要提示，因为已经无条件全部保存过了。
		if (!m_isGlobelItem)
		{
			if (QMessageBox::Yes == QMessageBox::question(this, tr("Save Change"), tr("%1 style configuration has been modified. Do you want to save it?").arg(m_previousSysLangItem->text())))
			{
				saveCurLangSettings();
			}
		}
	}

	if (!item->isSelected())
	{
		item->setSelected(true);
	}

	ui.styleListWidget->clear();

	if (m_selectLexer != nullptr)
	{
		delete m_selectLexer;
		m_selectLexer = nullptr;
	}

	int lexId = item->data(Qt::UserRole).toInt();

	//全局的这个要单独对待，暂时没有任何风格。
	if (lexId == L_GLOBAL)
	{
		m_isGlobelItem = true;
		ui.stackedWidget->setCurrentIndex(1);
	}
	else
	{
		m_isGlobelItem = false;
		ui.stackedWidget->setCurrentIndex(0);

		}

	QsciLexer *pLexer = ScintillaEditView::createLexer(lexId);
	if (nullptr != pLexer)
	{
		m_selectLexer = pLexer;

		QString keyword(m_selectLexer->keywords(1));

		ui.keywordTe->setPlainText(keyword);
		ui.motherLangCb->setCurrentIndex(0);

		//设置关联文件项
		QString langName = item->text();
		QStringList extList;
		ExtLexerManager::getInstance()->getExtlistByLangTag(langName, extList);
		ui.extFileType->setText(extList.join(','));

		for (int i = 0; i <= 255; ++i)
		{
			QString desc = pLexer->description(i);
			if (!desc.isEmpty())
			{
				QListWidgetItem *itemtemp = new QListWidgetItem(desc);
				itemtemp->setData(Qt::UserRole, i);
				ui.styleListWidget->addItem(itemtemp);
			}
		}
	}

	m_previousSysLangItem = item;

	//自动显示第一条的结果
	QListWidgetItem *itemtemp = ui.styleListWidget->item(0);
	if (itemtemp != nullptr)
	{
		itemtemp->setSelected(true);
		slot_styleItemSelect(itemtemp);
	}

	m_isStyleChange = false;

	//ui.extFileType->clear();
	ui.motherLangCb->setCurrentIndex(0);
}

void QtLangSet::slot_langListCurRowChanged(int row)
{
	QListWidgetItem* current = ui.langListWidget->item(row);
	slot_itemSelect(current);
}

//根据母亲模块语言，把属性风格显示出来
void QtLangSet::displayUserMotherLangsStyle(QString langTagName, UserLangMother motherLang)
{
	QsciLexer *pLexer = nullptr;
	
	ui.styleListWidget->clear();

	switch (motherLang)
	{
	case MOTHER_NONE:
		pLexer = ScintillaEditView::createLexer(L_USER_TXT, langTagName);
		break;
	case MOTHER_CPP:
		pLexer = ScintillaEditView::createLexer(L_USER_CPP, langTagName);
		break;
	default:
		pLexer = ScintillaEditView::createLexer(L_USER_TXT, langTagName);
		break;
	}

	if (nullptr != pLexer)
	{
		//这里一定要以新语言的名称作为lexer的tag。
		//pLexer->setLexerTag(langTagName);
		pLexer->setIsUserDefineKeywords(true);

		if (m_selectLexer != nullptr)
		{
			delete m_selectLexer;
		}

		m_selectLexer = pLexer;

		for (int i = 0; i <= 255; ++i)
		{
			QString desc = pLexer->description(i);
			if (!desc.isEmpty())
			{
				QListWidgetItem *itemtemp = new QListWidgetItem(desc);
				itemtemp->setData(Qt::UserRole, i);
				ui.styleListWidget->addItem(itemtemp);
			}
		}
	}

	//自动显示第一条的结果
	QListWidgetItem *itemtemp = ui.styleListWidget->item(0);
	if (itemtemp != nullptr)
	{
		itemtemp->setSelected(true);
		slot_styleItemSelect(itemtemp);
	}

	m_isStyleChange = false;

}


//点击当前用户自定义语言的语言item。todo:暂时没有做前一个的修改保存，需要用户每次修改后自己点击save，否则可能不会保存
void QtLangSet::slot_userLangItemSelect(QListWidgetItem *item)
{
	if (item == nullptr)
	{
		return;
	}

	//肯定不能是全局设置了
	if(m_isGlobelItem || (ui.stackedWidget->currentIndex() == 1))
	{
		m_isGlobelItem = false;
		ui.stackedWidget->setCurrentIndex(0);

		/*if (!ui.modiryAllColor->isEnabled())
		{
			ui.modiryAllColor->setEnabled(true);
		}
		if (!ui.modifyAllFont->isEnabled())
		{
			ui.modifyAllFont->setEnabled(true);
		}*/
		}


	m_previousSysLangItem = nullptr;
	ui.langListWidget->setCurrentItem(nullptr);

	QString langName = item->text();

	QString keyword;
	QString motherLang;
	QString extTypes;

	if (readLangSetFile(langName, keyword, motherLang, extTypes))
	{
		ui.keywordTe->setPlainText(keyword);
		ui.motherLangCb->setCurrentText(motherLang);
		ui.extFileType->setText(extTypes);

		//根据母亲模块语言，把属性风格显示出来
		displayUserMotherLangsStyle(langName, (UserLangMother)(LangType::L_USER_TXT + ui.motherLangCb->currentIndex()));
	}
	else
	{
		ui.statusBar->showMessage(tr("Read %1 language user define  format error.").arg(langName), 10000);
	}
}


//获取当前ui上的字体
void QtLangSet::getCurUiFont(QFont& font)
{
	font = ui.fontComboBox->currentFont();
	font.setBold(ui.boldCheckBox->isChecked());
	font.setItalic(ui.italicCheckBox->isChecked());
	font.setUnderline(ui.underlineCheckBox->isChecked());
	font.setPointSize(ui.fontSpinBox->value());
}

void QtLangSet::closeEvent(QCloseEvent * e)
{
	saveCurLangSettings();
}

//把item的对应风格显示在界面上
void QtLangSet::syncShowStyleItemToUI(QListWidgetItem *item)
{
	if (item != nullptr && m_selectLexer != nullptr)
	{
		int styleId = item->data(Qt::UserRole).toInt();

		if (m_selectStyleId == styleId && (m_lastThemesId == m_themesId))
		{
			return;
		}

		if (!item->isSelected())
		{
			item->setSelected(true);
		}

		if (!ui.label_fc->isEnabled())
		{
			ui.label_fc->setEnabled(true);
			ui.toolButton->setEnabled(true);
		}
		if (!ui.label_bc->isEnabled())
		{
			ui.label_bc->setEnabled(true);
			ui.toolButton1->setEnabled(true);
		}
		if (!ui.fontComboBox->isEnabled())
		{
			ui.fontComboBox->setEnabled(true);
			ui.boldCheckBox->setEnabled(true);
			ui.italicCheckBox->setEnabled(true);
			ui.underlineCheckBox->setEnabled(true);
		}
		if (!ui.fontSpinBox->isEnabled())
		{
			ui.fontSpinBox->setEnabled(true);
		}
		
		m_selectStyleId = styleId;
		QsciLexer::StyleData & sd = m_selectLexer->styleData(styleId);
		m_curStyleData = sd;
		setStyleShow(sd.font, sd.color, sd.paper);
		m_isStyleChange = false;

		//如果是全局，则把不能修改的全局样式灰掉，避免干扰用户的选择
		if (m_isGlobelItem)
		{
			QsciLexerGlobal* pGlobalLexer = dynamic_cast<QsciLexerGlobal*>(m_selectLexer);
			if (pGlobalLexer != nullptr)
			{

				int enableMask = pGlobalLexer->changeOperBit(styleId);

				if (!(enableMask & FG_BIT))
				{
					ui.label_fc->setEnabled(false);
					ui.toolButton->setEnabled(false);
	}
				if (!(enableMask & BG_BIT))
				{
					ui.label_bc->setEnabled(false);
					ui.toolButton1->setEnabled(false);
}
				if (!(enableMask & FONT_BIT))
				{
					ui.fontComboBox->setEnabled(false);
					ui.boldCheckBox->setEnabled(false);
					ui.italicCheckBox->setEnabled(false);
					ui.underlineCheckBox->setEnabled(false);
				}
				if (!(enableMask & SIZE_BIT))
				{
					ui.fontSpinBox->setEnabled(false);
				}
			}
			
		}
	}
}
//点击当前的风格item
void QtLangSet::slot_styleItemSelect(QListWidgetItem *current)
{
	qDebug() << "slot_styleItemSelect";

	syncShowStyleItemToUI(current);

	if (m_isGlobelItem && ui.styleListWidget->item(0) == current)
	{
		ui.useGlobalColor->setVisible(true); 
		ui.useGlobalFont->setVisible(true);
	}
	else
	{
		if (ui.useGlobalColor->isVisible())
		{
			ui.useGlobalColor->setVisible(false);
}
		if (ui.useGlobalFont->isVisible())
		{
			ui.useGlobalFont->setVisible(false);
		}
	}

}

//使用键盘上下切换
void QtLangSet::slot_styleListCurRowChanged(int row)
{
	qDebug() << "slot_curRowChanged";
	QListWidgetItem* current = ui.styleListWidget->item(row);
	slot_styleItemSelect(current);
}

void QtLangSet::fillForegroundColor(QColor& fcColor)
{
	QPixmap f(32, 32);
	f.fill(fcColor);
	ui.label_fc->setPixmap(f);
}

void QtLangSet::fillBackgroundColor(QColor &bkColor)
{

	QPixmap b(32, 32);
	b.fill(bkColor);
	ui.label_bc->setPixmap(b);
}

void QtLangSet::setStyleShow(QFont& font, QColor& fcColor, QColor &bkColor)
{
	//修改字体会触发槽函数执行，务必先关闭一下信号
	enableFontChangeSensitive(false);

	ui.fontComboBox->setCurrentFont(font);
	ui.fontSpinBox->setValue(font.pointSize());

	ui.boldCheckBox->setChecked(font.bold());
	ui.italicCheckBox->setChecked(font.italic());
	ui.underlineCheckBox->setChecked(font.underline());

	fillForegroundColor(fcColor);
	fillBackgroundColor(bkColor);

	enableFontChangeSensitive(true);
	
}

//读取特定语言的设置；StyleId-1则读取当前主题，否则指定的StyleId主题
bool QtLangSet::readLangSettings(QsciLexer *lexer, QString tag, int StyleId)
{
	QString cfgPath = QString("notepad/userstyle/%1/%2").arg((StyleId == -1) ? StyleSet::getCurrentStyle() : StyleSet::getStyleName(StyleId)).arg(tag);

	QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
	if (QFile::exists(qs.fileName()))
	{
		return lexer->readSettings(qs);
	}
	else//这里应该加个逻辑完备的保护。如果没有读取到用户配置，则应该从标准目标去读原始配置
	{
		return readLangOriginSettings(lexer, tag, StyleId);
	}
	
	return false;
}

//读取特定语言的原始样式设置；StyleId-1则读取当前主题，否则指定的StyleId主题
bool QtLangSet::readLangOriginSettings(QsciLexer* lexer, QString tag, int StyleId)
{
	//默认主题不需要读取，内存中已经存在
	if (StyleId == 0)
	{
		return true;
	}

	//默认皮肤路径放在软件的同级目录下面的themes目录
	QString cfgPath = QString("%1/themes/%2/%3.ini").arg(QCoreApplication::applicationDirPath()).arg((StyleId == -1)?StyleSet::getCurrentStyle(): StyleSet::getStyleName(StyleId)).arg(tag);
	QSettings qs(cfgPath, QSettings::IniFormat);

	//QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
	if (QFile::exists(qs.fileName()))
	{
		return lexer->readSettings(qs);
	}
	return false;
}


//保存一种语言的配置。
void QtLangSet::saveLangeSet(QsciLexer *lexer, int StyleId)
{
	if (lexer != nullptr)
	{
		QString tag = lexer->lexerTag();

		QString cfgPath =  QString("notepad/userstyle/%1/%2").arg((StyleId == -1) ? StyleSet::getCurrentStyle() : StyleSet::getStyleName(StyleId)).arg(tag);

		QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
		lexer->writeSettings(qs);
		qs.sync();
	}
}

//保存当前选定语言的配置
bool QtLangSet::saveCurLangSettings()
{
	if (m_isStyleChange && m_selectLexer != nullptr)
	{
		saveLangeSet(m_selectLexer);
		m_isStyleChange = false;
		ui.statusBar->showMessage(tr("Save Finished !"),10000);
		return true;
	}
	ui.statusBar->showMessage(tr("Not change, no need save !"),10000);
	return false;
}

bool QtLangSet::isUseGlobalFgColor()
{
	return ui.useGlobalColor->isVisible() && ui.useGbFc->isChecked();
}

bool QtLangSet::isUseGlobalBgColor()
{
	return ui.useGlobalColor->isVisible() && ui.useGbBc->isChecked();
}

//修改前景色
void QtLangSet::slot_changeFgColor()
{
	QColor color = QColorDialog::getColor(m_curStyleData.color, this, tr("Style Foreground Color"));
	if (color.isValid())
	{
		//全局修改
		if (m_isGlobelItem)
		{
			if (m_curStyleData.color != color)
			{
				m_curStyleData.color = color;
				fillForegroundColor(color);
				m_isStyleChange = true;

				//即时设置风格
				if (m_selectLexer != nullptr)
				{
					//此时一定是QsciLexerGlobal
					m_selectLexer->setColor(color, m_selectStyleId);
				}

				//只有在第一个全局样式时，isUseGlobalFgColor下面才为true。把所有语言的所有风格都设置
				if (isUseGlobalFgColor())
				{
					slot_useAlobalFgColor(true);
				}
				else if(!ui.useGlobalColor->isVisible())
				{
					//全局样式的非第一个
					setGlobalFgColor(m_selectStyleId, color);

					//这里就是全局的样式。通知当前所有的编辑框，去修改他们本身的全局样式
					CCNotePad* pMainNote = dynamic_cast<CCNotePad*>(parent());
					if (pMainNote != nullptr)
					{
						pMainNote->setGlobalFgColor(m_selectStyleId);
			}
			
		}
				
			}
			
		}
		else
		{
			//当前前景色是否变化
			if (m_curStyleData.color != color)
			{
				m_curStyleData.color = color;
				fillForegroundColor(color);
				m_isStyleChange = true;

				//即时设置风格
				if (m_selectLexer != nullptr)
				{
					m_selectLexer->setColor(color, m_selectStyleId);

					emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, color, m_curStyleData.paper, m_curStyleData.font, false);
				}
			}
		}
		/*else if (!ui.modiryAllColor->isChecked())
		{
			当前前景色是否变化
			if (m_curStyleData.color != color)
			{
			m_curStyleData.color = color;
			fillForegroundColor(color);
			m_isStyleChange = true;

				即时设置风格
			if (m_selectLexer != nullptr)
			{
					m_selectLexer->setColor(color, m_selectStyleId);

					emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, color, m_curStyleData.paper, m_curStyleData.font, false);
				}
			}
		}*/
		/*else
				{
			全部风格颜色修改
			当前前景色是否变化
			m_curStyleData.color = color;
			fillForegroundColor(color);
			m_isStyleChange = true;

			即时设置风格
			if (m_selectLexer != nullptr)
			{
					m_selectLexer->setColor(color, -1);
				saveCurLangSettings();
				emit viewLexerChange(m_selectLexer->lexerTag());
			}
			*/
	//	}
		}
	
}

//修改背景色
void QtLangSet::slot_changeBkColor()
{
	QColor color = QColorDialog::getColor(m_curStyleData.paper, this, tr("Style Background Color"));
	if (color.isValid())
	{
		//全局修改，把所有语言的所有风格都设置
		if (m_isGlobelItem)
		{
			if (m_curStyleData.paper != color)
			{
				m_curStyleData.paper = color;
				fillBackgroundColor(color);
				m_isStyleChange = true;

				//即时设置风格
				if (m_selectLexer != nullptr)
				{
					m_selectLexer->setPaper(color, m_selectStyleId);
				}

				if (isUseGlobalBgColor())
				{
					slot_useAlobalBkColor(true);
				}
				else if (!ui.useGlobalColor->isVisible())
		{
					setGlobalBgColor(m_selectStyleId, color);

					//这里就是全局的样式。通知当前所有的编辑框，去修改他们本身的全局样式
					CCNotePad* pMainNote = dynamic_cast<CCNotePad*>(parent());
					if (pMainNote != nullptr)
		{
						pMainNote->setGlobalBgColor(m_selectStyleId);
			}

		}
	}

		}
	//	else if (!ui.modiryAllColor->isChecked())
	//	{
	//	//当前前景色是否变化
	//	if (m_curStyleData.paper != color)
	//	{
	//		m_curStyleData.paper = color;
	//		fillBackgroundColor(color);
	//		m_isStyleChange = true;

	//		//即时设置风格
	//		if (m_selectLexer != nullptr)
	//		{
	//			m_selectLexer->setPaper(color, m_selectStyleId);
	//			emit viewStyleChange(m_selectLexer->lexerTag(), m_selectStyleId, color, m_curStyleData.paper, m_curStyleData.font, false);
	//		}
	//	}
	//}
		else
		{
			//全部风格颜色修改
			//当前前景色是否变化
			m_curStyleData.paper = color;
			fillBackgroundColor(color);
			m_isStyleChange = true;

			//即时设置风格
			if (m_selectLexer != nullptr)
			{
				m_selectLexer->setPaper(color, -1);
				
				saveCurLangSettings();
				emit viewLexerChange(m_selectLexer->lexerTag());
}

		}
	}
}

void QtLangSet::slot_saveClick()
{
	saveCurLangSettings();
}

//恢复初始化设置
void QtLangSet::slot_reset()
{
	if (m_selectLexer != nullptr)
	{
		if (!m_isGlobelItem)
		{
			if (QMessageBox::Yes != QMessageBox::question(this, tr("Reset Style"), tr("Are you sure to reset language %1 sytle").arg(m_selectLexer->lexerTag())))
			{
				return;
			}

			//一旦重置，当前修改无条件不保存。否则避免当前的刚刚重置，又被保存
			m_isStyleChange = false;

			//如果存在自定义的配置，也删除掉
			QString cfgPath = QString("notepad/userstyle/%1/%2").arg(StyleSet::getCurrentStyle()).arg(m_selectLexer->lexerTag());

			QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
			if (QFile::exists(qs.fileName()))
			{
				QFile::remove(qs.fileName());
			}

			//一定要先保存，因为selectInitLangTag后可能会变化
			int row = ui.styleListWidget->currentRow();

			//强行让slot_itemSelect里面的进行切换
			m_previousSysLangItem = nullptr;

			selectInitLangTag(m_selectLexer->lexerTag());

			//手动刷新UI
			QListWidgetItem* styleItem = ui.styleListWidget->item(row);

			m_selectStyleId = -1;

			syncShowStyleItemToUI(styleItem);

			emit viewLexerChange(m_selectLexer->lexerTag());
		}
		else
		{
			if (ui.useGlobalFont->isVisible())
			{
			//如果是全局重置，则把所有语言都重置
			if (QMessageBox::Yes != QMessageBox::question(this, tr("Reset All Style"), tr("Are you sure to reset All language sytle")))
			{
				return;
			}
			restoreOriginLangAllStyle();

			previewAllGoblalChange();

			//手动刷新UI。全局目前只有一行全局设置

			if (m_selectLexer != nullptr)
			{
				delete m_selectLexer;
			}

				m_selectLexer = ScintillaEditView::createLexer(L_GLOBAL);

			QListWidgetItem* styleItem = ui.styleListWidget->item(0);

				m_selectStyleId = -1;

			syncShowStyleItemToUI(styleItem);
		}
			else
			{
				//重置所有全局样式
				//一旦重置，当前修改无条件不保存。否则避免当前的刚刚重置，又被保存
				m_isStyleChange = false;

				//如果存在自定义的配置，也删除掉
				QString cfgPath = QString("notepad/userstyle/%1/%2").arg(StyleSet::getCurrentStyle()).arg(m_selectLexer->lexerTag());

				QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
				if (QFile::exists(qs.fileName()))
				{
					QFile::remove(qs.fileName());
	}

				//一定要先保存，因为selectInitLangTag后可能会变化

				int row = ui.styleListWidget->currentRow();

				//强行让slot_itemSelect里面的进行切换
				m_previousSysLangItem = nullptr;

				//这里必须写ALLglobal，否则中文查找不到
				selectInitLangTag(tr("AllGlobal"));

				//手动刷新UI
				QListWidgetItem* styleItem = ui.styleListWidget->item(row);

				m_selectStyleId = -1;

				syncShowStyleItemToUI(styleItem);

				//重新从加载一次纯净的全局样式。因为前面保存的已经删了，所以加载的是纯净的。
				StyleSet::loadGolbalStyle();

				updateThemes();
}
		}
	}
}


void QtLangSet::slot_useAlobalFgColor(bool check)
{
	if (check)
	{
		updateAllLangeStyleWithGlobal(GLOBAL_FG_COLOR);
		previewAllGoblalChange();
	}
	else
	{
		restoreOriginLangOneStyle(GLOBAL_FG_COLOR);
		previewAllGoblalChange();
	}

}

void QtLangSet::slot_useAlobalBkColor(bool check)
{
	if (check)
	{
		updateAllLangeStyleWithGlobal(GLOBAL_BK_COLOR);
		previewAllGoblalChange();
	}
	else
	{
		restoreOriginLangOneStyle(GLOBAL_BK_COLOR);
		previewAllGoblalChange();
	}
}

void QtLangSet::slot_useAlobalFont(bool check)
{
	if (check)
	{
		updateAllLangeStyleWithGlobal(GLOBAL_FONT);
		previewAllGoblalChange();
	}
	else
	{
		restoreOriginLangOneStyle(GLOBAL_FONT);
		previewAllGoblalChange();
	}
}

void QtLangSet::slot_useAlobalFontSize(bool check)
{
	if (check)
	{
		updateAllLangeStyleWithGlobal(GLOBAL_FONT_SIZE);
		previewAllGoblalChange();
	}
	else
	{
		restoreOriginLangOneStyle(GLOBAL_FONT_SIZE);
		previewAllGoblalChange();
	}
}

void QtLangSet::slot_useAlobalFontBold(bool check)
{
	if (check)
	{
		updateAllLangeStyleWithGlobal(GLOBAL_FONT_BOLD);
		previewAllGoblalChange();
	}
	else
	{
		restoreOriginLangOneStyle(GLOBAL_FONT_BOLD);
		previewAllGoblalChange();
	}
}

void QtLangSet::slot_useAlobalFontUnderline(bool check)
{
	if (check)
	{
		updateAllLangeStyleWithGlobal(GLOBAL_FONT_UNDERLINE);
		previewAllGoblalChange();
	}
	else
	{
		restoreOriginLangOneStyle(GLOBAL_FONT_UNDERLINE);
		previewAllGoblalChange();
	}
}

void QtLangSet::slot_useAlobalFontItalic(bool check)
{
	if (check)
	{
		updateAllLangeStyleWithGlobal(GLOBAL_FONT_ITALIC);
		previewAllGoblalChange();
	}
	else
	{
		restoreOriginLangOneStyle(GLOBAL_FONT_ITALIC);
		previewAllGoblalChange();
	}
}



//把默认语言风格的部分颜色，替换为暗色下显眼的颜色
//这个函数是可以处理自定义语言的。
//void QtLangSet::setLangFontFgColorToDarkStyle(LangType langId, QString tag)
//{
//	QColor blackColor(Qt::black);
//	QColor lightColor(0xdedede);
//
//	QColor blackColor1(0x0000ff);
//	QColor lightColor1(0xffaa00);
//
//
//	//C++注释的默认颜色，不显眼
//	QColor blackColor2(0x007f00);
//	QColor lightColor2(0x009000);
//
//	QColor blackColor3(0x7f7f00);
//	QColor lightColor3(0xfca287);
//
//
//	QColor blackColor4(0x8000ff);
//	QColor lightColor4(0xffaa00);
//
//	注意下面这个颜色和void StyleSet::setBlackStyle()中保持一致
//	QColor bkPaperColor(0, 0, 0);
//
//	QsciLexer *pLexer = ScintillaEditView::createLexer(langId, tag);
//		if (nullptr != pLexer)
//		{
//			for (int i = 0; i <= 255; ++i)
//			{
//				if (!pLexer->description(i).isEmpty())
//				{
//				/*if (blackColor == pLexer->color(i))
//					{
//					pLexer->setColor(lightColor, i);
//					}
//					else if (blackColor1 == pLexer->color(i))
//					{
//						pLexer->setColor(lightColor1, i);
//					}
//				else if (blackColor2 == pLexer->color(i))
//				{
//					pLexer->setColor(lightColor2, i);
//				}
//				else if (blackColor3 == pLexer->color(i))
//				{
//					pLexer->setColor(lightColor3, i);
//				}
//				else if (blackColor4 == pLexer->color(i))
//				{
//					pLexer->setColor(lightColor4, i);
//				}*/
//
//				if (QtLangSet::s_darkColorMap.contains(pLexer->color(i).name()))
//				{
//					pLexer->setColor(QtLangSet::s_darkColorMap.value(pLexer->color(i).name()), i);
//				}
//
//				背景也变成暗黑色
//				if (StyleSet::foldbgColor != pLexer->paper(i))
//				{
//					pLexer->setPaper(StyleSet::foldbgColor, i);
//				}
//			}
//		}
//		默认纸背景色会和QPalette保持一致。但是因为黑色特色，其背景和QPalette不一样，所有需要单独设置一下
//		pLexer->setDefaultPaper(StyleSet::foldbgColor);
//			saveLangeSet(pLexer);
//			delete pLexer;
//		}
//	}

//把暗色系下面不容易看清的颜色给替换一下。默认是有风格的，但是默认风格是亮色系的。
//把凡是0x000000的字体，变成0xdedede
//凡是0x0000ff的颜色，变成0xffff00
//这个函数没有处理自定义语言。
//void QtLangSet::setAllLangFontFgColorToDarkStyle()
//{
//	initDarkColorMap();
//
//	for (int index = 0; index <= L_TXT; ++index)
//	{
//		setLangFontFgColorToDarkStyle((LangType)index);
//	}
//}


//从默认主题模板创建一份用户的语言风格文件
void QtLangSet::createOneLangStyleFromThemes(int styleId, LangType langId, QString tag)
{
	if (langId == L_GLOBAL)
	{
		qDebug("global call");
	}
	//指定从原始模板的主题中读取一份数据
	QsciLexer* pLexer = ScintillaEditView::createLexer(langId, tag, true, styleId);
	if (nullptr != pLexer)
	{
		saveLangeSet(pLexer, styleId);
		delete pLexer;
	}
}

//从原始主题模板中，创建一份用户主题文件
void QtLangSet::createUserStyleFormThemes(int styleId)
{
	for (int index = 0; index <= L_TXT; ++index)
	{
		createOneLangStyleFromThemes(styleId, (LangType)index);
	}
}

//检查当前是否已经存在暗黑色语言配置
//bool QtLangSet::isExistDarkLangSetings()
//{
//	QString cfgPath = QString("notepad/userstyle/black/AllGlobal");
//	QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
//	return QFile::exists(qs.fileName());
//}

//检查当前是否已经存在主题配置
bool QtLangSet::isExistThemesSetings(int styleId)
{
	QString cfgPath = QString("notepad/userstyle/%1/AllGlobal").arg(StyleSet::getStyleName(styleId));
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, cfgPath);
	return QFile::exists(qs.fileName());
}

//当前主题发生变化
void QtLangSet::on_themesChange(int styleIndex)
{
	//第一次启动时，防止初始情况没有初始化主题，故检测一下isExistThemesSetings
	if (m_themesId == styleIndex && isExistThemesSetings(styleIndex))
	{
		return;
	}
	ui.statusBar->showMessage(tr("themes changing, please waiting ..."), 5000);

	m_themesId = styleIndex;

	//这个有个耦合：先要设置QsciLexer的当前主题。只有默认主题才有详细独立的风格。
	//非默认主题，其初始风格全是默认样式。

	QsciLexer::setCurThemes(m_themesId);

	//如果不存在主题设置，则从模板里面拷贝一份出来
	if (!isExistThemesSetings(styleIndex))
	{
		createUserStyleFormThemes(styleIndex);
	}

	StyleSet::setCurrentStyle(styleIndex);

	NddSetting::updataKeyValueFromNumSets(SKIN_KEY, m_themesId);

	//这里就要及时切换当前主题，预览颜色
	updateThemes();

	//下一步就是要更好当前编辑器的语法设置
	previewAllGoblalChange();

	int row = ui.langListWidget->currentRow();

	//手动刷新UI
	QListWidgetItem* styleItem = ui.langListWidget->item(row);

	slot_itemSelect(styleItem);

	m_lastThemesId = m_themesId;

	ui.statusBar->showMessage(tr("themes changed finished ..."), 5000);
}

//更新当前编辑框中主题的样式
void QtLangSet::updateThemes()
{
	CCNotePad* pMainNote = dynamic_cast<CCNotePad*>(parent());
	if (pMainNote != nullptr)
	{
		pMainNote->updateThemes();
	}
}

//全局的前景背景和一般的不一样。
void QtLangSet::setGlobalFgColor(int style, QColor color)
{
	One_Stype_Info* pStyle = &StyleSet::s_global_style->global_style;

	if (pStyle[style].fgColor != color)
	{
		pStyle[style].fgColor = color;
	}
}

void QtLangSet::setGlobalBgColor(int style, QColor color)
{
	One_Stype_Info* pStyle = &StyleSet::s_global_style->global_style;

	if (pStyle[style].bgColor != color)
	{
		pStyle[style].bgColor = color;
	}
}

void QtLangSet::setGlobalFont(int style, QFont font)
{
	One_Stype_Info* pStyle = &StyleSet::s_global_style->global_style;

	if (pStyle[style].font != font)
	{
		pStyle[style].font = font;
	}
}
