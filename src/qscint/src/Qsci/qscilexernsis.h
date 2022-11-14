#pragma once
#include <QObject>
#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexer.h>

class QSCINTILLA_EXPORT QsciLexerNsis : public QsciLexer
{
	Q_OBJECT

public:
	enum {
		NSIS_DEFAULT=0,
		NSIS_COMMENT,
		NSIS_STRINGDQ,
		NSIS_STRINGLQ,
		NSIS_STRINGRQ,
		NSIS_FUNCTION,
		NSIS_VARIABLE,
		NSIS_LABEL,
		NSIS_USERDEFINED,
		NSIS_SECTIONDEF,
		NSIS_SUBSECTIONDEF,
		NSIS_IFDEFINEDEF,
		NSIS_MACRODEF,
		NSIS_STRINGVAR,
		NSIS_NUMBER,
		NSIS_SECTIONGROUP,
		NSIS_PAGEEX,
		NSIS_FUNCTIONDEF,
		NSIS_COMMENTBOX,
	};
	QsciLexerNsis(QObject *parent=0);
	virtual ~QsciLexerNsis();

	//! Returns the name of the language.
	const char* language() const;

	const char * lexer() const;

	//! Returns the foreground colour of the text for style number \a style.
	//!
	//! \sa defaultPaper()
	QColor defaultColor(int style) const;

	//! Returns the end-of-line fill for style number \a style.
	bool defaultEolFill(int style) const;

	//! Returns the font for style number \a style.
	QFont defaultFont(int style) const;

	//! Returns the background colour of the text for style number \a style.
	//!
	//! \sa defaultColor()
	QColor defaultPaper(int style) const;

	int lexerId() const;

	//! Returns the set of keywords for the keyword set \a set recognised
	//! by the lexer as a space separated string.
	const char* keywords(int set);

	//! Returns the descriptive name for style number \a style.  If the
	//! style is invalid for this language then an empty QString is returned.
	//! This is intended to be used in user preference dialogs.
	QString description(int style) const;

private:
	QsciLexerNsis(const QsciLexerNsis&);
	QsciLexerNsis& operator=(const QsciLexerNsis&);

};
