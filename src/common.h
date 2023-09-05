#pragma once
#include <QAction>
#include <qscilexer.h>

const int ResultItemPos = Qt::UserRole + 1;//每一条查找结果，section下面的子级别
const int ResultItemEditor = Qt::UserRole + 2;//查找结果的一个总节点的属性字段，section级别
const int ResultWhatFind = Qt::UserRole + 3;
const int ResultItemRoot = Qt::UserRole + 4; //一次查找结果的根节点的属性字段，多个section的父级别
const int ResultItemEditorFilePath = Qt::UserRole + 5;
const int ResultItemLen = Qt::UserRole + 6;
const int ResultItemDesc = Qt::UserRole + 7;

//打开模式。1 文本 2 二进制 3 大文本只读 4 文本只读
static const char* Open_Attr = "openid";

#if 0
enum LangType {
	L_TEXT, L_PHP, L_C, L_CPP, L_CS, L_OBJC, L_JAVA, L_RC, \
	L_HTML, L_XML, L_MAKEFILE, L_PASCAL, L_BATCH, L_INI, L_ASCII, L_USER, \
	L_ASP, L_SQL, L_VB, L_JS, L_CSS, L_PERL, L_PYTHON, L_LUA, \
	L_TEX, L_FORTRAN, L_BASH, L_FLASH, L_NSIS, L_TCL, L_LISP, L_SCHEME, \
	L_ASM, L_DIFF, L_PROPS, L_PS, L_RUBY, L_SMALLTALK, L_VHDL, L_KIX, L_AU3, \
	L_CAML, L_ADA, L_VERILOG, L_MATLAB, L_HASKELL, L_INNO, L_SEARCHRESULT, \
	L_CMAKE, L_YAML, L_COBOL, L_GUI4CLI, L_D, L_POWERSHELL, L_R, L_JSP, \
	L_COFFEESCRIPT, L_JSON, L_JAVASCRIPT, L_FORTRAN_77, L_BAANC, L_SREC, \
	L_IHEX, L_TEHEX, L_SWIFT, \
	L_ASN1, L_AVS, L_BLITZBASIC, L_PUREBASIC, L_FREEBASIC, \
	L_CSOUND, L_ERLANG, L_ESCRIPT, L_FORTH, L_LATEX, \
	L_MMIXAL, L_NIM, L_NNCRONTAB, L_OSCRIPT, L_REBOL, \
	L_REGISTRY, L_RUST, L_SPICE, L_TXT2TAGS, L_VISUALPROLOG, L_TYPESCRIPT, \
	L_EDIFACT, L_MARKDOWN, L_OCTAVE, L_PO, L_POV, L_IDL, L_GO, L_TXT,\
	// Don't use L_JS, use L_JAVASCRIPT instead
	// The end of enumated language type, so it should be always at the end
	L_EXTERNAL = 100,
};
#endif

int nbDigitsFromNbLines(size_t nbLines);

class LexerNode {
public:
	QAction* pAct;
	LangType index;
	LexerNode():pAct(nullptr), index(L_TXT)
	{

	}
};
