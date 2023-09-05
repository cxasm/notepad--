#pragma once

#pragma once
#include <QObject>
#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexer.h>

enum VB_STYLE {
	VB_DEFAULT = 0,
	VB_COMMENT,
	VB_NUMBER,
	VB_KEYWORD,
	VB_STRING,
	VB_PREPROCESSOR,
	VB_OPERATOR,
	VB_IDENTIFIER,
	VB_DATE,
	VB_STRINGEOL,
	VB_KEYWORD2,
	VB_KEYWORD3,
	VB_KEYWORD4,
	VB_CONSTANT,
	VB_ASM,
	VB_LABEL,
	VB_ERROR,
	VB_HEXNUMBER,
	VB_BINNUMBER,
	VB_COMMENTBLOCK,
	VB_DOCLINE,
	VB_DOCBLOCK,
	VB_DOCKEYWORD,
	VB_PROPS_DEFAULT,
};

class QSCINTILLA_EXPORT QsciLexerVB  : public QsciLexer
{
	Q_OBJECT

public:
	QsciLexerVB(QObject *parent=0);
	virtual ~QsciLexerVB();

	//! Returns the name of the language.
	const char* language() const;

	const char * lexer() const;

	//! Returns the background colour of the text for style number \a style.
	//!
	//! \sa defaultColor()
	QColor defaultPaper(int style) const;


	//! Returns the set of keywords for the keyword set \a set recognised
	//! by the lexer as a space separated string.
	const char* keywords(int set);

	QColor defaultColor(int style) const;

	QFont defaultFont(int style) const;

	//! Returns the descriptive name for style number \a style.  If the
	//! style is invalid for this language then an empty QString is returned.
	//! This is intended to be used in user preference dialogs.
	QString description(int style) const;

private:
	QsciLexerVB(const QsciLexerVB&);
	QsciLexerVB& operator=(const QsciLexerVB&);
};
