#include "Qsci/qscilexervb.h"

QsciLexerVB::QsciLexerVB(QObject *parent)
	: QsciLexer(parent)
{
	setLexerId(L_VB);
	m_commentSymbol = "'";
}

QsciLexerVB::~QsciLexerVB()
{}

const char * QsciLexerVB::language() const
{
	return "VB";
}

const char * QsciLexerVB::lexer() const
{
	return "vb";
}

QColor QsciLexerVB::defaultPaper(int style) const
{
	return QsciLexer::defaultPaper(style);
}

const char * QsciLexerVB::keywords(int set)
{
	return "and beep begin boolean byte cbool cbyte ccur cdate cdbl cdec "
		"cint clng csng cstr cverr cvar call case choose clear "
		"command const currency date deftype dim do doevents double "
		"each else end environ eqv erase err error exit explicit fix "
		"for function get gosub goto if imp int integer is isarray isdate "
		"isempty iserror ismissing isnull isnumeric isobject lbound let like "
		"long loop me mod module new next not object on option or private "
		"property public raise redim resume return select sendkeys shell single "
		"static stop string sub switch then typename ubound vartype variant wend "
		"while with xor";
}

QColor QsciLexerVB::defaultColor(int style) const
{
	switch (style)
	{
	case VB_KEYWORD:
	case VB_KEYWORD2:
	case VB_KEYWORD3:
	case VB_KEYWORD4:
	case VB_DOCKEYWORD:
		return QColor(0x00, 0x00, 0xff);
	case VB_NUMBER:
		return QColor(0x00, 0x7f, 0x7f);
	case VB_STRING:
		return QColor(0x7f, 0x00, 0x7f);
	case VB_OPERATOR:
		return QColor(0xff, 0x00, 0x00);
	default:
		break;
	}
	return QsciLexer::defaultColor(style);
}

QFont QsciLexerVB::defaultFont(int style) const
{
	switch (style)
	{
	case VB_KEYWORD:
	case VB_KEYWORD2:
	case VB_KEYWORD3:
	case VB_KEYWORD4:
	case VB_DOCKEYWORD:
	{
		QFont f = QsciLexer::defaultFont(style);
		f.setBold(true);
		return f;
	}
	default:
		break;
	}
	return QsciLexer::defaultFont(style);
}

QString QsciLexerVB::description(int style) const
{
	switch (style)
	{
	case VB_DEFAULT:
		return "Default";
	case VB_COMMENT:
		return "comment";
	case VB_NUMBER:
		return "num";
	case VB_KEYWORD:
		return "keyword";
	case VB_STRING:
		return "string";
	case VB_PREPROCESSOR:
		return "preprocessor";
	case VB_OPERATOR:
		return "operator";
	case VB_IDENTIFIER:
		return "identifier";
	case VB_DATE:
		return "date";
	case VB_STRINGEOL:
		return "stringeol";
	case VB_KEYWORD2:
		return "keyword2";
	case VB_KEYWORD3:
		return "keyword3";
	case VB_KEYWORD4:
		return "keyword4";
	case VB_CONSTANT:
		return "constant";
	case VB_ASM:
		return "asm";
	case VB_LABEL:
		return "label";
	case VB_ERROR:
		return "error";
	case VB_HEXNUMBER:
		return "hexnumber";
	case VB_BINNUMBER:
		return "binnumber";
	case VB_COMMENTBLOCK:
		return "commentblock";
	case VB_DOCLINE:
		return "docline";
	case VB_DOCBLOCK:
		return "docblock";
	case VB_DOCKEYWORD:
		return "dockeyword";
	case VB_PROPS_DEFAULT:
		return "propdefault";
	default:
		break;
	}
	return QString();
}
