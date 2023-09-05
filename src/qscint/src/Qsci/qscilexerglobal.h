#pragma once
#include <QObject>
#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexer.h>


class QsciScintilla;
class QsciStyle;

enum GLOBAL_STYLES {
	GLOBAL_OVERRIDE=0,
	DEFAULT_STYLE,
	INDENT_GUIDELINE,
	BRACE_HIGHIGHT,
	BAD_BRACE_COLOUR,
	CURRENT_LINE_BACKGROUND_COLOR,
	SELECT_TEXT_COLOR,
	CARET_COLOUR,
	EDGE_COLOUR,
	LINE_NUMBER_MARGIN,
	BOOKMARK_MARGIN,
	FOLD,
	FOLD_ACTIVE,
	FOLD_MARGIN,
	WHITE_SPACE_SYMBOL,
	SMART_HIGHLIGHTING,
	FIND_MARK_STYLE,
	MARK_STYLE_1,
	MARK_STYLE_2,
	MARK_STYLE_3,
	MARK_STYLE_4,
	MARK_STYLE_5,
	INCREMENTAL_HIGHLIGHT,
	TAGS_MATCH_HIGHLIGHT,
	TAGS_ATTRIBUTE,
	//ACTIVE_TAB_FOCUSED,
	//ACTIVE_TAB_UNFOCUSED,
	//ACTIVE_TAB_TEXT,
	//INACTIVE_TABS,
	URL_HOVERRED,
};

enum CHANGE_OPER_BIT {
	FG_BIT = 0x1,
	BG_BIT = 0x2,
	FONT_BIT = 0x4,
	SIZE_BIT = 0x8,
	ALL_OPER_BIT = 0xf,
};

class QSCINTILLA_EXPORT QsciLexerGlobal  : public QsciLexer
{
	Q_OBJECT

public:
	QsciLexerGlobal(QObject *parent=0);
	virtual ~QsciLexerGlobal();

	int lexerId() const;
	const char* language() const;
	QString description(int style) const;
	QFont defaultFont(int style) const;
	virtual QColor defaultColor(int style) const;
	virtual QColor defaultPaper(int style) const;

	int changeOperBit(int style) const;

	static int getRealStyleId(int style);
};
