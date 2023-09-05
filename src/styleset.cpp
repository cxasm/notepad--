#include "styleset.h"
#include "ccnotepad.h"
#include <QFile>
#include <QPalette>
#include <QApplication>
#include <QDebug>
#include <Qsci/qscilexerglobal.h>


QColor StyleSet::foldfgColor(0xe9, 0xe9, 0xe9, 100);
QColor StyleSet::foldbgColor(0xff, 0xff, 0xff);
QColor StyleSet::marginsBackgroundColor(0xf0f0f0);
QColor StyleSet::bookmarkBkColor(0xececec);

int StyleSet::m_curStyleId = 0;


GLOBAL_STYLE_OPS* StyleSet::s_global_style = nullptr;

StyleSet::StyleSet()
{
	init();
}

StyleSet::~StyleSet()
{}

void StyleSet::init()
{
	if (s_global_style == nullptr)
	{
		s_global_style = new GLOBAL_STYLE_OPS();

		s_global_style->default_style.styleId = 32;
		s_global_style->indent_guideline.styleId = 37;
		s_global_style->brace_highight.styleId = 34;
		s_global_style->bad_brace_color.styleId = 35;
		s_global_style->current_line_background_color.styleId = -1;
		s_global_style->select_text_color.styleId = -1;
		s_global_style->caret_colour.styleId = 2069;
		s_global_style->edge_colour.styleId = -1;
		s_global_style->line_number_margin.styleId = 33;
		s_global_style->bookmark_margin.styleId = -1;
		s_global_style->fold.styleId = -1;
		s_global_style->fold_active.styleId = -1;
		s_global_style->fold_margin.styleId = -1;
		s_global_style->white_space_stybol.styleId = -1;
		s_global_style->smart_highlighting.styleId = 29;
		s_global_style->find_mark_style.styleId = 31;
		s_global_style->mark_style_1.styleId = 25;
		s_global_style->mark_style_2.styleId = 24;
		s_global_style->mark_style_3.styleId = 23;
		s_global_style->mark_style_4.styleId = 22;
		s_global_style->mark_style_5.styleId = 21;
		s_global_style->incremental_highlight.styleId = 28;
		s_global_style->tags_match_highlight.styleId = 27;
		s_global_style->tags_attribute.styleId = 26;
		//s_global_style->active_tab_focused.styleId = -1;
		//s_global_style->active_tab_unfocused.styleId = -1;
		//s_global_style->active_tab_text.styleId = -1;
		//s_global_style->inactive_tabs.styleId = -1;
		s_global_style->url_hoverred.styleId = -1;
	}

}

void StyleSet::setCommonStyle(QColor foldfgColor_, QColor foldbgColor_, QColor marginsBackgroundColor_, QString colorName)
{
	foldfgColor = foldfgColor_;
	foldbgColor = foldbgColor_;
	marginsBackgroundColor = marginsBackgroundColor_;

	QFile file(":/qss/lightbluestyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, foldbgColor_);
		palette.setColor(QPalette::Base, foldbgColor_);
		palette.setColor(QPalette::Button, foldbgColor_);
		qApp->setPalette(palette);

		if (colorName != "#EAF7FF")
		{
			styleSheet.replace("#EAF7FF", colorName);
		}
		qApp->setStyleSheet(styleSheet);
	}
	file.close();
}

void StyleSet::setSkin(int id)
{
	m_curStyleId = id;
	QsciLexer::setCurThemes(m_curStyleId);
	StyleSet::init();
	loadGolbalStyle();

	switch (id)
	{
	case DEFAULT_SE:
		setDefaultStyle();
		break;
	case BESPIN:
	case BLACK_BOARD:
	case BLUE_LIGHT:
	case CHOCO:
	case DANSLE_RUSH_DARK:
	case DEEP_BLACK:
	case LAVENDER:
	case HOT_FUDGE_SUNDAE:
	case MISTY_ROSE:
	case MONO_INDUSTRIAL:
	case MONOKAI:
	case OBSIDIAN:
	case PLASTIC_CODE:
	case RUBY_BLUE:
	case TWILIGHT:
	case VIBRANT_INK:
	case YELLOW_RICE:
		setCommonStyle();
		break;
	case MAX_SE:
		break;
	default:
		break;
	}
}

//重新加载Global全局风格
void StyleSet::reloadGolbalStyleFromSetFile()
{
	loadGolbalStyle();
}

void StyleSet::loadGolbalStyle()
{
	QsciLexer* pLexer = ScintillaEditView::createLexer(L_GLOBAL);
	QsciLexerGlobal* pGlobalLexer = dynamic_cast<QsciLexerGlobal*>(pLexer);
	if (pGlobalLexer != nullptr)
	{
		//获取其属性颜色，然后填入。后续在ScintillaEditView中直接设置这些One_Stype_Info的属性值
		One_Stype_Info* pAddr = &s_global_style->global_style;

		for (int i = 0; i <= URL_HOVERRED; ++i)
		{
			pAddr[i].fgColor = pGlobalLexer->color(i);
			pAddr[i].bgColor = pGlobalLexer->paper(i);
			pAddr[i].font = pGlobalLexer->font(i);
		}
	}
	delete pLexer;
}

QString StyleSet::getCurrentStyle()
{
	return getStyleName(m_curStyleId);
}

//是否深色风格系列
bool StyleSet::isCurrentDeepStyle()
{
	switch (m_curStyleId)
	{
	case DEFAULT_SE:
	case BLUE_LIGHT:
	case YELLOW_RICE:
	case MISTY_ROSE:
	case LAVENDER:
		return false;

	case BESPIN:
	case BLACK_BOARD:
	case CHOCO:
	case DANSLE_RUSH_DARK:
	case DEEP_BLACK:
	case HOT_FUDGE_SUNDAE:
	case MONO_INDUSTRIAL:
	case MONOKAI:
	case OBSIDIAN:
	case PLASTIC_CODE:
	case RUBY_BLUE:
	case TWILIGHT:
	case VIBRANT_INK:
		return true;

	default:
		break;
	}
	return false;
}

QString StyleSet::getStyleName(int styleId)
{
	static const QString style[MAX_SE] = { "Default","Bespin","Black board","Blue light",\
		"Choco","DansLeRuSH-Dark",\
		"Deep Black","lavender","HotFudgeSundae","misty rose",\
		"Mono Industrial","Monokai","Obsidian","Plastic Code Wrap",\
		"Ruby Blue","Twilight","Vibrant Ink",\
	"yellow rice" };

	return style[styleId];
}

void StyleSet::setCurrentStyle(int themes)
{
	setSkin(themes);
}

void StyleSet::setDefaultStyle()
{
	m_curStyleId = DEFAULT_SE;

	foldfgColor = QColor(0xe9, 0xe9, 0xe9, 100);
	foldbgColor = QColor(0xff, 0xff, 0xff);
	marginsBackgroundColor = QColor(0xf0f0f0);
	bookmarkBkColor = QColor(0xececec);

	QFile file(":/qss/mystyle.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xf0, 0xf0, 0xf0));
		palette.setColor(QPalette::Base, QColor(0xff, 0xff, 0xff));
		palette.setColor(QPalette::Button, QColor(0xf0, 0xf0, 0xf0));
		qApp->setPalette(palette);
		qApp->setStyleSheet(styleSheet);
	}
	file.close();
}

void StyleSet::setLightStyle()
{
	//m_curStyleId = LIGHT_SE;
	bookmarkBkColor = QColor(0xE0F3Fc);
	setCommonStyle(QColor(0xea, 0xf7, 0xff, 100), QColor(0xeaf7ff), QColor(0xe8f5fd), "#EAF7FF");
}

void StyleSet::setThinBlueStyle()
{
	//m_curStyleId = THIN_BLUE_SE;
	bookmarkBkColor = QColor(0xE3e0F0);
	setCommonStyle(QColor(0xd7, 0xe3, 0xf4, 100), QColor(0xd7e3f4), QColor(0xd5e1f1), "#D7E3F4");
}

//纸黄
void StyleSet::setThinYellowStyle()
{
	//m_curStyleId = THIN_YELLOW_SE;
	bookmarkBkColor = QColor(0xF4F0E0);
	setCommonStyle(QColor(0xf9, 0xf0, 0xe1, 100), QColor(0xf9f0e1), QColor(0xf7f0e0), "#F9F0E1");
}

//宣纸黄
void StyleSet::setRiceYellowStyle()
{
	//m_curStyleId = RICE_YELLOW_SE;
	bookmarkBkColor = QColor(0xF0F0E8);
	setCommonStyle(QColor(0xf6, 0xf3, 0xea, 100), QColor(0xf6f3ea), QColor(0xf4f1e9), "#F6F3EA");
}

//银色
void StyleSet::setSilverStyle()
{
	//m_curStyleId = SILVER_SE;
	bookmarkBkColor = QColor(0xE4E4E4);
	setCommonStyle(QColor(0xe9, 0xe8, 0xe4, 100), QColor(0xe9e8e4), QColor(0xe7e6e2), "#E9E8E4");
}

//谈紫色#FFF0F5
void StyleSet::setLavenderBlushStyle()
{
	//m_curStyleId = LAVENDER_SE;
	bookmarkBkColor = QColor(0xFCF0F0);
	setCommonStyle(QColor(0xff, 0xf0, 0xf5, 100), QColor(0xFFF0F5), QColor(0xFdF0F3), "#FFF0F5");
}

//MistyRose
void StyleSet::setMistyRoseStyle()
{
	//m_curStyleId = MISTYROSE_SE;
	bookmarkBkColor = QColor(0xFCE0E0);
	setCommonStyle(QColor(0xff, 0xe4, 0xe1, 100), QColor(0xFFE4E1), QColor(0xFdE2E0), "#FFE4E1");
}

void StyleSet::setBlackStyle()
{
#if 0
	m_curStyleId = BLACK_SE;
	foldfgColor = QColor(0,0, 0);
	foldbgColor = QColor(32, 32, 40);
	
	//注意暗黑模式是单独的窗口背景颜色。上面的foldbgColor决定了编辑器的背景颜色。
	//暗黑模式窗体背景不完全是黑色，而是686868
	QColor blackPaletteColor(68,68,68);

	marginsBackgroundColor = QColor(57, 58, 60);
	bookmarkBkColor = QColor(53,54,56);

	QFile file(":/qss/black.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		QPalette palette;
		palette.setColor(QPalette::Window, blackPaletteColor);
		palette.setColor(QPalette::Base, blackPaletteColor);
		palette.setColor(QPalette::Button, blackPaletteColor);

		qApp->setPalette(palette);

		styleSheet = file.readAll();

		qApp->setStyleSheet(styleSheet);
	}
	file.close();
#endif

	m_curStyleId = BLACK_SE;

	foldfgColor = QColor(0xe9, 0xe9, 0xe9, 100);
	foldbgColor = QColor(0xff, 0xff, 0xff);
	marginsBackgroundColor = QColor(57, 58, 60);
	bookmarkBkColor = QColor(53, 54, 56);

	QFile file(":/qss/myblack.qss"); //qss文件路径:/lightblue.qss
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xf0, 0xf0, 0xf0));
		palette.setColor(QPalette::Base, QColor(0xff, 0xff, 0xff));
		palette.setColor(QPalette::Button, QColor(0xf0, 0xf0, 0xf0));
		qApp->setPalette(palette);
		qApp->setStyleSheet(styleSheet);
	}
	file.close();
}
void StyleSet::setCommonStyle()
{
	QFile file(":/qss/common.qss");
	QString styleSheet;
	if (file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		styleSheet = file.readAll();
		styleSheet.replace("#ffffff", s_global_style->default_style.fgColor.name());
		styleSheet.replace("#444444", s_global_style->default_style.bgColor.name());
		if (isCurrentDeepStyle())
		{
			styleSheet.replace("#00CCFF", "#0000ff");
		}
		
		QPalette palette;
		palette.setColor(QPalette::Window, QColor(0xf0, 0xf0, 0xf0));
		palette.setColor(QPalette::Base, QColor(0xff, 0xff, 0xff));
		palette.setColor(QPalette::Button, QColor(0xf0, 0xf0, 0xf0));
		qApp->setPalette(palette);
		qApp->setStyleSheet(styleSheet);
	}
	file.close();
}

void StyleSet::setGlobalFgColor(int style, QColor color)
{
	One_Stype_Info* pStyle = &StyleSet::s_global_style->global_style;

	if (pStyle[style].fgColor != color)
	{
		pStyle[style].fgColor = color;
	}
}

void StyleSet::setGlobalBgColor(int style, QColor color)
{
	One_Stype_Info* pStyle = &StyleSet::s_global_style->global_style;

	if (pStyle[style].bgColor != color)
	{
		pStyle[style].bgColor = color;
	}
}

void StyleSet::setGlobalFont(int style, QFont font)
{
	One_Stype_Info* pStyle = &StyleSet::s_global_style->global_style;

	if (pStyle[style].font != font)
	{
		pStyle[style].font = font;
	}
}
