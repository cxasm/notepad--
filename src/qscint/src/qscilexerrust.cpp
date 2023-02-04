#include "Qsci/qscilexerrust.h"
#include "Qsci/qsciscintillabase.h"

#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>

QsciLexerRust::QsciLexerRust(QObject *parent)
	: QsciLexer(parent)
{
	m_commentSymbol = "//";
	m_commentStart = "/*";
	m_commentEnd = "*/";
}

QsciLexerRust::~QsciLexerRust()
{
}

// Returns the language name.
const char* QsciLexerRust::language() const
{
	return "RUST";
}

// Returns the lexer name.
const char *QsciLexerRust::lexer() const
{
	return "rust";
}

// Returns the set of keywords.
const char* QsciLexerRust::keywords(int set)
{
	return "as break const continue crate dyn else enum extern "
		"false fn for if impl in let loop match mod move mut "
		"pub ref return Self self static struct super trait "
		"true type unsafe use where while";
}

// Returns the foreground colour of the text for a style.
QColor QsciLexerRust::defaultColor(int style) const
{
	//if (style == VerbatimString)
	//	return QColor(0x00, 0x7f, 0x00);
	switch (style)
	{
		case RUST_WORD:
		case RUST_WORD2:
		case RUST_WORD3:
		case RUST_WORD4:
		case RUST_WORD5:
		case RUST_WORD6:
		case RUST_WORD7:
			return QColor(0x00, 0x00, 0xff);
	default:
		break;
	}
	return QsciLexer::defaultColor(style);
}

// Returns the font of the text for a style.
QFont QsciLexerRust::defaultFont(int style) const
{
	return QsciLexer::defaultFont(style);
}

// Returns the user name of a style.
QString QsciLexerRust::description(int style) const
{
	switch (style)
	{
	case RUST_DEFAULT:
		return "Default";
	case RUST_COMMENTBLOCK:
		return "comment";
	case RUST_COMMENTLINE:
		return "commentline";
	case RUST_COMMENTBLOCKDOC:
		return "commentblockdoc";
	case RUST_COMMENTLINEDOC:
		return "commentlinedoc";
	case RUST_NUMBER:
		return "number";
	case RUST_WORD:
		return "word";
	case RUST_WORD2:
		return "word2";
	case RUST_WORD3:
		return "word3";
	case RUST_WORD4:
		return "word4";
	case RUST_WORD5:
		return "word5";
	case RUST_WORD6:
		return "word6";
	case RUST_WORD7:
		return "word7";
	case RUST_STRING:
		return "string";
	case RUST_STRINGR:
		return "stringr";
	case RUST_CHARACTER:
		return "character";
	case RUST_OPERATOR:
		return "operator";
	case RUST_IDENTIFIER:
		return "identifier";
	case RUST_LIFETIME:
		return "lifetime";
	case RUST_MACRO:
		return "macro";
	case RUST_LEXERROR:
		return "lexerror";
	case RUST_BYTESTRING:
		return "bytestring";
	case RUST_BYTESTRINGR:
		return "bytestringr";
	case RUST_BYTECHARACTER:
		return "bytecharacter";
	default:
		break;
	}
	return QString();
}


// Returns the background colour of the text for a style.
QColor QsciLexerRust::defaultPaper(int style) const
{
	return QsciLexer::defaultPaper(style);
}

// Return the lexer identifier.
int QsciLexerRust::lexerId() const
{
	return L_RUST;
}
