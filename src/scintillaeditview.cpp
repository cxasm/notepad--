#include "scintillaeditview.h"

#include "common.h"
#include "rgba_icons.h"
#include "ccnotepad.h"
#include "styleset.h"
#include "qtlangset.h"

#include <Scintilla.h>
#include <SciLexer.h>
#include <QImage>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexeravs.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexercmake.h>
#include <Qsci/qscilexercoffeescript.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexercustom.h>
#include <Qsci/qscilexerd.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexeredifact.h>
#include <Qsci/qscilexerfortran.h>
#include <Qsci/qscilexerfortran77.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexeridl.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexermatlab.h>
#include <Qsci/qscilexeroctave.h>
#include <Qsci/qscilexerpascal.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerpo.h>
#include <Qsci/qscilexerpostscript.h>
#include <Qsci/qscilexerpov.h>
#include <Qsci/qscilexerproperties.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexerspice.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexertcl.h>
#include <Qsci/qscilexertex.h>
#include <Qsci/qscilexerverilog.h>
#include <Qsci/qscilexervhdl.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexeryaml.h>
#include <Qsci/qscilexergo.h>
#include <Qsci/qscilexertext.h>
#include <Qsci/qscilexernsis.h>
#include <QScrollBar>
#include <unordered_set>
#include <QDebug>

#include <stdexcept>
#include "findwin.h"

// initialize the static variable
#define DEFAULT_FONT_NAME "Courier New" //"Microsoft YaHei"  


int ScintillaEditView::s_tabLens = 4;
//默认不使用tab，使用space替换
bool ScintillaEditView::s_noUseTab = true;

int ScintillaEditView::s_bigTextSize = 100;


const int ScintillaEditView::_SC_MARGE_LINENUMBER = 0;
const int ScintillaEditView::_SC_MARGE_SYBOLE = 1;
const int ScintillaEditView::_SC_MARGE_FOLDER = 2;

const int MAX_PRE_NEXT_TIMES = 30;

//const int ScintillaEditView::_markersArray[][NB_FOLDER_STATE] = {
//  {SC_MARKNUM_FOLDEROPEN, SC_MARKNUM_FOLDER, SC_MARKNUM_FOLDERSUB, SC_MARKNUM_FOLDERTAIL, SC_MARKNUM_FOLDEREND,        SC_MARKNUM_FOLDEROPENMID,     SC_MARKNUM_FOLDERMIDTAIL},
//  {SC_MARK_MINUS,         SC_MARK_PLUS,      SC_MARK_EMPTY,        SC_MARK_EMPTY,         SC_MARK_EMPTY,               SC_MARK_EMPTY,                SC_MARK_EMPTY},
//  {SC_MARK_ARROWDOWN,     SC_MARK_ARROW,     SC_MARK_EMPTY,        SC_MARK_EMPTY,         SC_MARK_EMPTY,               SC_MARK_EMPTY,                SC_MARK_EMPTY},
//  {SC_MARK_CIRCLEMINUS,   SC_MARK_CIRCLEPLUS,SC_MARK_VLINE,        SC_MARK_LCORNERCURVE,  SC_MARK_CIRCLEPLUSCONNECTED, SC_MARK_CIRCLEMINUSCONNECTED, SC_MARK_TCORNERCURVE},
//  {SC_MARK_BOXMINUS,      SC_MARK_BOXPLUS,   SC_MARK_VLINE,        SC_MARK_LCORNER,       SC_MARK_BOXPLUSCONNECTED,    SC_MARK_BOXMINUSCONNECTED,    SC_MARK_TCORNER}
//};
#ifdef Q_OS_WIN
LanguageName ScintillaEditView::langNames[L_EXTERNAL + 1] = {
{QString("normal"),		QString("Normal QString"),		QString("Normal text file"),								L_TXT,			SCLEX_NULL},
{QString("php"),			QString("PHP"),				QString("PHP Hypertext Preprocessor file"),				L_PHP,			SCLEX_HTML},
{QString("c"),				QString("C"),					QString("C source file"),									L_C,			SCLEX_CPP},
{QString("cpp"),			QString("C++"),				QString("C++ source file"),								L_CPP,			SCLEX_CPP},
{QString("cs"),			QString("C#"),					QString("C# source file"),									L_CS,			SCLEX_CPP},
{QString("objc"),			QString("Objective-C"),		QString("Objective-C source file"),						L_OBJC,			SCLEX_CPP},
{QString("java"),			QString("Java"),				QString("Java source file"),								L_JAVA,			SCLEX_CPP},
{QString("rc"),			QString("RC"),					QString("Windows Resource file"),							L_RC,			SCLEX_CPP},
{QString("html"),			QString("HTML"),				QString("Hyper Text Markup Language file"),				L_HTML,			SCLEX_HTML},
{QString("xml"),			QString("XML"),				QString("eXtensible Markup Language file"),				L_XML,			SCLEX_XML},
{QString("makefile"),		QString("Makefile"),			QString("Makefile"),										L_MAKEFILE,		SCLEX_MAKEFILE},
{QString("pascal"),		QString("Pascal"),				QString("Pascal source file"),								L_PASCAL,		SCLEX_PASCAL},
{QString("batch"),			QString("Batch"),				QString("Batch file"),										L_BATCH,		SCLEX_BATCH},
{QString("ini"),			QString("ini"),				QString("MS ini file"),									L_INI,			SCLEX_PROPERTIES},
{QString("nfo"),			QString("NFO"),				QString("MSDOS Style/ASCII Art"),							L_ASCII,		SCLEX_NULL},
{QString("udf"),			QString("udf"),				QString("User Defined language file"),						L_USER,			SCLEX_USER},
{QString("asp"),			QString("ASP"),				QString("Active Server Pages script file"),				L_ASP,			SCLEX_HTML},
{QString("sql"),			QString("SQL"),				QString("Structured Query Language file"),					L_SQL,			SCLEX_SQL},
{QString("vb"),			QString("Visual Basic"),		QString("Visual Basic file"),								L_VB,			SCLEX_VB},
{QString("javascript"),	QString("JavaScript"),			QString("JavaScript file"),								L_JS,			L_JAVASCRIPT},
{QString("css"),			QString("CSS"),				QString("Cascade Style Sheets File"),						L_CSS,			SCLEX_CSS},
{QString("perl"),			QString("Perl"),				QString("Perl source file"),								L_PERL,			SCLEX_PERL},
{QString("python"),		QString("Python"),				QString("Python file"),									L_PYTHON,		SCLEX_PYTHON},
{QString("lua"),			QString("Lua"),				QString("Lua source File"),								L_LUA,			SCLEX_LUA},
{QString("tex"),			QString("TeX"),				QString("TeX file"),										L_TEX,			SCLEX_TEX},
{QString("fortran"),		QString("Fortran free form"),	QString("Fortran free form source file"),					L_FORTRAN,		SCLEX_FORTRAN},
{QString("bash"),			QString("Shell"),				QString("Unix script file"),								L_BASH,			SCLEX_BASH},
{QString("actionscript"),	QString("ActionScript"),		QString("Flash ActionScript file"),						L_FLASH,		SCLEX_CPP},
{QString("nsis"),			QString("NSIS"),				QString("Nullsoft Scriptable Install System script file"),	L_NSIS,			SCLEX_NSIS},
{QString("tcl"),			QString("TCL"),				QString("Tool Command Language file"),						L_TCL,			SCLEX_TCL},
{QString("lisp"),			QString("Lisp"),				QString("List Processing language file"),					L_LISP,			SCLEX_LISP},
{QString("scheme"),		QString("Scheme"),				QString("Scheme file"),									L_SCHEME,		SCLEX_LISP},
{QString("asm"),			QString("Assembly"),			QString("Assembly language source file"),					L_ASM,			SCLEX_ASM},
{QString("diff"),			QString("Diff"),				QString("Diff file"),										L_DIFF,			SCLEX_DIFF},
{QString("props"),			QString("Properties file"),	QString("Properties file"),								L_PROPS,		SCLEX_PROPERTIES},
{QString("postscript"),	QString("PostScript"),			QString("PostScript file"),								L_PS,			SCLEX_PS},
{QString("ruby"),			QString("Ruby"),				QString("Ruby file"),										L_RUBY,			SCLEX_RUBY},
{QString("smalltalk"),		QString("Smalltalk"),			QString("Smalltalk file"),									L_SMALLTALK,	SCLEX_SMALLTALK},
{QString("vhdl"),			QString("VHDL"),				QString("VHSIC Hardware Description Language file"),		L_VHDL,			SCLEX_VHDL},
{QString("kix"),			QString("KiXtart"),			QString("KiXtart file"),									L_KIX,			SCLEX_KIX},
{QString("autoit"),		QString("AutoIt"),				QString("AutoIt"),											L_AU3,			SCLEX_AU3},
{QString("caml"),			QString("CAML"),				QString("Categorical Abstract Machine Language"),			L_CAML,			SCLEX_CAML},
{QString("ada"),			QString("Ada"),				QString("Ada file"),										L_ADA,			SCLEX_ADA},
{QString("verilog"),		QString("Verilog"),			QString("Verilog file"),									L_VERILOG,		SCLEX_VERILOG},
{QString("matlab"),		QString("MATLAB"),				QString("MATrix LABoratory"),								L_MATLAB,		SCLEX_MATLAB},
{QString("haskell"),		QString("Haskell"),			QString("Haskell"),										L_HASKELL,		SCLEX_HASKELL},
{QString("inno"),			QString("Inno Setup"),			QString("Inno Setup script"),								L_INNO,			SCLEX_INNOSETUP},
{QString("searchResult"),	QString("Internal Search"),	QString("Internal Search"),								L_SEARCHRESULT,	SCLEX_SEARCHRESULT},
{QString("cmake"),			QString("CMake"),				QString("CMake file"),										L_CMAKE,		SCLEX_CMAKE},
{QString("yaml"),			QString("YAML"),				QString("YAML Ain't Markup Language"),						L_YAML,			SCLEX_YAML},
{QString("cobol"),			QString("COBOL"),				QString("COmmon Business Oriented Language"),				L_COBOL,		SCLEX_COBOL},
{QString("gui4cli"),		QString("Gui4Cli"),			QString("Gui4Cli file"),									L_GUI4CLI,		SCLEX_GUI4CLI},
{QString("d"),				QString("D"),					QString("D programming language"),							L_D,			SCLEX_D},
{QString("powershell"),	QString("PowerShell"),			QString("Windows PowerShell"),								L_POWERSHELL,	SCLEX_POWERSHELL},
{QString("r"),				QString("R"),					QString("R programming language"),							L_R,			SCLEX_R},
{QString("jsp"),			QString("JSP"),				QString("JavaServer Pages script file"),					L_JSP,			SCLEX_HTML},
{QString("coffeescript"),	QString("CoffeeScript"),		QString("CoffeeScript file"),								L_COFFEESCRIPT,	SCLEX_COFFEESCRIPT},
{QString("json"),			QString("json"),				QString("JSON file"),										L_JSON,			SCLEX_JSON },
{QString("javascript.js"), QString("JavaScript"),			QString("JavaScript file"),								L_JAVASCRIPT,	SCLEX_CPP },
{QString("fortran77"),		QString("Fortran fixed form"),	QString("Fortran fixed form source file"),					L_FORTRAN_77,	SCLEX_F77},
{QString("baanc"),			QString("BaanC"),				QString("BaanC File"),										L_BAANC,		SCLEX_BAAN },
{QString("srec"),			QString("S-Record"),			QString("Motorola S-Record binary data"),					L_SREC,			SCLEX_SREC},
{QString("ihex"),			QString("Intel HEX"),			QString("Intel HEX binary data"),							L_IHEX,			SCLEX_IHEX},
{QString("tehex"),			QString("Tektronix extended HEX"),	QString("Tektronix extended HEX binary data"),			L_TEHEX,		SCLEX_TEHEX},
{QString("swift"),			QString("Swift"),              QString("Swift file"),										L_SWIFT,		SCLEX_CPP},
{QString("asn1"),			QString("ASN.1"),				QString("Abstract Syntax Notation One file"),				L_ASN1,			SCLEX_ASN1},
{QString("avs"),			QString("AviSynth"),			QString("AviSynth scripts files"),							L_AVS,			SCLEX_AVS},
{QString("blitzbasic"),	QString("BlitzBasic"),			QString("BlitzBasic file"),								L_BLITZBASIC,	SCLEX_BLITZBASIC},
{QString("purebasic"),		QString("PureBasic"),			QString("PureBasic file"),									L_PUREBASIC,	SCLEX_PUREBASIC},
{QString("freebasic"),		QString("FreeBasic"),			QString("FreeBasic file"),									L_FREEBASIC,	SCLEX_FREEBASIC},
{QString("csound"),		QString("Csound"),				QString("Csound file"),									L_CSOUND,		SCLEX_CSOUND},
{QString("erlang"),		QString("Erlang"),				QString("Erlang file"),									L_ERLANG,		SCLEX_ERLANG},
{QString("escript"),		QString("ESCRIPT"),			QString("ESCRIPT file"),									L_ESCRIPT,		SCLEX_ESCRIPT},
{QString("forth"),			QString("Forth"),				QString("Forth file"),										L_FORTH,		SCLEX_FORTH},
{QString("latex"),			QString("LaTeX"),				QString("LaTeX file"),										L_LATEX,		SCLEX_LATEX},
{QString("mmixal"),		QString("MMIXAL"),				QString("MMIXAL file"),									L_MMIXAL,		SCLEX_MMIXAL},
{QString("nim"),			QString("Nim"),				QString("Nim file"),										L_NIM,			SCLEX_NIMROD},
{QString("nncrontab"),		QString("Nncrontab"),			QString("extended crontab file"),							L_NNCRONTAB,	SCLEX_NNCRONTAB},
{QString("oscript"),		QString("OScript"),			QString("OScript source file"),							L_OSCRIPT,		SCLEX_OSCRIPT},
{QString("rebol"),			QString("REBOL"),				QString("REBOL file"),										L_REBOL,		SCLEX_REBOL},
{QString("registry"),		QString("registry"),			QString("registry file"),									L_REGISTRY,		SCLEX_REGISTRY},
{QString("rust"),			QString("Rust"),				QString("Rust file"),										L_RUST,			SCLEX_RUST},
{QString("spice"),			QString("Spice"),				QString("spice file"),										L_SPICE,		SCLEX_SPICE},
{QString("txt2tags"),		QString("txt2tags"),			QString("txt2tags file"),									L_TXT2TAGS,		SCLEX_TXT2TAGS},
{QString("visualprolog"),	QString("Visual Prolog"),		QString("Visual Prolog file"),								L_VISUALPROLOG,	SCLEX_VISUALPROLOG},
{QString("typescript"),	QString("TypeScript"),			QString("TypeScript file"),								L_TYPESCRIPT,	SCLEX_CPP},
{QString("edifact"),	QString("Edifact"),			QString("Edifact file"),								L_EDIFACT,	SCLEX_EDIFACT},
{QString("markdown"),	QString("Markdown"),			QString("Markdown file"),								L_MARKDOWN,	SCLEX_MARKDOWN},
{QString("octave"),	QString("Octave"),			QString("Octave file"),								L_OCTAVE,	SCLEX_OCTAVE},
{QString("po"),	QString("PO"),			QString("PO file"),								L_PO,	SCLEX_PO},
{QString("pov"),	QString("POV"),			QString("POV file"),								L_POV,	SCLEX_POV},
{QString("avs"),	QString("AVS"),			QString("AVS file"),								L_AVS,	SCLEX_AVS},
{QString("idl"),	QString("IDL"),			QString("IDL file"),								L_IDL,	SCLEX_IDL},
{QString("go"),	QString("GO"),			QString("GO file"),								L_GO,	SCLEX_GO},
{QString("txt"),	QString("TEXT"),			QString("TXT file"),								L_TXT,	SCLEX_TXT},
{QString("ext"),			QString("External"),			QString("External"),					L_EXTERNAL,		SCLEX_NULL}
};
#endif

ScintillaEditView::ScintillaEditView(QWidget *parent)
	: QsciScintilla(parent), m_NoteWin(nullptr), m_preFirstLineNum(0), m_curPos(0), m_hasHighlight(false)
{
	init();
}

ScintillaEditView::~ScintillaEditView()
{
	releaseAllMark();
}

//void ScintillaEditView::resetDefaultFontStyle()
//{
//	QFont font(DEFAULT_FONT_NAME, 11/*QsciLexer::s_defaultFontSize*/, QFont::Normal);
//	setFont(font);
//	setMarginsFont(font);
//	setMarginsForegroundColor(QColor(0x80, 0x80, 0x80));
//}

void ScintillaEditView::setNoteWidget(QWidget * win)
{
	m_NoteWin = win;
}


void ScintillaEditView::updateLineNumbersMargin(bool forcedToHide) {
	if (forcedToHide)
	{
		execute(SCI_SETMARGINWIDTHN, _SC_MARGE_LINENUMBER, (sptr_t)0);
	}
	else 
	{
		updateLineNumberWidth(0);
	}

}

//根据现有滚动条来决定是否更新屏幕线宽长度。每滚动2000个单位必须调整line宽
void ScintillaEditView::autoAdjustLineWidth(int xScrollValue)
{

	if (std::abs(xScrollValue - m_preFirstLineNum) > 400)
	{
		m_preFirstLineNum = xScrollValue;

		updateLineNumberWidth(1);
	}
}

void ScintillaEditView::updateLineNumberWidth(int lineNumberMarginDynamicWidth)
{
	auto linesVisible = execute(SCI_LINESONSCREEN);
	if (linesVisible)
	{
		int nbDigits = 0;

		if(lineNumberMarginDynamicWidth != 0)
		{
			auto firstVisibleLineVis = execute(SCI_GETFIRSTVISIBLELINE);
			auto lastVisibleLineVis = linesVisible + firstVisibleLineVis + 1;
			auto lastVisibleLineDoc = execute(SCI_DOCLINEFROMVISIBLE, (long)lastVisibleLineVis);

			nbDigits = nbDigitsFromNbLines(lastVisibleLineDoc);
			nbDigits = nbDigits < 4 ? 4 : nbDigits;
		}
		else
		{
		auto nbLines = execute(SCI_GETLINECOUNT);
		nbDigits = nbDigitsFromNbLines(nbLines);
			nbDigits = nbDigits < 5 ? 5 : nbDigits;
		}
		
		auto pixelWidth = 6 + nbDigits * execute(SCI_TEXTWIDTH, STYLE_LINENUMBER, reinterpret_cast<sptr_t>("8"));
		execute(SCI_SETMARGINWIDTHN, _SC_MARGE_LINENUMBER, pixelWidth);
	}
	
}


void ScintillaEditView::showMargin(int whichMarge, bool willBeShowed)
{
	if (whichMarge == _SC_MARGE_LINENUMBER)
	{
		bool forcedToHide = !willBeShowed;
		updateLineNumbersMargin(forcedToHide);
	}
	else
	{
	//后面再来解决像素的问题
		//DPIManager& dpiManager = NppParameters::getInstance()._dpiManager;
		int width = 3;
		if (whichMarge == _SC_MARGE_SYBOLE)
			width = 8;
		else if (whichMarge == _SC_MARGE_FOLDER)
			width = 14;
		execute(SCI_SETMARGINWIDTHN, whichMarge, willBeShowed ? width : 0);
	}
}

sptr_t ScintillaEditView::execute(quint32 Msg, uptr_t wParam, sptr_t lParam) const {
	try {
		return (m_pScintillaFunc) ? m_pScintillaFunc(m_pScintillaPtr, Msg, wParam, lParam) : -1;
	}
	catch (...)
	{
		return -1;
	}
};

//status : true 表示存在， false 表示不存在
//tag，只有在用户自定义语法是，才需要给出。内部自带的语法不需要给出
QsciLexer* ScintillaEditView::createLexer(int lexerId,QString tag)
{
	QsciLexer* ret = nullptr;

	switch (lexerId)
	{
	case L_PHP:
		ret = new QsciLexerHTML();
		ret->setLexerTag("php");
		break;
	case L_HTML:
		ret = new QsciLexerHTML();
		ret->setLexerTag("html");
		break;
	case L_ASP:
		ret = new QsciLexerHTML();
		ret->setLexerTag("asp");
		break;
	case L_JSP:
		ret = new QsciLexerHTML();
		ret->setLexerTag("jsp");
		break;
	case L_C:
		ret = new QsciLexerCPP();
		ret->setLexerTag("c");
		break;
	case L_RC:
		ret = new QsciLexerCPP();
		ret->setLexerTag("rc");
		break;
	case L_CPP:
		ret = new QsciLexerCPP();
		break;
	case L_OBJC:
		ret = new QsciLexerCPP();
		ret->setLexerTag("objc");
		break;
	case L_CS:
		ret = new QsciLexerCSharp();
		ret->setLexerTag("csharp");
		break;
	case L_JAVA:
		ret = new QsciLexerJava();
		ret->setLexerTag("java");
		break;
	case L_XML:
		ret = new QsciLexerXML();
		break;
	case L_MAKEFILE:
		ret = new QsciLexerMakefile();
		break;
	case L_PASCAL:
		ret = new QsciLexerPascal();
		break;
	case L_BATCH:
		ret = new QsciLexerBatch();
		break;
	case L_INI:
		ret = new QsciLexerProperties();
		ret->setLexerTag("ini");
		break;
	case L_ASCII:
		break;
	case L_USER:
		break;
	case L_SQL:
		ret = new QsciLexerSQL();
		break;
	case L_VB:
		break;
	case L_CSS:
		ret = new QsciLexerCSS();
		break;
	case L_PERL:
		ret = new QsciLexerPerl();
		break;
	case L_PYTHON:
		ret = new QsciLexerPython();
		break;
	case L_LUA:
		ret = new QsciLexerLua();
		break;
	case L_TEX:
		break;
	case L_FORTRAN:
		ret = new QsciLexerFortran();
		break;
	case L_BASH:
		ret = new QsciLexerBash();
		break;
	case L_FLASH:
		ret = new QsciLexerCPP();
		ret->setLexerTag("flash");
		break;
	case L_NSIS:
		ret = new QsciLexerNsis();
		break;
	case L_TCL:
		ret = new QsciLexerTCL();
		break;
	case L_LISP:
		break;
	case L_SCHEME:
		break;
	case L_ASM:
		break;
	case L_DIFF:
		ret = new QsciLexerDiff();
		break;
	case L_PROPS:
		ret = new QsciLexerProperties();
		break;
	case L_PS:
		break;
	case L_RUBY:
		ret = new QsciLexerRuby();
		break;
	case L_SMALLTALK:
		break;
	case L_VHDL:
		ret = new QsciLexerVHDL();
		break;
	case L_KIX:
		break;
	case L_AU3:
		break;
	case L_CAML:
		break;
	case L_ADA:
		break;
	case L_VERILOG:
		ret = new QsciLexerVerilog();
		break;
	case L_MATLAB:
		ret = new QsciLexerMatlab();
		break;
	case L_HASKELL:
		break;
	case L_INNO:
		break;
	case L_SEARCHRESULT:
		break;
	case L_CMAKE:
		ret = new QsciLexerCMake();
		break;
	case L_YAML:
		ret = new QsciLexerYAML();
		break;
	case L_COBOL:
		break;
	case L_GUI4CLI:
		break;
	case L_D:
		break;
	case L_POWERSHELL:
		break;

	case L_COFFEESCRIPT:
		ret = new QsciLexerCoffeeScript();
		break;
	case L_JSON:
		ret = new QsciLexerJSON();
		break;
	case L_JAVASCRIPT:
		ret = new QsciLexerJavaScript();
		ret->setLexerTag("javascript");
		break;
	case L_FORTRAN_77:
		ret = new QsciLexerFortran77();
		break;
	case L_BAANC:
		break;
	case L_SREC:
		break;
	case L_IHEX:
		break;
	case L_TEHEX:
		break;
	case L_SWIFT:
		break;
	case L_ASN1:
		break;
	case L_AVS:
		ret = new QsciLexerAVS();
		break;
	case L_BLITZBASIC:
		break;
	case L_PUREBASIC:
		break;
	case L_FREEBASIC:
		break;
	case L_CSOUND:
		break;
	case L_ERLANG:
		break;
	case L_ESCRIPT:
		break;
	case L_FORTH:
		break;
	case L_LATEX:
		break;
	case L_MMIXAL:
		break;
	case L_NIM:
		break;
	case L_NNCRONTAB:
		break;
	case L_OSCRIPT:
		break;
	case L_REBOL:
		break;
	case L_REGISTRY:
		break;
	case L_RUST:
		break;
	case L_SPICE:
		ret = new QsciLexerSpice();
		break;
	case L_TXT2TAGS:
		break;
	case L_VISUALPROLOG:
		break;
	case L_TYPESCRIPT:
		ret = new QsciLexerCPP();
		ret->setLexerTag("typescript");
		break;
	case L_EXTERNAL:
		break;
	case L_IDL:
		ret = new QsciLexerIDL();
		ret->setLexerTag("idl");
		break;
	case L_GO:
		ret = new QsciLexerGO();
		ret->setLexerTag("go");
		break;
	case L_TXT:
		ret = new QsciLexerText();
		ret->setLexerTag("txt");
		break;
	case L_USER_TXT:
		//使用txt的语法解析器，但是自定义关键字，自定义tag语言标签。下同
		ret = new QsciLexerText();
		ret->setLexerTag(tag);
		ret->setIsUserDefineKeywords(true);
		break;
	case L_USER_CPP:
		ret = new QsciLexerCPP();
		ret->setLexerTag(tag);
		ret->setIsUserDefineKeywords(true);
		break;
	default:
		break;
	}
	if (ret != nullptr)
	{
		ret->setLexerId(lexerId);
		QtLangSet::readLangSettings(ret, ret->lexerTag());
	}
	return ret;
}


void ScintillaEditView::appendMarkRecord(FindRecords * r)
{
	m_curMarkList.append(r);
}

void ScintillaEditView::releaseAllMark()
{
	for (int i = 0; i < m_curMarkList.size(); ++i)
	{
		delete m_curMarkList.at(i);
	}
	m_curMarkList.clear();
}

QList<FindRecords *>& ScintillaEditView::getCurMarkRecord()
{
	return m_curMarkList;
}

//调整颜色
void ScintillaEditView::adjuctSkinStyle()
{
	setFoldMarginColors(StyleSet::foldfgColor, StyleSet::foldbgColor);
	setMarginsBackgroundColor(StyleSet::marginsBackgroundColor);
}

void ScintillaEditView::setIndentGuide(bool willBeShowed)
{
	QsciLexer* pLexer = this->lexer(); 
	if (nullptr == pLexer || (pLexer->lexerId() == L_TXT))
	{
		return;
	}
	const int docIndentMode = (pLexer->lexerId() == L_PYTHON) ? SC_IV_LOOKFORWARD : SC_IV_LOOKBOTH;
	execute(SCI_SETINDENTATIONGUIDES, willBeShowed ? docIndentMode : SC_IV_NONE);

	QsciScintilla::setIndentGuide(willBeShowed);
}

void ScintillaEditView::init()
{

	m_pScintillaFunc = (SCINTILLA_FUNC)this->SendScintillaPtrResult(SCI_GETDIRECTFUNCTION);
	m_pScintillaPtr = (SCINTILLA_PTR)this->SendScintillaPtrResult(SCI_GETDIRECTPOINTER);


	if (!m_pScintillaFunc)
	{
		throw std::runtime_error("ScintillaEditView::init : SCI_GETDIRECTFUNCTION message failed");
	}

	if (!m_pScintillaPtr)
	{
		throw std::runtime_error("ScintillaEditView::init : SCI_GETDIRECTPOINTER message failed");
	}

	//开启行号marge
	//markerDefine(SC_MARGIN_NUMBER,_SC_MARGE_LINENUMBER);
	setMarginLineNumbers(_SC_MARGE_LINENUMBER, true);
	
	//execute(SCI_SETMARGINMASKN, _SC_MARGE_FOLDER, SC_MARGIN_SYMBOL);
	//execute(SCI_SETMARGINMASKN, _SC_MARGE_FOLDER, SC_MASK_FOLDERS);

	QColor foldfgColor(StyleSet::foldfgColor);
	QColor foldbgColor(StyleSet::foldbgColor);//默认0xff,0xff,0xff
	//QColor activeFoldFgColor(0xFF, 0, 0);

	//通过fold发现，尽量使用qscint的功能，因为他做了大量封装和简化
	setFolding(BoxedTreeFoldStyle, _SC_MARGE_FOLDER);
	setFoldMarginColors(foldfgColor, foldbgColor);

	//当前fold高亮。QT下面有bug暂时不开启
	//execute(SCI_MARKERENABLEHIGHLIGHT, true);

	setMarginsBackgroundColor(StyleSet::marginsBackgroundColor); //0xea, 0xf7, 0xff //默认0xf0f0f0

	//execute(SCI_MARKERSETBACK, _SC_MARGE_FOLDER, 0x808080);
	//execute(SCI_MARKERSETBACKSELECTED, _SC_MARGE_FOLDER, 0xff0000);

	showMargin(_SC_MARGE_LINENUMBER, true);

	//行号、符号、折叠。中间符号留一个很小的间隔
	showMargin(_SC_MARGE_SYBOLE, true);
	
	//showMargin(_SC_MARGE_FOLDER, true);

	//设置选中背景色
	//setSelectionBackgroundColor(QColor(117, 217, 117));

	

	//开始括号匹配，比如html的<>，开启前后这类字段的匹配
	setBraceMatching(SloppyBraceMatch);
	setMatchedBraceForegroundColor(QColor(191, 141, 255));



	//setMatchedBraceBackgroundColor(QColor(191, 141, 255));

	//自动补全效果不好，不开启20211017
	//setAutoCompletionSource(QsciScintilla::AcsAPIs);   //设置源，自动补全所有地方出现的
	//setAutoCompletionCaseSensitivity(true);   //设置自动补全大小写敏感
	//setAutoCompletionThreshold(1);    //设置每输入一个字符就会出现自动补全的提示

	//设置字体
	QFont font(DEFAULT_FONT_NAME, 11/*QsciLexer::s_defaultFontSize*/, QFont::Normal);
	setFont(font);
	setMarginsFont(font);
	//execute(SCI_SETMARGINMASKN, _SC_MARGE_SYBOLE, (1 << MARK_BOOKMARK) | (1 << MARK_HIDELINESBEGIN) | (1 << MARK_HIDELINESEND) | (1 << MARK_HIDELINESUNDERLINE));
	setMarginsForegroundColor(QColor(0x80, 0x80, 0x80)); //默认0x80, 0x80, 0x80
	execute(SCI_SETTABWIDTH, 4);

	//setPaper(QColor(0xfc, 0xfc, 0xfc));//这个无效

	//使用空格替换tab
	execute(SCI_SETUSETABS, !ScintillaEditView::s_noUseTab);

	//这个无比要设置false，否则双击后高亮单词，拷贝时会拷贝多个选择。
	execute(SCI_SETMULTIPLESELECTION, true);
	execute(SCI_SETADDITIONALSELECTIONTYPING, true);
	
	//execute(SCI_SETADDITIONALSELALPHA, 100);
	execute(SCI_SETMULTIPASTE, SC_MULTIPASTE_EACH);
	execute(SCI_SETADDITIONALCARETSVISIBLE, true);

	execute(SCI_SETVIRTUALSPACEOPTIONS, SCVS_RECTANGULARSELECTION);

	//execute(SCI_SETADDITIONALSELFORE, 0);
	//execute(SCI_SETADDITIONALSELBACK, 0x98ff98);
	execute(SCI_SETSELFORE, true, 0x0);
	//execute(SCI_SETSELBACK, true, 0x9ACC9A);
	execute(SCI_SETSELBACK, true, 0x9bff9b); //0x00ffff原来的黄色

	//设置查找到Mark的风格。定义其前景颜色和形状
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE, 130);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE, false);
	execute(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE, 0x00ffff);

	//下面这两个是HTML文件的tag高亮的表示。
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_TAGMATCH, INDIC_STRAIGHTBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_TAGMATCH, 100);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_TAGMATCH, false);
	execute(SCI_INDICSETFORE, SCE_UNIVERSAL_TAGMATCH, 0xff0080);

	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_TAGATTR, INDIC_STRAIGHTBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_TAGATTR, 100);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_TAGATTR, false);
	execute(SCI_INDICSETFORE, SCE_UNIVERSAL_TAGATTR, 0x00ffff);

	//双击后同样的字段进行高亮
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE_SMART, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE_SMART, 100);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_SMART, false);
	execute(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_SMART, 0x00ff00);

	setCaretLineVisible(true);
	setCaretLineBackgroundColor(QColor(0xe8e8ff));

	//记住position变化。不能使用cursorPositionChanged，因为他的列考虑uft8字符，中文一个也算1个，每次列不一定相等。
	//要使用自定义的cursorPosChange，跟踪的是SCI_GETCURRENTPOS 的值。换行才会触发这个cursorPosChange。自定义的信号
	connect(this,&QsciScintilla::cursorPosChange,this,&ScintillaEditView::slot_linePosChanged, Qt::QueuedConnection);

	connect(this, &QsciScintilla::selectionChanged, this, &ScintillaEditView::slot_clearHightWord, Qt::QueuedConnection);

	execute(SCI_SETSCROLLWIDTH, 1);

	//20220226发现高亮全选如果范围过大，会导致卡死。借鉴notepad的只高亮可视化区域。但是滚动时也必须生效
	connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &ScintillaEditView::slot_scrollXValueChange);
	connect(this, &ScintillaEditView::delayWork, this,&ScintillaEditView::slot_delayWork, Qt::QueuedConnection);

	//设置换行符号的格式
#if defined(Q_OS_WIN)
	execute(SCI_SETEOLMODE, SC_EOL_CRLF);
#elif !defined(Q_OS_MAC)
	execute(SCI_SETEOLMODE, SC_EOL_CR);
#elif !defined(Q_OS_UNIX)
	execute(SCI_SETEOLMODE, SC_EOL_LF);
#endif

	//开启后可以保证长行在滚动条下完整显示
	execute(SCI_SETSCROLLWIDTHTRACKING, true);

}

//X方向滚动条值变化后的槽函数。一旦滚动则会出发这里，发送消息给中介，让中介去同步另外一方
void ScintillaEditView::slot_scrollXValueChange(int value)
{
	slot_delayWork();
	autoAdjustLineWidth(value);
}

void ScintillaEditView::slot_linePosChanged(int /*line*/, int pos)
{
	//避免循环触发的slot_positionChanged
	if (m_curPos == pos)
	{
		return;
	}

	m_prePosRecord.append(pos);

	if (m_prePosRecord.size() >= MAX_PRE_NEXT_TIMES)
	{
		m_prePosRecord.takeFirst();
	}
}

//回退一个位置
bool ScintillaEditView::gotoPrePos()
{
	if (m_prePosRecord.size() > 1)
	{
		int v = m_prePosRecord.takeLast();
		m_nextPosRecord.push_front(v);

		if (m_nextPosRecord.size() >= MAX_PRE_NEXT_TIMES)
		{
			m_nextPosRecord.takeLast();
		}

		int pos = m_prePosRecord.last();

		//下面这个函数比较坑，里面要循环Index次，每次返回的index值是不一样的。只能使用SCI_GOTOPOS可自定义的信号
		//setCursorPosition(pos.line, pos.index);

		//会循环触发slot_positionChanged
		this->execute(SCI_GOTOPOS, pos);
		m_curPos = pos;

		return true;
	}
	return false;
}

bool ScintillaEditView::gotoNextPos()
{
	if (m_nextPosRecord.size() > 0)
	{
		int pos =  m_nextPosRecord.takeFirst();
		m_prePosRecord.append(pos);
		this->execute(SCI_GOTOPOS, pos);
		m_curPos = pos;
		return true;
	}
	return false;
}

const int MAXLINEHIGHLIGHT = 400;

void ScintillaEditView::clearIndicator(int indicatorNumber) {
	size_t docStart = 0;
	size_t docEnd = length();
	execute(SCI_SETINDICATORCURRENT, indicatorNumber);
	execute(SCI_INDICATORCLEARRANGE, docStart, docEnd - docStart);
};

void ScintillaEditView::slot_clearHightWord()
{
	if (m_hasHighlight)
	{
		m_hasHighlight = false;
		clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);
	}
}


void ScintillaEditView::highlightViewWithWord(QString & word2Hilite)
{
	int originalStartPos = execute(SCI_GETTARGETSTART);
	int originalEndPos = execute(SCI_GETTARGETEND);

	int firstLine = static_cast<int>(this->execute(SCI_GETFIRSTVISIBLELINE));
	int nbLineOnScreen = this->execute(SCI_LINESONSCREEN);
	int nbLines = std::min(nbLineOnScreen, MAXLINEHIGHLIGHT) + 1;
	int lastLine = firstLine + nbLines;
	int startPos = 0;
	int endPos = 0;
	auto currentLine = firstLine;
	int prevDocLineChecked = -1;	//invalid start


	auto searchMark = [this](int &startPos, int &endPos, QByteArray &word2Mark){

		int targetStart = 0;
		int targetEnd = 0;

		long lens = word2Mark.length();

		while (targetStart >= 0)
		{
			execute(SCI_SETTARGETRANGE, startPos, endPos);

			targetStart = SendScintilla(SCI_SEARCHINTARGET, lens, word2Mark.data());

			if (targetStart == -1 || targetStart == -2)
				break;

			targetEnd = int(this->execute(SCI_GETTARGETEND));

			if (targetEnd > endPos)
			{
				//we found a result but outside our range, therefore do not process it
				break;
			}

			int foundTextLen = targetEnd - targetStart;

			if (foundTextLen > 0)
			{
				this->execute(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE_SMART);
				this->execute(SCI_INDICATORFILLRANGE, targetStart, foundTextLen);
			}

			if (targetStart + foundTextLen == endPos)
				break;

			startPos = targetStart + foundTextLen;

		}
	};


	QByteArray whatMark = word2Hilite.toUtf8();

	SendScintilla(SCI_SETSEARCHFLAGS, SCFIND_REGEXP | SCFIND_MATCHCASE | SCFIND_WHOLEWORD | SCFIND_REGEXP_SKIPCRLFASONE);

	for (; currentLine < lastLine; ++currentLine)
	{
		int docLine = static_cast<int>(this->execute(SCI_DOCLINEFROMVISIBLE, currentLine));
		if (docLine == prevDocLineChecked)
			continue;	//still on same line (wordwrap)
		prevDocLineChecked = docLine;
		startPos = static_cast<int>(this->execute(SCI_POSITIONFROMLINE, docLine));
		endPos = static_cast<int>(this->execute(SCI_POSITIONFROMLINE, docLine + 1));

		if (endPos == -1)
		{	//past EOF
			endPos = this->length() - 1;
			searchMark(startPos, endPos, whatMark);
			break;
		}
		else
		{
			searchMark(startPos, endPos, whatMark);
		}
	}

	m_hasHighlight = true;

	// restore the original targets to avoid conflicts with the search/replace functions
	this->execute(SCI_SETTARGETRANGE, originalStartPos, originalEndPos);
}

void ScintillaEditView::slot_delayWork()
{
	if (!hasSelectedText())
	{
		return;
	}

	QString word = selectedText();
	if (!word.isEmpty())
	{
		highlightViewWithWord(word);
#if 0
		QVector<int>resultPos;
		resultPos.reserve(50);

		int firstLine = execute(SCI_GETFIRSTVISIBLELINE);
		int nbLineOnScreen = execute(SCI_LINESONSCREEN);
		int nbLines = std::min(nbLineOnScreen, MAXLINEHIGHLIGHT) + 1;
		int lastLine = firstLine + nbLines;


		long startPos = execute(SCI_POSITIONFROMLINE, firstLine);
		long endPos = execute(SCI_POSITIONFROMLINE, lastLine);

		if (endPos == -1)
		{	
			endPos = execute(SCI_GETLENGTH);
		}

		int curpos = execute(SCI_GETCURRENTPOS);
		int mainSelect = 1;

		//struct Sci_TextToFind findOptions;
		////20220226发现高亮全选如果范围过大，会导致卡死。借鉴notepad的只高亮可视化区域。但是滚动时也必须生效
		////findOptions.chrg.cpMin = 0;
		////findOptions.chrg.cpMax = execute(SCI_GETLENGTH);
		//
		//findOptions.chrg.cpMin = startPos;
		//findOptions.chrg.cpMax = endPos;
		//findOptions.lpstrText = wordStr.c_str();
		//20220422 升级高版本scint后，发现Sci_TextToFind要崩溃，替换为SCI_SEARCHINTARGET
		//int pos = execute(SCI_FINDTEXT, SCFIND_MATCHCASE | SCFIND_WHOLEWORD, reinterpret_cast<sptr_t>(&findOptions));

		//std::string wordStr = word.toStdString();
		QByteArray wordStr = word.toUtf8();

		SendScintilla(SCI_SETTARGETSTART, startPos);
		SendScintilla(SCI_SETTARGETEND, endPos);

		SendScintilla(SCI_SETSEARCHFLAGS, SCFIND_MATCHCASE|SCFIND_WHOLEWORD);

		long lens = wordStr.length();
		long pos = SendScintilla(SCI_SEARCHINTARGET, lens, wordStr.data()/*,wordStr.c_str()*/);

		while ((pos != -1)&&(pos < endPos))
		{
			resultPos.append(pos);

			if (pos <= curpos)
			{
				mainSelect = resultPos.size();
			}

			SendScintilla(SCI_SETTARGETSTART, pos + lens);
			SendScintilla(SCI_SETTARGETEND, endPos);
			pos = SendScintilla(SCI_SEARCHINTARGET, wordStr.length(), wordStr.data()/*,wordStr.c_str()*/);
			//findOptions.chrg.cpMin = findOptions.chrgText.cpMax;
			//pos = execute(SCI_FINDTEXT, SCFIND_MATCHCASE | SCFIND_WHOLEWORD, reinterpret_cast<sptr_t>(&findOptions));
		}

		for (int i = 0, size = resultPos.size(); i < size; ++i)
		{
			//execute(SCI_ADDSELECTION, resultPos.at(i) + lens, resultPos.at(i) );

			execute(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE_SMART);

			execute(SCI_INDICATORFILLRANGE, resultPos.at(i), lens);
		}

		//当前有高亮的字段
		if (resultPos.size() > 0)
		{
			m_hasHighlight = true;
		}

		/*if (!resultPos.isEmpty())
		{
			execute(SCI_SETMAINSELECTION, mainSelect - 1);
		}*/
#endif
	}
}

void ScintillaEditView::dragEnterEvent(QDragEnterEvent* event)
{
	//if (event->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
	//{
	//	event->accept(); //可以在这个窗口部件上拖放对象
	//}
	//else
	//{
	//	event->ignore();
	//}
	event->accept();
	}

void ScintillaEditView::dropEvent(QDropEvent* e)
{
	QList<QUrl> urls = e->mimeData()->urls();
	if (urls.isEmpty())
		return;

	CCNotePad* pv = dynamic_cast<CCNotePad*>(m_NoteWin);
	if (pv != nullptr)
		pv->receiveEditDrop(e);

	//qDebug() << ui.leftSrc->geometry() << ui.rightSrc->geometry() << QCursor::pos() << this->mapFromGlobal(QCursor::pos());
}



void ScintillaEditView::mouseDoubleClickEvent(QMouseEvent * e)
{
	QsciScintilla::mouseDoubleClickEvent(e);

	if (hasSelectedText())
	{
		emit delayWork();
	}
}

void ScintillaEditView::changeCase(const TextCaseType & caseToConvert, QString& strToConvert) const
{
	if (strToConvert.isEmpty())
		return;

	int nbChars = strToConvert.size();

	switch (caseToConvert)
	{
	case UPPERCASE:
	{
		strToConvert = strToConvert.toUpper();
		break;
	}
	case LOWERCASE:
	{
		strToConvert = strToConvert.toLower();
		break;
	} 
	case TITLECASE_FORCE:
	case TITLECASE_BLEND:
	{
		for (int i = 0; i < nbChars; ++i)
		{
			if (strToConvert[i].isLetter())
			{
				if ((i < 1) ? true : !strToConvert[i - 1].isLetterOrNumber())
					strToConvert[i] = strToConvert[i].toUpper();
				else if (caseToConvert == TITLECASE_FORCE)
					strToConvert[i] = strToConvert[i].toLower();
				//An exception
				if ((i < 2) ? false : (strToConvert[i - 1] == L'\'' && (strToConvert[i - 2].isLetter())))
					strToConvert[i] = strToConvert[i].toLower();
			}
		}
		break;
	} //case TITLECASE
	case SENTENCECASE_FORCE:
	case SENTENCECASE_BLEND:
	{
		bool isNewSentence = true;
		bool wasEolR = false;
		bool wasEolN = false;
		for (int i = 0; i < nbChars; ++i)
		{
			if (strToConvert[i].isLetter())
			{
				if (isNewSentence)
				{
					strToConvert[i] = strToConvert[i].toUpper();
					isNewSentence = false;
				}
				else if (caseToConvert == SENTENCECASE_FORCE)
				{
					strToConvert[i] = strToConvert[i].toLower();
				}
				wasEolR = false;
				wasEolN = false;
				//An exception
				if (strToConvert[i] == L'i' &&
					((i < 1) ? false : (strToConvert[i - 1].isSpace() || strToConvert[i - 1] == '(' || strToConvert[i - 1] == '"')) && \
					((i + 1 == nbChars) ? false : (strToConvert[i + 1].isSpace() || strToConvert[i + 1] == '\'')))
				{
					strToConvert[i] = L'I';
				}
			}
			else if (strToConvert[i] == '.' || strToConvert[i] == '!' || strToConvert[i] == '?')
			{
				if ((i + 1 == nbChars) ? true : strToConvert[i + 1].isLetterOrNumber())
					isNewSentence = false;
				else
					isNewSentence = true;
			}
			else if (strToConvert[i] == '\r')
			{
				if (wasEolR)
					isNewSentence = true;
				else
					wasEolR = true;
			}
			else if (strToConvert[i] == '\n')
			{
				if (wasEolN)
					isNewSentence = true;
				else
					wasEolN = true;
			}
		}
		break;
	} //case SENTENCECASE
	case INVERTCASE:
	{
		for (int i = 0; i < nbChars; ++i)
		{
			if (strToConvert[i].isLower())
				strToConvert[i] = strToConvert[i].toUpper();
			else
				strToConvert[i] = strToConvert[i].toLower();
		}
		break;
	} //case INVERTCASE
	case RANDOMCASE:
	{
		for (int i = 0; i < nbChars; ++i)
		{
			if (strToConvert[i].isLetter())
			{
				if (std::rand() & true)
					strToConvert[i] = strToConvert[i].toUpper();
				else
					strToConvert[i] = strToConvert[i].toLower();
			}
		}
		break;
	}
	} //switch (caseToConvert)
}

void ScintillaEditView::convertSelectedTextTo(const TextCaseType & caseToConvert)
{
	int selectionStart = execute(SCI_GETSELECTIONSTART);
	int selectionEnd = execute(SCI_GETSELECTIONEND);

	int strLen = (selectionEnd - selectionStart);
	if (strLen != 0)
	{
		int strSize = strLen + 1;

		char *selectedStr = new char[strSize];

		execute(SCI_GETSELTEXT, 0, reinterpret_cast<sptr_t>(selectedStr));

		QString utf8Str(selectedStr);
		changeCase(caseToConvert, utf8Str);

		QByteArray bytes = utf8Str.toUtf8();

		execute(SCI_SETTARGETRANGE, selectionStart, selectionEnd);

		execute(SCI_BEGINUNDOACTION);
		execute(SCI_REPLACETARGET, strLen, reinterpret_cast<sptr_t>(bytes.data()));
		execute(SCI_ENDUNDOACTION);
		execute(SCI_SETSEL, selectionStart, selectionEnd);
		delete[] selectedStr;
	}
}

//获取当前选择的行范围
std::pair<size_t, size_t> ScintillaEditView::getSelectionLinesRange(intptr_t selectionNumber /* = -1 */) const
{
	size_t numSelections = execute(SCI_GETSELECTIONS);

	size_t start_pos, end_pos;

	if ((selectionNumber < 0) || (static_cast<size_t>(selectionNumber) >= numSelections))
	{
		start_pos = execute(SCI_GETSELECTIONSTART);
		end_pos = execute(SCI_GETSELECTIONEND);
	}
	else
	{
		start_pos = execute(SCI_GETSELECTIONNSTART, selectionNumber);
		end_pos = execute(SCI_GETSELECTIONNEND, selectionNumber);
	}

	size_t line1 = execute(SCI_LINEFROMPOSITION, start_pos);
	size_t line2 = execute(SCI_LINEFROMPOSITION, end_pos);

	if ((line1 != line2) && (static_cast<size_t>(execute(SCI_POSITIONFROMLINE, line2)) == end_pos))
	{
		// if the end of the selection includes the line-ending, 
		// then don't include the following line in the range
		--line2;
	}

	return std::pair<size_t, size_t>(line1, line2);
}

//获取内容。是uft8格式的。所有字符都以utf8编码
void ScintillaEditView::getText(char *dest, size_t start, size_t end) const
{
	Sci_TextRange tr;
	tr.chrg.cpMin = static_cast<Sci_PositionCR>(start);
	tr.chrg.cpMax = static_cast<Sci_PositionCR>(end);
	tr.lpstrText = dest;

	this->SendScintilla(SCI_GETTEXTRANGE, 0, &tr);
}

QString ScintillaEditView::getGenericTextAsQString(size_t start, size_t end) const
{
	assert(end > start);
	const size_t bufSize = end - start + 1;

	QByteArray bytes;
	bytes.resize(bufSize);
	getText(bytes.data(), start, end);

	return QString(bytes);
}

QString ScintillaEditView::getEOLString()
{
	intptr_t eol_mode = execute(SCI_GETEOLMODE);
	if (eol_mode == SC_EOL_CRLF)
	{
		return QString("\r\n");
	}
	else if (eol_mode == SC_EOL_LF)
	{
		return QString("\n");
	}
	else
	{
		return QString("\r");
	}
}



//替换fromTargetPos 到 toTargetPos的内容为str2replace。

intptr_t ScintillaEditView::replaceTarget(QByteArray& str2replace, intptr_t fromTargetPos, intptr_t toTargetPos) const
{
	if (fromTargetPos != -1 || toTargetPos != -1)
	{
		execute(SCI_SETTARGETRANGE, fromTargetPos, toTargetPos);
	}

	return execute(SCI_REPLACETARGET, str2replace.size(), reinterpret_cast<sptr_t>(str2replace.data()));
}

size_t vecRemoveDuplicates(QList<QString>& vec)
{

	std::unordered_set<std::string> seen;
	auto newEnd = std::remove_if(vec.begin(), vec.end(), [&seen](const QString& value)
	{
		return !seen.insert(value.toStdString()).second;
	});

	vec.erase(newEnd, vec.end());

	return vec.size();
}


void ScintillaEditView::removeAnyDuplicateLines()
{
	size_t fromLine = 0, toLine = 0;
	bool hasLineSelection = false;

	auto selStart = execute(SCI_GETSELECTIONSTART);
	auto selEnd = execute(SCI_GETSELECTIONEND);
	hasLineSelection = selStart != selEnd;

	if (hasLineSelection)
	{
		const std::pair<size_t, size_t> lineRange = getSelectionLinesRange();
		// One single line selection is not allowed.
		if (lineRange.first == lineRange.second)
		{
			return;
		}
		fromLine = lineRange.first;
		toLine = lineRange.second;
	}
	else
	{
		// No selection.
		fromLine = 0;
		toLine = execute(SCI_GETLINECOUNT) - 1;
	}

	if (fromLine >= toLine)
	{
		return;
	}

	const auto startPos = execute(SCI_POSITIONFROMLINE, fromLine);
	const auto endPos = execute(SCI_POSITIONFROMLINE, toLine) + execute(SCI_LINELENGTH, toLine);
	const QString text = getGenericTextAsQString(startPos, endPos);

	QStringList linesVect = text.split(getEOLString());

	const size_t lineCount = execute(SCI_GETLINECOUNT);

	const bool doingEntireDocument = (toLine == (lineCount - 1));
	if (!doingEntireDocument)
	{
		if (linesVect.rbegin()->isEmpty())
		{
			linesVect.pop_back();
		}
	}

	size_t origSize = linesVect.size();


	size_t newSize = vecRemoveDuplicates(linesVect);

	if (origSize != newSize)
	{
		QString joined = linesVect.join(getEOLString());

		if (!doingEntireDocument)
		{
			joined += getEOLString();
		}
		if (text != joined)
		{
			QByteArray str2replace;
			str2replace = joined.toUtf8();

			replaceTarget(str2replace, startPos, endPos);
		}
	}
}

void ScintillaEditView::insertCharsFrom(size_t position, const QByteArray & text2insert) const
{
	this->SendScintilla(SCI_INSERTTEXT, position, text2insert.data());
}

void ScintillaEditView::appandGenericText(const QByteArray &text2Append) const
{
	this->SendScintilla(SCI_APPENDTEXT, text2Append.size(), text2Append.data());
}

void ScintillaEditView::insertNewLineAboveCurrentLine(bool)
{
	QString newline = getEOLString();
	const auto current_line = getCurrentLineNumber();
	if (current_line == 0)
	{
		// Special handling if caret is at first line.
		insertCharsFrom(0, newline.toUtf8());
	}
	else
	{
		const auto eol_length = newline.length();
		const auto position = execute(SCI_POSITIONFROMLINE, current_line) - eol_length;
		insertCharsFrom(position, newline.toUtf8());
	}
	execute(SCI_SETEMPTYSELECTION, execute(SCI_POSITIONFROMLINE, current_line));
}

void ScintillaEditView::insertNewLineBelowCurrentLine(bool)
{
	QString newline = getEOLString();
	const auto line_count = execute(SCI_GETLINECOUNT);
	const auto current_line = getCurrentLineNumber();
	if (current_line == line_count - 1)
	{
		// Special handling if caret is at last line.
		appandGenericText(newline.toUtf8());
	}
	else
	{
		const auto eol_length = newline.length();
		const auto position = eol_length + execute(SCI_GETLINEENDPOSITION, current_line);
		insertCharsFrom(position, newline.toUtf8());
	}
	execute(SCI_SETEMPTYSELECTION, execute(SCI_POSITIONFROMLINE, current_line + 1));
}

void ScintillaEditView::sortLines(size_t fromLine, size_t toLine, ISorter* pSort)
{
	if (fromLine >= toLine)
	{
		return;
	}

	const auto startPos = execute(SCI_POSITIONFROMLINE, fromLine);
	const auto endPos = execute(SCI_POSITIONFROMLINE, toLine) + execute(SCI_LINELENGTH, toLine);

	const QString text = getGenericTextAsQString(startPos, endPos);
	QStringList splitText = text.split(getEOLString());


	const size_t lineCount = execute(SCI_GETLINECOUNT);
	const bool sortEntireDocument = toLine == lineCount - 1;
	if (!sortEntireDocument)
	{
		if (splitText.rbegin()->isEmpty())
		{
			splitText.pop_back();
		}
	}
	assert(toLine - fromLine + 1 == splitText.size());
	const QList<QString> sortedText = pSort->sort(splitText);

	QString joined = sortedText.join(getEOLString());
	if (sortEntireDocument)
	{
		assert(joined.length() == text.length());
	}
	else
	{
		assert(joined.length() + getEOLString().length() == text.length());
		joined += getEOLString();
	}
	if (text != joined)
	{
		QByteArray bytes = joined.toUtf8();
		replaceTarget(bytes, startPos, endPos);
	}
}
