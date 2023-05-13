// This module implements the QsciLexerGO class.
//
// Copyright (c) 2023 Riverbank Computing Limited <info@riverbankcomputing.com>
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


#include "Qsci/qscilexergo.h"
#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>


// The ctor.
QsciLexerGO::QsciLexerGO(QObject *parent)
	: QsciLexerCPP(parent)
{
	m_commentSymbol = "//";
	m_commentStart = "/*";
	m_commentEnd = "*/";
}


// The dtor.
QsciLexerGO::~QsciLexerGO()
{
}


// Returns the language name.
const char *QsciLexerGO::language() const
{
	return "GO";
}


// Returns the foreground colour of the text for a style.
QColor QsciLexerGO::defaultColor(int style) const
{
	if (style == VerbatimString)
		return QColor(0x00, 0x7f, 0x00);

	return QsciLexerCPP::defaultColor(style);
}


// Returns the end-of-line fill for a style.
bool QsciLexerGO::defaultEolFill(int style) const
{
	if (style == VerbatimString)
		return true;

	return QsciLexerCPP::defaultEolFill(style);
}


// Returns the font of the text for a style.
QFont QsciLexerGO::defaultFont(int style) const
{
	if (style == VerbatimString)
#if defined(Q_OS_WIN)
		return QFont("Courier New", QsciLexer::s_defaultFontSize);
#elif defined(Q_OS_MAC)
		return QFont("Courier", 12);
#else
		return QFont("Bitstream Vera Sans Mono", 9);
#endif

	return QsciLexerCPP::defaultFont(style);
}


// Returns the set of keywords.
const char *QsciLexerGO::keywords(int set)
{
	if (set != 1)
		return 0;

	return  "break case chan const continue default defer "
            "else fallthrough for func go goto if "
            "import interface map package range return "
            "select struct switch type var "
            "append copy int32 print bool false int64 println "
            "byte float32 iota real cap float64 len recover "
            "close imag make string complex int new "
            "true complex64 int8 nil uint complex128 int16 "
		"panic uint8 uint16 uint32 uint64 uintptr";
}


// Returns the user name of a style.
QString QsciLexerGO::description(int style) const
{
	if (style == VerbatimString)
		return tr("Verbatim string");

	return QsciLexerCPP::description(style);
}


// Returns the background colour of the text for a style.
QColor QsciLexerGO::defaultPaper(int style) const
{
	if (style == VerbatimString)
		return QColor(0xe0, 0xff, 0xe0);

	return QsciLexer::defaultPaper(style);
}
