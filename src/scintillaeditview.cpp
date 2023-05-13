#include "scintillaeditview.h"

#include "common.h"
#include "rgba_icons.h"
#include "ccnotepad.h"
#include "styleset.h"
#include "qtlangset.h"
#include "findwin.h"
#include "filemanager.h"
#include "shortcutkeymgr.h"
#include "markdownview.h"

#include <Scintilla.h>
#include <SciLexer.h>
#include <QImage>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerasm.h>
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
#include <Qsci/qscilexerr.h>
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
#include <Qsci/qscilexerglobal.h>
#include <Qsci/qscilexerrust.h>
#include <Qsci/qscilexervb.h>
#include <QScrollBar>
#include <unordered_set>
#include <QClipboard>
#include <QDesktopServices>
#include <QDebug>
#include <QMessageBox>


#include <stdexcept>
#include <mutex>



// initialize the static variable
#define DEFAULT_FONT_NAME "Courier New" //"Microsoft YaHei"  


int ScintillaEditView::s_tabLens = 4;
//默认不使用tab，使用space替换
bool ScintillaEditView::s_noUseTab = true;

int ScintillaEditView::s_bigTextSize = 100;


const int ScintillaEditView::_SC_MARGE_LINENUMBER = 0;
const int ScintillaEditView::_SC_MARGE_SYBOLE = 1;
const int ScintillaEditView::_SC_MARGE_FOLDER = 2;

//大文本是分块显示，务必要把真实的偏移行号给显示出来
const int SC_BIGTEXT_LINES = 0;

const int MAX_PRE_NEXT_TIMES = 30;

const int INIT_BIG_RO_TEXT_LINE_WIDTH = 8;

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

ScintillaEditView::ScintillaEditView(QWidget *parent,bool isBigText)
	: QsciScintilla(parent), m_NoteWin(nullptr), m_preFirstLineNum(0), m_curPos(0), m_hasHighlight(false), m_bookmarkPng(nullptr), m_styleColorMenu(nullptr), m_isBigText(isBigText), m_curBlockLineStartNum(0)
#ifdef Q_OS_WIN
    ,m_isInTailStatus(false)
#endif
{
	init();
}

ScintillaEditView::~ScintillaEditView()
{
	releaseAllMark();

	if (m_bookmarkPng != nullptr)
	{
		delete m_bookmarkPng;
}
#ifdef Q_OS_WIN
	deleteTailFileThread();
#endif
}

ScintillaEditView::ScintillaEditView():QsciScintilla(nullptr),m_NoteWin(nullptr), m_preFirstLineNum(0), m_curPos(0), m_hasHighlight(false), m_bookmarkPng(nullptr), m_styleColorMenu(nullptr), m_isBigText(false), m_curBlockLineStartNum(0)
#ifdef Q_OS_WIN
, m_isInTailStatus(false)
#endif
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
}

ScintillaEditView* ScintillaEditView::createEditForSearch()
{
	return new ScintillaEditView();
}


//截获ESC键盘，让界面去退出当前的子界面
void ScintillaEditView::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		if (m_NoteWin != nullptr)
		{
			m_NoteWin->on_quitActiveWindow();
		}
		break;
	default:
		break;
	}
	return QsciScintilla::keyPressEvent(event);
}


void ScintillaEditView::mouseReleaseEvent(QMouseEvent* ev)
{
	QsciScintilla::mouseReleaseEvent(ev);

	if (ev->button() == Qt::LeftButton)
	{
		if (hasSelectedText())
		{
			emit delayWork();
		}
	}
}


void ScintillaEditView::setBigTextMode(bool isBigText)
{
	m_isBigText = isBigText;
}

void ScintillaEditView::setLexer(QsciLexer * lexer)
{
	QsciScintilla::setLexer(lexer);

	if (lexer != nullptr && lexer->lexerId() == L_TXT)
	{
		showMargin(_SC_MARGE_FOLDER,false);
		updateThemes();
	}
	else if(lexer != nullptr && lexer->lexerId() != L_TXT)
	{
		showMargin(_SC_MARGE_FOLDER, true);
		updateThemes();
}
}

void ScintillaEditView::setNoteWidget(QWidget * win)
{
	CCNotePad* pv = dynamic_cast<CCNotePad*>(win);
	if (pv != nullptr)
	{
		m_NoteWin = pv;

		if (m_styleColorMenu != nullptr)
		{
			m_styleColorMenu->addAction(tr("Clear Select"), m_NoteWin, &CCNotePad::slot_clearWordHighlight);
			m_styleColorMenu->addAction(tr("Clear All"), m_NoteWin, &CCNotePad::slot_clearMark);
}
}
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

//根据现有滚动条来决定是否更新屏幕线宽长度。每滚动200个单位必须调整line宽
void ScintillaEditView::autoAdjustLineWidth(int xScrollValue)
{
	//如果是大文本模式，行号长度目前是固定不变的，不需要动态调整。
	if (m_isBigText)
	{
		return;
	}
	if (std::abs(xScrollValue - m_preFirstLineNum) > 200)
	{
		m_preFirstLineNum = xScrollValue;

		updateLineNumberWidth(1);
	}
}

void ScintillaEditView::updateLineNumberWidth(int lineNumberMarginDynamicWidth)
{
	if (!m_isBigText)
	{
	auto linesVisible = execute(SCI_LINESONSCREEN);
	if (linesVisible)
	{
		int nbDigits = 0;

			if (lineNumberMarginDynamicWidth != 0)
		{
			int firstVisibleLineVis = execute(SCI_GETFIRSTVISIBLELINE);
			int lastVisibleLineVis = linesVisible + firstVisibleLineVis + 1;
			int lastVisibleLineDoc = execute(SCI_DOCLINEFROMVISIBLE, (long)lastVisibleLineVis);

			nbDigits = nbDigitsFromNbLines(lastVisibleLineDoc);
			nbDigits = nbDigits < 4 ? 4 : nbDigits;
		}
		else
		{
			int nbLines = execute(SCI_GETLINECOUNT);
		nbDigits = nbDigitsFromNbLines(nbLines);
			nbDigits = nbDigits < 4 ? 4 : nbDigits;
		}

		auto pixelWidth = 6 + nbDigits * execute(SCI_TEXTWIDTH, STYLE_LINENUMBER, reinterpret_cast<sptr_t>("8"));
		execute(SCI_SETMARGINWIDTHN, _SC_MARGE_LINENUMBER, pixelWidth);

			
	}
	}
	else
	{
		int pixelWidth = 6 + INIT_BIG_RO_TEXT_LINE_WIDTH * execute(SCI_TEXTWIDTH, STYLE_LINENUMBER, reinterpret_cast<sptr_t>("8"));
		execute(SCI_SETMARGINWIDTHN, SC_BIGTEXT_LINES, pixelWidth);
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
			width = 14;
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


QString ScintillaEditView::getTagByLexerId(int lexerId)
{
	switch (lexerId)
	{
	case L_PHP:
		return "php";

	case L_HTML:
		return ("html");

	case L_ASP:
		return ("asp");

	case L_JSP:
		return("jsp");

	case L_C:
		return("c");

	case L_RC:
		return("rc");

	case L_CPP:
		return "cpp";

	case L_OBJC:
		return ("objc");
	
	case L_CS:
		return ("csharp");

	case L_JAVA:
		return ("java");

	case L_XML:
		return "xml";

	case L_MAKEFILE:
		return "makefile";

	case L_PASCAL:
		return "pascal";

	case L_BATCH:
		return "batch";

	case L_INI:
		return("ini");

	case L_ASCII:
		break;

	case L_USER:
		break;

	case L_SQL:
		return "sql";
		
	case L_VB:
		return "vb";

	case L_CSS:
		return "css";

	case L_PERL:
		return "perl";

	case L_PYTHON:
		return "python";

	case L_LUA:
		return "lua";

	case L_TEX:
		break;
	case L_FORTRAN:
		return "fortran";

	case L_BASH:
		return "bash";

	case L_FLASH:
		return("flash");

	case L_MATLAB:
		return ("matlab");

	case L_MARKDOWN:
		return("markdown");

	case L_NSIS:
		return "nsis";

	case L_TCL:
		return "tcl";

	case L_LISP:
		break;
	case L_SCHEME:
		break;
	case L_ASM:
		break;
	case L_DIFF:
		return "diff";

	case L_PROPS:
		return "props";

	case L_PS:
		break;

	case L_R:
		return "r";

	case L_RUBY:
		return "ruby";

	case L_SMALLTALK:
		break;
	case L_VHDL:
		return "vhdl";

	case L_KIX:
		break;
	case L_AU3:
		break;
	case L_CAML:
		break;
	case L_ADA:
		break;
	case L_VERILOG:
		return "verilog";

	case L_HASKELL:
		break;
	case L_INNO:
		break;
	case L_SEARCHRESULT:
		break;
	case L_CMAKE:
		return "cmake";

	case L_YAML:
		return "yaml";

	case L_COBOL:
		break;
	case L_GUI4CLI:
		break;
	case L_D:
		break;
	case L_POWERSHELL:
		break;

	case L_COFFEESCRIPT:
		return "coffeescript";

	case L_JSON:
		return "json";

	case L_JAVASCRIPT:
		return ("javascript");

	case L_FORTRAN_77:
		return "fortran77";

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
	//case L_AVS:
	//	return "avs";

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
		return "rust";
	case L_SPICE:
		return "spice";
	case L_TXT2TAGS:
		break;
	case L_VISUALPROLOG:
		break;
	case L_TYPESCRIPT:
		return("typescript");

	case L_EXTERNAL:
		break;
	case L_IDL:
		return("idl");

	case L_GO:
		return("go");

	case L_GLOBAL:
		return("AllGlobal");

	case L_TXT:
		return("txt");

	case L_USER_TXT:
		break;
	case L_USER_CPP:

		break;
	default:
		break;
	}
	
	return "";
}

//status : true 表示存在， false 表示不存在
//tag，只有在用户自定义语法是，才需要给出。内部自带的语法不需要给出
//isOrigin:是否原生lexer，即不读取用户修改过的配置风格
QsciLexer* ScintillaEditView::createLexer(int lexerId, QString tag, bool isOrigin, int styleId)
{
	QsciLexer* ret = nullptr;

	switch (lexerId)
	{
	case L_PHP:
		ret = new QsciLexerHTML();
		ret->setLexerTag("php");
		ret->setCommentLineSymbol("//");
		ret->setCommentStart("/*");
		ret->setCommentEnd("*/");
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
		ret = new QsciLexerVB();
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
	case L_MATLAB:
		ret = new QsciLexerMatlab();
		break;
	case L_MARKDOWN:
		ret = new QsciLexerMarkdown();
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
		ret = new QsciLexerAsm();
		break;
	case L_DIFF:
		ret = new QsciLexerDiff();
		break;
	case L_PROPS:
		ret = new QsciLexerProperties();
		break;
	case L_PS:
		break;
	case L_R:
		ret = new QsciLexerR();
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
		//ret = new QsciLexerAVS();
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
		ret = new QsciLexerRust();
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
	case L_GLOBAL:
		ret = new QsciLexerGlobal();
		ret->setLexerTag("AllGlobal");
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

		if (!isOrigin)
		{
			QtLangSet::readLangSettings(ret, ret->lexerTag(), styleId);
	}
		else
		{
			//如果是读取原生风格，则只有默认主题具备，其余主题要从模板中加载
			QtLangSet::readLangOriginSettings(ret, ret->lexerTag(), styleId);
	}
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

////调整颜色
//void ScintillaEditView::adjuctSkinStyle()
//{
//	if (StyleSet::m_curStyleId != BLACK_SE)
//	{
//		setMarginsForegroundColor(QColor(0x80, 0x80, 0x80)); //默认0x80, 0x80, 0x80
//	}
//	else
//	{
//		setMarginsForegroundColor(QColor(0xde, 0xde, 0xde)); //默认0x80, 0x80, 0x80
//	}
//	setMarginsBackgroundColor(StyleSet::marginsBackgroundColor);
//
//	setMarginBackgroundColor(_SC_MARGE_SYBOLE, StyleSet::bookmarkBkColor);
//	setFoldMarginColors(StyleSet::marginsBackgroundColor, StyleSet::marginsBackgroundColor);
//	setStyleOptions();
//}

void ScintillaEditView::setIndentGuide(bool willBeShowed)
{
	QsciLexer* pLexer = this->lexer(); 
	if (nullptr == pLexer)
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

	if (m_isBigText)
	{
		this->setMarginType(SC_BIGTEXT_LINES, TextMargin);
	}
	else
	{
	//开启行号marge
	setMarginLineNumbers(_SC_MARGE_LINENUMBER, true);
		showMargin(_SC_MARGE_LINENUMBER, true);
	//通过fold发现，尽量使用qscint的功能，因为他做了大量封装和简化
	setFolding(BoxedTreeFoldStyle, _SC_MARGE_FOLDER);
	}

	//行号、符号、折叠。中间符号留一个很小的间隔。用于圆形的标记符号
	showMargin(_SC_MARGE_SYBOLE, true);
	setMarginType(_SC_MARGE_SYBOLE, QsciScintilla::SymbolMarginColor);

	m_bookmarkPng = new QPixmap(QString(":/Resources/img/bookmark.png"));
	markerDefine(*m_bookmarkPng, _SC_MARGE_SYBOLE);
	setMarginSensitivity(_SC_MARGE_SYBOLE, true);
	connect(this, &QsciScintilla::marginClicked, this, &ScintillaEditView::slot_bookMarkClicked);

	
	//开始括号匹配，比如html的<>，开启前后这类字段的匹配
	setBraceMatching(SloppyBraceMatch);

	//自动补全效果不好，不开启20211017
	//setAutoCompletionSource(QsciScintilla::AcsAPIs);   //设置源，自动补全所有地方出现的
	//setAutoCompletionCaseSensitivity(true);   //设置自动补全大小写敏感
	//setAutoCompletionThreshold(1);    //设置每输入一个字符就会出现自动补全的提示

	//设置字体
	QFont font(DEFAULT_FONT_NAME, 11, QFont::Normal);
	setFont(font);
	setMarginsFont(font);
	
	execute(SCI_SETTABWIDTH, ScintillaEditView::s_tabLens);

	//使用空格替换tab
	setIndentationsUseTabs(!ScintillaEditView::s_noUseTab);

	//这个无比要设置false，否则双击后高亮单词，拷贝时会拷贝多个选择。
	execute(SCI_SETMULTIPLESELECTION, true);
	execute(SCI_SETADDITIONALSELECTIONTYPING, true);
	
	execute(SCI_SETMULTIPASTE, SC_MULTIPASTE_EACH);
	execute(SCI_SETADDITIONALCARETSVISIBLE, true);

	execute(SCI_SETVIRTUALSPACEOPTIONS, SCVS_RECTANGULARSELECTION);

	//execute(SCI_SETSELFORE, true, 0x0);
	//execute(SCI_SETSELBACK, true, 0x9bff9b); //0x00ffff原来的黄色

	//设置查找到Mark的风格。定义其前景颜色和形状
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE_EXT5, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE_EXT5, 130);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_EXT5, false);
	//execute(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT5, 0x00ffff);

	//设置查找到Mark的风格。定义其前景颜色和形状
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE_EXT4, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE_EXT4, 130);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_EXT4, false);
	//execute(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT4, 0xffff00);

	//设置查找到Mark的风格。定义其前景颜色和形状
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE_EXT3, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE_EXT3, 130);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_EXT3, false);
	//execute(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT3, 0x0080ff);

	//设置查找到Mark的风格。定义其前景颜色和形状
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE_EXT2, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE_EXT2, 130);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_EXT2, false);
	//execute(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT2, 0xff0080);

	//设置查找到Mark的风格。定义其前景颜色和形状
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE_EXT1, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE_EXT1, 130);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_EXT1, false);
	//execute(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT1, 0xff8000);

	//下面这两个是HTML文件的tag高亮的表示。
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_TAGMATCH, INDIC_STRAIGHTBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_TAGMATCH, 100);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_TAGMATCH, false);
	//execute(SCI_INDICSETFORE, SCE_UNIVERSAL_TAGMATCH, 0xff0080);

	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_TAGATTR, INDIC_STRAIGHTBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_TAGATTR, 100);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_TAGATTR, false);
	//execute(SCI_INDICSETFORE, SCE_UNIVERSAL_TAGATTR, 0x00ffff);

	//双击后同样的字段进行高亮
	execute(SCI_INDICSETSTYLE, SCE_UNIVERSAL_FOUND_STYLE_SMART, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA, SCE_UNIVERSAL_FOUND_STYLE_SMART, 100);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_SMART, false);
	//execute(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_SMART, 0x00ff00);

	//设置空白字符的默认前景色
	//execute(SCI_SETWHITESPACEFORE, true, 0x6ab5ff); 
	execute(SCI_SETWHITESPACESIZE,3);

	setCaretLineVisible(true);

	//统一设置全局前景、背景、字体大小三个要素
	updateThemes();

	//记住position变化。不能使用cursorPositionChanged，因为他的列考虑uft8字符，中文一个也算1个，每次列不一定相等。
	//要使用自定义的cursorPosChange，跟踪的是SCI_GETCURRENTPOS 的值。换行才会触发这个cursorPosChange。自定义的信号
	connect(this,&QsciScintilla::cursorPosChange,this,&ScintillaEditView::slot_linePosChanged, Qt::QueuedConnection);

	connect(this, &QsciScintilla::selectionChanged, this, &ScintillaEditView::slot_clearHightWord, Qt::QueuedConnection);

	execute(SCI_SETSCROLLWIDTH, 1);

	connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &ScintillaEditView::slot_scrollYValueChange);
	connect(this, &ScintillaEditView::delayWork, this,&ScintillaEditView::slot_delayWork, Qt::QueuedConnection);

	//设置换行符号的格式
#if defined(Q_OS_WIN)
	execute(SCI_SETEOLMODE, SC_EOL_CRLF);
#elif !defined(Q_OS_MAC)
	execute(SCI_SETEOLMODE, SC_EOL_CR);
#elif !defined(Q_OS_UNIX)
	execute(SCI_SETEOLMODE, SC_EOL_LF);
#endif

	//开启新行自动缩进
	setAutoIndent(true);

	//开启后可以保证长行在滚动条下完整显示
	execute(SCI_SETSCROLLWIDTHTRACKING, true);

}

//大文本不能显示行号，其实显示的是每一行的地址。
//因为要跳转，所以没法实时计算当前位置所在的行号
void ScintillaEditView::showBigTextLineAddr(qint64 fileOffset)
{
	int nbDigits = 0;

	if (fileOffset < 0xffffffff)
	{
		nbDigits = 9;
	}
	else
	{
		nbDigits = 13;
	}
	char* lineString = new char[17];
	memset(lineString, 0, 17);

	auto pixelWidth = 6 + nbDigits * execute(SCI_TEXTWIDTH, STYLE_LINENUMBER, reinterpret_cast<sptr_t>("8"));
	this->execute(SCI_SETMARGINWIDTHN, SC_BIGTEXT_LINES, pixelWidth);

	int lineNums = this->lines();
	int lineLength = 0;

	int style = STYLE_LINENUMBER;

	for (int i = 0; i < lineNums; ++i)
	{
		memset(lineString, 0, 17);

		lineLength = this->lineLength(i);

		if (fileOffset < 0xffffffff)
		{
			sprintf(lineString, "%08llX ", fileOffset);
		}
		else
		{
			sprintf(lineString, "%012llX ", fileOffset);
		}

		QString num(lineString);

		fileOffset += lineLength;
		this->setMarginText(i, num, style);
	}

	delete[]lineString;
}

void ScintillaEditView::clearSuperBitLineCache()
{
	m_addrLineNumMap.clear();
}
//20230116新增，尽可能的还是显示行号。如果发生了跳转，则没有办法计算前面的行号，
//则只能显示地址。如果没跳转，而是动态顺序翻页，则可以显示行号
//20230201发现一个问题。底层qscint是按照utf8字节流来计算字符大小的。如果原始文件的编码
//不是utf8,比如GBK LE等，则大小是不能统一的。这是一个显示问题，但是不影响什么。
//通过this->lineLength(i);来计算是以utf8计算。
void ScintillaEditView::showBigTextLineAddr(qint64 fileOffset, qint64 fileEndOffset)
{
	int nbDigits = 0;

	if (fileOffset < 0xffffffff)
	{
		nbDigits = INIT_BIG_RO_TEXT_LINE_WIDTH;
	}
	else
	{
		nbDigits = 12;
	}
	char* lineString = new char[17];
	memset(lineString, 0, 17);

	auto pixelWidth = 6 + nbDigits * execute(SCI_TEXTWIDTH, STYLE_LINENUMBER, reinterpret_cast<sptr_t>("8"));
	this->execute(SCI_SETMARGINWIDTHN, SC_BIGTEXT_LINES, pixelWidth);

	int lineNums = this->lines();
	int lineLength = 0;

	int style = STYLE_LINENUMBER;

	quint32 startLineNumOffset = 0;
	quint32 endLineNumOffset = 0;

	bool isShowLineNum = false; //显示行号还是地址
	bool startLineExist = false;
	bool endLineExist = false;


	if (fileOffset == 0)
	{
		isShowLineNum = true;
		startLineExist = true;
		m_addrLineNumMap.insert(0, 1); //0地址对应第1行
		m_addrLineNumMap.insert(fileEndOffset, lineNums+1); //fileEndOffset地址对应最后一行
	}

	if (lineNums >= 1)
	{
		if (m_addrLineNumMap.contains(fileOffset))
		{
			isShowLineNum = true;
			startLineExist = true;

			startLineNumOffset = m_addrLineNumMap.value(fileOffset);
		}
		else if (m_addrLineNumMap.contains(fileEndOffset))
		{
			isShowLineNum = true;
			endLineExist = true;

			endLineNumOffset = m_addrLineNumMap.value(fileEndOffset);
		}
	}

	//不存在行号，只能显示地址
	if (!isShowLineNum)
	{
		for (int i = 0; i < lineNums; ++i)
		{
			memset(lineString, 0, 17);

			lineLength = this->lineLength(i);

			if (fileOffset < 0xffffffff)
			{
				sprintf(lineString, "%08llX ", fileOffset);
			}
			else
			{
				sprintf(lineString, "%012llX ", fileOffset);
			}

			QString num(lineString);

			fileOffset += lineLength;
			this->setMarginText(i, num, style);
		}
	}
	else
	{
		//首行地址存在，从头到尾增加行号
		if (startLineExist)
		{
	
			for (int i = 0; i < lineNums; ++i)
			{
				if (i == (lineNums - 1))
				{
					//m_addrLineNumMap.insert(fileOffset, startLineNumOffset + i);
					m_addrLineNumMap.insert(fileEndOffset, startLineNumOffset + i);
				}

				memset(lineString, 0, 17);
				//lineLength = this->lineLength(i);
				sprintf(lineString, "%08lld ", startLineNumOffset + i);
				QString num(lineString);

				//fileOffset += lineLength;
				this->setMarginText(i, num, style);
			}
		}
		else if (endLineExist)
		{

			startLineNumOffset = endLineNumOffset - lineNums;

			for (int i = 0; i < lineNums; ++i)
			{
				if (i == 0)
				{
					//m_addrLineNumMap.insert(fileOffset, startLineNumOffset + i);
					m_addrLineNumMap.insert(fileOffset, startLineNumOffset + i);
				}

				memset(lineString, 0, 17);
				//lineLength = this->lineLength(i);
				sprintf(lineString, "%08lld ", startLineNumOffset + i);
				QString num(lineString);

				//fileOffset += lineLength;
				this->setMarginText(i, num, style);
			}
		}
	}

	delete[]lineString;
}

//大文本只读模式下，显示其文本
void ScintillaEditView::showBigTextRoLineNum(BigTextEditFileMgr* txtFile, int blockIndex)
{

	BlockIndex bi = txtFile->blocks.at(blockIndex);

	int nbDigits = 0;

	if (bi.fileOffset < 0xffffffff)
	{
		nbDigits = INIT_BIG_RO_TEXT_LINE_WIDTH;
	}
	else
	{
		nbDigits = 12;
	}
	char* lineString = new char[17];
	memset(lineString, 0, 17);

	auto pixelWidth = 6 + nbDigits * execute(SCI_TEXTWIDTH, STYLE_LINENUMBER, reinterpret_cast<sptr_t>("8"));
	this->execute(SCI_SETMARGINWIDTHN, SC_BIGTEXT_LINES, pixelWidth);

	int lineNums = this->lines();

	qint64 curLineNum = bi.lineNumStart + 1;//行号从1开始

	int style = STYLE_LINENUMBER;

	for (int i = 0; i < lineNums; ++i)
	{
		memset(lineString, 0, 17);

		if (bi.fileOffset < 0xffffffff)
		{
			sprintf(lineString, "%08lld ", curLineNum+i);
		}
		else
		{
			sprintf(lineString, "%012lld ", curLineNum + i);
		}

		QString num(lineString);
		this->setMarginText(i, num, style);
	}

	delete[]lineString;
}

void ScintillaEditView::bookmarkNext(bool forwardScan)
{
	size_t lineno = this->getCurrentLineNumber();
	int sci_marker = SCI_MARKERNEXT;
	size_t lineStart = lineno + 1;	//Scan starting from next line
	int lineRetry = 0;				//If not found, try from the beginning
	if (!forwardScan)
	{
		lineStart = lineno - 1;		//Scan starting from previous line
		lineRetry = int(this->execute(SCI_GETLINECOUNT));	//If not found, try from the end
		sci_marker = SCI_MARKERPREVIOUS;
	}
	int nextLine = int(this->execute(sci_marker, lineStart, 1 << _SC_MARGE_SYBOLE));
	if (nextLine < 0)
		nextLine = int(this->execute(sci_marker, lineRetry, 1 << _SC_MARGE_SYBOLE));

	if (nextLine < 0)
		return;

	this->execute(SCI_ENSUREVISIBLEENFORCEPOLICY, nextLine);
	this->execute(SCI_GOTOLINE, nextLine);
}

void ScintillaEditView::bookmarkAdd(intptr_t lineno) const {
	if (lineno == -1)
		lineno = getCurrentLineNumber();
	if (!bookmarkPresent(lineno))
		this->execute(SCI_MARKERADD, lineno, _SC_MARGE_SYBOLE);
}

void ScintillaEditView::bookmarkDelete(size_t lineno) const {
	if (lineno == -1)
		lineno = this->getCurrentLineNumber();
	while (bookmarkPresent(lineno))
		this->execute(SCI_MARKERDELETE, lineno, _SC_MARGE_SYBOLE);
}

bool ScintillaEditView::bookmarkPresent(intptr_t lineno) const {
	if (lineno == -1)
		lineno = this->getCurrentLineNumber();
	int state = this->execute(SCI_MARKERGET, lineno);
	return ((state & (1 << _SC_MARGE_SYBOLE)) != 0);
}

void ScintillaEditView::bookmarkToggle(intptr_t lineno) const {
	if (lineno == -1)
		lineno = this->getCurrentLineNumber();

	if (bookmarkPresent(lineno))
		bookmarkDelete(lineno);
	else
		bookmarkAdd(lineno);
}

void ScintillaEditView::bookmarkAdd(QSet<int>& lineSet)
{
	QSet<int>::const_iterator i = lineSet.constBegin();
	while (i != lineSet.constEnd()) {
		bookmarkAdd(*i);
		++i;
	}
}


void ScintillaEditView::bookmarkClearAll() const {
	this->execute(SCI_MARKERDELETEALL, _SC_MARGE_SYBOLE);
}

void ScintillaEditView::slot_bookMarkClicked(int margin, int line, Qt::KeyboardModifiers state)
{
	if (margin == _SC_MARGE_SYBOLE)
	{
		bookmarkToggle(line);
	}
}

std::mutex mark_mutex;

QString ScintillaEditView::getMarkedLine(int ln)
{
	auto lineLen = this->execute(SCI_LINELENGTH, ln);
	auto lineBegin = this->execute(SCI_POSITIONFROMLINE, ln);

	return this->getGenericTextAsQString(lineBegin, lineBegin + lineLen);
}

void ScintillaEditView::deleteMarkedline(int ln)
{
	int lineLen = static_cast<int32_t>(this->execute(SCI_LINELENGTH, ln));
	int lineBegin = static_cast<int32_t>(this->execute(SCI_POSITIONFROMLINE, ln));
	bookmarkDelete(ln);

	QByteArray str2replace;

	this->replaceTarget(str2replace, lineBegin, lineBegin + lineLen);
}

void str2Cliboard(QString& text)
{
	QClipboard* clip = QApplication::clipboard();
	clip->setText(text);
}

void ScintillaEditView::cutMarkedLines()
{
	std::lock_guard<std::mutex> lock(mark_mutex);

	int lastLine = this->lastZeroBasedLineNumber();
	QString globalStr;

	this->execute(SCI_BEGINUNDOACTION);
	for (int i = lastLine; i >= 0; i--)
	{
		if (bookmarkPresent(i))
		{
			QString currentStr = getMarkedLine(i) + globalStr;
			globalStr = currentStr;

			deleteMarkedline(i);
		}
	}
	this->execute(SCI_ENDUNDOACTION);
	str2Cliboard(globalStr);
}

void ScintillaEditView::copyMarkedLines()
{
	int lastLine = this->lastZeroBasedLineNumber();
	QString globalStr;
	for (int i = lastLine; i >= 0; i--)
	{
		if (bookmarkPresent(i))
		{
			QString currentStr = getMarkedLine(i) + globalStr;
			globalStr = currentStr;
		}
	}
	str2Cliboard(globalStr);
}

void ScintillaEditView::replaceMarkedline(int ln, QByteArray& str)
{
	int lineBegin = static_cast<int32_t>(this->execute(SCI_POSITIONFROMLINE, ln));
	int lineEnd = static_cast<int32_t>(this->execute(SCI_GETLINEENDPOSITION, ln));

	this->replaceTarget(str, lineBegin, lineEnd);
}

void ScintillaEditView::pasteToMarkedLines()
{
	std::lock_guard<std::mutex> lock(mark_mutex);

	QClipboard* clip = QApplication::clipboard();
	const  QMimeData *mimeData = clip->mimeData();
	if (!mimeData->hasText())
	{
		return;
	}

	int lastLine = this->lastZeroBasedLineNumber();
	QString clipboardStr = clip->text();

	this->execute(SCI_BEGINUNDOACTION);
	for (int i = lastLine; i >= 0; i--)
	{
		if (bookmarkPresent(i))
		{
			QByteArray text = clipboardStr.toUtf8();
			replaceMarkedline(i, text);
		}
	}
	this->execute(SCI_ENDUNDOACTION);
}

//Y方向滚动条值变化后的槽函数
void ScintillaEditView::slot_scrollYValueChange(int value)
{
	//使用滚动条翻页的效果并不好。暂时不放开
	/*bool changePage = false;

	qDebug() << value << verticalScrollBar()->maximum() << verticalScrollBar()->minimum();

	if (value >= this->verticalScrollBar()->maximum())
	{
		if (m_NoteWin != nullptr)
		{
			changePage = m_NoteWin->nextPage(this);
		}
	}
	else if (value == this->verticalScrollBar()->minimum())
	{
		if (m_NoteWin != nullptr)
		{
			changePage = m_NoteWin->prePage(this);
		}
	}*/

	if (value >= this->verticalScrollBar()->maximum())
	{
		if (m_NoteWin != nullptr)
		{
			m_NoteWin->showChangePageTips(this);
		}
	}
	else if (value == this->verticalScrollBar()->minimum())
	{
		if (m_NoteWin != nullptr)
		{
			m_NoteWin->showChangePageTips(this);
		}
	}

	slot_delayWork();
	autoAdjustLineWidth(value);

}


void ScintillaEditView::deleteMarkedLines(bool isMarked)
{
	std::lock_guard<std::mutex> lock(mark_mutex);

	int lastLine = this->lastZeroBasedLineNumber();

	this->execute(SCI_BEGINUNDOACTION);
	for (int i = lastLine; i >= 0; i--)
	{
		if (bookmarkPresent(i) == isMarked)
			deleteMarkedline(i);
	}
	this->execute(SCI_ENDUNDOACTION);
}

void ScintillaEditView::inverseMarks()
{
	int lastLine = this->lastZeroBasedLineNumber();
	for (int i = 0; i <= lastLine; ++i)
	{
		if (bookmarkPresent(i))
		{
			bookmarkDelete(i);
		}
		else
		{
			bookmarkAdd(i);
		}
	}
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


intptr_t ScintillaEditView::searchInTarget(QByteArray& text2Find,size_t fromPos, size_t toPos) const
{
	execute(SCI_SETTARGETRANGE, fromPos, toPos);
	return execute(SCI_SEARCHINTARGET, text2Find.size(), reinterpret_cast<sptr_t>(text2Find.data()));
}

intptr_t ScintillaEditView::replaceTargetRegExMode(QByteArray& re, intptr_t fromTargetPos, intptr_t toTargetPos) const
{
	if (fromTargetPos != -1 || toTargetPos != -1)
	{
		execute(SCI_SETTARGETRANGE, fromTargetPos, toTargetPos);
	}
	return execute(SCI_REPLACETARGETRE, re.size(), reinterpret_cast<sptr_t>(re.data()));
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

	SendScintilla(SCI_SETSEARCHFLAGS,  SCFIND_MATCHCASE /*| SCFIND_WHOLEWORD*/ | SCFIND_REGEXP_SKIPCRLFASONE);

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

void ScintillaEditView::slot_markColorGroup(QAction *action)
{
	CCNotePad::s_curMarkColorId = action->data().toInt();
	if (m_NoteWin != nullptr)
	{
		m_NoteWin->slot_wordHighlight();
	}
}

//修改标记样式的颜色
void ScintillaEditView::changeStyleColor(int sytleId)
{
	if (m_styleColorMenu == nullptr)
	{
		initStyleColorMenu();
	}
	if( sytleId < 5)
	{
		QPixmap colorBar(36, 36);
		colorBar.fill((&StyleSet::s_global_style->mark_style_1)[sytleId].bgColor);
		m_styleMarkActList.at(sytleId)->setIcon(colorBar);

		if (m_NoteWin != nullptr)
		{
			m_NoteWin->changeMarkColor(sytleId);
		}
	}
}

void ScintillaEditView::initStyleColorMenu()
{
	if (m_styleColorMenu == nullptr)
	{
		m_styleColorMenu = new QMenu(tr("mark with color"), this);
		QPixmap colorBar(36, 36);

		QActionGroup* markColorGroup = new QActionGroup(this);
		connect(markColorGroup, &QActionGroup::triggered, this, &ScintillaEditView::slot_markColorGroup, Qt::QueuedConnection);

		int index = 1;
		auto initColorBar = [this, markColorGroup, &index](QPixmap& colorBar)->QAction* {
			QAction* action = new QAction(m_styleColorMenu);
			action->setIcon(colorBar);
			action->setCheckable(true);
			action->setText(tr("Color %1").arg(index));
			action->setData(index + SCE_UNIVERSAL_FOUND_STYLE_EXT5 - 1);
			++index;
			m_styleColorMenu->addAction(action);
			markColorGroup->addAction(action);
			return action;
		};

		colorBar.fill(StyleSet::s_global_style->mark_style_1.bgColor);
		m_styleMarkActList.append(initColorBar(colorBar));

		colorBar.fill(StyleSet::s_global_style->mark_style_2.bgColor);
		m_styleMarkActList.append(initColorBar(colorBar));

		colorBar.fill(StyleSet::s_global_style->mark_style_3.bgColor);
		m_styleMarkActList.append(initColorBar(colorBar));

		colorBar.fill(StyleSet::s_global_style->mark_style_4.bgColor);
		m_styleMarkActList.append(initColorBar(colorBar));

		colorBar.fill(StyleSet::s_global_style->mark_style_5.bgColor);
		m_styleMarkActList.append(initColorBar(colorBar));


		m_styleColorMenu->addSeparator();

		if (m_NoteWin != nullptr)
		{
		m_styleColorMenu->addAction(tr("Clear Select"), m_NoteWin, &CCNotePad::slot_clearWordHighlight);
		m_styleColorMenu->addAction(tr("Clear All"), m_NoteWin, &CCNotePad::slot_clearMark);
	}
}
}

void ScintillaEditView::contextUserDefineMenuEvent(QMenu* menu)
{
	//QAction* action;
	if (menu != nullptr && (m_NoteWin !=nullptr))
	{
		menu->addAction(tr("Show File in Explorer"), m_NoteWin, &CCNotePad::slot_showFileInExplorer);

		if (m_styleColorMenu == nullptr)
		{
			m_styleColorMenu = new QMenu(tr("mark with color"),this);
			QPixmap colorBar(36, 36);

			QActionGroup* markColorGroup = new QActionGroup(this);
			connect(markColorGroup, &QActionGroup::triggered, this, &ScintillaEditView::slot_markColorGroup, Qt::QueuedConnection);

			int index = 1;
			auto initColorBar = [this, markColorGroup, &index](QPixmap& colorBar)->QAction* {
				QAction* action = new QAction(m_styleColorMenu);
				action->setIcon(colorBar);
				action->setCheckable(true);
				action->setText(tr("Color %1").arg(index));
				action->setData(index + SCE_UNIVERSAL_FOUND_STYLE_EXT5-1);
				++index;
				m_styleColorMenu->addAction(action);
				markColorGroup->addAction(action);
				return action;
			};

			colorBar.fill(StyleSet::s_global_style->mark_style_1.fgColor);
			m_styleMarkActList.append(initColorBar(colorBar));

			colorBar.fill(StyleSet::s_global_style->mark_style_2.fgColor);
			m_styleMarkActList.append(initColorBar(colorBar));

			colorBar.fill(StyleSet::s_global_style->mark_style_3.fgColor);
			m_styleMarkActList.append(initColorBar(colorBar));

			colorBar.fill(StyleSet::s_global_style->mark_style_4.fgColor);
			m_styleMarkActList.append(initColorBar(colorBar));

			colorBar.fill(StyleSet::s_global_style->mark_style_5.fgColor);
			m_styleMarkActList.append(initColorBar(colorBar));


			m_styleColorMenu->addSeparator();

			m_styleColorMenu->addAction(tr("Clear Select"), m_NoteWin, &CCNotePad::slot_clearWordHighlight);
			m_styleColorMenu->addAction(tr("Clear All"), m_NoteWin, &CCNotePad::slot_clearMark);
	}
		menu->addMenu(m_styleColorMenu);

		menu->addSeparator();

		menu->addAction(tr("Add/Del line comment"), [this]() {
			doBlockComment(cm_toggle);
		});
		menu->addAction(tr("Add Block comment"), [this]() {
			doStreamComment();
		});
		menu->addAction(tr("Del Block comment"), [this]() {
			undoStreamComment();
		});

		menu->addSeparator();

		menu->addAction(tr("Markdown View"), this, &ScintillaEditView::on_viewMarkdown);

		menu->addAction(tr("Word Count"), [this]() {
			showWordNums();
			});
		
	}
	menu->show();
}

void ScintillaEditView::replaceSelWith(const char* replaceText)
{
	execute(SCI_REPLACESEL, 0, reinterpret_cast<sptr_t>(replaceText));
}

bool ScintillaEditView::doBlockComment(Comment_Mode currCommentMode)
{
	if (this->isReadOnly())
	{
		return false;
	}

	//-- BlockToStreamComment:
	QByteArray commentStart;
	QByteArray commentEnd;

	QByteArray symbolStart;
	QByteArray symbolEnd;

	QByteArray commentLineSymbol;
	QByteArray symbol;


	bool isSingleLineAdvancedMode = false;
	QsciLexer* pLexer = this->lexer();

	if (pLexer == nullptr)
	{
		return false;
	}
	else
	{
		commentLineSymbol = pLexer->getCommentLineSymbol();
		commentStart = pLexer->getCommentStart();
		commentEnd = pLexer->getCommentEnd();
	}

	if ((commentLineSymbol.isEmpty()) || commentLineSymbol.isEmpty())
	{
		// BlockToStreamComment: If there is no block-comment symbol, try the stream comment:
		if (!(commentStart.isEmpty() || commentEnd.isEmpty()))
		{
			if (currCommentMode == cm_comment)
			{
				isSingleLineAdvancedMode = true;

			}
			else if (currCommentMode == cm_uncomment)
			{
				return undoStreamComment(false);
			}
			else if (currCommentMode == cm_toggle)
			{
				isSingleLineAdvancedMode = true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	//For Single Line NORMAL MODE
	QByteArray comment;
	size_t comment_length = 0;

	//For Single Line ADVANCED MODE
	QByteArray advCommentStart;
	QByteArray advCommentEnd;
	size_t advCommentStart_length = 0;
	size_t advCommentEnd_length = 0;

	QByteArray aSpace(" ");

	//Only values that have passed through will be assigned, to be sure they are valid!
	if (!isSingleLineAdvancedMode)
	{
		comment = commentLineSymbol;

		if (!(pLexer->lexerId() == L_BAANC)) // BaanC standardization - no space.
		{
			comment += aSpace;
		}

		comment_length = comment.length();
	}
	else // isSingleLineAdvancedMode
	{
		advCommentStart = commentStart;
		advCommentStart += aSpace;
		advCommentEnd = aSpace;
		advCommentEnd += commentEnd;

		advCommentStart_length = advCommentStart.length();
		advCommentEnd_length = advCommentEnd.length();
	}

	size_t selectionStart = this->execute(SCI_GETSELECTIONSTART);
	size_t selectionEnd = this->execute(SCI_GETSELECTIONEND);
	size_t caretPosition = this->execute(SCI_GETCURRENTPOS);

	bool move_caret = caretPosition < selectionEnd;
	intptr_t selStartLine = this->execute(SCI_LINEFROMPOSITION, selectionStart);
	intptr_t selEndLine = this->execute(SCI_LINEFROMPOSITION, selectionEnd);
	intptr_t lines = selEndLine - selStartLine;

	if ((lines > 0) && (selectionEnd == static_cast<size_t>(this->execute(SCI_POSITIONFROMLINE, selEndLine))))
	{
		selEndLine--;
	}

	// count lines which were un-commented to decide if undoStreamComment() shall be called.
	int nUncomments = 0;
	//Some Lexers need line-comments at the beginning of a line.
	const bool avoidIndent = (pLexer->lexerId() == L_FORTRAN_77 || pLexer->lexerId() == L_BAANC);
	//Some Lexers comment blank lines, per their standards.
	const bool commentEmptyLines = (pLexer->lexerId() == L_BAANC);

	this->execute(SCI_BEGINUNDOACTION);

	for (intptr_t i = selStartLine; i <= selEndLine; ++i)
	{
		size_t lineStart = this->execute(SCI_POSITIONFROMLINE, i);
		size_t lineIndent = this->execute(SCI_GETLINEINDENTPOSITION, i);
		size_t lineEnd = this->execute(SCI_GETLINEENDPOSITION, i);

		// empty lines are not commented, unless required by the language.
		if (lineIndent == lineEnd && !commentEmptyLines)
			continue;

		if (avoidIndent)
			lineIndent = lineStart;

		//这里linebufferSize不需要包含字符串后面的\0，所以不需要预留1个\0符号
		size_t linebufferSize = lineEnd - lineIndent;

		QByteArray linebuf;
		linebuf.resize(linebufferSize);
		this->getText(linebuf.data(), lineIndent, lineEnd);

		QByteArray linebufStr = linebuf;

		if (currCommentMode != cm_comment) // uncomment/toggle
		{
			if (!isSingleLineAdvancedMode)
			{
				if (qstrncmp(linebufStr.data(), comment.data(), !(pLexer->lexerId() == L_BAANC) ? comment_length - 1 : comment_length) == 0)
				{
					size_t len = (linebufStr[(int)comment_length - 1] == aSpace[0]) ? comment_length : (!(pLexer->lexerId() == L_BAANC) ? comment_length - 1 : comment_length);

					this->execute(SCI_SETSEL, lineIndent, lineIndent + len);
					this->replaceSelWith("");

					// SELECTION RANGE ADJUSTMENT .......................
					if (i == selStartLine) // first selected line
					{
						if (selectionStart > lineIndent + len)
							selectionStart -= len;
						else if (selectionStart > lineIndent)
							selectionStart = lineIndent;
					} // ................................................
					if (i == selEndLine) // last selected line
					{
						if (selectionEnd > lineIndent + len)
							selectionEnd -= len;
						else if (selectionEnd > lineIndent)
						{
							selectionEnd = lineIndent;
							if (lineIndent == lineStart && i != selStartLine)
								++selectionEnd; // avoid caret return in this case
						}
					} // ................................................
					else // every iteration except the last selected line
						selectionEnd -= len;
					// ..................................................

					++nUncomments;
					continue;
				}
			}
			else 
			{
				if ((qstrncmp(linebufStr.data(), advCommentStart.data(), advCommentStart_length - 1) == 0) &&
					(qstrncmp(linebufStr.mid(linebufStr.length() - advCommentEnd_length + 1, advCommentEnd_length - 1).data(), advCommentEnd.mid(1, advCommentEnd_length - 1).data(), advCommentEnd_length - 1) == 0))
				{
					size_t startLen = linebufStr[(int)advCommentStart_length - 1] == aSpace[0] ? advCommentStart_length : advCommentStart_length - 1;
					size_t endLen = linebufStr[int(linebufStr.length() - advCommentEnd_length)] == aSpace[0] ? advCommentEnd_length : advCommentEnd_length - 1;

					this->execute(SCI_SETSEL, lineIndent, lineIndent + startLen);
					this->replaceSelWith("");
					this->execute(SCI_SETSEL, lineEnd - startLen - endLen, lineEnd - startLen);
					this->replaceSelWith("");

					// SELECTION RANGE ADJUSTMENT .......................
					if (i == selStartLine) // first selected line
					{
						if (selectionStart > lineEnd - endLen)
							selectionStart = lineEnd - startLen - endLen;
						else if (selectionStart > lineIndent + startLen)
							selectionStart -= startLen;
						else if (selectionStart > lineIndent)
							selectionStart = lineIndent;
					} // ................................................
					if (i == selEndLine) // last selected line
					{
						if (selectionEnd > lineEnd)
							selectionEnd -= (startLen + endLen);
						else if (selectionEnd > lineEnd - endLen)
							selectionEnd = lineEnd - startLen - endLen;
						else if (selectionEnd > lineIndent + startLen)
							selectionEnd -= startLen;
						else if (selectionEnd > lineIndent)
						{
							selectionEnd = lineIndent;
							if (lineIndent == lineStart && i != selStartLine)
								++selectionEnd; // avoid caret return in this case
						}
					} // ................................................
					else // every iteration except the last selected line
						selectionEnd -= (startLen + endLen);
					// ..................................................

					++nUncomments;
					continue;
				}
			}
		} // uncomment/toggle

		if (currCommentMode != cm_uncomment) // comment/toggle
		{
			if (!isSingleLineAdvancedMode)
			{
				this->insertCharsFrom(lineIndent, comment);

				// SELECTION RANGE ADJUSTMENT .......................
				if (i == selStartLine) // first selected line
				{
					if (selectionStart >= lineIndent)
						selectionStart += comment_length;
				} // ................................................
				if (i == selEndLine) // last selected line
				{
					if (selectionEnd >= lineIndent)
						selectionEnd += comment_length;
				} // ................................................
				else // every iteration except the last selected line
					selectionEnd += comment_length;
				// ..................................................
			}
			else // isSingleLineAdvancedMode
			{
				this->insertCharsFrom(lineIndent, advCommentStart);
				this->insertCharsFrom(lineEnd + advCommentStart_length, advCommentEnd);

				// SELECTION RANGE ADJUSTMENT .......................
				if (i == selStartLine) // first selected line
				{
					if (selectionStart >= lineIndent)
						selectionStart += advCommentStart_length;
				} // ................................................
				if (i == selEndLine) // last selected line
				{
					if (selectionEnd > lineEnd)
						selectionEnd += (advCommentStart_length + advCommentEnd_length);
					else if (selectionEnd >= lineIndent)
						selectionEnd += advCommentStart_length;
				} // ................................................
				else // every iteration except the last selected line
					selectionEnd += (advCommentStart_length + advCommentEnd_length);
				// ..................................................
			}
		} // comment/toggle
	} // for (...)

	if (move_caret)
	{
		// moving caret to the beginning of selected block
		this->execute(SCI_GOTOPOS, selectionEnd);
		this->execute(SCI_SETCURRENTPOS, selectionStart);
	}
	else
	{
		this->execute(SCI_SETSEL, selectionStart, selectionEnd);
	}
	this->execute(SCI_ENDUNDOACTION);

	// undoStreamComment: If there were no block-comments to un-comment try uncommenting of stream-comment.
	if ((currCommentMode == cm_uncomment) && (nUncomments == 0))
	{
		return undoStreamComment(false);
	}
	return true;
}

//显示文字的字数
void ScintillaEditView::showWordNums()
{
	if (hasSelectedText())
	{
		QString word = selectedText();
		if (!word.isEmpty())
		{
			int lineNum = word.count("\n");
			if (!word.endsWith("\n"))
			{
				++lineNum;
		}

			//\s是包含了换行符的，所有要单独统计\r\n换换行符，排除一下
			QRegExp warpRe("[\r\n]");
			int wrapNums = word.count(warpRe);
			QRegExp bkRe("\\s");
			int blank = word.count(bkRe);
			QMessageBox::about(this, tr("Word Nums"), tr("Current Select Word Nums is %1 . \nLine nums is %2 . \nSpace nums is %3, Non-space is %4 .").\
				arg(word.size()-wrapNums).arg(lineNum).arg(blank-wrapNums).arg(word.size()-blank));
	}
	}
	else
	{
		int lineNum = this->lines();
		QString text = this->text();

		//\s是包含了换行符的，所有要单独统计\r\n换换行符，排除一下
		QRegExp warpRe("[\r\n]");
		int wrapNums = text.count(warpRe);
		QRegExp bkRe("\\s");
		int blank = text.count(bkRe);

		QMessageBox::about(this, tr("Word Nums"), tr("Current Doc Word Nums is %1 . \nLine nums is %2 . \nSpace nums is %3, Non-space is %4 .").\
			arg(text.size() - wrapNums).arg(lineNum).arg(blank - wrapNums).arg(text.size() - blank));
	}
	
}

bool ScintillaEditView::undoStreamComment(bool tryBlockComment)
{
	QByteArray commentStart;
	QByteArray commentEnd;
	QByteArray commentLineSymbol;

	QByteArray symbolStart;
	QByteArray symbolEnd;
	QByteArray symbol;

	const int charbufLen = 10;
	QByteArray charbuf;
	charbuf.reserve(charbufLen);

	bool retVal = false;

	QsciLexer* pLexer = this->lexer();
	//-- Avoid side-effects (e.g. cursor moves number of comment-characters) when file is read-only.
	if (this->isReadOnly())
		return false;

	if (pLexer->lexer() == nullptr)
	{
		return false;
	}
	else
	{
		commentLineSymbol = pLexer->getCommentLineSymbol();
		commentStart = pLexer->getCommentStart();
		commentEnd = pLexer->getCommentEnd();
	}


	// BlockToStreamComment: If there is no stream-comment symbol and we came not from doBlockComment, try the block comment:
	if (commentStart.isEmpty() || commentEnd.isEmpty())
	{
		if (!commentLineSymbol.isEmpty() && tryBlockComment)
			return doBlockComment(cm_uncomment);
		else
			return false;
	}

	QByteArray start_comment(commentStart);
	QByteArray end_comment(commentEnd);
	QByteArray white_space(" ");
	size_t start_comment_length = start_comment.length();
	size_t end_comment_length = end_comment.length();

	// do as long as stream-comments are within selection
	do
	{
		auto selectionStart = this->execute(SCI_GETSELECTIONSTART);
		auto selectionEnd = this->execute(SCI_GETSELECTIONEND);
		auto caretPosition = this->execute(SCI_GETCURRENTPOS);
		auto docLength = this->execute(SCI_GETLENGTH);

		// checking if caret is located in _beginning_ of selected block
		bool move_caret = caretPosition < selectionEnd;

		//-- Note: The caretPosition is either at selectionEnd or at selectionStart!! selectionStart is always before (smaller) than selectionEnd!!

		//-- First, search all start_comment and end_comment before and after the selectionStart and selectionEnd position.
		const int iSelStart = 0, iSelEnd = 1;
		const size_t N_CMNT = 2;
		intptr_t posStartCommentBefore[N_CMNT], posEndCommentBefore[N_CMNT], posStartCommentAfter[N_CMNT], posEndCommentAfter[N_CMNT];
		bool blnStartCommentBefore[N_CMNT], blnEndCommentBefore[N_CMNT], blnStartCommentAfter[N_CMNT], blnEndCommentAfter[N_CMNT];
		intptr_t posStartComment, posEndComment;
		intptr_t selectionStartMove, selectionEndMove;
		int flags;

		//-- Directly use Scintilla-Functions
		//   rather than _findReplaceDlg.processFindNext()which does not return the find-position and is not quiet!
		flags = SCFIND_WORDSTART;
		this->execute(SCI_SETSEARCHFLAGS, flags);
		//-- Find all start- and end-comments before and after the selectionStart position.
		//-- When searching upwards the start-position for searching must be moved one after the current position
		//   to find a search-string just starting before the current position!
		//-- Direction DIR_UP ---
		posStartCommentBefore[iSelStart] = this->searchInTarget(start_comment, selectionStart, 0);
		(posStartCommentBefore[iSelStart] == -1 ? blnStartCommentBefore[iSelStart] = false : blnStartCommentBefore[iSelStart] = true);
		posEndCommentBefore[iSelStart] = this->searchInTarget(end_comment, selectionStart, 0);
		(posEndCommentBefore[iSelStart] == -1 ? blnEndCommentBefore[iSelStart] = false : blnEndCommentBefore[iSelStart] = true);
		//-- Direction DIR_DOWN ---
		posStartCommentAfter[iSelStart] = this->searchInTarget(start_comment, selectionStart, docLength);
		(posStartCommentAfter[iSelStart] == -1 ? blnStartCommentAfter[iSelStart] = false : blnStartCommentAfter[iSelStart] = true);
		posEndCommentAfter[iSelStart] = this->searchInTarget(end_comment, selectionStart, docLength);
		(posEndCommentAfter[iSelStart] == -1 ? blnEndCommentAfter[iSelStart] = false : blnEndCommentAfter[iSelStart] = true);

		//-- Check, if selectionStart or selectionEnd is within a stream comment -----
		//   or if the selection includes a complete stream-comment!! ----------------

		//-- First, check if there is a stream-comment around the selectionStart position:
		if ((blnStartCommentBefore[iSelStart] && blnEndCommentAfter[iSelStart])
			&& (!blnEndCommentBefore[iSelStart] || (posStartCommentBefore[iSelStart] >= posEndCommentBefore[iSelStart]))
			&& (!blnStartCommentAfter[iSelStart] || (posEndCommentAfter[iSelStart] <= posStartCommentAfter[iSelStart])))
		{
			posStartComment = posStartCommentBefore[iSelStart];
			posEndComment = posEndCommentAfter[iSelStart];
		}
		else //-- Second, check if there is a stream-comment around the selectionEnd position:
		{
			//-- Find all start- and end-comments before and after the selectionEnd position.
			//-- Direction DIR_UP ---
			posStartCommentBefore[iSelEnd] = this->searchInTarget(start_comment, selectionEnd, 0);
			(posStartCommentBefore[iSelEnd] == -1 ? blnStartCommentBefore[iSelEnd] = false : blnStartCommentBefore[iSelEnd] = true);
			posEndCommentBefore[iSelEnd] = this->searchInTarget(end_comment, selectionEnd, 0);
			(posEndCommentBefore[iSelEnd] == -1 ? blnEndCommentBefore[iSelEnd] = false : blnEndCommentBefore[iSelEnd] = true);
			//-- Direction DIR_DOWN ---
			posStartCommentAfter[iSelEnd] = this->searchInTarget(start_comment, selectionEnd, docLength);
			(posStartCommentAfter[iSelEnd] == -1 ? blnStartCommentAfter[iSelEnd] = false : blnStartCommentAfter[iSelEnd] = true);
			posEndCommentAfter[iSelEnd] = this->searchInTarget(end_comment, selectionEnd, docLength);
			(posEndCommentAfter[iSelEnd] == -1 ? blnEndCommentAfter[iSelEnd] = false : blnEndCommentAfter[iSelEnd] = true);

			if ((blnStartCommentBefore[iSelEnd] && blnEndCommentAfter[iSelEnd])
				&& (!blnEndCommentBefore[iSelEnd] || (posStartCommentBefore[iSelEnd] >= posEndCommentBefore[iSelEnd]))
				&& (!blnStartCommentAfter[iSelEnd] || (posEndCommentAfter[iSelEnd] <= posStartCommentAfter[iSelEnd])))
			{
				posStartComment = posStartCommentBefore[iSelEnd];
				posEndComment = posEndCommentAfter[iSelEnd];
			}
			//-- Third, check if there is a stream-comment within the selected area:
			else if ((blnStartCommentAfter[iSelStart] && (posStartCommentAfter[iSelStart] < selectionEnd))
				&& (blnEndCommentBefore[iSelEnd] && (posEndCommentBefore[iSelEnd] > selectionStart)))
			{
				//-- If there are more than one stream-comment within the selection, take the first one after selectionStart!!
				posStartComment = posStartCommentAfter[iSelStart];
				posEndComment = posEndCommentAfter[iSelStart];
			}
			//-- Finally, if there is no stream-comment, return
			else
				return retVal;
		}

		//-- Ok, there are valid start-comment and valid end-comment around the caret-position.
		//   Now, un-comment stream-comment:
		retVal = true;
		intptr_t startCommentLength = start_comment_length;
		intptr_t endCommentLength = end_comment_length;

		//-- First delete end-comment, so that posStartCommentBefore does not change!
		//-- Get character before end-comment to decide, if there is a white character before the end-comment, which will be removed too!
		this->getText(charbuf.data(), posEndComment - 1, posEndComment);
		if (qstrncmp(charbuf, white_space, white_space.length()) == 0)
		{
			endCommentLength += 1;
			posEndComment -= 1;
		}
		//-- Delete end stream-comment string ---------
		this->execute(SCI_BEGINUNDOACTION);
		this->execute(SCI_SETSEL, posEndComment, posEndComment + endCommentLength);
		this->execute(SCI_REPLACESEL, 0, reinterpret_cast<sptr_t>(""));

		//-- Get character after start-comment to decide, if there is a white character after the start-comment, which will be removed too!
		this->getText(charbuf.data(), posStartComment + startCommentLength, posStartComment + startCommentLength + 1);
		if (qstrncmp(charbuf, white_space, white_space.length()) == 0)
			startCommentLength += 1;

		//-- Delete starting stream-comment string ---------
		this->execute(SCI_SETSEL, posStartComment, posStartComment + startCommentLength);
		this->execute(SCI_REPLACESEL, 0, reinterpret_cast<sptr_t>(""));
		this->execute(SCI_ENDUNDOACTION);

		//-- Reset selection before calling the routine
		//-- Determine selection movement
		//   selectionStart
		if (selectionStart > posStartComment)
		{
			if (selectionStart >= posStartComment + startCommentLength)
				selectionStartMove = -startCommentLength;
			else
				selectionStartMove = -selectionStart - posStartComment;
		}
		else
			selectionStartMove = 0;

		//   selectionEnd
		if (selectionEnd >= posEndComment + endCommentLength)
			selectionEndMove = -startCommentLength + endCommentLength;
		else if (selectionEnd <= posEndComment)
			selectionEndMove = -startCommentLength;
		else
			selectionEndMove = -startCommentLength + (selectionEnd - posEndComment);

		//-- Reset selection of text without deleted stream-comment-string
		if (move_caret)
		{
			// moving caret to the beginning of selected block
			this->execute(SCI_GOTOPOS, selectionEnd + selectionEndMove);
			this->execute(SCI_SETCURRENTPOS, selectionStart + selectionStartMove);
		}
		else
		{
			this->execute(SCI_SETSEL, selectionStart + selectionStartMove, selectionEnd + selectionEndMove);
		}
	} while (1); //do as long as stream-comments are within selection
}

bool ScintillaEditView::doStreamComment()
{
	QByteArray commentStart;
	QByteArray commentEnd;

	QByteArray symbolStart;
	QByteArray symbolEnd;

	// BlockToStreamComment:
	QByteArray commentLineSymbol;
	QByteArray symbol;


	if (this->isReadOnly())
		return false;

	QsciLexer* pLexer = this->lexer();

	if (pLexer == nullptr)
	{
		return false;
	}
	else
	{
		// BlockToStreamComment: Next line needed to decide, if block-comment can be called below!
		commentLineSymbol = pLexer->getCommentLineSymbol();
		commentStart = pLexer->getCommentStart();
		commentEnd = pLexer->getCommentEnd();
	}

	// BlockToStreamComment: If there is no stream-comment symbol, try the block comment:
	if (commentStart.isEmpty()||commentEnd.isEmpty())
	{
		if (!commentLineSymbol.isEmpty())
			return doBlockComment(cm_comment);
		else
		{
			return false;
		}
	}

	QByteArray start_comment(commentStart);
	QByteArray end_comment(commentEnd);
	QByteArray white_space(" ");

	start_comment += white_space;
	white_space += end_comment;
	end_comment = white_space;
	size_t start_comment_length = start_comment.length();
	size_t selectionStart = this->execute(SCI_GETSELECTIONSTART);
	size_t selectionEnd = this->execute(SCI_GETSELECTIONEND);
	size_t caretPosition = this->execute(SCI_GETCURRENTPOS);
	// checking if caret is located in _beginning_ of selected block
	bool move_caret = caretPosition < selectionEnd;

	// if there is no selection?
	if (selectionEnd - selectionStart <= 0)
	{
		auto selLine = this->execute(SCI_LINEFROMPOSITION, selectionStart);
		selectionStart = this->execute(SCI_GETLINEINDENTPOSITION, selLine);
		selectionEnd = this->execute(SCI_GETLINEENDPOSITION, selLine);
	}
	this->execute(SCI_BEGINUNDOACTION);
	this->insertCharsFrom(selectionStart, start_comment);
	selectionEnd += start_comment_length;
	selectionStart += start_comment_length;
	this->insertCharsFrom(selectionEnd, end_comment);
	if (move_caret)
	{
		// moving caret to the beginning of selected block
		this->execute(SCI_GOTOPOS, selectionEnd);
		this->execute(SCI_SETCURRENTPOS, selectionStart);
	}
	else
	{
		this->execute(SCI_SETSEL, selectionStart, selectionEnd);
	}
	this->execute(SCI_ENDUNDOACTION);
	return true;
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
		}
			}

void ScintillaEditView::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept();
	}

void ScintillaEditView::dropEvent(QDropEvent* e)
{
	QList<QUrl> urls = e->mimeData()->urls();
	if (!urls.isEmpty())
	{

	CCNotePad* pv = dynamic_cast<CCNotePad*>(m_NoteWin);
	if (pv != nullptr)
	{
		pv->receiveEditDrop(e);
}
		return;
	}
	QsciScintilla::dropEvent(e);
}



void ScintillaEditView::mouseDoubleClickEvent(QMouseEvent * e)
{
	if (CCNotePad::s_hightWebAddr == 1)
	{
		do {
			int position = this->execute(SCI_GETCURRENTPOS);

			auto indicMsk = this->execute(SCI_INDICATORALLONFOR, position);
			if (!(indicMsk & (1 << URL_INDIC)))
				break;

			auto startPos = this->execute(SCI_INDICATORSTART, URL_INDIC, position);
			auto endPos = this->execute(SCI_INDICATOREND, URL_INDIC, position);

			if ((position < startPos) || (position > endPos))
				break;

			// WM_LBUTTONUP goes to opening browser instead of Scintilla here, because the mouse is not captured.
			// The missing message causes mouse cursor flicker as soon as the mouse cursor is moved to a position outside the text editing area.
			//::PostMessage(this->getHSelf(), WM_LBUTTONUP, 0, 0);

			// Revert selection of current word. Best to this early, otherwise the
			// selected word is visible all the time while the browser is starting
			execute(SCI_SETSEL, position, position);

			// Open URL
			QByteArray url;
			url.resize(endPos - startPos);
			this->getText(url.data(), static_cast<size_t>(startPos), static_cast<size_t>(endPos));
			QUrl urlweb(url);
			if (urlweb.isValid())
			{
				QDesktopServices::openUrl(urlweb);
				return;
			}
		} while (0);
	}

	//20230219先双击选中，再按住ctrl，再双击别的地方选中。此时是进入多选状态。
	//这里必须要直接返回，不能做delayWork。否则因为delaywork里面不能判断多选，而数据越界崩溃
	if (hasSelectedText())
	{
	QsciScintilla::mouseDoubleClickEvent(e);
		return;
	}

	//执行下面mouseDoubleClickEvent后，又会选中。此时如果上面已经有选中，则会多选
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

void ScintillaEditView::setFoldColor(int margin, QColor fgClack, QColor bkColor, QColor foreActive)
{
	SendScintilla(SCI_MARKERSETFORE, margin, fgClack);
	SendScintilla(SCI_MARKERSETBACK, margin, bkColor);
	SendScintilla(SCI_MARKERSETBACKSELECTED, margin, foreActive);
}

ColumnModeInfos ScintillaEditView::getColumnModeSelectInfo()
{
	ColumnModeInfos columnModeInfos;
	if (execute(SCI_GETSELECTIONS) > 1) // Multi-Selection || Column mode
	{
		intptr_t nbSel = execute(SCI_GETSELECTIONS);

		for (int i = 0; i < nbSel; ++i)
		{
			intptr_t absPosSelStartPerLine = execute(SCI_GETSELECTIONNANCHOR, i);
			intptr_t absPosSelEndPerLine = execute(SCI_GETSELECTIONNCARET, i);
			intptr_t nbVirtualAnchorSpc = execute(SCI_GETSELECTIONNANCHORVIRTUALSPACE, i);
			intptr_t nbVirtualCaretSpc = execute(SCI_GETSELECTIONNCARETVIRTUALSPACE, i);

			if (absPosSelStartPerLine == absPosSelEndPerLine && execute(SCI_SELECTIONISRECTANGLE))
			{
				bool dir = nbVirtualAnchorSpc < nbVirtualCaretSpc ? L2R : R2L;
				columnModeInfos.push_back(ColumnModeInfo(absPosSelStartPerLine, absPosSelEndPerLine, i, dir, nbVirtualAnchorSpc, nbVirtualCaretSpc));
			}
			else if (absPosSelStartPerLine > absPosSelEndPerLine)
				columnModeInfos.push_back(ColumnModeInfo(absPosSelEndPerLine, absPosSelStartPerLine, i, R2L, nbVirtualAnchorSpc, nbVirtualCaretSpc));
			else
				columnModeInfos.push_back(ColumnModeInfo(absPosSelStartPerLine, absPosSelEndPerLine, i, L2R, nbVirtualAnchorSpc, nbVirtualCaretSpc));
		}
	}
	return columnModeInfos;
}


void ScintillaEditView::columnReplace(ColumnModeInfos& cmi, QByteArray& str)
{
	intptr_t totalDiff = 0;
	for (size_t i = 0, len = cmi.size(); i < len; ++i)
	{
		if (cmi[i].isValid())
		{
			intptr_t len2beReplace = cmi[i]._selRpos - cmi[i]._selLpos;
			intptr_t diff = str.size() - len2beReplace;

			cmi[i]._selLpos += totalDiff;
			cmi[i]._selRpos += totalDiff;
			bool hasVirtualSpc = cmi[i]._nbVirtualAnchorSpc > 0;

			if (hasVirtualSpc) // if virtual space is present, then insert space
			{
				for (intptr_t j = 0, k = cmi[i]._selLpos; j < cmi[i]._nbVirtualCaretSpc; ++j, ++k)
				{
					execute(SCI_INSERTTEXT, k, reinterpret_cast<sptr_t>(" "));
				}
				cmi[i]._selLpos += cmi[i]._nbVirtualAnchorSpc;
				cmi[i]._selRpos += cmi[i]._nbVirtualCaretSpc;
			}

			execute(SCI_SETTARGETRANGE, cmi[i]._selLpos, cmi[i]._selRpos);

			execute(SCI_REPLACETARGET, static_cast<int>(-1), reinterpret_cast<sptr_t>(str.data()));

			if (hasVirtualSpc)
			{
				totalDiff += cmi[i]._nbVirtualAnchorSpc + str.size();

				// Now there's no more virtual space
				cmi[i]._nbVirtualAnchorSpc = 0;
				cmi[i]._nbVirtualCaretSpc = 0;
			}
			else
			{
				totalDiff += diff;
			}
			cmi[i]._selRpos += diff;
		}
	}
}


void ScintillaEditView::setMultiSelections(const ColumnModeInfos& cmi)
{
	for (size_t i = 0, len = cmi.size(); i < len; ++i)
	{
		if (cmi[i].isValid())
		{
			intptr_t selStart = cmi[i]._direction == L2R ? cmi[i]._selLpos : cmi[i]._selRpos;
			intptr_t selEnd = cmi[i]._direction == L2R ? cmi[i]._selRpos : cmi[i]._selLpos;
			execute(SCI_SETSELECTIONNSTART, i, selStart);
			execute(SCI_SETSELECTIONNEND, i, selEnd);
		}

		if (cmi[i]._nbVirtualAnchorSpc)
			execute(SCI_SETSELECTIONNANCHORVIRTUALSPACE, i, cmi[i]._nbVirtualAnchorSpc);
		if (cmi[i]._nbVirtualCaretSpc)
			execute(SCI_SETSELECTIONNCARETVIRTUALSPACE, i, cmi[i]._nbVirtualCaretSpc);
	}
}

int getNbDigits(int aNum, int base)
{
	int nbChiffre = 1;
	int diviseur = base;

	for (;;)
	{
		int result = aNum / diviseur;
		if (!result)
			break;
		else
		{
			diviseur *= base;
			++nbChiffre;
		}
	}
	if ((base == 16) && (nbChiffre % 2 != 0))
		nbChiffre += 1;

	return nbChiffre;
}

void ScintillaEditView::columnReplace(ColumnModeInfos& cmi, int initial, int incr, int repeat, int format, bool isCapital, QByteArray& prefix)
{
	assert(repeat > 0);

	// If there is no column mode info available, no need to do anything
	// If required a message can be shown to user, that select column properly or something similar
	// It is just a double check as taken in callee method (in case this method is called from multiple places)
	if (cmi.size() <= 0)
		return;
	// 0000 00 00 : Dec BASE_10
	// 0000 00 01 : Hex BASE_16
	// 0000 00 10 : Oct BASE_08
	// 0000 00 11 : Bin BASE_02

	// 0000 01 00 : 0 leading

	//Defined in ScintillaEditView.h :
	//const UCHAR MASK_FORMAT = 0x03;
	//const UCHAR MASK_ZERO_LEADING = 0x04;

	int base = format;

	const int stringSize = 512;
	QByteArray str;
	str.reserve(stringSize) ;

	// Compute the numbers to be placed at each column.
	std::vector<int> numbers;
	{
		int curNumber = initial;
		const size_t kiMaxSize = cmi.size();
		while (numbers.size() < kiMaxSize)
		{
			for (int i = 0; i < repeat; i++)
			{
				numbers.push_back(curNumber);
				if (numbers.size() >= kiMaxSize)
				{
					break;
				}
			}
			curNumber += incr;
		}
	}

	assert(numbers.size() > 0);

	/*const int kibEnd = getNbDigits(*numbers.rbegin(), base);
	const int kibInit = getNbDigits(initial, base);
	const int kib = std::max<int>(kibInit, kibEnd);*/

	intptr_t totalDiff = 0;
	const size_t len = cmi.size();
	for (size_t i = 0; i < len; i++)
	{
		if (cmi[i].isValid())
		{
			const intptr_t len2beReplaced = cmi[i]._selRpos - cmi[i]._selLpos;

			if (base != 16)
			{
				str = prefix + QString::number(numbers.at(i), base).toUtf8();
			}
			else
			{
				//16进制，判断大小写
				if (isCapital)
				{
					str = prefix + QString::number(numbers.at(i), base).toUpper().toUtf8();
				}
				else
				{
					str = prefix + QString::number(numbers.at(i), base).toUtf8();
				}
			}

			const intptr_t diff = str.size() - len2beReplaced;

			cmi[i]._selLpos += totalDiff;
			cmi[i]._selRpos += totalDiff;

			

			const bool hasVirtualSpc = cmi[i]._nbVirtualAnchorSpc > 0;
			if (hasVirtualSpc) // if virtual space is present, then insert space
			{
				for (intptr_t j = 0, k = cmi[i]._selLpos; j < cmi[i]._nbVirtualCaretSpc; ++j, ++k)
				{
					execute(SCI_INSERTTEXT, k, reinterpret_cast<sptr_t>(" "));
				}
				cmi[i]._selLpos += cmi[i]._nbVirtualAnchorSpc;
				cmi[i]._selRpos += cmi[i]._nbVirtualCaretSpc;
			}
			execute(SCI_SETTARGETRANGE, cmi[i]._selLpos, cmi[i]._selRpos);

			execute(SCI_REPLACETARGET, static_cast<int>(-1), reinterpret_cast<sptr_t>(str.data()));

			if (hasVirtualSpc)
			{
				totalDiff += cmi[i]._nbVirtualAnchorSpc + str.size();
				// Now there's no more virtual space
				cmi[i]._nbVirtualAnchorSpc = 0;
				cmi[i]._nbVirtualCaretSpc = 0;
			}
			else
			{
				totalDiff += diff;
			}
			cmi[i]._selRpos += diff;
		}
	}
}

void ScintillaEditView::getVisibleStartAndEndPosition(int * startPos, int * endPos)
{
	assert(startPos != NULL && endPos != NULL);
	// Get the position of the 1st and last showing chars from the edit view
	QRect rcEditView;
	rcEditView = this->rect();
	int pos = execute(SCI_POSITIONFROMPOINT, 0, 0);
	int line = execute(SCI_LINEFROMPOSITION, pos);
	*startPos = static_cast<int32_t>(execute(SCI_POSITIONFROMLINE, line));
	pos = execute(SCI_POSITIONFROMPOINT, rcEditView.right() - rcEditView.left(), rcEditView.bottom() - rcEditView.top());
	line = execute(SCI_LINEFROMPOSITION, pos);
	*endPos = static_cast<int32_t>(execute(SCI_GETLINEENDPOSITION, line));
}

bool isUrlSchemeStartChar(QChar const c)
{
	return ((c >= 'A') && (c <= 'Z'))
		|| ((c >= 'a') && (c <= 'z'));
}

bool isUrlSchemeDelimiter(QChar const c)
{
	return   !(((c >= '0') && (c <= '9'))
		|| ((c >= 'A') && (c <= 'Z'))
		|| ((c >= 'a') && (c <= 'z'))
		|| (c == '_'));
}

bool scanToUrlStart(QString &text, int textLen, int start, int* distance, int* schemeLength)
{
	int p = start;
	int p0 = 0;
	enum { sUnknown, sScheme } s = sUnknown;
	while (p < textLen)
	{
		switch (s)
		{
		case sUnknown:
			if (isUrlSchemeStartChar(text[p]) && ((p == 0) || isUrlSchemeDelimiter(text[p - 1])))
			{
				p0 = p;
				s = sScheme;
			}
			break;

		case sScheme:
			if (text[p] == ':')
			{
				*distance = p0 - start;
				*schemeLength = p - p0 + 1;
				return true;
			}
			if (!isUrlSchemeStartChar(text[p]))
				s = sUnknown;
			break;
		}
		p++;
	}
	*schemeLength = 0;
	*distance = p - start;
	return false;
}

bool isUrlTextChar(QChar const c)
{
	if (c <= ' ') return false;
	switch (c.digitValue())
	{
	case ('"'):
	case ('#'):
	case ('<'):
	case ('>'):
	case ('{'):
	case ('}'):
	case ('?'):
	case ('\u007F'):
		return false;
	}
	return true;
}

bool isUrlQueryDelimiter(QChar const c)
{
	switch (c.digitValue())
	{
	case '&':
	case '+':
	case '=':
	case ';':
		return true;
	}
	return false;
}

void scanToUrlEnd(QString & text, int textLen, int start, int* distance)
{
	int p = start;
	QChar q = 0;
	enum { sHostAndPath, sQuery, sQueryAfterDelimiter, sQueryQuotes, sQueryAfterQuotes, sFragment } s = sHostAndPath;
	while (p < textLen)
	{
		switch (s)
		{
		case sHostAndPath:
			if (text[p] == QChar('?'))
				s = sQuery;
			else if (text[p] == '#')
				s = sFragment;
			else if (!isUrlTextChar(text[p]))
			{
				*distance = p - start;
				return;
			}
			break;

		case sQuery:
			if (text[p] == '#')
				s = sFragment;
			else if (isUrlQueryDelimiter(text[p]))
				s = sQueryAfterDelimiter;
			else if (!isUrlTextChar(text[p]))
			{
				*distance = p - start;
				return;
			}
			break;

		case sQueryAfterDelimiter:
			if ((text[p] == '\'') || (text[p] == '"') || (text[p] == '`'))
			{
				q = text[p];
				s = sQueryQuotes;
			}
			else if (text[p] == '(')
			{
				q = ')';
				s = sQueryQuotes;
			}
			else if (text[p] == '[')
			{
				q = ']';
				s = sQueryQuotes;
			}
			else if (text[p] == '{')
			{
				q = '}';
				s = sQueryQuotes;
			}
			else if (isUrlTextChar(text[p]))
				s = sQuery;
			else
			{
				*distance = p - start;
				return;
			}
			break;

		case sQueryQuotes:
			if (text[p] < ' ')
			{
				*distance = p - start;
				return;
			}
			if (text[p] == q)
				s = sQueryAfterQuotes;
			break;

		case sQueryAfterQuotes:
			if (isUrlQueryDelimiter(text[p]))
				s = sQueryAfterDelimiter;
			else
			{
				*distance = p - start;
				return;
			}
			break;

		case sFragment:
			if (!isUrlTextChar(text[p]))
			{
				*distance = p - start;
				return;
			}
			break;
		}
		p++;
	}
	*distance = p - start;
}

// removeUnwantedTrailingCharFromUrl removes a single unwanted trailing character from an URL.
// It has to be called repeatedly, until it returns false, meaning that all unwanted characters are gone.
bool removeUnwantedTrailingCharFromUrl(QChar const *text, int* length)
{
	int l = *length - 1;
	if (l <= 0) return false;
	{ // remove unwanted single characters
		const char *singleChars = ".,:;?!#";
		for (int i = 0; singleChars[i]; i++)
			if (text[l] == singleChars[i])
			{
				*length = l;
				return true;
			}
	}
	{ // remove unwanted closing parenthesis
		const char *closingParenthesis = ")]";
		const char *openingParenthesis = "([";
		for (int i = 0; closingParenthesis[i]; i++)
			if (text[l] == closingParenthesis[i])
			{
				int count = 0;
				for (int j = l - 1; j >= 0; j--)
				{
					if (text[j] == closingParenthesis[i])
						count++;
					if (text[j] == openingParenthesis[i])
						if (count > 0)
							count--;
						else
							return false;
				}
				if (count != 0)
					return false;
				*length = l;
				return true;
			}
	}
	return false;
}

bool isUrl(QString& text, int textLen, int start, int* segmentLen)
{
	int dist = 0, schemeLen = 0;
	if (scanToUrlStart(text, textLen, start, &dist, &schemeLen))
	{
		if (dist)
		{
			*segmentLen = dist;
			return false;
		}
		int len = 0;
		scanToUrlEnd(text, textLen, start + schemeLen, &len);
		if (len)
		{
			len += schemeLen;

			QString urlStr = text.mid(start, len);
			if (urlStr.startsWith("http://") || urlStr.startsWith("https://"))
			{

				QUrl url(urlStr);

				bool r = url.isValid();
				if (r)
				{
					while (removeUnwantedTrailingCharFromUrl(text.data() + start, &len));
					*segmentLen = len;
					return true;
				}
			}
		}
		len = 1;
		int lMax = textLen - start;
		while (isUrlSchemeStartChar(text[start + len]) && (len < lMax)) len++;
		*segmentLen = len;
		return false;
	}
	*segmentLen = dist;
	return false;
}

quint32 ScintillaEditView::getBigTextBlockStartLine()
{
	return m_curBlockLineStartNum;
}

void ScintillaEditView::setBigTextBlockStartLine(quint32 line)
{
	m_curBlockLineStartNum = line;
}

void ScintillaEditView::addHotSpot()
{
	if (CCNotePad::s_hightWebAddr == 1)
	{
		int urlAction = urlNoUnderLineFg;
		int indicStyle = INDIC_PLAIN;
		int indicHoverStyle = INDIC_EXPLORERLINK;
		int indicStyleCur = this->execute(SCI_INDICGETSTYLE, URL_INDIC);
		int indicHoverStyleCur = this->execute(SCI_INDICGETHOVERSTYLE, URL_INDIC);

		if ((indicStyleCur != indicStyle) || (indicHoverStyleCur != indicHoverStyle))
		{
			this->execute(SCI_INDICSETSTYLE, URL_INDIC, indicStyle);
			this->execute(SCI_INDICSETHOVERSTYLE, URL_INDIC, indicHoverStyle);
			this->execute(SCI_INDICSETALPHA, URL_INDIC, 70);
			this->execute(SCI_INDICSETFLAGS, URL_INDIC, SC_INDICFLAG_VALUEFORE);
		}

		int startPos = 0;
		int endPos = -1;
		this->getVisibleStartAndEndPosition(&startPos, &endPos);
		if (startPos >= endPos)
		{
			return;
		}

		this->execute(SCI_SETINDICATORCURRENT, URL_INDIC);
		if (urlAction == urlDisable)
		{
			this->execute(SCI_INDICATORCLEARRANGE, startPos, endPos - startPos);
			return;
		}

		int indicFore = this->execute(SCI_STYLEGETFORE, STYLE_DEFAULT);
		this->execute(SCI_SETINDICATORVALUE, indicFore);

		QByteArray encodedText;
		encodedText.resize(endPos - startPos);
		this->getText(encodedText.data(), startPos, endPos);

		QString encodedTextStr(encodedText);

		int wideTextLen = encodedTextStr.size();


		if (wideTextLen > 0)
		{
			int startWide = 0;
			int lenWide = 0;
			int startEncoded = 0;
			int lenEncoded = 0;
			while (true)
			{
				bool r = isUrl(encodedTextStr, encodedTextStr.size(), startWide, &lenWide);
				if (lenWide <= 0)
					break;

				lenEncoded = encodedTextStr.mid(startWide, lenWide).toUtf8().size();

				if (r)
					this->execute(SCI_INDICATORFILLRANGE, startEncoded + startPos, lenEncoded);
				else
					this->execute(SCI_INDICATORCLEARRANGE, startEncoded + startPos, lenEncoded);

				startWide += lenWide;
				startEncoded += lenEncoded;

				if ((startWide >= wideTextLen) || ((startEncoded + startPos) >= endPos))
					break;
			}
			assert((startEncoded + startPos) == endPos);
			assert(startWide == wideTextLen);
		}
	}
}

void ScintillaEditView::setStyleOptions()
{
#if 0
	//如果是黑色主题，则单独做一些风格设置
	if (StyleSet::m_curStyleId == BLACK_SE)
	{
		setCaretLineBackgroundColor(QColor(0x333333));
		setMatchedBraceForegroundColor(QColor(246, 81, 246));
		setMatchedBraceBackgroundColor(QColor(18, 90, 36));
		setCaretForegroundColor(QColor(255, 255, 255));
		setFoldColor(SC_MARKNUM_FOLDEROPEN, QColor(45, 130, 45), QColor(222, 222, 222));
		setFoldColor(SC_MARKNUM_FOLDER, QColor(45, 130, 45), QColor(222, 222, 222));
		setFoldColor(SC_MARKNUM_FOLDERSUB, QColor(45, 130, 45), QColor(222, 222, 222));
		setFoldColor(SC_MARKNUM_FOLDERTAIL, QColor(45, 130, 45), QColor(222, 222, 222));
		setFoldColor(SC_MARKNUM_FOLDEREND, QColor(45, 130, 45), QColor(222, 222, 222));
		setFoldColor(SC_MARKNUM_FOLDEROPENMID, QColor(45, 130, 45), QColor(222, 222, 222));
		setFoldColor(SC_MARKNUM_FOLDERMIDTAIL, QColor(45, 130, 45), QColor(222, 222, 222));
	}
	else
	{
		setCaretLineBackgroundColor(QColor(0xe8e8ff)); 
		setMatchedBraceForegroundColor(QColor(191, 141, 255));
		setMatchedBraceBackgroundColor(QColor(222, 222, 222));
		setCaretForegroundColor(QColor(0, 0, 0));
		setFoldColor(SC_MARKNUM_FOLDEROPEN, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDER, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDERSUB, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDERTAIL, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDEREND, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDEROPENMID, QColor(Qt::white), QColor(128, 128, 128));
		setFoldColor(SC_MARKNUM_FOLDERMIDTAIL, QColor(Qt::white), QColor(128, 128, 128));
	}
#endif
}

//在ScintillaEditView中直接设置这些One_Stype_Info的属性值
//这里的style不一定是在lexer中的，而是默认的STYLE_*自定义的那些全局内部风格。
void ScintillaEditView::updateThemes()
{
	//第0个是GLOBAL_OVERRIDE，是Lexer的样式，不属于全局
	for (int i = 1; i <= URL_HOVERRED; ++i)
	{
		setGlobalFgColor(i);
		setGlobalBgColor(i);
		setGlobalFont(i);
	}
}

static void getFoldColor(QColor& fgColor, QColor& bgColor, QColor& activeFgColor)
{
	//这里看起来反了，但是实际代码就是如此
	fgColor = StyleSet::s_global_style->fold.bgColor;
	bgColor = StyleSet::s_global_style->fold.fgColor;
	
	activeFgColor = StyleSet::s_global_style->fold_active.fgColor;
	
}

void ScintillaEditView::setGlobalFgColor(int style)
{
	switch (style)
	{
		case GLOBAL_OVERRIDE:
		{
			//全局前景色修改，是针对语法的lexer属性进行的修改，而非其余全局属性的修改。这点要区分开来
		}
		break;

		case DEFAULT_STYLE:
		{
			//修改默认前景色
			SendScintilla(SCI_STYLESETFORE, StyleSet::s_global_style->default_style.styleId, StyleSet::s_global_style->default_style.fgColor);
		}
		break;

	case INDENT_GUIDELINE:
	{
		//修改默认前景色
		SendScintilla(SCI_STYLESETFORE, StyleSet::s_global_style->indent_guideline.styleId, StyleSet::s_global_style->indent_guideline.fgColor);
	}
	break;

	case BRACE_HIGHIGHT:
	{
		//修改默认前景色
		SendScintilla(SCI_STYLESETFORE, StyleSet::s_global_style->brace_highight.styleId, StyleSet::s_global_style->brace_highight.fgColor);
	}
	break;

	case BAD_BRACE_COLOUR:
	{
		//修改默认前景色
		SendScintilla(SCI_STYLESETFORE, StyleSet::s_global_style->bad_brace_color.styleId, StyleSet::s_global_style->bad_brace_color.fgColor);
	}
	break;

	case CURRENT_LINE_BACKGROUND_COLOR:
		//不能设置前景色，只能设置背景
		break;
		
	case SELECT_TEXT_COLOR:
		SendScintilla(SCI_SETSELFORE, true, StyleSet::s_global_style->select_text_color.fgColor);
		break;

	case CARET_COLOUR:
		SendScintilla(SCI_SETCARETFORE, StyleSet::s_global_style->caret_colour.fgColor);
		break;

	case EDGE_COLOUR:
		SendScintilla(SCI_SETEDGECOLOUR, StyleSet::s_global_style->edge_colour.fgColor);
		break;

	case LINE_NUMBER_MARGIN:
	{
		//修改默认前景色
		SendScintilla(SCI_STYLESETFORE, StyleSet::s_global_style->line_number_margin.styleId, StyleSet::s_global_style->line_number_margin.fgColor);
	}
	break;

	case BOOKMARK_MARGIN:
		//不能设置
		break;

	case FOLD:
	case FOLD_ACTIVE:
	{
		QColor foldfgColor = Qt::white, foldbgColor = Qt::gray, activeFoldFgColor = Qt::red;
		getFoldColor(foldfgColor, foldbgColor, activeFoldFgColor);

		setFoldColor(SC_MARKNUM_FOLDEROPEN, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDER, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDERSUB, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDERTAIL, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDEREND, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDEROPENMID, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDERMIDTAIL, foldfgColor, foldbgColor, activeFoldFgColor);

		//暂时开启，看看后面是否有错误
		//execute(SCI_MARKERENABLEHIGHLIGHT, true);
	}
		break;

	case FOLD_MARGIN:
		//前景背景一起设置，不分开
	{
		setFoldMarginColors(StyleSet::s_global_style->fold_margin.fgColor, StyleSet::s_global_style->fold_margin.bgColor);
	}
	break;

	case WHITE_SPACE_SYMBOL:
		SendScintilla(SCI_SETWHITESPACEFORE, true, StyleSet::s_global_style->white_space_stybol.fgColor);
		break;

	case SMART_HIGHLIGHTING:
		//只能设置前景，不能设置背景
		SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_SMART, StyleSet::s_global_style->smart_highlighting.fgColor);
		break;

	case FIND_MARK_STYLE:
		//只能设置前景，不能设置背景。目前这条是空的，暂时没有使用
		SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE, StyleSet::s_global_style->find_mark_style.fgColor);
		break;

	case MARK_STYLE_1:
	case MARK_STYLE_2:
	case MARK_STYLE_3:
	case MARK_STYLE_4:
	case MARK_STYLE_5:
	case INCREMENTAL_HIGHLIGHT:
		//暂时没有使用
		break;

	case TAGS_MATCH_HIGHLIGHT:
		SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_TAGMATCH, StyleSet::s_global_style->tags_match_highlight.fgColor);
		break;

	case TAGS_ATTRIBUTE:
		SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_TAGATTR, StyleSet::s_global_style->tags_attribute.fgColor);
		break;

	//case ACTIVE_TAB_FOCUSED:
	//case ACTIVE_TAB_UNFOCUSED:
	//case ACTIVE_TAB_TEXT:
	//case INACTIVE_TABS:
	//	break;

	case URL_HOVERRED:
		SendScintilla(SCI_INDICSETHOVERFORE, URL_INDIC, StyleSet::s_global_style->url_hoverred.fgColor);
		break;

	default:
		break;
	}
};

void ScintillaEditView::setGlobalBgColor(int style)
{
	switch (style)
	{
	case GLOBAL_OVERRIDE:
	{

	}
	break;

	case DEFAULT_STYLE:
	{
		//修改默认前景色
		SendScintilla(SCI_STYLESETBACK, StyleSet::s_global_style->default_style.styleId, StyleSet::s_global_style->default_style.bgColor);
	}
	break;

	case INDENT_GUIDELINE:
	{
		//修改默认前景色
		SendScintilla(SCI_STYLESETBACK, StyleSet::s_global_style->indent_guideline.styleId, StyleSet::s_global_style->indent_guideline.bgColor);
	}
	break;

	case BRACE_HIGHIGHT:
	{
		//修改默认前景色
		SendScintilla(SCI_STYLESETBACK, StyleSet::s_global_style->brace_highight.styleId, StyleSet::s_global_style->brace_highight.bgColor);
	}
	break;

	case BAD_BRACE_COLOUR:
	{
		//修改默认前景色
		SendScintilla(SCI_STYLESETBACK, StyleSet::s_global_style->bad_brace_color.styleId, StyleSet::s_global_style->bad_brace_color.bgColor);
	}
	break;

	case CURRENT_LINE_BACKGROUND_COLOR:
		//不能设置前景色，只能设置背景
		SendScintilla(SCI_SETCARETLINEBACK, StyleSet::s_global_style->current_line_background_color.bgColor);
		break;

	case SELECT_TEXT_COLOR:
		SendScintilla(SCI_SETSELBACK, true, StyleSet::s_global_style->select_text_color.bgColor);
		break;

	case CARET_COLOUR:
		//不能设置
		break;

	case EDGE_COLOUR:
		//不能设置
		break;

	case LINE_NUMBER_MARGIN:
	{
		//修改默认背景色
		SendScintilla(SCI_STYLESETBACK, StyleSet::s_global_style->line_number_margin.styleId, StyleSet::s_global_style->line_number_margin.bgColor);
	}
	break;

	case BOOKMARK_MARGIN:
	{
		if (StyleSet::s_global_style->bookmark_margin.bgColor.isValid())
		{
			SendScintilla(SCI_SETMARGINBACKN, _SC_MARGE_SYBOLE, StyleSet::s_global_style->bookmark_margin.bgColor);
		}
		else
		{
			SendScintilla(SCI_SETMARGINBACKN, _SC_MARGE_SYBOLE, StyleSet::s_global_style->line_number_margin.bgColor);
		}
	}
		break;

	case FOLD:
	case FOLD_ACTIVE:
	{
		QColor foldfgColor = Qt::white, foldbgColor = Qt::gray, activeFoldFgColor = Qt::red;
		getFoldColor(foldfgColor, foldbgColor, activeFoldFgColor);

		setFoldColor(SC_MARKNUM_FOLDEROPEN, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDER, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDERSUB, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDERTAIL, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDEREND, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDEROPENMID, foldfgColor, foldbgColor, activeFoldFgColor);
		setFoldColor(SC_MARKNUM_FOLDERMIDTAIL, foldfgColor, foldbgColor, activeFoldFgColor);

		//暂时不能开启。因为QT下面有一个1Pix的差异，如果开启，当前fold的变化会缺失1pix的宽度，看起来难看。
		// 这是QT的bug，暂时解决不了。
		//execute(SCI_MARKERENABLEHIGHLIGHT, true);
	}
	break;

	case FOLD_MARGIN:
		//前景背景一起设置，不分开
	{
		setFoldMarginColors(StyleSet::s_global_style->fold_margin.fgColor, StyleSet::s_global_style->fold_margin.bgColor);
	}
	break;

	case WHITE_SPACE_SYMBOL:
		//不能设置
		break;

	case SMART_HIGHLIGHTING:
		//不能设置
		break;

	case FIND_MARK_STYLE:
		//不能设置
		break;

	//下面五个比较特殊，选择改动的是背景。单本质改动的样式却是前景
	case MARK_STYLE_1:
		//只能设置前景，不能设置背景
		SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT5, StyleSet::s_global_style->mark_style_1.bgColor);
		changeStyleColor(0);
		break;

	case MARK_STYLE_2:
		//只能设置前景，不能设置背景
		SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT4, StyleSet::s_global_style->mark_style_2.bgColor);
		changeStyleColor(1);
		break;

	case MARK_STYLE_3:
		//只能设置前景，不能设置背景
		SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT3, StyleSet::s_global_style->mark_style_3.bgColor);
		changeStyleColor(2);
		break;

	case MARK_STYLE_4:
		//只能设置前景，不能设置背景
		SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT2, StyleSet::s_global_style->mark_style_4.bgColor);
		changeStyleColor(3);
		break;

	case MARK_STYLE_5:
		//只能设置前景，不能设置背景
		SendScintilla(SCI_INDICSETFORE, SCE_UNIVERSAL_FOUND_STYLE_EXT1, StyleSet::s_global_style->mark_style_5.bgColor);
		changeStyleColor(4);
		break;

	case INCREMENTAL_HIGHLIGHT:
	case TAGS_MATCH_HIGHLIGHT:
	case TAGS_ATTRIBUTE:
		break;

	//case ACTIVE_TAB_FOCUSED:
	//case ACTIVE_TAB_UNFOCUSED:
	//case ACTIVE_TAB_TEXT:
	//case INACTIVE_TABS:
	//	break;
	case URL_HOVERRED:
		break;
	default:
		break;
	}
};

// Set the font for a style.
//enum Font_Set_Bit {
//	Bold_Bit = 0x1,
//	Italic_Bit = 0x2,
//	Underline_Bit = 0x4,
//	Font_Name_Bit = 0x8,
//	Font_Size_Bit = 0x10,
//};
void ScintillaEditView::setStylesFont(const QFont& f, int style, int setBitMask)
{
	if (setBitMask & Font_Name_Bit)
	{
		SendScintilla(SCI_STYLESETFONT, style, f.family().toUtf8().data());
	}
	if (setBitMask & Bold_Bit)
	{
		SendScintilla(SCI_STYLESETBOLD, style, f.bold());
	}
	if (setBitMask & Italic_Bit)
	{
		SendScintilla(SCI_STYLESETITALIC, style, f.italic());
	}
	if (setBitMask & Underline_Bit)
	{
		SendScintilla(SCI_STYLESETUNDERLINE, style, f.underline());
	}

	if (setBitMask & Font_Size_Bit && f.pointSize() > 2)
	{
		SendScintilla(SCI_STYLESETSIZE, style, f.pointSize());
	}
	//如果是行号大小变化，则必须动态计算行宽度
	if (style == STYLE_LINENUMBER)
	{
		updateLineNumberWidth(1);
	}
}

void ScintillaEditView::setGlobalFont(int style)
{
	switch (style)
	{
	case GLOBAL_OVERRIDE:
	case INDENT_GUIDELINE:
	case CURRENT_LINE_BACKGROUND_COLOR:
	case SELECT_TEXT_COLOR:
	case CARET_COLOUR:
	case EDGE_COLOUR:
	case BOOKMARK_MARGIN:
	case FOLD:
	case FOLD_ACTIVE:
	case FOLD_MARGIN:
	case WHITE_SPACE_SYMBOL:
	case SMART_HIGHLIGHTING:
	case FIND_MARK_STYLE:
	case MARK_STYLE_1:
	case MARK_STYLE_2:
	case MARK_STYLE_3:
	case MARK_STYLE_4:
	case MARK_STYLE_5:
	case INCREMENTAL_HIGHLIGHT:
	case TAGS_MATCH_HIGHLIGHT:
	case TAGS_ATTRIBUTE:
	//case ACTIVE_TAB_FOCUSED:
	//case ACTIVE_TAB_UNFOCUSED:
	//case ACTIVE_TAB_TEXT:
	//case INACTIVE_TABS:
	case URL_HOVERRED:
		break;

	case DEFAULT_STYLE:
	{
		setStylesFont(StyleSet::s_global_style->default_style.font, STYLE_DEFAULT, 0x1f);
	}
	break;

	case BRACE_HIGHIGHT:
	{
		setStylesFont(StyleSet::s_global_style->brace_highight.font, STYLE_BRACELIGHT, 0x1f);
	}
	break;

	case BAD_BRACE_COLOUR:
	{
		setStylesFont(StyleSet::s_global_style->bad_brace_color.font, STYLE_BRACEBAD, 0x1f);
	}
	break;

	case LINE_NUMBER_MARGIN:
	{
		//除了下划线不加，其余的都需要设置
		setStylesFont(StyleSet::s_global_style->line_number_margin.font, STYLE_LINENUMBER, 0x1b);
	}
	break;
	default:
		break;
	}
}

bool ScintillaEditView::isFoldIndentBased() const
{
	QsciLexer* lexer = this->lexer();

	if (lexer != nullptr)
	{
		int lexerId = lexer->lexerId();

		return lexerId == L_PYTHON
			|| lexerId == L_COFFEESCRIPT
			|| lexerId == L_HASKELL
			|| lexerId == L_VB
			|| lexerId == L_YAML;
	}
	return false;
}

const int  MAX_FOLD_COLLAPSE_LEVEL = 8;

struct FoldLevelStack
{
	int levelCount = 0;
	intptr_t levelStack[MAX_FOLD_COLLAPSE_LEVEL]{};

	void push(intptr_t level)
	{
		while (levelCount != 0 && level <= levelStack[levelCount - 1])
		{
			--levelCount;
		}
		levelStack[levelCount++] = level;
	}
};

bool ScintillaEditView::isFolded(size_t line)
{
	return (0 != execute(SCI_GETFOLDEXPANDED, line));
};

void ScintillaEditView::fold(size_t line, bool mode)
{
	auto endStyled = execute(SCI_GETENDSTYLED);
	auto len = execute(SCI_GETTEXTLENGTH);

	if (endStyled < len)
		execute(SCI_COLOURISE, 0, -1);

	intptr_t headerLine;
	auto level = execute(SCI_GETFOLDLEVEL, line);

	if (level & SC_FOLDLEVELHEADERFLAG)
		headerLine = line;
	else
	{
		headerLine = execute(SCI_GETFOLDPARENT, line);
		if (headerLine == -1)
			return;
	}

	if (isFolded(headerLine) != mode)
	{
		execute(SCI_TOGGLEFOLD, headerLine);

		//SCNotification scnN;
		//scnN.nmhdr.code = SCN_FOLDINGSTATECHANGED;
		//scnN.nmhdr.hwndFrom = _hSelf;
		//scnN.nmhdr.idFrom = 0;
		//scnN.line = headerLine;
		//scnN.foldLevelNow = isFolded(headerLine) ? 1 : 0; //folded:1, unfolded:0

		//::SendMessage(_hParent, WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&scnN));
	}
}

void ScintillaEditView::collapseFoldIndentBased(int level, bool mode)
{
	execute(SCI_COLOURISE, 0, -1);

	FoldLevelStack levelStack;
	++level;

	const intptr_t maxLine = execute(SCI_GETLINECOUNT);
	intptr_t line = 0;

	while (line < maxLine)
	{
		intptr_t level = execute(SCI_GETFOLDLEVEL, line);
		if (level & SC_FOLDLEVELHEADERFLAG)
		{
			level &= SC_FOLDLEVELNUMBERMASK;
			levelStack.push(level);
			if (level == levelStack.levelCount)
			{
				if (isFolded(line) != mode)
				{
					fold(line, mode);
				}
				line = execute(SCI_GETLASTCHILD, line, -1);
			}
		}
		++line;
	}
}

void ScintillaEditView::collapse(int level, bool mode)
{
	if (isFoldIndentBased())
	{
		return collapseFoldIndentBased(level, mode);
	}

	execute(SCI_COLOURISE, 0, -1);

	intptr_t maxLine = execute(SCI_GETLINECOUNT);

	for (int line = 0; line < maxLine; ++line)
	{
		intptr_t lineLevel = execute(SCI_GETFOLDLEVEL, line);
		if (lineLevel & SC_FOLDLEVELHEADERFLAG)
		{
			lineLevel -= SC_FOLDLEVELBASE;
			if (level == (lineLevel & SC_FOLDLEVELNUMBERMASK))
				if (isFolded(line) != mode)
				{
					fold(line, mode);
				}
		}
	}
}

void ScintillaEditView::comment(int type)
{
	switch (type)
	{
	case ADD_DEL_LINE_COM:
		doBlockComment(cm_toggle);
		break;
	case ADD_BK_COM:
		doStreamComment();
		break;
	case DEL_BK_COM:
		undoStreamComment();
		break;
	default:
		break;
	}
}

#ifdef Q_OS_WIN
void ScintillaEditView::deleteTailFileThread()
{
	if (m_isInTailStatus)
	{
		m_isInTailStatus = false;

		qlonglong threadAddr = this->property(Tail_Thread).toLongLong();

		std::thread* pListenThread = (std::thread*)(threadAddr);

		if (pListenThread->joinable())
		{
			pListenThread->join();
		}

		delete pListenThread;
	}
}
#endif

//显示markdown编辑器
void ScintillaEditView::on_viewMarkdown()
{
	if (m_markdownWin.isNull())
	{
		m_markdownWin = new MarkdownView(this);
		m_markdownWin->setAttribute(Qt::WA_DeleteOnClose);
		connect(this, &ScintillaEditView::textChanged, this, &ScintillaEditView::on_updataMarkdown);
	}
	
	QString text = this->text();
	m_markdownWin->viewMarkdown(text);
	m_markdownWin->show();
}

void ScintillaEditView::on_updataMarkdown()
{
	if (!m_markdownWin.isNull())
	{
		QString text = this->text();
		m_markdownWin->viewMarkdown(text);
		m_markdownWin->show();
	}
}
