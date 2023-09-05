#pragma once
#include <QMap>
#include <qscilexer.h>

#if 0
enum LangType {
	L_UNKNOWN = -1, L_PHP = 0, L_C, L_CPP, L_CS, L_OBJC, L_JAVA, L_RC, \
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
	L_EDIFACT, L_MARKDOWN, L_OCTAVE, L_PO, L_POV, L_IDL, L_GO, L_TXT, \
	// Don't use L_JS, use L_JAVASCRIPT instead
	// The end of enumated language type, so it should be always at the end
	L_EXTERNAL = 100, L_USER_DEFINE = 200, L_USER_TXT, L_USER_CPP, L_USER_HTML, L_USER_JS,
	L_USER_XML, L_USER_INI,
};
#endif

struct LexerInfo{
	LangType lexerId; //如果是L_USER_DEFINE，则表示母版的类型
	QString tagName; //语言tag名称。
	bool isUserDefineLangs()
	{
		return (lexerId > L_USER_DEFINE);
	}
	LexerInfo() = default;
	LexerInfo(LangType id, QString name):lexerId(id), tagName(name)
	{

	}
};

class ExtLexerManager
{
public:
	static ExtLexerManager* getInstance();
	~ExtLexerManager();

	int size();

	bool contains(QString ext);

	void remove(QString ext);

	void addNewExtType(QString ext, LangType lexerId, QString langTagName="");

	bool getLexerTypeByExt(QString ext, LexerInfo& lexer);

	//列出tag语言下面的所有关联的文件后缀列表
	void getExtlistByLangTag(QString tag, QStringList& extList);

	//列出所有语言下面的所有关联的文件后缀列表
	void getExtlistByLangTag(QMap<QString, QStringList>& extLangMap);


private:
	ExtLexerManager();

	static ExtLexerManager* s_instance;

	QMap<QString, LexerInfo> m_extToLexerIdMap;
};

