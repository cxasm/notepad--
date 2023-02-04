#pragma once
#include <QObject>
#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexer.h>

enum RUST_STYLE{
	RUST_DEFAULT=0,
	RUST_COMMENTBLOCK,
	RUST_COMMENTLINE,
	RUST_COMMENTBLOCKDOC,
	RUST_COMMENTLINEDOC,
	RUST_NUMBER,
	RUST_WORD,
	RUST_WORD2,
	RUST_WORD3,
	RUST_WORD4,
	RUST_WORD5,
	RUST_WORD6,
	RUST_WORD7,
	RUST_STRING,
	RUST_STRINGR,
	RUST_CHARACTER,
	RUST_OPERATOR,
	RUST_IDENTIFIER,
	RUST_LIFETIME,
	RUST_MACRO,
	RUST_LEXERROR,
	RUST_BYTESTRING,
	RUST_BYTESTRINGR,
	RUST_BYTECHARACTER,
};
class QSCINTILLA_EXPORT QsciLexerRust : public QsciLexer
{
	Q_OBJECT

public:
	//enum {
	//	Default = 0,//中文
	//	Ascii = 1,//英文
	//	Keyword = 2, //关键字，只有以TXT为母版的自定义语言才有，默认txt是没有关键字说法的
	//};
	QsciLexerRust(QObject *parent=0);
	virtual ~QsciLexerRust();

	//! Returns the name of the language.
	const char* language() const;

	const char * lexer() const;

	//! Returns the background colour of the text for style number \a style.
	//!
	//! \sa defaultColor()
	QColor defaultPaper(int style) const;

	int lexerId() const;

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
	QsciLexerRust(const QsciLexerRust&);
	QsciLexerRust& operator=(const QsciLexerRust&);
};
