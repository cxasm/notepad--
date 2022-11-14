#include "Qsci/qscilexertext.h"
#include "Qsci/qsciscintillabase.h"

#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>

#if defined(Q_OS_WIN)
QFont QsciLexerText::s_defaultTxtFont(u8"宋体", QsciLexer::s_defaultFontSize);
#elif defined(Q_OS_MAC)
    QFont QsciLexerText::s_defaultTxtFont("STSong",18);
#else
        QFont QsciLexerText::s_defaultTxtFont(u8"CESI宋体-GB2312",12);
#endif


QsciLexerText::QsciLexerText(QObject *parent)
	: QsciLexer(parent)
{
	
}

QsciLexerText::~QsciLexerText()
{
}

// Returns the language name.
const char* QsciLexerText::language() const
{
	return "TXT";
}

// Returns the lexer name.
const char *QsciLexerText::lexer() const
{
	return "txt";
}


// Returns the foreground colour of the text for a style.
QColor QsciLexerText::defaultColor(int style) const
{
	//if (style == VerbatimString)
	//	return QColor(0x00, 0x7f, 0x00);
	if (style == Keyword)
	{
		return QColor(0x00, 0x00, 0xff);
	}

	return QsciLexer::defaultColor(style);
}


// Returns the end-of-line fill for a style.
bool QsciLexerText::defaultEolFill(int style) const
{
	//if (style == VerbatimString)
	//	return true;

	return QsciLexer::defaultEolFill(style);
}

void QsciLexerText::setGlobalDefaultFont(const QFont & font)
{
	s_defaultTxtFont = font;
}

// Returns the font of the text for a style.
QFont QsciLexerText::defaultFont(int style) const
{
	switch (style)
	{
	case Default:
#if defined(Q_OS_WIN)
		return s_defaultTxtFont; // QFont("Microsoft YaHei", QsciLexer::s_defaultFontSize);
#elif defined(Q_OS_MAC)
		//return QFont("Courier", 12);
		return s_defaultTxtFont;
#else
		return s_defaultTxtFont;//sQFont("Bitstream Vera Sans Mono", 9);
#endif
		break;
	case Ascii:
#if defined(Q_OS_WIN)
		return QFont("Courier New", 14);
#elif defined(Q_OS_MAC)
		return QFont("Courier New", 18);
#else
		return s_defaultTxtFont;
#endif
		break;
	case Keyword:
#if defined(Q_OS_WIN)
	{
		QFont f("Courier New", 14);
		f.setBold(true);
		return f;
	}
#elif defined(Q_OS_MAC)
	{
		QFont f("Courier New", 18);
		f.setBold(true);
		return f;
	}
#else
	{
		QFont f(s_defaultTxtFont);
		f.setBold(true);
		return f;
	}
#endif
			break;
	default:
		break;
}
	return s_defaultTxtFont;
}


// Returns the set of keywords.
const char* QsciLexerText::keywords(int set)
{
	if (m_isUserDefineKeyword)
	{
		//如果是自定义用户关键字，则根据语言tag获取。只实现set=1的一组
		if (set == 1)
		{
			return getUserDefineKeywords();
		}
	}
	return 0;
}


// Returns the user name of a style.
QString QsciLexerText::description(int style) const
{
	switch (style)
	{
	case Default:
		return tr("Chinese And Others");
		break;
	case Ascii:
		return tr("Ascii");
		break;
	case Keyword:
		if (m_isUserDefineKeyword)
		{
			return tr("Keyword");
		}
		break;
	default:
		break;
	}
	return QString();
}


// Returns the background colour of the text for a style.
QColor QsciLexerText::defaultPaper(int style) const
{
	//if (style == VerbatimString)
	//	return QColor(0xe0, 0xff, 0xe0);

	return QsciLexer::defaultPaper(style);
}

// Return the lexer identifier.
int QsciLexerText::lexerId() const
{
	return L_TXT;
}
