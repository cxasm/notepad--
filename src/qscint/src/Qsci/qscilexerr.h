#pragma once
#pragma once
// This defines the interface to the QsciLexerR class.
//
// Copyright (c) 2021 Riverbank Computing Limited <info@riverbankcomputing.com>
//
// This file is part of QScintilla.
//
// This file may be used under the terms of the GNU General Public License
// version 3.0 as published by the Free Software Foundation and appearing in
// the file LICENSE included in the packaging of this file.  Please review the
// following information to ensure the GNU General Public License version 3.0
// requirements will be met: http://www.gnu.org/copyleft/gpl.html.
//
// If you do not wish to use this file under the terms of the GPL version 3.0
// then you may purchase a commercial license.  For more information contact
// info@riverbankcomputing.com.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#include <QObject>

#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexer.h>


//! \brief The QsciLexerR class encapsulates the Scintilla Bash lexer.
class QSCINTILLA_EXPORT QsciLexerR : public QsciLexer
{
	Q_OBJECT

public:
	//! This enum defines the meanings of the different styles used by the
	//! Bash lexer.
	enum {
		DEFAULT=0,
		COMMENT,
		KWORD,
		BASEKWORD,
		OTHERKWORD,
		NUMBER,
		STRING,
		STRING2,
		OPERATOR,
		IDENTIFIER,
		INFIX,
		INFIXEOL,
	};

	//! Construct a QsciLexerR with parent \a parent.  \a parent is
	//! typically the QsciScintilla instance.
	QsciLexerR(QObject* parent = 0);

	//! Destroys the QsciLexerR instance.
	virtual ~QsciLexerR();

	//! Returns the name of the language.
	const char* language() const;

	//! Returns the name of the lexer.  Some lexers support a number of
	//! languages.
	const char* lexer() const;

	//! Returns the string of characters that comprise a word.
	const char* wordCharacters() const;

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

	//! Returns the set of keywords for the keyword set \a set recognised
	//! by the lexer as a space separated string.
	const char* keywords(int set);

	//! Returns the descriptive name for style number \a style.  If the
	//! style is invalid for this language then an empty QString is returned.
	//! This is intended to be used in user preference dialogs.
	QString description(int style) const;


public slots:

protected:
	//! The lexer's properties are read from the settings \a qs.  \a prefix
	//! (which has a trailing '/') should be used as a prefix to the key of
	//! each setting.  true is returned if there is no error.
	//!
private:
	QsciLexerR(const QsciLexerR&);
	QsciLexerR& operator=(const QsciLexerR&);
};
