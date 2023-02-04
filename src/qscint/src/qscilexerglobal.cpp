#include "Qsci/qscilexerglobal.h"
#include "Qsci/qsciscintilla.h"
#include "Scintilla.h"
#include "SciLexer.h"

//golbal是一个特殊的语法编辑器，他不影响某一种具体的语法。而是对所有编辑器lexer以外的风格进行表示

QsciLexerGlobal::QsciLexerGlobal(QObject *parent)
	: QsciLexer(parent)
{}

QsciLexerGlobal::~QsciLexerGlobal()
{}

int QsciLexerGlobal::lexerId() const
{
	return L_GLOBAL;
}

// Returns the language name.
const char* QsciLexerGlobal::language() const
{
	return "Global";
}

QString QsciLexerGlobal::description(int style) const
{
	switch (style)
	{
	case GLOBAL_OVERRIDE:
		return tr("Global override");
	case DEFAULT_STYLE:
		return tr("Default");
	case INDENT_GUIDELINE:
		return tr("Indent guideline style");
	case BRACE_HIGHIGHT:
		return tr("Brace highlight style");
	case BAD_BRACE_COLOUR:
		return tr("Bad brace colour");
	case CURRENT_LINE_BACKGROUND_COLOR:
		return  tr("Current line background colour");
	case SELECT_TEXT_COLOR:
		return tr("Select text colour");
	case CARET_COLOUR:
		return tr("Caret colour");
	case EDGE_COLOUR:
		return tr("Edge colour");
	case LINE_NUMBER_MARGIN:
		return tr("Line number margin");
	case BOOKMARK_MARGIN:
		return tr("Bookmark margin");
	case FOLD:
		return tr("Fold");
	case FOLD_ACTIVE:
		return tr("Fold active");
	case FOLD_MARGIN:
		return tr("Fold margin");
	case WHITE_SPACE_SYMBOL:
		return tr("White space symbol");
	case SMART_HIGHLIGHTING:
		return tr("Smart HighLighting");
	case FIND_MARK_STYLE:
		return tr("Find Mark Style");
	case MARK_STYLE_1:
		return tr("Mark Style 1");
	case MARK_STYLE_2:
		return tr("Mark Style 2");
	case MARK_STYLE_3:
		return tr("Mark Style 3");
	case MARK_STYLE_4:
		return tr("Mark Style 4");
	case MARK_STYLE_5:
		return tr("Mark Style 5");
	case INCREMENTAL_HIGHLIGHT:
		return tr("Incremental highlight");
	case TAGS_MATCH_HIGHLIGHT:
		return tr("Tags match highlight");
	case TAGS_ATTRIBUTE:
		return tr("Tags attribute");
	/*case ACTIVE_TAB_FOCUSED:
		return tr("Active tab focused");
	case ACTIVE_TAB_UNFOCUSED:
		return tr("Active tab unfocused indicator");
	case ACTIVE_TAB_TEXT:
		return tr("Active tab text");
	case INACTIVE_TABS:
		return tr("Inactive tabs");*/
	case URL_HOVERRED:
		return tr("URL hovered");
	default:
		break;
	}

	return QString();
}

//字体大小。对于DEFAULT_STYLE BRACE_HIGHIGHT BAD_BRACE_COLOUR LINE_NUMBER_MARGIN
//四个特殊属性而言，他们的字体大小，是用来表示符号的大小的。

QFont QsciLexerGlobal::defaultFont(int style) const
{
#if defined(Q_OS_WIN)
	QFont f("Courier New", s_defaultFontSize);
#elif defined(Q_OS_MAC)
	QFont f("Menlo", s_defaultFontSize);
#else
	QFont f("Courier 10 Pitch", s_defaultFontSize);
#endif

	switch (style)
	{

	case INDENT_GUIDELINE:
	case CURRENT_LINE_BACKGROUND_COLOR:
	case SELECT_TEXT_COLOR:
	case CARET_COLOUR:
	case EDGE_COLOUR:
	case BOOKMARK_MARGIN:
	case FOLD:
	case FOLD_ACTIVE:
	case FOLD_MARGIN:
	case WHITE_SPACE_SYMBOL:
	case SMART_HIGHLIGHTING:
	case FIND_MARK_STYLE:
	case MARK_STYLE_1:
	case MARK_STYLE_2:
	case MARK_STYLE_3:
	case MARK_STYLE_4:
	case MARK_STYLE_5:
	case INCREMENTAL_HIGHLIGHT:
	case TAGS_MATCH_HIGHLIGHT:
	case TAGS_ATTRIBUTE:
	/*case ACTIVE_TAB_FOCUSED:
	case ACTIVE_TAB_UNFOCUSED:
	case ACTIVE_TAB_TEXT:
	case INACTIVE_TABS:*/
	case URL_HOVERRED:
		//这些都是不能设置字体的，统一设置为空
		f.setFamily("");
		f.setPointSize(-1);
		break;

	case GLOBAL_OVERRIDE:
	case BAD_BRACE_COLOUR:
	case DEFAULT_STYLE:
	case BRACE_HIGHIGHT:
		return f;

	case LINE_NUMBER_MARGIN:
		//这个对应STYLE_LINENUMBER的默认大小为10
		f.setPointSize(12);
		return f;
	default:
		break;
	}
	return QsciLexer::s_defaultLangFont;
}


// Returns the foreground colour of the text for a style.
QColor QsciLexerGlobal::defaultColor(int style) const
{
	switch (style)
	{
	case GLOBAL_OVERRIDE:
		return QColor(0xFFFF80);

	case DEFAULT_STYLE:
		return QColor(Qt::black);

	case INDENT_GUIDELINE:
		return QColor(0xC0C0C0);

	case BRACE_HIGHIGHT:
		return QColor(0xFF0000);

	case BAD_BRACE_COLOUR:
		return QColor(0x800000);

	case CURRENT_LINE_BACKGROUND_COLOR:
		return QColor(0x0080C0);

	case SELECT_TEXT_COLOR:
		return QColor(Qt::black);

	case CARET_COLOUR:
		return QColor(0x8000FF);

	case EDGE_COLOUR:
		return QColor(0x80FFFF);

	case LINE_NUMBER_MARGIN:
		return QColor(0x808080);

	case BOOKMARK_MARGIN:
		return QColor(0xEEEEEC);

	case FOLD:
		return QColor(0x808080);

	case FOLD_ACTIVE:
		return QColor(0xFF0000);

	case FOLD_MARGIN:
		return QColor(0xFFFFFF);

	case WHITE_SPACE_SYMBOL:
		return QColor(0xFFB56A);

	case SMART_HIGHLIGHTING:
		return QColor(0x00ff00);

	case FIND_MARK_STYLE:
		return QColor(0xFCAF3E);

	case MARK_STYLE_1:
		return QColor(0x555753);

	case MARK_STYLE_2:
		return QColor(0xFCAF3E);

	case MARK_STYLE_3:
		return QColor(0xFAAA3C);

	case MARK_STYLE_4:
		return QColor(0xFFCAB0);

	case MARK_STYLE_5:
		return QColor(Qt::black);

	case INCREMENTAL_HIGHLIGHT:
		return QColor(0x808080);

	case TAGS_MATCH_HIGHLIGHT:
		return QColor(0xFFCAB0);

	case TAGS_ATTRIBUTE:
		return QColor(0xFFCAB0);

	/*case ACTIVE_TAB_FOCUSED:
		return QColor(0xFAAA3C);

	case ACTIVE_TAB_UNFOCUSED:
		return QColor(0xFFCAB0);

	case ACTIVE_TAB_TEXT:
		return QColor(Qt::black);

	case INACTIVE_TABS:
		return QColor(0x808080);*/

	case URL_HOVERRED:
		return QColor(0xFFFF80);

	default:
		break;
	}

	return QsciLexer::defaultColor(style);
}


//global其实不是一个风格，它里面的风格是约束编辑器其他细节设置的。但是保存又是安装内部的styleid来进行的。
//故提供该函数，把内部风格转换为外部的风格id功能。没有则为-2
int QsciLexerGlobal::getRealStyleId(int style)
{
	switch (style)
	{
		//可设置前景、背景、字体
	case DEFAULT_STYLE:
		return STYLE_DEFAULT;

	case INDENT_GUIDELINE:
		return STYLE_INDENTGUIDE;

	case BRACE_HIGHIGHT:
		return STYLE_BRACELIGHT;

	case BAD_BRACE_COLOUR:
		return STYLE_BRACEBAD;

	case CARET_COLOUR:
		return SCI_SETCARETFORE;

	case LINE_NUMBER_MARGIN:
		return STYLE_LINENUMBER;

	case SMART_HIGHLIGHTING:
		return SCE_UNIVERSAL_FOUND_STYLE_SMART;

	case FIND_MARK_STYLE:
		return SCE_UNIVERSAL_FOUND_STYLE;

	case MARK_STYLE_1:
		return SCE_UNIVERSAL_FOUND_STYLE_EXT1;

	case MARK_STYLE_2:
		return SCE_UNIVERSAL_FOUND_STYLE_EXT2;

	case MARK_STYLE_3:
		return SCE_UNIVERSAL_FOUND_STYLE_EXT3;

	case MARK_STYLE_4:
		return SCE_UNIVERSAL_FOUND_STYLE_EXT4;

	case MARK_STYLE_5:
		return SCE_UNIVERSAL_FOUND_STYLE_EXT5;

	case INCREMENTAL_HIGHLIGHT:
		return SCE_UNIVERSAL_FOUND_STYLE_INC;

	case TAGS_MATCH_HIGHLIGHT:
		return SCE_UNIVERSAL_TAGMATCH;

	case TAGS_ATTRIBUTE:
		return SCE_UNIVERSAL_TAGATTR;

	default:
		break;
	}

	//-1是表示所以，已经被占用，所以使用-2
	return -2;
}

QColor QsciLexerGlobal::defaultPaper(int style) const
{
	switch (style)
	{
	case GLOBAL_OVERRIDE:
		return QColor(0xFF8000);

	case DEFAULT_STYLE:
		return QColor(0xFFFFFF);

	case INDENT_GUIDELINE:
		return QColor(0xFFFFFF);

	case BRACE_HIGHIGHT:
		return QColor(0xFFFFFF);

	case BAD_BRACE_COLOUR:
		return QColor(0xFFFFFF);

	case CURRENT_LINE_BACKGROUND_COLOR:
		return QColor(0xE8E8FF);

	case SELECT_TEXT_COLOR:
		return QColor(0xC0C0C0);

	case CARET_COLOUR:
		return QColor(0x919994);
			
	case EDGE_COLOUR:
		return QColor(0x112435);

	case LINE_NUMBER_MARGIN:
		return QColor(0xE4E4E4);

	case BOOKMARK_MARGIN:
		return QColor(0xE0E0E0);

	case FOLD:
		return QColor(0xF3F3F3);

	case FOLD_ACTIVE:
		return QColor(0x2E3436);

	case FOLD_MARGIN:
		return QColor(0xE9E9E9);

	case WHITE_SPACE_SYMBOL:
		return QColor(0x80FF00);

	case SMART_HIGHLIGHTING:
		return QColor(0x00FF00);

	case FIND_MARK_STYLE:
		return QColor(0xFF0000);

	case MARK_STYLE_1:
		return QColor(0x00FFFF);

	case MARK_STYLE_2:
		return QColor(0xFF8000);

	case MARK_STYLE_3:
		return QColor(0xFFFF00);

	case MARK_STYLE_4:
		return QColor(0x8000FF);

	case MARK_STYLE_5:
		return QColor(0x008000);

	case INCREMENTAL_HIGHLIGHT:
		return QColor(0x0080FF);

	case TAGS_MATCH_HIGHLIGHT:
		return QColor(0x8000FF);

	case TAGS_ATTRIBUTE:
		return QColor(0xFFFF00);

	//case ACTIVE_TAB_FOCUSED:
	//	return QColor(0x8000FF);

	//case ACTIVE_TAB_UNFOCUSED:
	//	return QColor(0xFFFF00);

	//case ACTIVE_TAB_TEXT:
	//	return QColor(0xFF0000);

	//case INACTIVE_TABS:
	//	return QColor(0xC0C0C0);

	case URL_HOVERRED:
		return QColor(0xC0C0C0);

	default:
		break;
	}

	return QsciLexer::defaultPaper(style);
}
//enum CHANGE_OPER_BIT {
//	FG_BIT = 1,
//	BG_BIT = 2,
//	FONT_BIT = 4,
//	SIZE_BIT = 8,
//};

//全局样式哪些位可以修改。在设置的界面，把不能修改的界面回调
int QsciLexerGlobal::changeOperBit(int style) const
{
	switch (style)
	{
	case GLOBAL_OVERRIDE:
		return ALL_OPER_BIT;

	case DEFAULT_STYLE:
		return ALL_OPER_BIT;

	case INDENT_GUIDELINE:
		return FG_BIT | BG_BIT;

	case BRACE_HIGHIGHT:
		return ALL_OPER_BIT;

	case BAD_BRACE_COLOUR:
		return ALL_OPER_BIT;

	case CURRENT_LINE_BACKGROUND_COLOR:
		return BG_BIT;

	case SELECT_TEXT_COLOR:
		return FG_BIT | BG_BIT;

	case CARET_COLOUR:
		return FG_BIT;

	case EDGE_COLOUR:
		return FG_BIT;

	case LINE_NUMBER_MARGIN:
		return ALL_OPER_BIT;

	case BOOKMARK_MARGIN:
		return BG_BIT;

	case FOLD:
	case FOLD_ACTIVE:
	case FOLD_MARGIN:
		return FG_BIT | BG_BIT;

	case WHITE_SPACE_SYMBOL:
		return FG_BIT;

	case SMART_HIGHLIGHTING:
		return FG_BIT;

	case FIND_MARK_STYLE:
		return FG_BIT;

	case MARK_STYLE_1:
	case MARK_STYLE_2:
	case MARK_STYLE_3:
	case MARK_STYLE_4:
	case MARK_STYLE_5:
		return BG_BIT;

	case INCREMENTAL_HIGHLIGHT:
		break;
	case TAGS_MATCH_HIGHLIGHT:
		return FG_BIT;
	case TAGS_ATTRIBUTE:
		return FG_BIT;
	case URL_HOVERRED:
		return FG_BIT;

	default:
		break;
	}
	return 0;
}
