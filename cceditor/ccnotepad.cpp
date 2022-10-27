#include "ccnotepad.h"
#include "filemanager.h"
#include "Encode.h"
#include "findwin.h"
#include "jsondeploy.h"
#include "findresultwin.h"
#include "scintillaeditview.h"
#include "scintillahexeditview.h"
#include "encodeconvert.h"
#include "optionsview.h"
#include "donate.h"
#include "renamewin.h"
#include "doctypelistview.h"
#include "hexfilegoto.h"
#include "qscilexertext.h"
#include "styleset.h"
#include "qtlangset.h"
#include "columnedit.h"


#include <QFileDialog>
#include <QDebug>
#include <QTabBar>
#include <QVariant> 
#include <QTextCodec>
#include <QMessageBox> 
#include <QToolButton>
#include <qsciscintilla.h>
#include <QDockWidget>
#include <QInputDialog>
#include <QPair>
#include <QMenu>
#include <QProcess>
#include <QSettings>
#include <QSharedMemory>
#include <QMimeDatabase>
#include <QDateTime>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <Windows.h>
#endif

#ifdef Q_OS_WIN
extern bool s_isAdminAuth;

inline std::wstring StringToWString(const std::string& str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wide = new wchar_t[len + 1];
	memset(wide, '\0', sizeof(wchar_t) * (len + 1));
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, len);
	std::wstring w_str(wide);
	delete[] wide;
	return w_str;
}
#endif


int CCNotePad::s_padTimes = 0;

QString CCNotePad::s_lastOpenDirPath = "";

//QFont CCNotePad::s_txtFont;
//QFont CCNotePad::s_proLangFont;


QList<CCNotePad*> *CCNotePad::s_padInstances = nullptr;

//文件保存的路径，qstring
const char* Edit_View_FilePath = "filePath";

//int 新文件的id序号。非新建文件为-1
const char* Edit_File_New = "newfile";

//是否修改 true false
const char* Edit_Text_Change = "change";

//编码类型,int
const char* Edit_Text_Code = "code";

//line行尾符号
const char* Edit_Text_End = "lineend";

//外部修改
const char* Modify_Outside = "modify";

//文档类型 1:text 2 hex
const char* Doc_Type = "type";


void setFileOpenAttrProperty(QWidget* pwidget, OpenAttr attr)
{
	QVariant v(attr);
	pwidget->setProperty(Open_Attr, v);
}

const QString OpenAttrToString(OpenAttr openType)
{
	QString ret;
	switch (openType)
	{
	case Text:
		ret = QObject::tr("Text Mode");
		break;
	case HexReadOnly:
		ret = QObject::tr("Hex ReadOnly Mode");
		break;
	case BigTextReadOnly:
		ret = QObject::tr("Bit Text ReadOnly Mode");
		break;
	case TextReadOnly:
		ret = QObject::tr("Text ReadOnly Mode");
		break;
	default:
		ret = QObject::tr("File Mode");
		break;
	}
	return ret;
}

QString getFileOpenAttrProperty(QWidget* pwidget)
{
	OpenAttr openType = (OpenAttr)pwidget->property(Open_Attr).toInt();
	return OpenAttrToString(openType);
}

void setFilePathProperty(QWidget* pwidget, QString filePath)
{
	QVariant v(filePath);
	pwidget->setProperty(Edit_View_FilePath, v);
}

QString getFilePathProperty(QWidget* pwidget)
{
	return pwidget->property(Edit_View_FilePath).toString();
}

void setFileNewIndexProperty(QWidget* pwidget, int index)
{
	QVariant v(index);
	pwidget->setProperty(Edit_File_New, v);
}

int getFileNewIndexProperty(QWidget* pwidget)
{
	return pwidget->property(Edit_File_New).toInt();
}

void setTextChangeProperty(QWidget* pwidget, bool status)
{
	QVariant v(status);
	pwidget->setProperty(Edit_Text_Change, v);
}

bool getTextChangeProperty(QWidget* pwidget)
{
	return pwidget->property(Edit_Text_Change).toBool();
}

void setCodeTypeProperty(QWidget* pwidget, int type)
{
	QVariant v(type);
	pwidget->setProperty(Edit_Text_Code, v);
}

int getCodeTypeProperty(QWidget* pwidget)
{
	return pwidget->property(Edit_Text_Code).toInt();
}

void setEndTypeProperty(QWidget* pwidget, int type)
{
	QVariant v(type);
	pwidget->setProperty(Edit_Text_End, v);
}

int getEndTypeProperty(QWidget* pwidget)
{
	return pwidget->property(Edit_Text_End).toInt();
}

//根据当前路径，得到交互文件的名称
QString getSwapFilePath(QString filePath)
{
	QFileInfo fi(filePath);

#ifdef _WIN32
	return QString("%1\\.%2.swp").arg(fi.absolutePath()).arg(fi.fileName());
#else
	return QString("%1/.%2.swp").arg(fi.absolutePath()).arg(fi.fileName());
#endif
}

//1 文本 2 hex
enum NddDocType {
	TXT_TYPE = 1,
	BIG_TEXT_TYPE,
	HEX_TYPE,
};

void setDocTypeProperty(QWidget* pwidget, NddDocType type)
{
	QVariant v(type);
	pwidget->setProperty(Doc_Type, v);
}

int getDocTypeProperty(QWidget* pwidget)
{
	return pwidget->property(Doc_Type).toInt();
}

//#define STYLE_DEEPBLUE

#define STYLE_NOTEPAD

#ifdef STYLE_BLACK
const char *NewFileIcon = ":/Resources/edit/styleblack/newfile.png";
const char *OpenFileIcon = ":/Resources/edit/styleblack/openfile.png";
const char *NeedSaveBarIcon = ":/Resources/edit/styleblack/needsavebar.png";
const char *NoNeedSaveBarIcon = ":/Resources/edit/styleblack/noneedsavebar.png";
const char *NeedSaveAllBarIcon = ":/Resources/edit/styleblack/needsaveallbar.png";
const char *NoNeedSaveAllBarIcon = ":/Resources/edit/styleblack/noneedsaveallbar.png";
const char *CloseFileIcon = ":/Resources/edit/styleblack/closefile.png";
const char *CloseAllFileIcon = ":/Resources/edit/styleblack/closeall.png";
const char *CutIcon = ":/Resources/edit/styleblack/cut.png";
const char *CopyFileIcon = ":/Resources/edit/styleblack/copy.png";
const char *PasteIcon = ":/Resources/edit/styleblack/paste.png";
const char *UndoIcon = ":/Resources/edit/styleblack/undo.png";
const char *RedoIcon = ":/Resources/edit/styleblack/redo.png";
const char *FindIcon = ":/Resources/edit/styleblack/find.png";
const char *ReplaceIcon = ":/Resources/edit/styleblack/replace.png";
const char *ZoominIcon = ":/Resources/edit/styleblack/zoomin.png";
const char *ZoomoutIcon = ":/Resources/edit/styleblack/zoomout.png";
const char *CrlfIcon = ":/Resources/edit/styleblack/crlf.png";
const char *WhiteIcon = ":/Resources/edit/styleblack/white.png";
const char *FileCompareIcon = ":/Resources/edit/styleblack/filecompare.png";
const char *DirCompareIcon = ":/Resources/edit/styleblack/dircompare.png";
const char *TransCodeIcon = ":/Resources/edit/styleblack/transcode.png";
const char *RenameIcon = ":/Resources/edit/styleblack/rename.png";
#endif

#ifdef STYLE_DEEPBLUE
const char *NewFileIcon = ":/Resources/edit/styledeepblue/newfile.png";
const char *OpenFileIcon = ":/Resources/edit/styledeepblue/openfile.png";
const char *NeedSaveBarIcon = ":/Resources/edit/styledeepblue/needsavebar.png";
const char *NoNeedSaveBarIcon = ":/Resources/edit/styledeepblue/noneedsavebar.png";
const char *NeedSaveAllBarIcon = ":/Resources/edit/styledeepblue/needsaveallbar.png";
const char *NoNeedSaveAllBarIcon = ":/Resources/edit/styledeepblue/noneedsaveallbar.png";
const char *CloseFileIcon = ":/Resources/edit/styledeepblue/closefile.png";
const char *CloseAllFileIcon = ":/Resources/edit/styledeepblue/closeall.png";
const char *CutIcon = ":/Resources/edit/styledeepblue/cut.png";
const char *CopyFileIcon = ":/Resources/edit/styledeepblue/copy.png";
const char *PasteIcon = ":/Resources/edit/styledeepblue/paste.png";
const char *UndoIcon = ":/Resources/edit/styledeepblue/undo.png";
const char *RedoIcon = ":/Resources/edit/styledeepblue/redo.png";
const char *FindIcon = ":/Resources/edit/styledeepblue/find.png";
const char *ReplaceIcon = ":/Resources/edit/styledeepblue/replace.png";
const char *ZoominIcon = ":/Resources/edit/styledeepblue/zoomin.png";
const char *ZoomoutIcon = ":/Resources/edit/styledeepblue/zoomout.png";
const char *CrlfIcon = ":/Resources/edit/styledeepblue/crlf.png";
const char *WhiteIcon = ":/Resources/edit/styledeepblue/white.png";
const char *FileCompareIcon = ":/Resources/edit/styledeepblue/filecompare.png";
const char *DirCompareIcon = ":/Resources/edit/styledeepblue/dircompare.png";
const char *BinCmpIcon = ":/Resources/edit/styledeepblue/bincmp.png";
const char *TransCodeIcon = ":/Resources/edit/styledeepblue/transcode.png";
const char *RenameIcon = ":/Resources/edit/styledeepblue/rename.png";
const char *PreHexIcon = ":/Resources/edit/styledeepblue/pre.png";
const char *NextHexIcon = ":/Resources/edit/styledeepblue/next.png";
const char *GotoHexIcon = ":/Resources/edit/styledeepblue/goto.png";

const char *TabNeedSave = ":/Resources/edit/global/needsave.png";
const char *TabNoNeedSave = ":/Resources/edit/global/noneedsave.png";
#endif

#ifdef STYLE_NOTEPAD
const char* NewFileIcon = ":/notepad/newFile.png";
const char* OpenFileIcon = ":/notepad/openFile.png";
const char* NeedSaveBarIcon = ":/notepad/saveFile.png";
const char* NoNeedSaveBarIcon = ":/notepad/saveFile.png";
const char* NeedSaveAllBarIcon = ":/notepad/saveAll.png";
const char* NoNeedSaveAllBarIcon = ":/notepad/saveAll.png";
const char* AutoTimeSaveBarIcon = ":/notepad/autosave.png";
const char* CloseFileIcon = ":/notepad/closeFile.png";
const char* CloseAllFileIcon = ":/notepad/closeAll.png";
const char* CutIcon = ":/notepad/cut.png";
const char* CopyFileIcon = ":/notepad/copy.png";
const char* PasteIcon = ":/notepad/paste.png";
const char* UndoIcon = ":/notepad/undo.png";
const char* RedoIcon = ":/notepad/redo.png";
const char* FindIcon = ":/notepad/find.png";
const char* ReplaceIcon = ":/notepad/findReplace.png";
const char* MarkIcon = ":/notepad/mark.png";
const char* SignIcon = ":/notepad/sign.png";
const char* ClearSignIcon = ":/notepad/clearsign.png";
const char* ZoominIcon = ":/notepad/zoomIn.png";
const char* ZoomoutIcon = ":/notepad/zoomOut.png";
const char* CrlfIcon = ":/notepad/wrap.png";
const char* WhiteIcon = ":/notepad/invisibleChar.png";
const char* IndentIcon = ":/notepad/indentGuide.png";
const char* FileCompareIcon = ":/notepad/cmpfile.png";
const char* DirCompareIcon = ":/notepad/cmpdir.png";
const char* BinCmpIcon = ":/notepad/cmpbin.png";
const char* TransCodeIcon = ":/notepad/ecg.png";
const char* RenameIcon = ":/notepad/rename.png";
const char* PreHexIcon = ":/notepad/pre.png";
const char* NextHexIcon = ":/notepad/next.png";
const char* GotoHexIcon = ":/notepad/go.png";
const char* RightCloseIcon = ":/notepad/rightClose.png";

const char *TabNeedSave = ":/notepad/needsave.png";
const char *TabNoNeedSave = ":/notepad/noneedsave.png";
#endif

QString watchFilePath;
//文件后缀与语言关联,与在ScintillaEditView::langNames中的序号为关联

static QMap<QString, int> s_fileTypeToLangMap;

QList<QString> CCNotePad::s_findHistroy;

int CCNotePad::s_autoWarp = 0; //自动换行
int CCNotePad::s_indent = 0; //自动缩进
int CCNotePad::s_restoreLastFile = 1;//自动恢复上次打开的文件

//lexerName to index

struct FileExtLexer
{
	QString ext;
	LangType id;
};

const int FileExtMapLexerIdLen = L_EXTERNAL;

FileExtLexer s_fileExtMapLexerId[FileExtMapLexerIdLen] = {
{QString("h"), L_C},
{QString("c"), L_C},
{QString("cs"), L_CS},
{QString("cpp"), L_CPP},
{QString("cxx"), L_CPP},
{QString("rc"), L_RC},
{QString("html"), L_HTML},
{QString("htm"), L_HTML},
{QString("htmls"), L_HTML},
{QString("ini"), L_INI},
{QString("js"), L_JAVASCRIPT},
{QString("ts"), L_TYPESCRIPT},
{QString("css"), L_CSS},
{QString("java"), L_JAVA},
{QString("xml"), L_XML},
{QString("py"), L_PYTHON},
{QString("pas"), L_PASCAL},
{QString("php"), L_PHP},
{QString("sh"), L_BASH},
{QString("pl"), L_PERL},
{QString("rb"), L_RUBY},
{QString("bat"), L_BATCH},
{QString("go"), L_GO},
{QString("txt"), L_TXT},
{QString("pro"), L_INI},
{QString("pri"), L_INI},
{QString("json"), L_JSON},
{QString("lua"), L_LUA},
{QString("sql"), L_SQL},
{QString("yml"), L_YAML},
{QString("nsi"), L_NSIS},
{QString("NULL"), L_EXTERNAL},
};


//根据文件的后缀来确定文件的编程语言，进而设置默认的LEXER
void initFileTypeLangMap()
{
	if (s_fileTypeToLangMap.isEmpty())
	{
		for (int i = 0; i < FileExtMapLexerIdLen; ++i)
		{
			if (s_fileExtMapLexerId[i].id == L_EXTERNAL)
			{
				break;
			}
			else
			{
				s_fileTypeToLangMap.insert(s_fileExtMapLexerId[i].ext, s_fileExtMapLexerId[i].id);
			}
		}
	}
}

void  CCNotePad::initLexerNameToIndex()
{
	if (m_lexerNameToIndex.isEmpty())
	{
		LexerNode* pNodes = new LexerNode[100];

		int i = 0;

		pNodes[i].pAct = ui.actionAVS;
		pNodes[i].index = L_AVS;
		QVariant data((int)L_AVS);
		ui.actionAVS->setData(data);
		m_lexerNameToIndex.insert("avs", pNodes[i]);
        ++i;

		pNodes[i].pAct = ui.actionbash;
		pNodes[i].index = L_BASH;
		data.setValue(int(L_BASH));
		ui.actionbash->setData(data);
		m_lexerNameToIndex.insert("bash", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionBatch;
		pNodes[i].index = L_BATCH;
		data.setValue(int(L_BATCH));
		ui.actionBatch->setData(data);
		m_lexerNameToIndex.insert("batch", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionCMake;
		pNodes[i].index = L_CMAKE;
		data.setValue(int(L_CMAKE));
		ui.actionCMake->setData(data);
		m_lexerNameToIndex.insert("cmake", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionCoffeeScript;
		pNodes[i].index = L_COFFEESCRIPT;
		data.setValue(int(L_COFFEESCRIPT));
		ui.actionCoffeeScript->setData(data);
		m_lexerNameToIndex.insert("coffeescript", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionCPP;
		pNodes[i].index = L_CPP;
		data.setValue(int(L_CPP));
		ui.actionCPP->setData(data);
		m_lexerNameToIndex.insert("cpp", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionCShape;
		pNodes[i].index = L_CS;
		data.setValue(int(L_CS));
		ui.actionCShape->setData(data);
		m_lexerNameToIndex.insert("csharp", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionCss;
		pNodes[i].index = L_CSS;
		data.setValue(int(L_CSS));
		ui.actionCss->setData(data);
		m_lexerNameToIndex.insert("css", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionD_2;
		pNodes[i].index = L_D;
		data.setValue(int(L_D));
		ui.actionD_2->setData(data);
		m_lexerNameToIndex.insert("d", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionDiff;
		pNodes[i].index = L_DIFF;
		data.setValue(int(L_DIFF));
		ui.actionDiff->setData(data);
		m_lexerNameToIndex.insert("diff", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionEdifact;
		pNodes[i].index = L_EDIFACT;
		data.setValue(int(L_EDIFACT));
		ui.actionEdifact->setData(data);
		m_lexerNameToIndex.insert("edifact", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionFortran;
		pNodes[i].index = L_FORTRAN;
		data.setValue(int(L_FORTRAN));
		ui.actionFortran->setData(data);
		m_lexerNameToIndex.insert("fortran", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionFortran77;
		pNodes[i].index = L_FORTRAN_77;
		data.setValue(int(L_FORTRAN_77));
		ui.actionFortran77->setData(data);
		m_lexerNameToIndex.insert("fortran77", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionHTML;
		pNodes[i].index = L_HTML;
		data.setValue(int(L_HTML));
		ui.actionHTML->setData(data);
		m_lexerNameToIndex.insert("html", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionIDL;
		pNodes[i].index = L_IDL;
		data.setValue(int(L_IDL));
		ui.actionIDL->setData(data);
		m_lexerNameToIndex.insert("idl", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionJava;
		pNodes[i].index = L_JAVA;
		data.setValue(int(L_JAVA));
		ui.actionJava->setData(data);
		m_lexerNameToIndex.insert("java", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionJavaScript;
		pNodes[i].index = L_JAVASCRIPT;
		data.setValue(int(L_JAVASCRIPT));
		ui.actionJavaScript->setData(data);
		m_lexerNameToIndex.insert("javascript", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionjson;
		pNodes[i].index = L_JSON;
		data.setValue(int(L_JSON));
		ui.actionjson->setData(data);
		m_lexerNameToIndex.insert("json", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionLua;
		pNodes[i].index = L_LUA;
		data.setValue(int(L_LUA));
		ui.actionLua->setData(data);
		m_lexerNameToIndex.insert("lua", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionMakefile;
		pNodes[i].index = L_MAKEFILE;
		data.setValue(int(L_MAKEFILE));
		ui.actionMakefile->setData(data);
		m_lexerNameToIndex.insert("makefile", pNodes[i]);
		++i;

        pNodes[i].pAct = ui.actionMarkDown_2;
		pNodes[i].index = L_MARKDOWN;
		data.setValue(int(L_MARKDOWN));
        ui.actionMarkDown_2->setData(data);
		m_lexerNameToIndex.insert("markdown", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionMatlab;
		pNodes[i].index = L_MATLAB;
		data.setValue(int(L_MATLAB));
		ui.actionMatlab->setData(data);
		m_lexerNameToIndex.insert("matlab", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionNSIS;
		pNodes[i].index = L_NSIS;
		data.setValue(int(L_NSIS));
		ui.actionNSIS->setData(data);
		m_lexerNameToIndex.insert("nsis", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionOctave;
		pNodes[i].index = L_OCTAVE;
		data.setValue(int(L_OCTAVE));
		ui.actionOctave->setData(data);
		m_lexerNameToIndex.insert("octave", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionPascal;
		pNodes[i].index = L_PASCAL;
		data.setValue(int(L_PASCAL));
		ui.actionPascal->setData(data);
		m_lexerNameToIndex.insert("pascal", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionPerl;
		pNodes[i].index = L_PERL;
		data.setValue(int(L_PERL));
		ui.actionPerl->setData(data);
		m_lexerNameToIndex.insert("perl", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionPo;
		pNodes[i].index = L_PO;
		data.setValue(int(L_PO));
		ui.actionPo->setData(data);
		m_lexerNameToIndex.insert("po", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionPostScript;
		pNodes[i].index = L_PS;
		data.setValue(int(L_PS));
		ui.actionPostScript->setData(data);
		m_lexerNameToIndex.insert("postscript", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionPov;
		pNodes[i].index = L_POV;
		data.setValue(int(L_POV));
		ui.actionPov->setData(data);
		m_lexerNameToIndex.insert("pov", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionProperties_file;
		pNodes[i].index = L_INI;
		data.setValue(int(L_INI));
		ui.actionProperties_file->setData(data);
		m_lexerNameToIndex.insert("props", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionPython;
		pNodes[i].index = L_PYTHON;
		data.setValue(int(L_PYTHON));
		ui.actionPython->setData(data);
		m_lexerNameToIndex.insert("python", pNodes[i]);
		++i;


		pNodes[i].pAct = ui.actionRuby;
		pNodes[i].index = L_RUBY;
		data.setValue(int(L_RUBY));
		ui.actionRuby->setData(data);
		m_lexerNameToIndex.insert("ruby", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionSpice;
		pNodes[i].index = L_SPICE;
		data.setValue(int(L_SPICE));
		ui.actionSpice->setData(data);
		m_lexerNameToIndex.insert("spice", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionSql;
		pNodes[i].index = L_SQL;
		data.setValue(int(L_SQL));
		ui.actionSql->setData(data);
		m_lexerNameToIndex.insert("sql", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionTcl;
		pNodes[i].index = L_TCL;
		data.setValue(int(L_TCL));
		ui.actionTcl->setData(data);
		m_lexerNameToIndex.insert("tcl", pNodes[i]);
		++i;


		pNodes[i].pAct = ui.actionTex;
		pNodes[i].index = L_TEX;
		data.setValue(int(L_TEX));
		ui.actionTex->setData(data);
		m_lexerNameToIndex.insert("tex", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionVerilog;
		pNodes[i].index = L_VERILOG;
		data.setValue(int(L_VERILOG));
		ui.actionVerilog->setData(data);
		m_lexerNameToIndex.insert("verilog", pNodes[i]);
		++i;


		pNodes[i].pAct = ui.actionVHDL;
		pNodes[i].index = L_VHDL;
		data.setValue(int(L_VHDL));
		ui.actionVHDL->setData(data);
		m_lexerNameToIndex.insert("vhdl", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionXML;
		pNodes[i].index = L_XML;
		data.setValue(int(L_XML));
		ui.actionXML->setData(data);
		m_lexerNameToIndex.insert("xml", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionYAML;
		pNodes[i].index = L_YAML;
		data.setValue(int(L_YAML));
		ui.actionYAML->setData(data);
		m_lexerNameToIndex.insert("yaml", pNodes[i]);
		++i;

		//共用C++
		pNodes[i].pAct = ui.actionC;
		pNodes[i].index = L_C;
		data.setValue(int(L_C));
		ui.actionC->setData(data);
		m_lexerNameToIndex.insert("c", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionObjective_C;
		pNodes[i].index = L_OBJC;
		data.setValue(int(L_OBJC));
		ui.actionObjective_C->setData(data);
		m_lexerNameToIndex.insert("objc", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionRC;
		pNodes[i].index = L_RC;
		data.setValue(int(L_RC));
		ui.actionRC->setData(data);
		m_lexerNameToIndex.insert("rc", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionASP;
		pNodes[i].index = L_ASP;
		data.setValue(int(L_ASP));
		ui.actionASP->setData(data);
		m_lexerNameToIndex.insert("asp", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionJsp;
		pNodes[i].index = L_JSP;
		data.setValue(int(L_JSP));
		ui.actionJsp->setData(data);
		m_lexerNameToIndex.insert("jsp", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionPhp;
		pNodes[i].index = L_PHP;
		data.setValue(int(L_PHP));
		ui.actionPhp->setData(data);
		m_lexerNameToIndex.insert("php", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionini;
		pNodes[i].index = L_INI;
		data.setValue(int(L_INI));
		ui.actionini->setData(data);
		m_lexerNameToIndex.insert("ini", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionTypeScript;
		pNodes[i].index = L_TYPESCRIPT;
		data.setValue(int(L_TYPESCRIPT));
		ui.actionTypeScript->setData(data);
		m_lexerNameToIndex.insert("typescript", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionGo;
		pNodes[i].index = L_GO;
		data.setValue(int(L_GO));
		ui.actionGo->setData(data);
		m_lexerNameToIndex.insert("go", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionTxt;
		pNodes[i].index = L_TXT;
		data.setValue(int(L_TXT));
		ui.actionTxt->setData(data);
		m_lexerNameToIndex.insert("txt", pNodes[i]);
		++i;

		delete[]pNodes;

#if 0 //以下是目前不支持的
		pNodes[i].pAct = ui.actionNfo;
		pNodes[i].index = L_R;

		pNodes[i].pAct = ui.actionVirsual_Basic;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionShell;
		pNodes[i].index = L_R;

		pNodes[i].pAct = ui.actionNSIS;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionLisp;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionScheme;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionAssembly;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionSmalltalk;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionAutoIt;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionPowerShell;
		pNodes[i].index = L_R;

		pNodes[i].pAct = ui.actionBaanC;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionS_Record;
		pNodes[i].index = L_R;
	
		pNodes[i].pAct = ui.actionVisual_Prolog;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionTxt2tags;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionRust;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionRegistry;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionREBOL;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionOScript;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionNncrontab;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionNim;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionMMIXAL;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionLaTex;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionForth;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionESCRIPT;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionErlang;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionCsound;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionFreeBasic;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionBlitzBasic;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionPureBasic;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionAviSynth;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionASN1;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionSwift;
		pNodes[i].index = L_R;
		pNodes[i].pAct = ui.actionIntel_HEX;
		pNodes[i].index = L_R;
#endif

	}
}


#ifdef Q_OS_WIN
int CCNotePad::runAsAdmin(const QString& filePath)
{
	//已经是管理员了，直接返回错误
	if (s_isAdminAuth)
	{
		ui.statusBar->showMessage(tr("Run As Admin Failed to save the file. Please check the file permissions."));
		return 1;
	}

	TCHAR nddFullPath[MAX_PATH];
	::GetModuleFileName(NULL, nddFullPath, MAX_PATH);

	////先释放掉单一占位的内存
	//if (m_shareMem->isAttached())
	//{
	//	m_shareMem->detach();
	//}
	QString argStr = QString("-muti %1").arg(filePath);

	std::basic_string<TCHAR> args = StringToWString(argStr.toStdString());
	size_t shellExecRes = (size_t)::ShellExecute(NULL, TEXT("runas"), nddFullPath, args.c_str(), TEXT("."), SW_SHOW);

	// If the function succeeds, it returns a value greater than 32. If the function fails,
	// it returns an error value that indicates the cause of the failure.
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb762153%28v=vs.85%29.aspx

	if (shellExecRes < 32)
	{
		//失败后弹框。
		QMessageBox::warning(nullptr, tr("Error"), tr("Can't Get Admin Auth, Open File %1 failed").arg(filePath));
		return 1;
	}
	else
	{
		//成功后暂时不退出，因为amin存在的还有存在一些问题，比如后续文件不能右键菜单打开
		//所以还需要维持当前这个界面存在
		//成功获取权限后当前退出。这里其实还需要做一些释放操作
		/*if (m_shareMem->isAttached())
		{
			m_shareMem->detach();
		}
		exit(0);*/
		return 1;
	}
}
#endif

//根据文件类型给出语言id
LangType CCNotePad::getLangLexerIdByFileExt(QString filePath)
{
	QFileInfo fi(filePath);
	QString ext = fi.suffix();

	if (s_fileTypeToLangMap.contains(ext))
	{
		return static_cast<LangType>(s_fileTypeToLangMap.value(ext));
	}

	return L_TXT;
}

CCNotePad::CCNotePad(bool isMainWindows, QWidget *parent)
	: QMainWindow(parent), m_cutFile(nullptr),m_copyFile(nullptr), m_dockSelectTreeWin(nullptr), \
	m_pResultWin(nullptr),m_isQuitCancel(false), m_tabRightClickMenu(nullptr), m_shareMem(nullptr),m_isMainWindows(isMainWindows),\
	m_openInNewWinAct(nullptr), m_showFileDirAct(nullptr), m_timerAutoSave(nullptr)
{
	ui.setupUi(this);


#ifdef Q_OS_MAC
    setWindowIcon(QIcon(":/mac.icns"));
#endif

	JsonDeploy::init();

	m_translator = new QTranslator(this);

		m_curSoftLangs = JsonDeploy::getKeyValueFromNumSets(LANGS_KEY);

		switch (m_curSoftLangs)
		{
		case 0: //自动选择
		{
		QLocale local;
		if (local.language() == QLocale::Chinese)
		{
			slot_changeChinese();
		}
			break;
	}
		case 1:
			slot_changeChinese();
			break;
		case 2:
			slot_changeEnglish();
			break;
		default:
			break;
		}
		
	if (s_padInstances == nullptr)
	{
		s_padInstances = new QList<CCNotePad*>();
		s_padInstances->append(this);
	}
	//启用拖动
	setAcceptDrops(true);

	initFileTypeLangMap();
	initLexerNameToIndex();

	//这里在主线程里面调用一下，避免后续因为没有创建，而可能在子线程中初始化里面的值，而且多个子线程引发重入竞争问题
	//20220402在1.11中发现这个问题。所以加上这里的手动调用
	DocTypeListView::initSupportFileTypes();

	ui.editTabWidget->setTabsClosable(true);
#if 0
	QString tabQss = "QTabBar::tab{ \
		background-color:#EAF7FF;\
		margin-top:1px; \
		margin-right:1px; \
		margin-left:1px; \
		margin-bottom:2px; \
		padding:0px;\
		}\
		QTabBar::tab:selected{ \
		background-color:rgb(255,255,255);\
		border-top:3px solid;\
		border-top-color:#FAAA3C;\
		margin-top:1px; \
		margin-right:1px; \
		margin-left:1px; \
		margin-bottom:2px; \
		padding:0px;\
		}\
		QTabBar::close-button{ \
		image: url(\":/notepad/closeTabButton.png\");\
		}\
		QTabBar::close-button:hover{ \
		image: url(\":/notepad/closeTabButton_hover.png\");\
		}\
		QTabBar{\
		qproperty-expanding:false;\
		qproperty-usesScrollButtons:true;\
		qproperty-documentMode:true;\
		}\
		";

	//ui.editTabWidget->setStyleSheet(tabQss);
#endif


	QTabBar* pBar = ui.editTabWidget->tabBar();

	connect(pBar,&QTabBar::tabCloseRequested,this,&CCNotePad::slot_tabClose);


	m_codeStatusLabel = new QLabel("UTF8", ui.statusBar);
#ifdef Q_OS_WIN
	m_lineEndLabel = new QLabel("Windows(CR LF)", ui.statusBar);
#endif
#ifdef Q_OS_UNIX
	m_lineEndLabel = new QLabel("Windows(LF)", ui.statusBar);
#endif
	m_lineNumLabel = new QLabel(tr("Ln:0	Col:0"), ui.statusBar);

	m_langDescLabel = new QLabel("Txt", ui.statusBar);
	m_codeStatusLabel->setMinimumWidth(120);
	m_lineEndLabel->setMinimumWidth(100);
	m_lineNumLabel->setMinimumWidth(120);
	m_langDescLabel->setMinimumWidth(100);

	//0在前面，越小越在左边
	ui.statusBar->insertPermanentWidget(0, m_langDescLabel);
	ui.statusBar->insertPermanentWidget(1, m_lineNumLabel);
	ui.statusBar->insertPermanentWidget(2, m_lineEndLabel);
	ui.statusBar->insertPermanentWidget(3, m_codeStatusLabel);

	initToolBar();

	m_saveFile->setEnabled(false);
	m_saveAllFile->setEnabled(false);

	initReceneOpenFileMenu();

	//最后加入退出菜单
	ui.menuFile->addSeparator();
	m_quitAction = ui.menuFile->addAction(tr("Quit"), this, &CCNotePad::slot_quit);
	m_quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

	connect(ui.editTabWidget,&QTabWidget::currentChanged,this,&CCNotePad::slot_tabCurrentChanged);
	connect(ui.editTabWidget, &QTabWidget::tabBarClicked, this, &CCNotePad::slot_tabBarClicked);

	m_fileWatch = new QFileSystemWatcher(this);
	connect(m_fileWatch,&QFileSystemWatcher::fileChanged,this, &CCNotePad::slot_fileChange);


	//只有主窗口才监控openwith的文件
	if (isMainWindows)
	{
		initNotePadSqlOptions();
	}

	setToFileRightMenu();
	}

CCNotePad::~CCNotePad()
{
	//只有主窗口，才有保存最近打开列表的权力
	if (m_isMainWindows)
	{
		saveReceneOpenFile();
		saveNotePadSqlOptions();
		savePadUseTimes();
	}
	JsonDeploy::close();
}

#ifdef Q_OS_WIN
void CCNotePad::checkAppFont()
{
	//检查win下面的字体，win11有时默认字体是楷体
	QFont srcFont = QApplication::font();
	//win11发现字体是楷体。检测并设置一下
	if (QString("SimSun") != srcFont.family())
	{
		QFont font(QString("Courier"), 9);
		QApplication::setFont(font);
		ui.statusBar->showMessage(tr("If display exceptions,Please Install System Font Courier"));
	}
}
#endif

void CCNotePad::syncCurSkinToMenu(int id)
{
	switch (id)
	{
	case 0:
		ui.actionDefaultStyle->setChecked(true);
		break;
	case 1:
		ui.actionLightBlueStyle->setChecked(true);
		break;
	case 2:
		ui.actionThinBlue->setChecked(true);
		break;
	case 3:
		ui.actionYellow->setChecked(true);
		break;
	case 4:
		ui.actionRiceYellow->setChecked(true);
		break;
	case 5:
		ui.actionSilver->setChecked(true);
		break;
	case 6:
		ui.actionLavenderBlush->setChecked(true);
		break;
	case 7:
		ui.actionMistyRose->setChecked(true);
		break;
	default:
		ui.actionDefaultStyle->setChecked(true);
		break;
	}
}

void CCNotePad::slot_changeChinese()
{
	if (m_translator->load(":/realcompare_zh.qm"))
	{
		qApp->installTranslator(m_translator);
		ui.retranslateUi(this);

		if (m_curSoftLangs != 1)
		{
			m_curSoftLangs = 1;
			JsonDeploy::updataKeyValueFromNumSets(LANGS_KEY, m_curSoftLangs);
	}
}
}

#ifdef uos
void CCNotePad::adjustWInPos(QWidget* pWin)
{
    QPoint globalPos = this->mapToGlobal(QPoint(0,0));//父窗口绝对坐标
    int x = globalPos.x() + 300;//x坐标
    int y = globalPos.y() + 100;//y坐标
    pWin->move(x, y);
}
#endif

void CCNotePad::slot_changeEnglish()
{
	m_translator->load("");
	qApp->installTranslator(m_translator);
	ui.retranslateUi(this);

	if (m_curSoftLangs != 2)
	{
		m_curSoftLangs = 2;
		JsonDeploy::updataKeyValueFromNumSets(LANGS_KEY, m_curSoftLangs);
}
}
#if 0
void CCNotePad::saveDefFont()
{
	if (s_txtFont.toString() != m_txtFontStr)
	{
		QString newFont = s_txtFont.toString();
		JsonDeploy::updataKeyValueFromSets(TXT_FONT, newFont);
	}

	if (s_proLangFont.toString() != m_proLangFontStr)
	{
		QString newFont = s_proLangFont.toString();
		JsonDeploy::updataKeyValueFromSets(PRO_LANG_FONT, newFont);
}
}
#endif

void CCNotePad::savePadUseTimes()
{
	QString key("padtimes");

	int times = JsonDeploy::getKeyValueFromNumSets(key);

	JsonDeploy::updataKeyValueFromNumSets(key, s_padTimes + times);
}

void CCNotePad::slot_searchResultShow()
{
	if (m_dockSelectTreeWin != nullptr)
	{
		m_dockSelectTreeWin->show();
	}
}

//读取Sql的全局配置
void CCNotePad::initNotePadSqlOptions()
{
	//tab的长度，默认为4
	QString key("tablens");
	ScintillaEditView::s_tabLens = JsonDeploy::getKeyValueFromNumSets(key);

	//space replace tab空格替换tab，默认1
	QString key1("tabnouse");
	ScintillaEditView::s_noUseTab = (1 == JsonDeploy::getKeyValueFromNumSets(key1)) ? true : false;

	ScintillaEditView::s_bigTextSize = JsonDeploy::getKeyValueFromNumSets(MAX_BIG_TEXT);
	if (ScintillaEditView::s_bigTextSize < 50 || ScintillaEditView::s_bigTextSize > 300)
	{
		ScintillaEditView::s_bigTextSize = 100;
}

	s_restoreLastFile = JsonDeploy::getKeyValueFromNumSets(RESTORE_CLOSE_FILE);
}
//保存Sql的全局配置
void CCNotePad::saveNotePadSqlOptions()
{
	QString key("tablens");
	JsonDeploy::updataKeyValueFromNumSets(key, ScintillaEditView::s_tabLens);

	QString key1("tabnouse");
	JsonDeploy::updataKeyValueFromNumSets(key1, ScintillaEditView::s_noUseTab?1:0);

	JsonDeploy::updataKeyValueFromNumSets(MAX_BIG_TEXT, ScintillaEditView::s_bigTextSize);

	JsonDeploy::updataKeyValueFromNumSets(RESTORE_CLOSE_FILE, CCNotePad::s_restoreLastFile);
}

//设置程序为文件右键的关联项目
void CCNotePad::setToFileRightMenu()
{
#if 0
#ifdef _WIN32
#ifdef _DEBUG
	return;
#endif
	QString exepath = QCoreApplication::applicationFilePath();
	exepath = exepath.replace("/", "\\");
	QString iconTxt = exepath;
	exepath += " \"%1\"";

	QString menuDisplayName(tr("Edit with Notepad--"));
	QString keyPath = "HKEY_CLASSES_ROOT\\*\\shell\\" + menuDisplayName + "\\command";
	QString iconPath = "HKEY_CLASSES_ROOT\\*\\shell\\" + menuDisplayName;
	QSettings settings(keyPath, QSettings::NativeFormat);
	QSettings iconSettings(iconPath, QSettings::NativeFormat);

	if (settings.value(".").toString() != exepath)
	{
		settings.setValue(".", exepath);
		iconSettings.setValue("Icon", iconTxt);

		//在读一次
		if (settings.value(".").toString() != exepath)
		{
			//QMessageBox::warning(nullptr, tr("Notice"), tr("Please run in admin auth"));
			ui.statusBar->showMessage(tr("Please run in admin auth"));
		}
	}

#endif
#endif
}

void CCNotePad::slot_fileChange(QString filePath)
{
	
	QWidget *pw = nullptr;

	for (int i = 0; i < ui.editTabWidget->count(); ++i)
	{
		pw = ui.editTabWidget->widget(i);
		if (pw != nullptr)
		{
			if (pw->property(Edit_View_FilePath) == filePath)
			{
		
				//这里只设置1个标志，下次获取焦点时，才判定询问是否需要重新加载
				pw->setProperty(Modify_Outside, QVariant(true));;
				break;
			}
		}

	}
}

void  CCNotePad::slot_tabBarClicked(int index)
{
	QWidget* pw = ui.editTabWidget->widget(index);
	int docType = getDocTypeProperty(pw);
	if (TXT_TYPE == docType)
	{
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (checkRoladFile(pEdit))
		{
			return;
		}
	}

	//右键菜单
	if (Qt::RightButton == QGuiApplication::mouseButtons())
	{
		if (m_tabRightClickMenu == nullptr)
		{
			m_tabRightClickMenu = new QMenu(this);

			m_tabRightClickMenu->addAction(tr("Close Current Document"),this,&CCNotePad::slot_actionClose);
			m_tabRightClickMenu->addAction(tr("Close Non-Current documents"),this, &CCNotePad::slot_actionCloseNonCurDoc);
			m_tabRightClickMenu->addAction(tr("Close Left All"),this, &CCNotePad::slot_actionCloseLeftAll);
			m_tabRightClickMenu->addAction(tr("Close Right All"),this, &CCNotePad::slot_actionCloseRightAll);
			m_tabRightClickMenu->addSeparator();

			m_tabRightClickMenu->addAction(tr("Rename Current Document "), this, &CCNotePad::slot_actionRenameFile_toggle);
			m_tabRightClickMenu->addAction(tr("Current Document Sava as..."), this, &CCNotePad::slot_actionSaveAsFile_toggle);
			m_openInNewWinAct = m_tabRightClickMenu->addAction(tr("Open in New Window"),this, &CCNotePad::slot_openFileInNewWin);
			m_showFileDirAct = m_tabRightClickMenu->addAction(tr("Show File in Explorer..."), this, &CCNotePad::slot_showFileInExplorer);
			m_tabRightClickMenu->addSeparator();

			m_openWithText = m_tabRightClickMenu->addAction(tr("Reload With Text Mode"), this, &CCNotePad::slot_reOpenTextMode);
			m_openWithHex = m_tabRightClickMenu->addAction(tr("Reload With Hex Mode"), this, &CCNotePad::slot_reOpenHexMode);

			m_tabRightClickMenu->addSeparator();
			m_selectLeftCmp = m_tabRightClickMenu->addAction(tr("Select Left Cmp File"), this, &CCNotePad::slot_selectLeftFile);
			m_selectRightCmp = m_tabRightClickMenu->addAction(tr("Select Right Cmp File"), this, &CCNotePad::slot_selectRightFile);
	
		}

		//非new文件才能重新打开和定位到文件
		if (getFileNewIndexProperty(pw) == -1)
		{
			m_openInNewWinAct->setEnabled(true);
			m_showFileDirAct->setEnabled(true);
		}
		else
		{
			m_openInNewWinAct->setEnabled(false);
			m_showFileDirAct->setEnabled(false);
		}


		if ((TXT_TYPE == docType) && (getFileNewIndexProperty(pw) == -1))
		{
			m_openWithText->setEnabled(false);
			m_openWithHex->setEnabled(true);

			m_selectLeftCmp->setEnabled(true);
			m_selectRightCmp->setEnabled(true);
		}
		else if (HEX_TYPE == docType)
		{
			m_openWithText->setEnabled(true);
			m_openWithHex->setEnabled(false);

			m_selectLeftCmp->setEnabled(false);
			m_selectRightCmp->setEnabled(false);
		}
		else if ((TXT_TYPE == docType) && (getFileNewIndexProperty(pw) != -1))
		{
			//新的文本，都不能
			m_openWithText->setEnabled(false);
			m_openWithHex->setEnabled(false);

			m_selectLeftCmp->setEnabled(false);
			m_selectRightCmp->setEnabled(false);
		}
		else if(BIG_TEXT_TYPE == docType)
		{
			m_openWithText->setEnabled(false);
			m_openWithHex->setEnabled(true);
			m_selectLeftCmp->setEnabled(false);
			m_selectRightCmp->setEnabled(false);
		}

		ui.editTabWidget->setCurrentIndex(index);
		m_tabRightClickMenu->move(cursor().pos());
		m_tabRightClickMenu->show();
	}

}

void CCNotePad::slot_reOpenTextMode()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	QString filePath = getFilePathProperty(pw);

	if (HEX_TYPE != getDocTypeProperty(pw))
	{
		ui.statusBar->showMessage(tr("The currently file %1 is already in text mode").arg(filePath),5000);
		return;
	}

	slot_actionClose(true);
	openTextFile(filePath,false);
	
}

void CCNotePad::slot_reOpenHexMode()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	QString filePath = getFilePathProperty(pw);

	if (HEX_TYPE == getDocTypeProperty(pw))
	{
		ui.statusBar->showMessage(tr("The currently file %1 is already in bin mode").arg(filePath),5000);
		return;
	}

	slot_actionClose(true);
	openHexFile(filePath);
}

void CCNotePad::slot_selectLeftFile()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr)
	{
		m_cmpLeftFilePath = pw->property(Edit_View_FilePath).toString();

		if (!m_cmpLeftFilePath.isEmpty() && !m_cmpRightFilePath.isEmpty())
		{
			cmpSelectFile();
			m_cmpLeftFilePath.clear();
			m_cmpRightFilePath.clear();
		}
	}

}

void CCNotePad::slot_selectRightFile()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr)
	{
		m_cmpRightFilePath = pw->property(Edit_View_FilePath).toString();

		if (!m_cmpLeftFilePath.isEmpty() && !m_cmpRightFilePath.isEmpty())
		{
			cmpSelectFile();
			m_cmpLeftFilePath.clear();
			m_cmpRightFilePath.clear();
		}
	}
}

//为了避免路径中\\不一样导致的查找不到问题，进行统一替换
QString getRegularFilePath(QString& path)
{
#ifdef _WIN32
	path = path.replace("/", "\\");
#else
	path = path.replace("\\", "/");
#endif

	return path;
}

void CCNotePad::slot_showFileInExplorer()
{
	QString path, cmd;
	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr)
	{
		path = pw->property(Edit_View_FilePath).toString();
	}
	
#ifdef _WIN32
	path = path.replace("/", "\\");
	cmd = QString("explorer.exe /select,%1").arg(path);
#endif
#ifdef uos
	path = path.replace("\\", "/");
    cmd = QString("dde-file-manager %1").arg(path);
#endif

#if defined(Q_OS_MAC)
	path = path.replace("\\", "/");
	cmd = QString("open -R %1").arg(path);
#endif

	QProcess process;
	process.startDetached(cmd);
}


void CCNotePad::slot_openFileInNewWin()
{
	QString path;
	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr)
	{
		path = pw->property(Edit_View_FilePath).toString();
		slot_actionClose(true);

		CCNotePad* pNewWin = new CCNotePad(false,nullptr);
		pNewWin->setShareMem(this->getShareMem());
		pNewWin->setAttribute(Qt::WA_DeleteOnClose);
		pNewWin->openFile(path);
		pNewWin->show();
#ifdef uos
    adjustWInPos(pNewWin);
#endif
		s_padInstances->append(pNewWin);
	}
}

void CCNotePad::autoSetDocLexer(ScintillaEditView* pEdit)
{
	QString filePath = pEdit->property(Edit_View_FilePath).toString();

	OpenAttr openType = (OpenAttr)pEdit->property(Open_Attr).toInt();
	if (OpenAttr::Text != openType)
	{
		return;
	}

	LangType type = getLangLexerIdByFileExt(filePath);

	QsciLexer* lexer = pEdit->createLexer(type);

	if (lexer != nullptr)
	{
		if (nullptr != pEdit->lexer())
		{
			delete pEdit->lexer();
		}

		pEdit->setLexer(lexer);
		syncCurDocLexerToMenu(pEdit);
	}
	else
	{
		setTxtLexer(pEdit);
		syncCurDocLexerToMenu(pEdit);
	}
}

void CCNotePad::updateTitleToCurDocFilePath()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr)
	{
		setWindowTitle(pw->property(Edit_View_FilePath).toString());
	}
}

//切换tab后
void CCNotePad::slot_tabCurrentChanged(int index)
{
	QWidget* pw = ui.editTabWidget->widget(index);
	if (pw != nullptr)
	{
		//16进制的处理逻辑
		int docType = getDocTypeProperty(pw);
		if (HEX_TYPE == docType)
		{
			//setWindowTitle(pw->property(Edit_View_FilePath).toString());
			//m_wordwrap->setChecked(false);
			//m_allWhite->setChecked(false);
			QString filePath = getFilePathProperty(pw);
			setWindowTitleMode(filePath, OpenAttr::HexReadOnly);
			return;
		}
		else if ((TXT_TYPE == docType)||(BIG_TEXT_TYPE == docType))
		{

			int code = pw->property(Edit_Text_Code).toInt();
			setCodeBarLabel(static_cast<CODE_ID>(code));

			int lineEnd = pw->property(Edit_Text_End).toInt();
			setLineEndBarLabel(static_cast<RC_LINE_FORM>(lineEnd));

			if (pw->property(Edit_Text_Change).toBool())
			{
				m_saveFile->setEnabled(true);
			}
			else
			{
				m_saveFile->setEnabled(false);
			}

			if (TXT_TYPE == docType)
			{
				//setWindowTitle(pw->property(Edit_View_FilePath).toString());
				setWindowTitleMode(pw->property(Edit_View_FilePath).toString(), (OpenAttr)pw->property(Open_Attr).toInt());
			}
			else if (BIG_TEXT_TYPE == docType)
			{
				//setWindowTitle(QString("%1 (%2)").arg(pw->property(Edit_View_FilePath).toString()).arg(tr("Big Text File ReadOnly")));
				setWindowTitleMode(pw->property(Edit_View_FilePath).toString(), OpenAttr::BigTextReadOnly);
			}

			syncCurDocEncodeToMenu(pw);
			syncCurDocLineEndStatusToMenu(pw);
			syncCurDocLexerToMenu(pw);

			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

			//目前只用了0 1 两种换行模式。
			if (s_autoWarp != pEdit->wrapMode())
			{
				pEdit->setWrapMode((QsciScintilla::WrapMode)s_autoWarp);
			}
			
			if (QsciScintilla::WsVisible == pEdit->whitespaceVisibility())
			{
				m_allWhite->setChecked(true);
			}
			else
			{
				m_allWhite->setChecked(false);
			}
		}
	}
}

void CCNotePad::initToolBar()
{
	const int ICON_SIZE = 24;

	s_autoWarp = JsonDeploy::getKeyValueFromNumSets(AUTOWARP_KEY);
	ui.actionWrap->setChecked((s_autoWarp == 1));
	

	s_indent = JsonDeploy::getKeyValueFromNumSets(INDENT_KEY);

	QToolButton* newFile = new QToolButton(ui.mainToolBar);
	connect(newFile, &QAbstractButton::clicked, this, &CCNotePad::slot_actionNewFile_toggle);
	newFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	newFile->setIcon(QIcon(NewFileIcon));
	newFile->setToolTip(tr("New File"));
	ui.mainToolBar->addWidget(newFile);

	QToolButton* openFile = new QToolButton(ui.mainToolBar);
	connect(openFile, &QAbstractButton::clicked, this, &CCNotePad::slot_actionOpenFile_toggle);
	openFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	openFile->setIcon(QIcon(OpenFileIcon));
	openFile->setToolTip(tr("Open File"));
	ui.mainToolBar->addWidget(openFile);

	m_saveFile = new QToolButton(ui.mainToolBar);
	connect(m_saveFile, &QAbstractButton::clicked, this, &CCNotePad::slot_actionSaveFile_toggle);
	m_saveFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon));
	m_saveFile->setToolTip(tr("Save File"));
	ui.mainToolBar->addWidget(m_saveFile);

	m_saveAllFile = new QToolButton(ui.mainToolBar);
	connect(m_saveAllFile, &QAbstractButton::clicked, this, &CCNotePad::slot_saveAllFile);
	m_saveAllFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_saveAllFile->setIcon(QIcon(NoNeedSaveAllBarIcon));
	m_saveAllFile->setToolTip(tr("Save All File"));
	ui.mainToolBar->addWidget(m_saveAllFile);

	m_autoSaveAFile = new QToolButton(ui.mainToolBar);
	m_autoSaveAFile->setCheckable(true);
	connect(m_autoSaveAFile, &QAbstractButton::clicked, this, &CCNotePad::slot_autoSaveFile);
	m_autoSaveAFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_autoSaveAFile->setIcon(QIcon(AutoTimeSaveBarIcon));
	m_autoSaveAFile->setToolTip(tr("Cycle Auto Save"));
	ui.mainToolBar->addWidget(m_autoSaveAFile);
	

	QToolButton* closeFile = new QToolButton(ui.mainToolBar);
	connect(closeFile, &QAbstractButton::clicked, this, &CCNotePad::slot_actionClose);
	closeFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	closeFile->setIcon(QIcon(CloseFileIcon));
	closeFile->setToolTip(tr("Close"));
	ui.mainToolBar->addWidget(closeFile);

	QToolButton* closeAllFile = new QToolButton(ui.mainToolBar);
	connect(closeAllFile, &QAbstractButton::clicked, this, &CCNotePad::slot_closeAllFile);
	closeAllFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	closeAllFile->setIcon(QIcon(CloseAllFileIcon));
	closeAllFile->setToolTip(tr("Close All"));
	ui.mainToolBar->addWidget(closeAllFile);

	ui.mainToolBar->addSeparator();

	m_cutFile = new QToolButton(ui.mainToolBar);
	connect(m_cutFile, &QAbstractButton::clicked, this, &CCNotePad::slot_cut);
	m_cutFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_cutFile->setIcon(QIcon(CutIcon));
	m_cutFile->setToolTip(tr("Cut"));
	ui.mainToolBar->addWidget(m_cutFile);

	m_copyFile = new QToolButton(ui.mainToolBar);
	connect(m_copyFile, &QAbstractButton::clicked, this, &CCNotePad::slot_copy);
	m_copyFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_copyFile->setIcon(QIcon(CopyFileIcon));
	m_copyFile->setToolTip(tr("Copy"));
	ui.mainToolBar->addWidget(m_copyFile);

	QToolButton* pasteFile = new QToolButton(ui.mainToolBar);
	connect(pasteFile, &QAbstractButton::clicked, this, &CCNotePad::slot_paste);
	pasteFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	pasteFile->setIcon(QIcon(PasteIcon));
	pasteFile->setToolTip(tr("Paste"));
	ui.mainToolBar->addWidget(pasteFile);

	ui.mainToolBar->addSeparator();

	m_undo = new QToolButton(ui.mainToolBar);
	connect(m_undo, &QAbstractButton::clicked, this, &CCNotePad::slot_undo);
	m_undo->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_undo->setIcon(QIcon(UndoIcon));
	m_undo->setToolTip(tr("Undo"));
	ui.mainToolBar->addWidget(m_undo);

	m_redo = new QToolButton(ui.mainToolBar);
	connect(m_redo, &QAbstractButton::clicked, this, &CCNotePad::slot_redo);
	m_redo->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_redo->setIcon(QIcon(RedoIcon));
	m_redo->setToolTip(tr("Redo"));
	ui.mainToolBar->addWidget(m_redo);

	ui.mainToolBar->addSeparator();

	QToolButton* findText = new QToolButton(ui.mainToolBar);
	connect(findText, &QAbstractButton::clicked, this, &CCNotePad::slot_find);
	findText->setFixedSize(ICON_SIZE, ICON_SIZE);
	findText->setIcon(QIcon(FindIcon));
	findText->setToolTip(tr("Find"));
	ui.mainToolBar->addWidget(findText);

	QToolButton* replaceText = new QToolButton(ui.mainToolBar);
	connect(replaceText, &QAbstractButton::clicked, this, &CCNotePad::slot_replace);
	replaceText->setFixedSize(ICON_SIZE, ICON_SIZE);
	replaceText->setIcon(QIcon(ReplaceIcon));
	replaceText->setToolTip(tr("Replace"));
	ui.mainToolBar->addWidget(replaceText);

	QToolButton* markText = new QToolButton(ui.mainToolBar);
	connect(markText, &QAbstractButton::clicked, this, &CCNotePad::slot_markHighlight);
	markText->setFixedSize(ICON_SIZE, ICON_SIZE);
	markText->setIcon(QIcon(MarkIcon));
	markText->setToolTip(tr("Mark"));
	ui.mainToolBar->addWidget(markText);

	ui.mainToolBar->addSeparator();

	//选择单词高亮
	QToolButton* signText = new QToolButton(ui.mainToolBar);
	connect(signText, &QAbstractButton::clicked, this, &CCNotePad::slot_wordHighlight);
	signText->setFixedSize(ICON_SIZE, ICON_SIZE);
	signText->setIcon(QIcon(SignIcon));
	signText->setToolTip(tr("word highlight(F8)"));
	signText->setShortcut(QKeySequence(Qt::Key_F8));
	ui.mainToolBar->addWidget(signText);

	//选择单词高亮
	QToolButton* clearMark = new QToolButton(ui.mainToolBar);
	connect(clearMark, &QAbstractButton::clicked, this, &CCNotePad::slot_clearMark);
	clearMark->setFixedSize(ICON_SIZE, ICON_SIZE);
	clearMark->setIcon(QIcon(ClearSignIcon));
	clearMark->setToolTip(tr("clear all highlight(F7)"));
	clearMark->setShortcut(QKeySequence(Qt::Key_F7));
	ui.mainToolBar->addWidget(clearMark);

	
	ui.mainToolBar->addSeparator();

	QToolButton* zoomin = new QToolButton(ui.mainToolBar);
	connect(zoomin, &QAbstractButton::clicked, this, &CCNotePad::slot_zoomin);
	zoomin->setFixedSize(ICON_SIZE, ICON_SIZE);
	zoomin->setIcon(QIcon(ZoominIcon));
	zoomin->setToolTip(tr("Zoom In"));
	ui.mainToolBar->addWidget(zoomin);

	QToolButton* zoomout = new QToolButton(ui.mainToolBar);
	connect(zoomout, &QAbstractButton::clicked, this, &CCNotePad::slot_zoomout);
	zoomout->setFixedSize(ICON_SIZE, ICON_SIZE);
	zoomout->setIcon(QIcon(ZoomoutIcon));
	zoomout->setToolTip(tr("Zoom Out"));
	ui.mainToolBar->addWidget(zoomout);

	ui.mainToolBar->addSeparator();

	m_wordwrap = new QToolButton(ui.mainToolBar);
	m_wordwrap->setCheckable(true);
	m_wordwrap->setChecked((s_autoWarp == 1));
	connect(m_wordwrap, &QAbstractButton::toggled, this, &CCNotePad::slot_wordwrap);
	m_wordwrap->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_wordwrap->setIcon(QIcon(CrlfIcon));
	m_wordwrap->setToolTip(tr("Word Wrap"));
	ui.mainToolBar->addWidget(m_wordwrap);

	ui.actionWrap->setChecked((s_autoWarp == 1));

	connect(ui.actionWrap, &QAction::toggled, m_wordwrap, [&](bool check) {
		//避免循环触发
		if (check != m_wordwrap->isChecked())
		{
			m_wordwrap->setChecked(check);
		}
	});


	m_allWhite = new QToolButton(ui.mainToolBar);
	m_allWhite->setCheckable(true);
	connect(m_allWhite, &QAbstractButton::toggled, this, &CCNotePad::slot_allWhite);
	m_allWhite->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_allWhite->setIcon(QIcon(WhiteIcon));
	m_allWhite->setToolTip(tr("Show Blank"));
	ui.mainToolBar->addWidget(m_allWhite);

	m_indentGuide = new QToolButton(ui.mainToolBar);
	m_indentGuide->setCheckable(true);
	m_indentGuide->setChecked((s_indent == 1));
	connect(m_indentGuide, &QAbstractButton::toggled, this, &CCNotePad::slot_indentGuide);
	m_indentGuide->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_indentGuide->setIcon(QIcon(IndentIcon));
	m_indentGuide->setToolTip(tr("Indent Guide"));
	ui.mainToolBar->addWidget(m_indentGuide);

	ui.mainToolBar->addSeparator();

	m_preHexPage = new QToolButton(ui.mainToolBar);
	connect(m_preHexPage, &QAbstractButton::clicked, this, &CCNotePad::slot_preHexPage);
	m_preHexPage->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_preHexPage->setIcon(QIcon(PreHexIcon));
	m_preHexPage->setToolTip(tr("Pre Hex Page"));
	ui.mainToolBar->addWidget(m_preHexPage);

	m_nextHexPage = new QToolButton(ui.mainToolBar);
	connect(m_nextHexPage, &QAbstractButton::clicked, this, &CCNotePad::slot_nextHexPage);
	m_nextHexPage->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_nextHexPage->setIcon(QIcon(NextHexIcon));
	m_nextHexPage->setToolTip(tr("Next Hex Page"));
	ui.mainToolBar->addWidget(m_nextHexPage);

	m_gotoHexPage = new QToolButton(ui.mainToolBar);
	connect(m_gotoHexPage, &QAbstractButton::clicked, this, &CCNotePad::slot_gotoHexPage);
	m_gotoHexPage->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_gotoHexPage->setIcon(QIcon(GotoHexIcon));
	m_gotoHexPage->setToolTip(tr("Goto Hex Page"));
	ui.mainToolBar->addWidget(m_gotoHexPage);

	ui.mainToolBar->addSeparator();

	QToolButton* transcode = new QToolButton(ui.mainToolBar);
	connect(transcode, &QAbstractButton::clicked, this, &CCNotePad::slot_batch_convert);
	transcode->setFixedSize(ICON_SIZE, ICON_SIZE);
	transcode->setIcon(QIcon(TransCodeIcon));
	transcode->setToolTip(tr("transform encoding"));
	ui.mainToolBar->addWidget(transcode);

	QToolButton* rename = new QToolButton(ui.mainToolBar);
	connect(rename, &QAbstractButton::clicked, this, &CCNotePad::slot_batch_rename);
	rename->setFixedSize(ICON_SIZE, ICON_SIZE);
	rename->setIcon(QIcon(RenameIcon));
	rename->setToolTip(tr("batch rename file"));
	ui.mainToolBar->addWidget(rename);

	

	//编码里面只能有一个当前被选中
	m_pEncodeActGroup = new QActionGroup(this);
	m_pEncodeActGroup->addAction(ui.actionencode_in_GBK);
	m_pEncodeActGroup->addAction(ui.actionencode_in_uft8);
	m_pEncodeActGroup->addAction(ui.actionencode_in_UTF8_BOM);
	m_pEncodeActGroup->addAction(ui.actionencode_in_UCS_BE_BOM);
	m_pEncodeActGroup->addAction(ui.actionencode_in_UCS_2_LE_BOM);
	m_pEncodeActGroup->setExclusive(true);

	m_pLineEndActGroup = new QActionGroup(this);
	m_pLineEndActGroup->addAction(ui.actionconver_windows_CR_LF);
	m_pLineEndActGroup->addAction(ui.actionconvert_Unix_LF);
	m_pLineEndActGroup->addAction(ui.actionconvert_Mac_CR);
	m_pEncodeActGroup->setExclusive(true);

	m_pLexerActGroup = new QActionGroup(this);
	m_pLexerActGroup->addAction(ui.actionAVS);
	m_pLexerActGroup->addAction(ui.actionbash);
	m_pLexerActGroup->addAction(ui.actionBatch);
	m_pLexerActGroup->addAction(ui.actionCMake);
	m_pLexerActGroup->addAction(ui.actionCoffeeScript);
	m_pLexerActGroup->addAction(ui.actionCPP);
	m_pLexerActGroup->addAction(ui.actionCShape);
	m_pLexerActGroup->addAction(ui.actionCss);
	m_pLexerActGroup->addAction(ui.actionD_2);
	m_pLexerActGroup->addAction(ui.actionDiff);
	m_pLexerActGroup->addAction(ui.actionEdifact);
	m_pLexerActGroup->addAction(ui.actionFortran);
	m_pLexerActGroup->addAction(ui.actionFortran77);
	m_pLexerActGroup->addAction(ui.actionHTML);
	m_pLexerActGroup->addAction(ui.actionIDL);
	m_pLexerActGroup->addAction(ui.actionJava);
	m_pLexerActGroup->addAction(ui.actionJavaScript);
	m_pLexerActGroup->addAction(ui.actionjson);
	m_pLexerActGroup->addAction(ui.actionLua);
	m_pLexerActGroup->addAction(ui.actionMakefile);
    m_pLexerActGroup->addAction(ui.actionMarkDown_2);
	m_pLexerActGroup->addAction(ui.actionMatlab);
	m_pLexerActGroup->addAction(ui.actionOctave);
	m_pLexerActGroup->addAction(ui.actionPascal);
	m_pLexerActGroup->addAction(ui.actionPerl);
	m_pLexerActGroup->addAction(ui.actionPo);
	m_pLexerActGroup->addAction(ui.actionPostScript);
	m_pLexerActGroup->addAction(ui.actionPov);
	m_pLexerActGroup->addAction(ui.actionProperties_file);
	m_pLexerActGroup->addAction(ui.actionPython);
	m_pLexerActGroup->addAction(ui.actionRuby);
	m_pLexerActGroup->addAction(ui.actionSpice);
	m_pLexerActGroup->addAction(ui.actionSql);
	m_pLexerActGroup->addAction(ui.actionTcl);
	m_pLexerActGroup->addAction(ui.actionTex);
	m_pLexerActGroup->addAction(ui.actionVerilog);
	m_pLexerActGroup->addAction(ui.actionVHDL);
	m_pLexerActGroup->addAction(ui.actionXML);
	m_pLexerActGroup->addAction(ui.actionYAML);
	m_pLexerActGroup->addAction(ui.actionPhp);
	m_pLexerActGroup->addAction(ui.actionC);
	m_pLexerActGroup->addAction(ui.actionObjective_C);
	m_pLexerActGroup->addAction(ui.actionRC);
	m_pLexerActGroup->addAction(ui.actionini);
	m_pLexerActGroup->addAction(ui.actionNfo);
	m_pLexerActGroup->addAction(ui.actionASP);
	m_pLexerActGroup->addAction(ui.actionVirsual_Basic);
	m_pLexerActGroup->addAction(ui.actionShell);
	m_pLexerActGroup->addAction(ui.actionActionScript);
	m_pLexerActGroup->addAction(ui.actionNSIS);
	m_pLexerActGroup->addAction(ui.actionLisp);
	m_pLexerActGroup->addAction(ui.actionScheme);
	m_pLexerActGroup->addAction(ui.actionAssembly);
	m_pLexerActGroup->addAction(ui.actionSmalltalk);
	m_pLexerActGroup->addAction(ui.actionAutoIt);
	m_pLexerActGroup->addAction(ui.actionPowerShell);
	m_pLexerActGroup->addAction(ui.actionJsp);
	m_pLexerActGroup->addAction(ui.actionBaanC);
	m_pLexerActGroup->addAction(ui.actionS_Record);
	m_pLexerActGroup->addAction(ui.actionTypeScript);
	m_pLexerActGroup->addAction(ui.actionVisual_Prolog);
	m_pLexerActGroup->addAction(ui.actionTxt2tags);
	m_pLexerActGroup->addAction(ui.actionRust);
	m_pLexerActGroup->addAction(ui.actionRegistry);
	m_pLexerActGroup->addAction(ui.actionREBOL);
	m_pLexerActGroup->addAction(ui.actionOScript);
	m_pLexerActGroup->addAction(ui.actionNncrontab);
	m_pLexerActGroup->addAction(ui.actionNim);
	m_pLexerActGroup->addAction(ui.actionMMIXAL);
	m_pLexerActGroup->addAction(ui.actionLaTex);
	m_pLexerActGroup->addAction(ui.actionForth);
	m_pLexerActGroup->addAction(ui.actionESCRIPT);
	m_pLexerActGroup->addAction(ui.actionErlang);
	m_pLexerActGroup->addAction(ui.actionCsound);
	m_pLexerActGroup->addAction(ui.actionFreeBasic);
	m_pLexerActGroup->addAction(ui.actionBlitzBasic);
	m_pLexerActGroup->addAction(ui.actionPureBasic);
	m_pLexerActGroup->addAction(ui.actionAviSynth);
	m_pLexerActGroup->addAction(ui.actionASN1);
	m_pLexerActGroup->addAction(ui.actionSwift);
	m_pLexerActGroup->addAction(ui.actionIntel_HEX); 
	m_pLexerActGroup->addAction(ui.actionGo);
	m_pLexerActGroup->addAction(ui.actionTxt);

	QActionGroup* skinStyleGroup = new QActionGroup(this);
	skinStyleGroup->addAction(ui.actionDefaultStyle);
	skinStyleGroup->addAction(ui.actionLightBlueStyle);
	skinStyleGroup->addAction(ui.actionThinBlue);
	skinStyleGroup->addAction(ui.actionYellow);
	skinStyleGroup->addAction(ui.actionRiceYellow);
	skinStyleGroup->addAction(ui.actionSilver);
	skinStyleGroup->addAction(ui.actionLavenderBlush);
	skinStyleGroup->addAction(ui.actionMistyRose);
	connect(skinStyleGroup, &QActionGroup::triggered, this, &CCNotePad::slot_skinStyleGroup, Qt::QueuedConnection);

	QActionGroup* langsGroup = new QActionGroup(this);
	langsGroup->addAction(ui.actionChinese);
	langsGroup->addAction(ui.actionEnglish);

	connect(ui.menuLanguage, &QMenu::triggered, this, &CCNotePad::slot_lexerActTrig);

	//这是在网上看到的一个方法，使用一个widget把位置占住，让后面的action跑到最后面 去
	QWidget* space = new QWidget();
	space->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui.mainToolBar->addWidget(space);


	QToolButton* closeX = new QToolButton(ui.mainToolBar);
	connect(closeX, &QAbstractButton::clicked, this, &CCNotePad::slot_actionClose);
	closeX->setFixedSize(ICON_SIZE, ICON_SIZE);
	closeX->setIcon(QIcon(RightCloseIcon));
	closeX->setToolTip(tr("Close"));
	ui.mainToolBar->addWidget(closeX);
}


void CCNotePad::slot_skinStyleGroup(QAction * /*action*/)
{
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		ScintillaEditView *pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr)
		{
			pEdit->adjuctSkinStyle();
			autoSetDocLexer(pEdit);
		}
	}
	//ui.statusBar->showMessage(tr("The window background that has been opened will take effect after it is reopened."),10000);
}

void CCNotePad::setTxtLexer(ScintillaEditView* pEdit)
{
	QsciLexerText* lexer = new QsciLexerText();
	lexer->setLexerTag("txt");
	pEdit->setLexer(lexer);
}

//点击语言lexer的槽函数
void CCNotePad::slot_lexerActTrig(QAction *action)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
	if (pEdit != nullptr)
	{
		QVariant data = action->data();
		if (data.isNull())
		{
			QsciLexer * curLexer = pEdit->lexer();
			if (curLexer != nullptr)
			{
				pEdit->setLexer(nullptr);
				delete curLexer;
				setTxtLexer(pEdit);
			}
			return;
		}

		int lexerId = data.toInt();

		QsciLexer * curLexer = pEdit->lexer();
		if (curLexer != nullptr)
		{
			QString tag = curLexer->lexerTag();

			if (m_lexerNameToIndex.contains(tag))
			{
				//当前已经相等了，则不需要重新设置lexer
				if (m_lexerNameToIndex.value(tag).index == lexerId)
				{
					return;
				}
			}
			else
			{
				//tag必须在其中
				assert(false);
			}

			delete curLexer;
		}

		QsciLexer * lexer = ScintillaEditView::createLexer(lexerId);
		if (lexer != nullptr)
		{
			pEdit->setLexer(lexer);
		}
		else
		{
			//默认按txt处理
			setTxtLexer(pEdit);
	}
}
}

//保存最近对比到数据库。文件只有在关闭时，才写入最近列表。不关闭的下次自动恢复打开
void CCNotePad::saveReceneOpenFile()
{
	QString rFile("recentopenfile");

	const int maxRecord = 15;

	if (JsonDeploy::isDbExist())
	{
		QStringList fileText;

		int count = 0;

		for (QList<QString>::iterator it = m_receneOpenFileList.begin(); it != m_receneOpenFileList.end(); ++it)
		{
			fileText.append(*it);

			if (count++ >= maxRecord)
			{
				break;
			}
		}


		if (!fileText.isEmpty())
		{
			QString fileSaveText = fileText.join('|');
			JsonDeploy::updataKeyValueFromLongSets(rFile, fileSaveText);
		}
	}
}

//从数据库读取最近对比的文件列表
void CCNotePad::initReceneOpenFileMenu()
{
	QString rFile("recentopenfile");

	if (JsonDeploy::isDbExist())
	{
		QString fileStr = JsonDeploy::getKeyValueFromLongSets(rFile);

		QStringList fileList = fileStr.split('|');

		for (QString var : fileList)
		{
			if (!var.isEmpty() && (!m_receneOpenFile.contains(var)))
			{
				QAction* act = ui.menuFile->addAction(var, this, &CCNotePad::slot_openReceneFile);
				act->setData(QVariant(var));
				m_receneOpenFile.insert(var, act);
				m_receneOpenFileList.append(var);
			}
		}
	}
}

//判断文件是否已经打开中,是则返回其tabindex，否则-1
int CCNotePad::findFileIsOpenAtPad(QString filePath)
{
	int ret = -1;
	getRegularFilePath(filePath);

	for (int i = 0; i < ui.editTabWidget->count(); ++i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		if (pw != nullptr)
		{
			QString curPath = pw->property(Edit_View_FilePath).toString();
			getRegularFilePath(curPath);

			if (curPath == filePath)
			{
				ret = i;
				break;
			}
		}
	}

	return ret;
}

//通过菜单打开最近文档
void CCNotePad::slot_openReceneFile()
{
	QAction* pA = dynamic_cast<QAction*>(sender());
	if (pA != nullptr)
	{
		//失败则删除对应的记录
		if (!openFile(pA->text()))
		{
			pA->deleteLater();
		}
	}
}

void  CCNotePad::setCodeBarLabel(CODE_ID code)
{
	QString codeStr = Encode::getCodeNameById(code);

	if (codeStr == "unknown")
	{
		codeStr = "UTF8";
	}
	else if (codeStr == "GBK")
	{
		codeStr = tr("GB18030(Simplified Chinese)");
	}
	m_codeStatusLabel->setText(codeStr);
}

void  CCNotePad::setLineEndBarLabel(RC_LINE_FORM lineEnd)
{
	QString endStr = Encode::getLineEndById(lineEnd);
	m_lineEndLabel->setText(endStr);
}

void CCNotePad::setLangsDescLable(QString &langDesc)
{
	m_langDescLabel->setText(tr("Language: %1").arg(langDesc));
}

void CCNotePad::reloadEditFile(ScintillaEditView* pEidt)
{
	QString filePath = pEidt->property(Edit_View_FilePath).toString();

	//设置为非脏，直接关闭，关闭后再打开
	pEidt->setProperty(Edit_Text_Change, QVariant(false));
	slot_tabClose(ui.editTabWidget->currentIndex());
	openFile(filePath);
}

bool CCNotePad::checkRoladFile(ScintillaEditView* pEdit)
{
	if (pEdit != nullptr && pEdit->property(Modify_Outside).toBool())
	{
		QString filePath = pEdit->property(Edit_View_FilePath).toString();

		QApplication::beep();

		if (QMessageBox::Yes == QMessageBox::question(this, tr("Reload"), tr("\"%1\" This file has been modified by another program. Do you want to reload it?").arg(filePath)))
		{
			reloadEditFile(pEdit);
		}
		else
		{
			pEdit->setProperty(Modify_Outside, QVariant(false));
		}
		return true;
	}

	return false;
}

//这个函数是在paint中调用，所以不要直连，否则调用QMessagebox后崩溃
void  CCNotePad::slot_LineNumIndexChange(int line, int index)
{
	QString lineNums = tr("Ln: %1	Col: %2").arg(line).arg(index);
	m_lineNumLabel->setText(lineNums);

	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(sender());
	checkRoladFile(pEdit);
}

//打开监控文件修改的信号
void CCNotePad::enableEditTextChangeSign(ScintillaEditView* pEdit)
{
	connect(pEdit, &ScintillaEditView::textChanged,this, &CCNotePad::slot_editViewMofidyChange);
}

//关闭监控文件修改的信号。这样是为了高效，一旦文字修改后，后续不需要在监控该信号。
//直到保存后，再放开
void CCNotePad::disEnableEditTextChangeSign(ScintillaEditView* pEdit)
{
	pEdit->disconnect(SIGNAL(textChanged()));
}

//编辑框文本变化后，设置对应的变化状态
void CCNotePad::slot_editViewMofidyChange()
{
	ScintillaEditView* pEditView = dynamic_cast<ScintillaEditView*>(sender());
	if (pEditView != nullptr)
	{
		//如果是未设置脏状态，则设置脏为true
		QVariant v = pEditView->property(Edit_Text_Change);

		bool isDirty = v.toBool();

		if (!isDirty)
		{
			v.setValue(true);

			pEditView->setProperty(Edit_Text_Change,v);
		}

		//一旦变化后，设置tab为红色
		int index = ui.editTabWidget->indexOf(pEditView);
		if (index != -1)
		{
			ui.editTabWidget->setTabIcon(index, QIcon(TabNeedSave));
		}

		//设置状态栏也是红色
		/*m_saveFile->setIcon(QIcon(NeedSaveBarIcon));
		m_saveAllFile->setIcon(QIcon(NeedSaveAllBarIcon));*/
		m_saveFile->setEnabled(true);
		m_saveAllFile->setEnabled(true);

		//断开监控。只有保存后再打开
		disEnableEditTextChangeSign(pEditView);
	}
}

//更新当前文件的保存状态
void CCNotePad::updateCurTabSaveStatus()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr)
	{
		if (pw->property(Edit_Text_Change).toBool())
		{
			//m_saveFile->setIcon(QIcon(NeedSaveBarIcon));
			m_saveFile->setEnabled(true);
		}
		else
		{
			//m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon));
			m_saveFile->setEnabled(false);
		}
	}
}

//只设置保存按钮的状态
void CCNotePad::setSaveButtonStatus(bool needSave)
{
	m_saveFile->setEnabled(needSave);
	//if (needSave)
	//{
	//	//m_saveFile->setIcon(QIcon(NeedSaveBarIcon));
	//}
	//else
	//{
	//	//m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon));
	//}
	}

void CCNotePad::setSaveAllButtonStatus(bool needSave)
{
	m_saveAllFile->setEnabled(needSave);

	/*if (needSave)
	{
		m_saveAllFile->setIcon(QIcon(NeedSaveAllBarIcon));
	}
	else
	{
		m_saveAllFile->setIcon(QIcon(NoNeedSaveAllBarIcon));
	}*/
	}

//点击tab上的关闭事件执行槽函数。注意这个index是其在tab中的序号。
//当中间有删除时，是会动态变化的。所以不能以这个id为一起的固定索引
void CCNotePad::slot_tabClose(int index)
{
	QWidget* pw = ui.editTabWidget->widget(index);
	QString filePath = pw->property(Edit_View_FilePath).toString();

	//16进制的处理逻辑
	int type = getDocTypeProperty(pw);

	if (HEX_TYPE == type)
	{
		ui.editTabWidget->removeTab(index);
		pw->deleteLater();

		FileManager::getInstance().closeHexFileHand(filePath);
		//关闭文件后，不要随意打开一个新的页面
		initTabNewOne();

		return;
	}
	else if (BIG_TEXT_TYPE == type)
	{
		ui.editTabWidget->removeTab(index);
		pw->deleteLater();

		FileManager::getInstance().closeBigTextFileHand(filePath);
		//关闭文件后，不要随意打开一个新的页面
		initTabNewOne();

		return;
	}

	//关闭之前，检查是否要保存。如果文档为脏，则询问是否要保存
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
	if ((pEdit != nullptr)&&(pEdit->property(Edit_Text_Change).toBool()))
	{
		QApplication::beep();
		int ret = QMessageBox::question(this, tr("Save File?"), tr("if save file %1 ?").arg(filePath), tr("Yes"), tr("No"), tr("Cancel"));

		//保存
		if (ret == 0)
		{
			slot_actionSaveFile_toggle(true);

			//如果还是取消，即没有保存，则不能关闭
			if (pEdit->property(Edit_Text_Change).toBool())
			{
				return;
		}
		}
		else if (ret == 2)
		{
			m_isQuitCancel = true;
			return;
		}
	}


	ui.editTabWidget->removeTab(index);

	QVariant v = pw->property(Edit_File_New);

	int newFileIndex = v.toInt();

	if (newFileIndex >= 0)
	{
		//如果是新建文件，则删除对应的记录
		FileManager::getInstance().delNewFileNode(newFileIndex);
	}

	//如果关闭的是没有保存的新建文件，则不需要保存在最近打开列表中。新建文件需要保存
	if (-1 == pEdit->property(Edit_File_New).toInt())
	{
		QAction* topRecentFile = nullptr;

		//如果关闭的文件，已经在最近列表中，则移动到最前面即可
		if (m_receneOpenFile.contains(filePath))
		{
			topRecentFile = m_receneOpenFile.take(filePath);

			ui.menuFile->removeAction(topRecentFile);
			m_receneOpenFileList.removeOne(filePath);
		}
		else
		{
			topRecentFile = new QAction(filePath, ui.menuFile);
			connect(topRecentFile, &QAction::triggered, this, &CCNotePad::slot_openReceneFile);
		}


		//在菜单最近列表上面添加。如果最近列表是空的，则放在退出菜单之上
		if (m_receneOpenFileList.isEmpty())
		{
			ui.menuFile->insertAction(m_quitAction, topRecentFile);
		}
		else
		{
			//放在列表最上面
			QString curTopActionPath = m_receneOpenFileList.first();
			if (m_receneOpenFile.contains(curTopActionPath))
			{
				ui.menuFile->insertAction(m_receneOpenFile.value(curTopActionPath), topRecentFile);
			}
		}

		m_receneOpenFileList.push_front(filePath);
		m_receneOpenFile.insert(filePath,topRecentFile);

		//不能无限制变大，及时删除一部分
		if (m_receneOpenFileList.size() > 15)
		{
			QString k = m_receneOpenFileList.takeLast();
			if (m_receneOpenFile.contains(k))
			{
				delete m_receneOpenFile.value(k);
				m_receneOpenFile.remove(k);
			}
		
		}
	}

	//取消监控文件
	removeWatchFilePath(filePath);

	pw->deleteLater();

	//删除交换文件

	QString swapfile = getSwapFilePath(filePath);
	if (QFile::exists(swapfile))
	{
		QFile::remove(swapfile);
	}

	//当前关闭后，更新下tab的是否全部需要关闭状态
	updateSaveAllToolBarStatus();

	updateCurTabSaveStatus();

	//关闭文件后，不要随意打开一个新的页面
	initTabNewOne();
}

//输入参数：名称和文件新建文件序号。一定是文本文件。contentPath：从这个路径加载文件内容，目前在恢复文件中使用。
ScintillaEditView* CCNotePad::newTxtFile(QString name, int index, QString contentPath)
{
	ScintillaEditView* pEdit = FileManager::getInstance().newEmptyDocument();
	pEdit->setNoteWidget(this);

	CODE_ID code(UTF8_NOBOM);

#ifdef _WIN32
	RC_LINE_FORM lineEnd(DOS_LINE);
#else
	RC_LINE_FORM lineEnd(UNIX_LINE);
#endif

	bool isChange = false;

	//如果非空，则从contentPath中加载文件内容。做恢复文件使用
	if (!contentPath.isEmpty())
	{
		int ret = FileManager::getInstance().loadFileDataInText(pEdit, contentPath, code, lineEnd, nullptr, false);
		if (ret != 0)
		{
			ui.statusBar->showMessage(tr("Restore Last Temp File %1 Failed").arg(contentPath));
		}
		isChange = true;
	}

	connect(pEdit, &ScintillaEditView::cursorPositionChanged, this, &CCNotePad::slot_LineNumIndexChange, Qt::QueuedConnection);
	connect(pEdit, &ScintillaEditView::copyAvailable, this, &CCNotePad::slot_copyAvailable);
	connect(pEdit, SIGNAL(SCN_ZOOM()), pEdit, SLOT(updateLineNumberWidth()));

	enableEditTextChangeSign(pEdit);

	//int index = FileManager::getInstance().getNextNewFileId();

	//QString label = QString("New %1").arg(index);

	QString label = name;

	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon(TabNoNeedSave), label);

	QVariant editViewFilePath(label);
	pEdit->setProperty(Edit_View_FilePath, editViewFilePath);
	ui.editTabWidget->setTabToolTip(curIndex, label);

	setFileOpenAttrProperty(pEdit, OpenAttr::Text);
	setWindowTitleMode(label, OpenAttr::Text);

	QVariant editViewNewFile(index);
	pEdit->setProperty(Edit_File_New, editViewNewFile);

	QVariant editTextChange(isChange);
	pEdit->setProperty(Edit_Text_Change, editTextChange);

	QVariant editTextCode((int)code);
	pEdit->setProperty(Edit_Text_Code, editTextCode);
	syncCurDocEncodeToMenu(pEdit);

	setDocTypeProperty(pEdit, TXT_TYPE);

#ifdef _WIN32
	QVariant editTextEnd((int)lineEnd);
	pEdit->setProperty(Edit_Text_End, editTextEnd);
	setLineEndBarLabel(DOS_LINE);

#else
	QVariant editTextEnd((int)lineEnd);
	pEdit->setProperty(Edit_Text_End, editTextEnd);
	setLineEndBarLabel(UNIX_LINE);
#endif
	syncCurDocLineEndStatusToMenu(pEdit);

	NewFileIdMgr fileId(index, pEdit);

	FileManager::getInstance().insertNewFileNode(fileId);

	ui.editTabWidget->setCurrentIndex(curIndex);

	autoSetDocLexer(pEdit);

	return pEdit;
}

void CCNotePad::slot_actionNewFile_toggle(bool /*checked*/)
{
	int index = FileManager::getInstance().getNextNewFileId();
	QString name = QString("New %1").arg(index);

	newTxtFile(name,index);
}

//适当做剪裁
QString CCNotePad::getShortName(const QString& name)
{
	if (name.size() > 20)
	{
		return QString("%1...").arg(name.left(16));
	}
	return name;
}

//重新加载文件以指定的编码方式
bool CCNotePad::reloadTextFileWithCode(CODE_ID code)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	//16进制的处理逻辑
	if (HEX_TYPE == getDocTypeProperty(pw))
	{
		ui.statusBar->showMessage(tr("Only Text File Can Use it, Current Doc is a Hex File !"), 10000);
		QApplication::beep();
		return false;
	}

	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	//新建文件不需要重新打开文件，只修改编码显示
	if (-1 == getFileNewIndexProperty(pw))
	{
		QString filePath = pw->property(Edit_View_FilePath).toString();

		RC_LINE_FORM lineEnd;

		pEdit->clear();

		int errCode = FileManager::getInstance().loadFileDataInText(pEdit, filePath, code, lineEnd,this, false);
		if (errCode == 5)
		{
			//只读模式。暂时什么也不做
		}
		else if(errCode != 0)
		{
			delete pEdit;
			return false;
		}

		if (pEdit->lexer() == nullptr)
		{
			autoSetDocLexer(pEdit);
	}
	}

	setCodeBarLabel(code);

	QVariant editTextCode((int)code);
	pEdit->setProperty(Edit_Text_Code, editTextCode);

	return true;
}

const int MAX_TEXT_FILE_SIZE = 100 * 1024 * 1024;

//按照大文本文件进行只读打开
bool CCNotePad::openBigTextFile(QString filePath)
{
	QFileInfo fi(filePath);
	QString fileLabel(fi.fileName());

	TextFileMgr* txtFile = nullptr;

	if (!FileManager::getInstance().loadFileData(filePath, txtFile))
	{
		return false;
	}

	ScintillaEditView* pEdit = FileManager::getInstance().newEmptyDocument();
	pEdit->setReadOnly(true);
	pEdit->setNoteWidget(this);

	//必须要在editTabWidget->addTab之前，因为一旦add时会出发tabchange，其中没有doctype会导致错误
	pEdit->execute(SCI_SETSCROLLWIDTH, 80 * 10);
	setDocTypeProperty(pEdit, BIG_TEXT_TYPE);

	showBigTextFile(pEdit, txtFile);

	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon(TabNoNeedSave), getShortName(fileLabel));

	ui.editTabWidget->setCurrentIndex(curIndex);

	QVariant editViewFilePath(filePath);
	pEdit->setProperty(Edit_View_FilePath, editViewFilePath);

	//setWindowTitle(QString("%1 (%2)").arg(filePath).arg(tr("Big Text File ReadOnly")));
	
	ui.editTabWidget->setTabToolTip(curIndex, filePath);

	QVariant editViewNewFile(-1);
	pEdit->setProperty(Edit_File_New, editViewNewFile);

	QVariant editTextChange(false);
	pEdit->setProperty(Edit_Text_Change, editTextChange);

	syncCurDocEncodeToMenu(pEdit);
	setFileOpenAttrProperty(pEdit, OpenAttr::BigTextReadOnly);
	setWindowTitleMode(filePath, OpenAttr::BigTextReadOnly);

	return true;
}

void CCNotePad::setWindowTitleMode(QString filePath, OpenAttr attr)
{
	QString title = QString("%1 [%2]").arg(filePath).arg(OpenAttrToString(attr));
	setWindowTitle(title);
}

//打开普通文本文件。
bool CCNotePad::openTextFile(QString filePath, bool isCheckHex, CODE_ID code)
{
	getRegularFilePath(filePath);

	//先检测交换文件是否存在，如果存在，说明上次崩溃了，提示用户恢复
	QString swapfile = getSwapFilePath(filePath);

	bool isNeedRestoreFile = false;

	QFileInfo fi(filePath);

	//如果文件大于300M,按照只读文件打开
	if (ScintillaEditView::s_bigTextSize <= 0 || ScintillaEditView::s_bigTextSize > 300)
	{
		ScintillaEditView::s_bigTextSize = 100;
	}

	if (fi.size() > ScintillaEditView::s_bigTextSize*1024*1024)
	{
		return openBigTextFile(filePath);
	}

	if (QFile::exists(swapfile))
	{
		QFileInfo spfi(swapfile);

		//如果存在交换文件，而且修改时间更晚，询问用户是否需要恢复
		if ((spfi.size() > 0) /*&& (spfi.lastModified() >= fi.lastModified())*/)
		{
			int ret = QMessageBox::question(this, tr("Recover File?"), tr("File %1 abnormally closed last time , Restore it ?").arg(filePath), tr("Restore"), tr("No"));
			//使用历史存档恢复文件
			if (ret == 0)
			{
				isNeedRestoreFile = true;
			}
		}
	}

	QString fileLabel(fi.fileName());

	ScintillaEditView* pEdit = FileManager::getInstance().newEmptyDocument();
	pEdit->setNoteWidget(this);

	//必须要在editTabWidget->addTab之前，因为一旦add时会出发tabchange，其中没有doctype会导致错误
	setDocTypeProperty(pEdit, TXT_TYPE);

	RC_LINE_FORM lineEnd;

	bool isReadOnly = false;

	//如果需要恢复，则加载交换文件的内容。
	if (!isNeedRestoreFile)
	{
		int ret = FileManager::getInstance().loadFileDataInText(pEdit, filePath, code, lineEnd, this, isCheckHex);
		if (4 == ret)
	{
		delete pEdit;
			//用户同意以二进制格式打开文件
			return openHexFile(filePath);
		}
		else if (5 == ret)
		{
			isReadOnly = true;
			//只读模式
		}
		else if (0 != ret)
		{
			delete pEdit;
		return false;
	}
	}
	else
	{
		if (0 != FileManager::getInstance().loadFileDataInText(pEdit, swapfile, code, lineEnd,this))
		{
			ui.statusBar->showMessage(tr("File %1 Open Failed").arg(swapfile));
			delete pEdit;
			return false;
		}
		//恢复时直接使用新内容保存到原始文件中
		saveFile(filePath, pEdit, false);
	}
	
	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon(TabNoNeedSave), getShortName(fileLabel));
	ui.editTabWidget->setCurrentIndex(curIndex);

	connect(pEdit, &ScintillaEditView::cursorPositionChanged, this, &CCNotePad::slot_LineNumIndexChange, Qt::QueuedConnection);
	enableEditTextChangeSign(pEdit);
	connect(pEdit, &ScintillaEditView::copyAvailable, this, &CCNotePad::slot_copyAvailable);
	connect(pEdit, SIGNAL(SCN_ZOOM()), pEdit, SLOT(updateLineNumberWidth()));

	//监控文件
	addWatchFilePath(filePath);

	setCodeBarLabel(code);
	setLineEndBarLabel(lineEnd);

	//注意顺序
	QVariant editTextEnd((int)lineEnd);
	pEdit->setProperty(Edit_Text_End, editTextEnd);
	setDocEolMode(pEdit, lineEnd);

	QVariant editViewFilePath(filePath);
	pEdit->setProperty(Edit_View_FilePath, editViewFilePath);

	//setWindowTitle(filePath);

	ui.editTabWidget->setTabToolTip(curIndex, filePath);

	QVariant editViewNewFile(-1);
	pEdit->setProperty(Edit_File_New, editViewNewFile);

	setTextChangeProperty(pEdit, false);

	QVariant editTextCode((int)code);
	pEdit->setProperty(Edit_Text_Code, editTextCode);

	syncCurDocEncodeToMenu(pEdit);
	syncCurDocLineEndStatusToMenu(pEdit);
	syncCurDocLexerToMenu(pEdit);

	//设置自动转换和缩进参考线
	if (s_autoWarp == 1)
	{
		pEdit->setWrapMode(QsciScintilla::WrapWord);
	}

	if (s_indent == 1)
	{
		pEdit->setIndentGuide(true);
	}

	if (!isReadOnly)
	{
		setFileOpenAttrProperty(pEdit, OpenAttr::Text);
		setWindowTitleMode(filePath, OpenAttr::Text);
		ui.statusBar->showMessage(tr("File %1 Open Finished [Text Mode]").arg(filePath),8000);
	}
	else
	{
		setFileOpenAttrProperty(pEdit, OpenAttr::TextReadOnly);
		setWindowTitleMode(filePath, OpenAttr::TextReadOnly);
		ui.statusBar->showMessage(tr("File %1 Open Finished [Text ReadOnly Mode] (Note: display up to 50K bytes ...)").arg(fi.fileName()),8000);
	}

	if (pEdit->lexer() == nullptr)
	{
		autoSetDocLexer(pEdit);
	}
	

	return true;
}

//显示二进制文件
bool CCNotePad::showHexFile(ScintillaHexEditView* pEdit, HexFileMgr* hexFile)
{
	//二进制.预留4倍空间，双字节+空格+字符显示就是4倍，还要前面的地址12+空格+换行2=15。预留16个
	//最后一行不慢16个字符的，也要把文字显示在对齐的16个字符的尾巴上。后面加的那个16*4就是多预留1行的空间
	//最后一行不足16，字符显示对齐到尾巴的16的位置上
	const int BUF_SIZE = hexFile->contentRealSize * 4 + 16 * (hexFile->contentRealSize / 16 + 1) + 16*4;

	char* textOut = new char[BUF_SIZE];
	memset(textOut, 0, BUF_SIZE);

	int lineMax = 0;

	int offset = 0;

	uchar* pchar = (uchar*)hexFile->contentBuf;

	qint64 addr = hexFile->fileOffset - hexFile->contentRealSize;

	ui.statusBar->showMessage(tr("Current offset is %1 , load Contens Size is %2, File Total Size is %3").arg(addr).arg(hexFile->contentRealSize).arg(hexFile->fileSize));

	char* lineString = new char[17];
	memset(lineString,0,17);

	for (int pos = 0; pos < hexFile->contentRealSize; ++pos)
	{
		if (lineMax == 0)
		{
			if (addr < 0xffffffff)
			{
				sprintf(textOut + offset, "%08llX ", addr);
				offset += 9;
			}
			else
			{
				sprintf(textOut + offset, "%012llX ", addr);
				offset += 13;
			}
			
		}

		sprintf(textOut + offset, "%02X ", *(pchar+pos));

		//如果在可显示字符内
		if (*(pchar + pos) >= 32 && *(pchar + pos) <= 126)
		{
			lineString[lineMax] = *(pchar + pos);
		}
		else
		{
			lineString[lineMax] = '.';
		}

		offset += 3;
		lineMax++;

		if (lineMax == 16)
		{
			memcpy(textOut + offset, lineString,16);
			offset += 16;
			sprintf(textOut + offset, "\r\n");
			offset += 2;
			lineMax = 0;
			addr += 16;
			memset(lineString, 0, 17);
		}
	}

	if (lineMax > 0)
	{
		//对于尾巴不慢16字符的，对齐一下，让文本总是显示在最后的16个空间上
		for (int i = 0; i < (16 - lineMax); ++i)
		{
			sprintf(textOut + offset, "-- ");
			offset += 3;
		}

		memcpy(textOut + offset, lineString, lineMax);
		offset += lineMax;

		sprintf(textOut + offset, "\r\n");
	}

	QString text(textOut);
	pEdit->setUtf8(false);
	pEdit->setText(text);

	delete[]lineString;
	delete[]textOut;
	
	

	return true;
}

//显示大文本文件只读
bool CCNotePad::showBigTextFile(ScintillaEditView* pEdit, TextFileMgr* txtFile)
{
	qint64 addr = txtFile->fileOffset - txtFile->contentRealSize;

	ui.statusBar->showMessage(tr("Current offset is %1 , load Contens Size is %2, File Total Size is %3").arg(addr).arg(txtFile->contentRealSize).arg(txtFile->fileSize));

	pEdit->setUtf8Text(txtFile->contentBuf, txtFile->contentRealSize);

	return true;
}


//打开并显示二进制文件
bool CCNotePad::openHexFile(QString filePath)
{
	getRegularFilePath(filePath);

	QFileInfo fi(filePath);
	QString fileLabel(fi.fileName());

	HexFileMgr* hexFile = nullptr;

	if (!FileManager::getInstance().loadFileData(filePath, hexFile))
	{
		return false;
	}

	ScintillaHexEditView* pEdit = FileManager::getInstance().newEmptyHexDocument();
	pEdit->setReadOnly(true);
	pEdit->setNoteWidget(this);
	pEdit->execute(SCI_SETSCROLLWIDTH, 80 * 10);
	

	setDocTypeProperty(pEdit, HEX_TYPE);

	showHexFile(pEdit,hexFile);

	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon(TabNoNeedSave), getShortName(fileLabel));
	
	ui.editTabWidget->setCurrentIndex(curIndex);

	QVariant editViewFilePath(filePath);
	pEdit->setProperty(Edit_View_FilePath, editViewFilePath);

	//setWindowTitle(filePath);
	ui.editTabWidget->setTabToolTip(curIndex, filePath);

	QVariant editViewNewFile(-1);
	pEdit->setProperty(Edit_File_New, editViewNewFile);

	QVariant editTextChange(false);
	pEdit->setProperty(Edit_Text_Change, editTextChange);

	syncCurDocEncodeToMenu(pEdit);

	setFileOpenAttrProperty(pEdit, OpenAttr::HexReadOnly);
	setWindowTitleMode(filePath, OpenAttr::HexReadOnly);

	ui.statusBar->showMessage(tr("File %1 Open Finished [Hex ReayOnly Mode]").arg(filePath),8000);

	return true;
}

static QString fileSuffix(const QString& filePath)
{
	QFileInfo fi(filePath);
	return fi.suffix();
}

bool CCNotePad::openFile(QString filePath)
{
	s_padTimes++;

	getRegularFilePath(filePath);

	QFileInfo fi(filePath);
	QString fileLabel(fi.fileName());

	if (!fi.exists())
	{
		QApplication::beep();
		QMessageBox::warning(this, tr("Error"), tr("file %1 not exist.").arg(filePath));
		return false;
	}

	s_lastOpenDirPath = fi.absoluteDir().absolutePath();

	//如果已经打开过，则直接返回到当前文档
	int retIndex = findFileIsOpenAtPad(filePath);

	if (-1 != retIndex)
	{
		ui.editTabWidget->setCurrentIndex(retIndex);
		ui.statusBar->showMessage(tr("file %1 already open at tab %2").arg(filePath).arg(retIndex),5000);
		return true;
	}

	//如果是已知的二进制文件，以二进制打开
	if (DocTypeListView::isHexExt(fileSuffix(filePath)))
	{
		return openHexFile(filePath);
	}

	//非已知的后缀文件，暂时无条件以二进制打开
	return openTextFile(filePath);

#if 0
	//如果不支持该文件ext，则以二进制形式打开
	if (DocTypeListView::isSupportExt(CompareDirs::fileSuffix(filePath)))
	{
		return openTextFile(filePath);
	}
	else
	{
		static QMimeDatabase db;
		QMimeType mime = db.mimeTypeForFile(fi);
		if (mime.name().startsWith("text")) {
			return openTextFile(filePath);
		}

		ui.statusBar->showMessage(tr("file %1 may be a hex file , try open with text file.").arg(filePath), 10000);

		return openTextFile(filePath);
	}
#endif
}

void CCNotePad::slot_slectionChanged()
{
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(sender());
	if (pEdit != nullptr)
	{
		if (pEdit->hasSelectedText())
		{
			QString selectText = pEdit->selectedText();
		}
	}
}

void CCNotePad::slot_actionOpenFile_toggle(bool /*checked*/)
{
	QFileDialog fd(this,QString(), s_lastOpenDirPath);
	fd.setFileMode(QFileDialog::ExistingFile);
	
	if (fd.exec() == QDialog::Accepted)   //如果成功的执行
	{
		QStringList fileNameList = fd.selectedFiles();      //返回文件列表的名称
		QFileInfo fi(fileNameList[0]);

		openFile(fi.filePath());

	}
	else
	{
		fd.close();
	}
}

//bool isBakWrite:是否进行保护写，即先写swap文件，再写源文件。这样可以避免突然断电导致源文件被清空
//isBakWrite 是否写保护swp文件，默认true。只有新文件时不需要，因为新文件不存在覆盖写的问题
//isStatic 是否静默：不弹出对话框，在外部批量查找替换文件夹时使用，避免弹窗中断。默认false
//isClearSwpFile:是否回收swp交换文件，在外部批量查找替换文件夹时使用，替换后直接删除swp文件。默认false
bool  CCNotePad::saveFile(QString fileName, ScintillaEditView* pEdit, bool isBakWrite, bool isStatic,bool isClearSwpFile)
{
	QFile srcfile(fileName);
	
	//如果文件存在，说明是旧文件，检测是否能写，不能写则失败。
	//反之文件不存，是保存为新文件
	bool isNewFile = false;

	if (srcfile.exists())
	{
		QFlags<QFileDevice::Permission> power = QFile::permissions(fileName);

		if (!power.testFlag(QFile::WriteOwner))
		{
			//文件不能写
			QApplication::beep();
			if (!isStatic)
			{
			QMessageBox::warning(this, tr("Error"), tr("Save File %1 failed. Can not write auth, Please save as new file").arg(fileName));
			}
			ui.statusBar->showMessage(tr("Save File %1 failed. Can not write auth, Please save as new file").arg(fileName));
			return false;
		}
	}
	else
	{
		isNewFile = true;
	}

	auto saveWork = [this, &pEdit,isStatic](QFile& file, QString &fileName)->bool{

	if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
	{
		QApplication::beep();
		if (!isStatic)
		{
#ifdef Q_OS_WIN
			//打开失败，这里一般是权限问题导致。如果是windows，在外面申请权限后继续处理
			if (QFileDevice::OpenError == file.error())
			{
		
				this->runAsAdmin(fileName);
				
				return false;
		}
#endif
			QMessageBox::warning(this, tr("Error"), tr("Save File %1 failed. You may not have write privileges \nPlease save as a new file!").arg(fileName));
		}
		return false;
	}

	QString textOut = pEdit->text();

	CODE_ID dstCode = static_cast<CODE_ID>(pEdit->property(Edit_Text_Code).toInt());

	//如果编码是已知如下类型，则后续保存其它行时，不修改编码格式，继续按照原编码进行保存
	if (dstCode == CODE_ID::UNICODE_BE)
	{
		QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-16BE"));
	}
	else if (dstCode == CODE_ID::UNICODE_LE)
	{
		QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-16LE"));
	}
	else if (dstCode == CODE_ID::UTF8_BOM)
	{
		QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

		//自动转换不会带UTF-8 BOM，所以自己要在前面写个BOM头。这是一个例外。需要手动写入头
		//其他必然BL LE则不需要。
		QByteArray codeFlag = Encode::getEncodeStartFlagByte(dstCode);

		if (!codeFlag.isEmpty())
		{
			//先写入标识头
			file.write(codeFlag);
		}
	}
	else if (dstCode == CODE_ID::GBK)
	{
		QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	}
	else
	{
		//对于其它非识别编码，统一转换为utf8。减去让用户选择的麻烦
		QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	}


	if (textOut.length() > 0)
	{
		//保存时注意编码问题。这个tolocal已经带了字符BOM头了。只有UTF8_BOM不会带，其他BE LE都会带
		QByteArray t = textOut.toLocal8Bit();
		file.write(textOut.toLocal8Bit());
	}
	file.close();
	return true;
	};

	//如果是新文件，第一次保存，直接保存
	//或者非保护写，直接写源文件即可
	if (isNewFile || !isBakWrite)
	{
		saveWork(srcfile, fileName);
	}
	else
	{
		QString swapFile = getSwapFilePath(fileName);
		QFile swapfile(swapFile);
		//老文件则先写入交换文件，避免断电后破坏文件不能恢复
		//再写入原本文件
		bool success = saveWork(swapfile, fileName);
		if (success)
		{
			success = saveWork(srcfile, fileName);
		if (!success)
		{
			return false;
		}
		}
		if (success && isClearSwpFile)
		{
			QFile::remove(swapFile);
	}
		
	}
	return true;
}

//bool isBakWrite:是否进行保护写，即先写swap文件，再写源文件。这样可以避免突然断电导致源文件被清空
//外部替换后保存文件时调用的函数，主要不弹出messagebox
void  CCNotePad::slot_saveFile(QString fileName, ScintillaEditView* pEdit)
{
	//写保护文件、静默安装、删除保护文件
	saveFile(fileName, pEdit,true,true,true);
}

//保存一个新建的文件后，更新相关配置
void CCNotePad::updateProAfterSaveNewFile(int curTabIndex, QString fileName, ScintillaEditView* pEdit)
{
	getRegularFilePath(fileName);
	//保存成功后，更新文件的路径和tab标签
	QFileInfo fi(fileName);
	QString fileLabel(fi.fileName());

	ui.editTabWidget->setTabText(curTabIndex, fileLabel);

	//删除新文件的索引
	QVariant v = pEdit->property(Edit_File_New);

	int newFileIndex = v.toInt();

	if (newFileIndex >= 0)
	{
		//如果是新建文件，则删除对于的记录
		FileManager::getInstance().delNewFileNode(newFileIndex);
	}

	v.setValue(-1);
	pEdit->setProperty(Edit_File_New, v);

	//更新路径名称
	QVariant fp(fileName);
	pEdit->setProperty(Edit_View_FilePath, fp);

	//setWindowTitle(fileName);
	setFileOpenAttrProperty(pEdit, OpenAttr::Text);
	setWindowTitleMode(fileName, OpenAttr::Text);

	ui.editTabWidget->setTabToolTip(curTabIndex, fileName);

	addWatchFilePath(fileName);
}

//保存文件的执行
void CCNotePad::slot_actionSaveFile_toggle(bool /*checked*/)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	int index = ui.editTabWidget->currentIndex();

	//16进制的处理逻辑
	if (HEX_TYPE == getDocTypeProperty(pw))
	{
		ui.statusBar->showMessage(tr("Only Text File Can Use it, Current Doc is a Hex File !"), 10000);
		QApplication::beep();
		return;
	}

	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	//保存成功后，开启修改监控
		//如果是未设置脏状态，则设置脏为true
	QVariant v = pEdit->property(Edit_Text_Change);

	bool isDirty = v.toBool();

	//不脏则不需要保存，直接跳过
	if (!isDirty)
	{
		return;
	}

	if (pEdit != nullptr)
	{
		//如果是新建的文件，则弹出保存对话框，进行保存
		if (pEdit->property(Edit_File_New) >= 0)
		{
			QString filter("Text files (*.txt);;XML files (*.xml);;h files (*.h);;cpp file(*.cpp);;All types(*.*)");
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(), filter);
			if (!fileName.isEmpty())
			{
				//如果已经打开过，则直接返回到当前文档
				int retIndex = findFileIsOpenAtPad(fileName);
				if (-1 != retIndex)
				{
					QMessageBox::warning(this,tr("Error"), tr("file %1 already open at tab %2, please select other file name.").arg(fileName).arg(retIndex));
					return;
				}

				if (!saveFile(fileName, pEdit))
				{
					return;
				}
				updateProAfterSaveNewFile(index, fileName, pEdit);
			}
			else
			{
				//这里点击了取消，不进行保存
				return;
			}
		}
		else
		{
			//如果是打开的文件了，则保存
			QString fileName = pEdit->property(Edit_View_FilePath).toString();
			if (!fileName.isEmpty())
			{
				//保存前取消文件的修改检测，避免自己修改触发自己
				removeWatchFilePath(fileName);

				if (!saveFile(fileName, pEdit))
				{
					addWatchFilePath(fileName);
					return;
				}

				addWatchFilePath(fileName);
			}
		}

		//保存成功后，开启修改监控
		//如果是未设置脏状态，则设置脏为true
		if (isDirty)
		{
			v.setValue(false);

			pEdit->setProperty(Edit_Text_Change, v);
		}

		//一旦保存后，设置tab为不需要保存状态
		ui.editTabWidget->setTabIcon(index, QIcon(TabNoNeedSave));
		//m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon));
		m_saveFile->setEnabled(false);

		
		updateSaveAllToolBarStatus();

		//只有保存后再打开文本变化监控
		enableEditTextChangeSign(pEdit);
	}
}


//文件重命名
void CCNotePad::slot_actionRenameFile_toggle(bool checked)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	int index = ui.editTabWidget->currentIndex();

	//非新建文件走改名逻辑
	if (-1 == getFileNewIndexProperty(pw))
	{
		//取消旧的文件监控
		QString oldName = pw->property(Edit_View_FilePath).toString();
		getRegularFilePath(oldName);
		QFileInfo oldfi(oldName);

		//如果是打开的本来就存在的文件，也弹出保存进行
		QString filter("Text files (*.txt);;XML files (*.xml);;h files (*.h);;cpp file(*.cpp);;All types(*.*)");
		QString fileName = QFileDialog::getSaveFileName(this, tr("Rename File As ..."), oldfi.absoluteDir().absolutePath(), filter);
		
		if (!fileName.isEmpty())
		{
			getRegularFilePath(fileName);
			

			if (oldName == fileName)
			{
				return;
			} 

			
			QFileInfo newfi(fileName);

			if (oldfi.absoluteDir().absolutePath() != newfi.absoluteDir().absolutePath())
			{
				return;
			}

			if (QFile::rename(oldName, fileName))
			{

				removeWatchFilePath(oldName);
				//保存成功后，更新文件的路径和tab标签
				QFileInfo fi(fileName);
				QString fileLabel(fi.fileName());

				ui.editTabWidget->setTabText(index, fileLabel);

				//更新路径名称
				QVariant fp(fileName);
				pw->setProperty(Edit_View_FilePath, fp);
				ui.editTabWidget->setTabToolTip(index, fileName);
				addWatchFilePath(fileName);
			}
			else
			{
				QApplication::beep();
				QMessageBox::warning(this, tr("Error"), tr("file %1 reanme failed!").arg(fileName));
				return;
			}
		}
		else
		{
			//这里点击了取消，不进行操作
			return;
		}
	}
	else
	{
		//新建文件直接走另外保存逻辑
		slot_actionSaveAsFile_toggle(checked);
	}
}

//保存文件的另存为槽函数。1）先执行保存。2）用保存后的新文件路径，替换当前的路径
void CCNotePad::slot_actionSaveAsFile_toggle(bool /*checked*/)
{
	QWidget* pw = ui.editTabWidget->currentWidget();

	//16进制的处理逻辑
	if (HEX_TYPE == getDocTypeProperty(pw))
	{
		ui.statusBar->showMessage(tr("Only Text File Can Use it, Current Doc is a Hex File !"), 10000);
		QApplication::beep();
		return;
	}

	int index = ui.editTabWidget->currentIndex();

	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		//如果是新建的文件，则弹出保存对话框，进行保存
		if (pEdit->property(Edit_File_New) >= 0)
		{
			QString filter("Text files (*.txt);;XML files (*.xml);;h files (*.h);;cpp file(*.cpp);;All types(*.*)");
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As ..."),QString(), filter);
			
			if (!fileName.isEmpty())
			{
				getRegularFilePath(fileName);
				//如果已经打开过，则直接返回到当前文档
				int retIndex = findFileIsOpenAtPad(fileName);
				if (-1 != retIndex)
				{
					QMessageBox::warning(this, tr("Error"), tr("file %1 already open at tab %2, please select other file name.").arg(fileName).arg(retIndex));
					return;
				}

				if (!saveFile(fileName, pEdit))
				{
					return;
				}

				updateProAfterSaveNewFile(index, fileName, pEdit);

				addWatchFilePath(fileName);
			}
			else
			{
				//这里点击了取消，不进行保存
				return;
			}
		}
		else
		{
			//取消旧的文件监控
			removeWatchFilePath(pEdit->property(Edit_View_FilePath).toString());

			//如果是打开的本来就存在的文件，也弹出保存进行
			QString filter("Text files (*.txt);;XML files (*.xml);;h files (*.h);;cpp file(*.cpp);;All types(*.*)");
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As ..."), QString(), filter);

			if (!fileName.isEmpty())
			{
				if (!saveFile(fileName, pEdit))
				{
					return;
				}

				//保存成功后，更新文件的路径和tab标签
				QFileInfo fi(fileName);
				QString fileLabel(fi.fileName());

				ui.editTabWidget->setTabText(index, fileLabel);

				//更新路径名称
				QVariant fp(fileName);
				pEdit->setProperty(Edit_View_FilePath, fp);
				ui.editTabWidget->setTabToolTip(index, fileName);
				addWatchFilePath(fileName);

				setWindowTitleMode(fileName, OpenAttr::Text);
			}
			else
			{
				//这里点击了取消，不进行保存
				return;
			}
		}

		//保存成功后，开启修改监控
		//如果是未设置脏状态，则设置脏为true
		QVariant v = pEdit->property(Edit_Text_Change);

		bool isDirty = v.toBool();

		if (isDirty)
		{
			v.setValue(false);

			pEdit->setProperty(Edit_Text_Change, v);
		}

		//保持完毕后，设置tab为蓝色，显示为不需要保持状态

		ui.editTabWidget->setTabIcon(index, QIcon(TabNoNeedSave));
		//m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon));
		m_saveFile->setEnabled(false);

		updateSaveAllToolBarStatus();

		//只有保存后再打开文本变化监控
		enableEditTextChangeSign(pEdit);

		//updateTitleToCurDocFilePath();
	}
}

//关闭当前文件
void CCNotePad::slot_actionClose(bool)
{
	int index = ui.editTabWidget->currentIndex();
	if (index >= 0)
	{
		slot_tabClose(index);
	}
}

//关闭非当前文档
void CCNotePad::slot_actionCloseNonCurDoc()
{
	int index = ui.editTabWidget->currentIndex();

	for (int i = ui.editTabWidget->count()-1; i >=0; --i)
	{
		if (i != index)
		{
			slot_tabClose(i);
		}
	}
}

void CCNotePad::slot_actionCloseLeftAll()
{
	int index = ui.editTabWidget->currentIndex();

	for (int i = index - 1; i >= 0; --i)
	{
			slot_tabClose(i);
	}
}

void CCNotePad::slot_actionCloseRightAll()
{
	int index = ui.editTabWidget->currentIndex();

	for (int i = ui.editTabWidget->count() - 1; i > index; --i)
	{
		slot_tabClose(i);
	}
}

//不管如何关闭，最终显示一个new1的新建在最上面
void CCNotePad::initTabNewOne()
{
	if (0 == ui.editTabWidget->count())
	{
		slot_actionNewFile_toggle(true);
		m_saveFile->setEnabled(false);
		m_saveAllFile->setEnabled(false);
	}
}

//退出所有
void CCNotePad::slot_quit(bool)
{
	slot_closeAllFile(true);

	//中途取消了关闭，则不退出
	if (m_isQuitCancel)
	{
		m_isQuitCancel = false;
		return;
	}

	exit(0);
}

void CCNotePad::slot_saveAllFile()
{
	int index = ui.editTabWidget->currentIndex();
	if (index == -1)
	{
		return;
	}

	//从尾部开始依次调用保存所有文件。没修改的不需要保存
	for (int i = ui.editTabWidget->count()-1; i >= 0; --i)
	{
		ui.editTabWidget->setCurrentIndex(i);

		slot_actionSaveFile_toggle(true);
	}
	//最后恢复之前的current
	ui.editTabWidget->setCurrentIndex(index);
}


//定时自动保存
void CCNotePad::slot_autoSaveFile(bool status)
{
	if (m_timerAutoSave == nullptr)
	{
		m_timerAutoSave = new QTimer(this);
		connect(m_timerAutoSave, &QTimer::timeout, this, &CCNotePad::slot_timerAutoSave);
		slot_timerAutoSave();
	}

	if (status)
	{
		if (!m_timerAutoSave->isActive())
		{
			//3分钟自动保存一次
			m_timerAutoSave->start(1000 * 60 * 3);
			ui.statusBar->showMessage(tr("Cycle autosave on ..."),5000);
		}
	}
	else
	{
		if (m_timerAutoSave->isActive())
		{
			m_timerAutoSave->stop();
			ui.statusBar->showMessage(tr("Cycle autosave off ..."), 5000);
		}
	}
}

//定时器周期触发的自动保存
void CCNotePad::slot_timerAutoSave()
{
	int curTabIndex = ui.editTabWidget->currentIndex();

	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		
		//如果是未修改，不执行保存
		if (!getTextChangeProperty(pw))
		{
			continue;
		}

		//16进制文件不执行保存
		if (HEX_TYPE == getDocTypeProperty(pw))
		{
			continue;
		}

		//新建文件不需要保存
		if (getFileNewIndexProperty(pw) >= 0)
		{
			continue;
		}

		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr)
		{
			//如果是打开的文件了，则保存
			QString fileName = getFilePathProperty(pw);
			if (!fileName.isEmpty())
			{
				//保存前取消文件的修改检测，避免自己修改触发自己
				removeWatchFilePath(fileName);

				if (!saveFile(fileName, pEdit))
				{
					continue;
				}

				addWatchFilePath(fileName);
			}
		}

		//如果是未设置脏状态，则设置脏为false
		setTextChangeProperty(pw, false);

		if (curTabIndex == i)
		{
			m_saveFile->setEnabled(false);
			ui.statusBar->showMessage(tr("The current document has been automatically saved"), 5000);
		}

		//只有保存后再打开文本变化监控
		enableEditTextChangeSign(pEdit);
	}
}

//保存文件为临时文件。一定是文本格式，可读写的。只在关闭时才需要调用该函数。
//凡是存在临时文件的，一定是脏文件，即关闭时没有保存的文件。
//而不存在临时文件，只有一个记录在list中的文件，表示不脏的文件，直接打开原始文件即可。
//1:非脏新建文件 2 非脏的已存在文件 3 脏的新建文件 4 脏的老文件。
void CCNotePad::saveTempFile(ScintillaEditView* pEdit,int index, QSettings& qs)
{
	//16进制的处理逻辑
	if (TXT_TYPE != getDocTypeProperty(pEdit))
	{
		return;
	}

	QVariant v = pEdit->property(Edit_Text_Change);
	bool isDirty = v.toBool();

	//不脏则不需要保存，直接跳过。不脏的文件，只记录1个名称，下次打开时恢复
	if (!isDirty)
	{
		QString fileName = pEdit->property(Edit_View_FilePath).toString();
		//把文件记录到qs中去
		//index一定不能重复。n表示新建
		//如果是新建的文件
		if (pEdit->property(Edit_File_New) >= 0)
		{
			qs.setValue(QString("%1").arg(index), QString("%1|1").arg(fileName));
		}
		else
		{
			qs.setValue(QString("%1").arg(index), QString("%1|2").arg(fileName));

			//非新建文件，清空交换文件
			QString swapfile = getSwapFilePath(fileName);
			if (QFile::exists(swapfile))
			{
				QFile::remove(swapfile);
		}
		}
		return;
	}

	//如果是新建的文件
	if (pEdit->property(Edit_File_New) >= 0)
	{
		QString qsSavePath = qs.fileName();

		QFileInfo fi(qsSavePath);
		QString saveDir = fi.dir().absolutePath();
		QString tempFileName = QString("%1/%2").arg(saveDir).arg(index);

		if (!saveFile(tempFileName, pEdit,false,true))
		{
			return;
		}
		else
		{
			QString fileName = pEdit->property(Edit_View_FilePath).toString();
			//把文件记录到qs中去
			//index一定不能重复。2表示新建
			qs.setValue(QString("%1").arg(index), QString("%1|3").arg(fileName));
		}

	}
	else
	{
		//如果是打开的文件了，则保存。脏的已经存在文件
		QString fileName = pEdit->property(Edit_View_FilePath).toString();
		if (!fileName.isEmpty())
		{
				//保存前取消文件的修改检测，避免自己修改触发自己
			removeWatchFilePath(fileName);

			QString qsSavePath = qs.fileName();
			QFileInfo fi(qsSavePath);
			QString saveDir = fi.dir().absolutePath();
			QString tempFileName = QString("%1/%2").arg(saveDir).arg(index);

			if (saveFile(tempFileName, pEdit, false, true))
			{
				//把文件记录到qs中去
				//index一定不能重复。2表示存在
				qs.setValue(QString("%1").arg(index), QString("%1|4").arg(fileName));
			}

			//要注意清空一下交换文件。因为这里的文件其实已经保存完毕了，直接把交换文件删除。
		   //否则下次恢复时，会检测到存在交换文件，会提示用户。

			QString swapfile = getSwapFilePath(fileName);
			if (QFile::exists(swapfile))
			{
				QFile::remove(swapfile);
		}
	}
}
}

void CCNotePad::closeFileStatic(int index, QSettings& qs)
{
	QWidget* pw = ui.editTabWidget->widget(index);
	QString filePath = pw->property(Edit_View_FilePath).toString();

	//16进制的处理逻辑
	int type = getDocTypeProperty(pw);

	if (HEX_TYPE == type)
	{
		ui.editTabWidget->removeTab(index);
		pw->deleteLater();
		FileManager::getInstance().closeHexFileHand(filePath);
		return;
	}
	else if (BIG_TEXT_TYPE == type)
	{
		ui.editTabWidget->removeTab(index);
		pw->deleteLater();

		FileManager::getInstance().closeBigTextFileHand(filePath);
		return;
	}

	//关闭之前，检查是否要保存。如果文档为脏,则静默保存为temp文件
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
	if ((pEdit != nullptr))
	{
		saveTempFile(pEdit,index,qs);
	}

	ui.editTabWidget->removeTab(index);

	QVariant v = pw->property(Edit_File_New);

	int newFileIndex = v.toInt();

	if (newFileIndex >= 0)
	{
		//如果是新建文件，则删除对应的记录
		FileManager::getInstance().delNewFileNode(newFileIndex);
	}

	//如果关闭的是没有保存的新建文件，则不需要保存在最近打开列表中。新建文件需要保存
	if (-1 == pEdit->property(Edit_File_New).toInt())
	{
		QAction* topRecentFile = nullptr;

		//如果关闭的文件，已经在最近列表中，则移动到最前面即可
		if (m_receneOpenFile.contains(filePath))
		{
			topRecentFile = m_receneOpenFile.take(filePath);

			ui.menuFile->removeAction(topRecentFile);
			m_receneOpenFileList.removeOne(filePath);
		}
		else
		{
			topRecentFile = new QAction(filePath, ui.menuFile);
			connect(topRecentFile, &QAction::triggered, this, &CCNotePad::slot_openReceneFile);
		}

		//在菜单最近列表上面添加。如果最近列表是空的，则放在退出菜单之上
		if (m_receneOpenFileList.isEmpty())
		{
			ui.menuFile->insertAction(m_quitAction, topRecentFile);
		}
		else
		{
			//放在列表最上面
			QString curTopActionPath = m_receneOpenFileList.first();
			if (m_receneOpenFile.contains(curTopActionPath))
			{
				ui.menuFile->insertAction(m_receneOpenFile.value(curTopActionPath), topRecentFile);
			}
		}

		m_receneOpenFileList.push_front(filePath);
		m_receneOpenFile.insert(filePath, topRecentFile);

		//不能无限制变大，及时删除一部分
		if (m_receneOpenFileList.size() > 15)
		{
			QString k = m_receneOpenFileList.takeLast();
			if (m_receneOpenFile.contains(k))
			{
				delete m_receneOpenFile.value(k);
				m_receneOpenFile.remove(k);
			}

		}
	}

	//取消监控文件
	removeWatchFilePath(filePath);
	pw->deleteLater();
}
void CCNotePad::closeAllFileStatic()
{
	QString tempFileList = QString("notepad/temp/list");
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, tempFileList);
	qs.setIniCodec("UTF-8");
	qs.clear();

	//从尾部开始依次调用保存所有文件。没修改的不需要保存
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		ui.editTabWidget->setCurrentIndex(i);
		closeFileStatic(i,qs);
	}
}

void CCNotePad::slot_closeAllFile(bool)
{
	m_isQuitCancel = false;

	//从尾部开始依次调用保存所有文件。没修改的不需要保存
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		ui.editTabWidget->setCurrentIndex(i);

		slot_tabClose(i);

		if (m_isQuitCancel)
		{
			break;
		}
	}
}

void CCNotePad::closeEvent(QCloseEvent * event)
{
	if (!m_pFindWin.isNull())
	{
		m_pFindWin.data()->close();
	}

	if (!m_pHexGotoWin.isNull())
	{
		m_pHexGotoWin.data()->close();
	}


#ifdef Q_OS_WIN
	if ((s_restoreLastFile==1) && m_isMainWindows && !s_isAdminAuth)
	{
	//走静默安装的函数。对于没有保存的文件，一律保存为临时文件
		//主窗口才需要保存。非主的还是提示
	closeAllFileStatic();

		m_isQuitCancel = false;
	}
	else
	{
		slot_closeAllFile(true);
	}
#else
	if ((s_restoreLastFile == 1) && m_isMainWindows)
	{
		closeAllFileStatic();
		m_isQuitCancel = false;
	}
	else
	{
		slot_closeAllFile(true);
	}
#endif
	
	if (m_isQuitCancel)
	{
		m_isQuitCancel = false;
		event->ignore();
		return;
	}

	s_padInstances->removeOne(this);

	//把hwnd切换到当前还在的notepad，否则右键打开失效。因为窗口隐藏了
	//退位让贤给当前还在的窗口
	if (!s_padInstances->isEmpty())
	{
		CCNotePad* c = s_padInstances->first();

		//主窗口还在，没有删除,不用切换
		//管理员窗口任何时候不做住窗口
#ifdef Q_OS_WIN
		if (c->m_isMainWindows || s_isAdminAuth)
#else
        if (c->m_isMainWindows)
#endif
		{
			//主窗口还在，则当前窗口直接退出。但是把主窗口呼出来一下
			if (c->isHidden() || c->isMinimized())
			{
				c->showNormal();
			}
			event->accept();
			return;
		}
		else
		{
			//把接位的窗口设置为主窗口,显示出来。隐藏的窗口时没有winid的，避免该错误。
			if (c->isHidden() || c->isMinimized())
			{
				c->showNormal();
			}
		qlonglong winId = (qlonglong)c->effectiveWinId();
		m_shareMem->lock();
		memcpy(m_shareMem->data(), &winId, sizeof(qlonglong));
		m_shareMem->unlock();
			c->m_isMainWindows = true;
	}
	}

	event->accept();
}

void CCNotePad::updateSaveAllToolBarStatus()
{
	bool isNeedSaveAll = false;

	//从尾部开始依次调用保存所有文件。没修改的不需要保存
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget *pw = ui.editTabWidget->widget(i);
		if (pw->property(Edit_Text_Change).toBool())
		{
			isNeedSaveAll = true;
			break;
		}
	}

	setSaveAllButtonStatus(isNeedSaveAll);
}

void CCNotePad::slot_copyAvailable(bool select)
{
	if (m_cutFile != nullptr && m_copyFile != nullptr)
	{
		m_cutFile->setEnabled(select);
		m_copyFile->setEnabled(select);
	}
}

void CCNotePad::slot_cut()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		pEdit->cut();
	}
}

void CCNotePad::slot_copy()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		pEdit->copy();
	}
}

void CCNotePad::slot_paste()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		pEdit->paste();
	}
}

void CCNotePad::slot_selectAll()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		pEdit->execute(SCI_SELECTALL);
	}
}

void CCNotePad::slot_undo()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		pEdit->undo();
	}
}

void CCNotePad::slot_redo()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		pEdit->redo();
	}
}

void CCNotePad::slot_zoomin()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		pEdit->zoomIn();
	}
}

void CCNotePad::slot_zoomout()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		pEdit->zoomOut();
	}
}

//只切换了当前文档。可能会耗时，所以不全部换行。在文档切换的时候，需要检查下当前文档的自动换行状态。
void CCNotePad::slot_wordwrap(bool checked)
{
	QWidget* pw = ui.editTabWidget->currentWidget();

	//ui.actionWrap->setChecked(checked);
	//m_wordwrap->setChecked(checked);

	//是m_wordwrap快捷按钮按钮发出，同步菜单上的状态
	if (nullptr != dynamic_cast<QToolButton*>(sender()))
	{
	ui.actionWrap->setChecked(checked);
	}

	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		if (checked)
		{
			pEdit->setWrapMode(QsciScintilla::WrapWord);
		}
		else
		{
			pEdit->setWrapMode(QsciScintilla::WrapNone);
		}
	}
	
	s_autoWarp = (checked) ? 1 : 0;
	JsonDeploy::updataKeyValueFromNumSets(AUTOWARP_KEY, s_autoWarp);
}

void CCNotePad::slot_allWhite(bool checked)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		if (checked)
		{
			pEdit->setWhitespaceVisibility(QsciScintilla::WsVisible);
			pEdit->setEolVisibility(true);
		}
		else
		{
			pEdit->setWhitespaceVisibility(QsciScintilla::WsInvisible);
			pEdit->setEolVisibility(false);
		}
	}
}


//全部都会生效，和自动换行不一样，不会全部设置
void CCNotePad::slot_indentGuide(bool willBeShowed)
{
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);

		//文本的处理逻辑
		if (TXT_TYPE == getDocTypeProperty(pw))
		{
			//关闭之前，检查是否要保存。如果文档为脏，则询问是否要保存
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			pEdit->setIndentGuide(willBeShowed);
		}
	}

	s_indent = (willBeShowed) ? 1 : 0;
	JsonDeploy::updataKeyValueFromNumSets(INDENT_KEY, s_indent);
}

void CCNotePad::slot_find()
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
#ifdef uos
    	pFind->activateWindow();
#endif
	pFind->showNormal();
#ifdef uos
    adjustWInPos(pFind);
#endif

	pFind->setFocus();
	pFind->setCurrentTab(FIND_TAB);
}

void CCNotePad::initFindWindow()
{
	FindWin* pFind = nullptr;
	QWidget* pw = ui.editTabWidget->currentWidget();
	int docType = getDocTypeProperty(pw);

	if (m_pFindWin.isNull())
	{
		m_pFindWin = new FindWin(this);
		pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
		pFind->setFindHistory(&s_findHistroy);
	
		pFind->setTabWidget(ui.editTabWidget);

		if((TXT_TYPE == docType) || (BIG_TEXT_TYPE == docType))
		{
			connect(pFind, &FindWin::sign_findAllInCurDoc, this, &CCNotePad::slot_showFindAllInCurDocResult);
			connect(pFind, &FindWin::sign_findAllInOpenDoc, this, &CCNotePad::slot_showfindAllInOpenDocResult);
			connect(pFind, &FindWin::sign_replaceSaveFile, this, &CCNotePad::slot_saveFile);
			connect(pFind, &FindWin::sign_clearResult, this, &CCNotePad::slot_clearFindResult);
		}
		else
		{
			//二进制DOC只读，不许替换编辑。不在这里做，会导致编辑框回调。在FindWin里面去判断
			//pFind->disableReplace();
		}
	}
	else
	{
		pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	}

	
	if ((TXT_TYPE == docType) || (BIG_TEXT_TYPE == docType))
	{
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr && pEdit->hasSelectedText())
		{
			QString text = pEdit->selectedText();
			pFind->setFindText(text);
		}
	}
	else if (HEX_TYPE == docType)
	{
		ScintillaHexEditView* pEdit = dynamic_cast<ScintillaHexEditView*>(pw);
		if (pEdit != nullptr && pEdit->hasSelectedText())
		{
			QString text = pEdit->selectedText();
			pFind->setFindText(text);
		}
	}

}

void CCNotePad::slot_replace()
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	pFind->setCurrentTab(REPLACE_TAB);
	pFind->activateWindow();
	pFind->showNormal();
}

//标记高亮
void CCNotePad::slot_markHighlight()
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	pFind->setCurrentTab(MARK_TAB);
	pFind->activateWindow();
	pFind->showNormal();
}

//取消所有标记高亮
void CCNotePad::slot_clearMark()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	int docType = getDocTypeProperty(pw);

	if ((TXT_TYPE == docType) || (BIG_TEXT_TYPE == docType))
	{
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		int docEnd = pEdit->length();
		pEdit->execute(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE);
		pEdit->execute(SCI_INDICATORCLEARRANGE, 0, docEnd);
		pEdit->releaseAllMark();
	}
}

void CCNotePad::slot_wordHighlight()
{

	QWidget* pw = ui.editTabWidget->currentWidget();
	int docType = getDocTypeProperty(pw);
	QString signWord;
	ScintillaEditView* pEdit;
	int srcPostion = -1;
//	int firstDisLineNum = 0;

	if ((TXT_TYPE == docType) || (BIG_TEXT_TYPE == docType))
	{
		pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr && pEdit->hasSelectedText())
		{
			signWord = pEdit->selectedText();
		}
	}
	else if (HEX_TYPE == docType)
	{
		/*ScintillaHexEditView* pEdit = dynamic_cast<ScintillaHexEditView*>(pw);
		if (pEdit != nullptr && pEdit->hasSelectedText())
		{
			signWord = pEdit->selectedText();
		}*/
		return;
	}

	if (signWord.isEmpty())
	{
		//如果没有选择内容，则自动获取当前的单词，进行1个匹配。注意中文也是一样，检测到前后的空格，看做1个单词
		srcPostion = pEdit->execute(SCI_GETCURRENTPOS);
		//firstDisLineNum = pEdit->execute(SCI_GETFIRSTVISIBLELINE);
		
		signWord = pEdit->wordAtPosition(srcPostion);
	}

	if (!signWord.isEmpty())
	{
		//如果当前已经高亮，则取消高亮
		QList<FindRecords*>& curMarkRecord = pEdit->getCurMarkRecord();

		bool isClearMark = false;

		int i = 0;
		for (; i < curMarkRecord.size(); ++i)
		{
			if (signWord == curMarkRecord.at(i)->findText)
			{
				isClearMark = true;
				break;
			}
		}

		//取消高亮
		if (isClearMark)
		{
			FindRecords* r = curMarkRecord.at(i);

			pEdit->execute(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE);

			for (int j = 0; j < r->records.size(); ++j)
			{
				const FindRecord& oneRecord = r->records.at(j);
				pEdit->execute(SCI_INDICATORCLEARRANGE, oneRecord.pos, oneRecord.end - oneRecord.pos);
			}

			//必须删除释放，否则内存泄露
			delete r;
			curMarkRecord.removeAt(i);
		}
		else
		{
			//反之高亮
			initFindWindow();
			FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
			//静默调用
			pFind->markAllWord(signWord);

			//if (srcPostion != -1)
			//{
			//	//恢复原来的Pos位置
			//	pEdit->execute(SCI_GOTOPOS, srcPostion); 
			//	pEdit->execute(SCI_SETFIRSTVISIBLELINE, firstDisLineNum);
			//}
			}
		}
	}


void  CCNotePad::initFindResultDockWin()
{
	//停靠窗口1
	if (m_dockSelectTreeWin == nullptr)
	{

		m_dockSelectTreeWin = new QDockWidget(tr("Find result"), this);
		m_dockSelectTreeWin->layout()->setMargin(0);
		m_dockSelectTreeWin->layout()->setSpacing(0);

		//暂时不提供关闭，因为关闭后需要同步菜单的check状态

		m_dockSelectTreeWin->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
		m_dockSelectTreeWin->setAllowedAreas(Qt::BottomDockWidgetArea);

		m_pResultWin = new FindResultWin(m_dockSelectTreeWin);
		connect(m_pResultWin, &FindResultWin::itemDoubleClicked, this, &CCNotePad::slot_findResultItemDoubleClick);
		connect(m_pResultWin, &FindResultWin::showMsg, this, [this](QString& msg) {
			ui.statusBar->showMessage(msg,5000);
		});

		m_dockSelectTreeWin->setWidget(m_pResultWin);

		addDockWidget(Qt::BottomDockWidgetArea, m_dockSelectTreeWin);
	}

}


//双击查找结果，定位到对应的地方
void CCNotePad::slot_findResultItemDoubleClick(const QModelIndex &index)
{
	//如果点击的是父节点，父节点有ResultItemEditor，则跳转到对应的编辑框
	const QModelIndex* item = &index;

		auto locationCurrentEdit = [this](const QModelIndex *rootItem)->ScintillaEditView* {

			QString filePath = rootItem->data(ResultItemEditorFilePath).toString();

		getRegularFilePath(filePath);

		if (!rootItem->data(ResultItemEditor).isNull())
		{
			ScintillaEditView* pEdit = reinterpret_cast<ScintillaEditView*>(rootItem->data(ResultItemEditor).toLongLong());

				//这里如果是目录查找过来的，是没有pEdit的
				if (pEdit == nullptr)
				{
					goto foundInDir;
				}

			//当前已经在结果窗口上
			if (dynamic_cast<ScintillaEditView*>(ui.editTabWidget->currentWidget()) == pEdit)
			{
				return pEdit;
			}

			//不在则遍历插值定位到
			for (int i = 0; i < ui.editTabWidget->count(); ++i)
			{
				ScintillaEditView* pe = dynamic_cast<ScintillaEditView*>(ui.editTabWidget->widget(i));
				if (pEdit == pe)
				{
					ui.editTabWidget->setCurrentIndex(i);
					return pe;
				}
			}
			foundInDir:
				//遍历文件路径查找
				//不在则遍历插值定位到
				for (int i = 0; i < ui.editTabWidget->count(); ++i)
				{
					ScintillaEditView* pe = dynamic_cast<ScintillaEditView*>(ui.editTabWidget->widget(i));
					if (filePath == getFilePathProperty(pe))
					{
						ui.editTabWidget->setCurrentIndex(i);
						return pe;
		}
				}
				//还是没有找到，则新建打开文件
				if (openFile(filePath))
				{
					ScintillaEditView*pEdit = dynamic_cast<ScintillaEditView*>(ui.editTabWidget->currentWidget());
					return pEdit;
				}
			}

			ui.statusBar->showMessage(tr("file %1 was not exists !").arg(filePath), 5000);
		QApplication::beep();
		return nullptr;
	};

		if (!item->data(ResultItemDesc).isNull())
		{
		//点击的是描述行
			return;
		}

	//点击的是查找的根节点
		if (!item->data(ResultItemEditor).isNull())
	{
		locationCurrentEdit(item);
	}
	else if (!item->data(ResultItemPos).isNull())
	{
		//点击的是查找的结果项目
		QModelIndex rootItem = item->parent();
		ScintillaEditView* pCurEdit = locationCurrentEdit(&rootItem);
		if(pCurEdit != nullptr)
		{
			//int findLens = rootItem.data(ResultWhatFind).toString().length();
			//发现是中文必须下面这样，不能直接取qstring长度
			//int findLens = rootItem.data(ResultWhatFind).toString().toUtf8().size();

			int pos = item->data(ResultItemPos).toInt();
			int len = item->data(ResultItemLen).toInt();
	
			pCurEdit->execute(SCI_SETSEL, pos, pos + len);
		}

	}

}

void CCNotePad::slot_showFindAllInCurDocResult(FindRecords* record)
{
	initFindResultDockWin();

	m_dockSelectTreeWin->setWindowTitle(tr("Find result - %1 hit").arg(record->records.size()));

	m_pResultWin->appendResultsToShow(record);

	m_dockSelectTreeWin->show();
}

void CCNotePad::slot_showfindAllInOpenDocResult(QVector<FindRecords*>* record, int hits, QString whatFind)
{
	initFindResultDockWin();

	m_dockSelectTreeWin->setWindowTitle(tr("Find result - %1 hit").arg(hits));

	m_pResultWin->appendResultsToShow(record, hits, whatFind);

	m_dockSelectTreeWin->show();
}

//清空查找结果
void CCNotePad::slot_clearFindResult()
{
	initFindResultDockWin();
	m_pResultWin->slot_clearAllContents();
}


void CCNotePad::slot_convertWinLineEnd(bool)
{
	convertDocLineEnd(DOS_LINE);
}

void CCNotePad::slot_convertUnixLineEnd(bool)
{
	convertDocLineEnd(UNIX_LINE);
}

void CCNotePad::slot_convertMacLineEnd(bool)
{
	convertDocLineEnd(MAC_LINE);
}


void CCNotePad::setDocEolMode(ScintillaEditView* pEdit, RC_LINE_FORM endStatus)
{
	int eolMode = 0;

	switch (endStatus)
	{

	case UNIX_LINE:
		eolMode = SC_EOL_LF;
		break;
	case DOS_LINE:
		eolMode = SC_EOL_CRLF;
		break;
	case MAC_LINE:
		eolMode = SC_EOL_CR;
		break;
	case PAD_LINE:
	case UNKNOWN_LINE:
	default:
		return;
	}

	if (pEdit != nullptr)
	{
		int curCode = pEdit->execute(SCI_GETEOLMODE);
		if (curCode != eolMode)
		{
			pEdit->execute(SCI_SETEOLMODE, eolMode);
		}
	}
}

void CCNotePad::convertDocLineEnd(RC_LINE_FORM endStatus)
{
	QWidget* pw = ui.editTabWidget->currentWidget();

	if (HEX_TYPE == getDocTypeProperty(pw))
	{
		ui.statusBar->showMessage(tr("Only Text File Can Use it, Current Doc is a Hex File !"), 10000);
		QApplication::beep();
		return;
	}

	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	int eolMode = 0;

	switch (endStatus)
	{

	case UNIX_LINE:
		eolMode = SC_EOL_LF;
		break;
	case DOS_LINE:
		eolMode = SC_EOL_CRLF;
		break;
	case MAC_LINE:
		eolMode = SC_EOL_CR;
		break;
	case PAD_LINE:
	case UNKNOWN_LINE:
	default:
		return;
	}

	if (pEdit != nullptr)
	{
		int curCode = pEdit->execute(SCI_GETEOLMODE);
		if (curCode != eolMode)
		{
			ui.statusBar->showMessage(tr("Convert end of line In progress, please wait ..."));

			pEdit->execute(SCI_SETEOLMODE, eolMode);
			pEdit->execute(SCI_CONVERTEOLS, eolMode);

			ui.statusBar->showMessage(tr("Convert end of line finish."),5000);

			QVariant editTextEnd((int)endStatus);
			pEdit->setProperty(Edit_Text_End, editTextEnd);

			QVariant textChanged(true);
			pEdit->setProperty(Edit_Text_Change, textChanged);

			setLineEndBarLabel(endStatus);
		}
	}
}


void CCNotePad::slot_gotoline()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		int lineCounts = 99999999; 

		bool ok;
		int num = QInputDialog::getInt(this, tr("Go to line"), tr("Line Num:"), 1, 1, lineCounts, 1, &ok);
		if (ok)
		{
			pEdit->execute(SCI_GOTOLINE, num-1);
		}
	}
}

void CCNotePad::slot_show_spaces(bool checked)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		if (checked)
		{
			pEdit->setWhitespaceVisibility(QsciScintilla::WsVisible);
		}
		else
		{
			pEdit->setWhitespaceVisibility(QsciScintilla::WsInvisible);
		}
	}
}

void CCNotePad::slot_show_line_end(bool checked)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		if (checked)
		{
			pEdit->setEolVisibility(true);
		}
		else
		{
			pEdit->setEolVisibility(false);
		}
	}
}

void CCNotePad::slot_show_all(bool checked)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		if (checked)
		{
			pEdit->setWhitespaceVisibility(QsciScintilla::WsVisible);
			pEdit->setEolVisibility(true);
		}
		else
		{
			pEdit->setWhitespaceVisibility(QsciScintilla::WsInvisible);
			pEdit->setEolVisibility(false);
		}
	}
}

void CCNotePad::slot_load_with_gbk()
{
	reloadTextFileWithCode(CODE_ID::GBK);
}

void CCNotePad::slot_load_with_utf8()
{
	reloadTextFileWithCode(CODE_ID::UTF8_NOBOM);
}

void CCNotePad::slot_load_with_utf8_bom()
{
	reloadTextFileWithCode(CODE_ID::UTF8_BOM);
}

void CCNotePad::slot_load_with_utf16_be()
{
	reloadTextFileWithCode(CODE_ID::UNICODE_BE);
}

void CCNotePad::slot_load_with_utf16_le()
{
	reloadTextFileWithCode(CODE_ID::UNICODE_LE);
}

void CCNotePad::slot_encode_gbk()
{
	transDocToEncord(GBK);
}

void CCNotePad::slot_encode_utf8()
{
	transDocToEncord(UTF8_NOBOM);
}

void CCNotePad::slot_encode_utf8_bom()
{
	transDocToEncord(UTF8_BOM);
}

void CCNotePad::slot_encode_utf16_be()
{
	transDocToEncord(UNICODE_BE);
}

void CCNotePad::slot_encode_utf16_le()
{
	transDocToEncord(UNICODE_LE);
}

//执行转换文档编码到指定编码
void CCNotePad::transDocToEncord(CODE_ID destCode)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		int srcCode = pEdit->property(Edit_Text_Code).toInt();
		int newCode = static_cast<int>(destCode);

		//这里只是做了编码状态的修改，并没有及时落盘，修改为已修改状态。
		//等到文件保存时才执行
		if (srcCode != newCode)
		{
			QVariant editTextCode((int)newCode);
			pEdit->setProperty(Edit_Text_Code, editTextCode);

			QVariant textChanged(true);
			pEdit->setProperty(Edit_Text_Change, textChanged);

			setSaveButtonStatus(true);
			setSaveAllButtonStatus(true);

			syncCurDocEncodeToMenu(pw);

			setCodeBarLabel(static_cast<CODE_ID>(newCode));

		}
	}
}

//同步当前文档的编码状态到菜单的Encode中。即每次切换当前编辑后，菜单上面的编码状态要同步
void CCNotePad::syncCurDocEncodeToMenu(QWidget* curEdit)
{
	if (curEdit != nullptr)
	{
		int srcCode = curEdit->property(Edit_Text_Code).toInt();

		switch (srcCode)
		{
		case GBK:
			ui.actionencode_in_GBK->setChecked(true);
			break;

		case UTF8_NOBOM:
			ui.actionencode_in_uft8->setChecked(true);
			break;

		case UTF8_BOM:
			ui.actionencode_in_UTF8_BOM->setChecked(true);
			break;

		case UNICODE_BE:
			ui.actionencode_in_UCS_BE_BOM->setChecked(true);
			break;

		case UNICODE_LE:
			ui.actionencode_in_UCS_2_LE_BOM->setChecked(true);
			break;

		default:
			break;
		}
	}
}


//同步当前文档的编码状态到菜单的Encode中。即每次切换当前编辑后，菜单上面的编码状态要同步
void CCNotePad::syncCurDocLineEndStatusToMenu(QWidget* curEdit)
{
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(curEdit);


	if (pEdit != nullptr)
	{
		int curCode = pEdit->execute(SCI_GETEOLMODE);
		switch (curCode)
		{
		case SC_EOL_CRLF:
			ui.actionconver_windows_CR_LF->setChecked(true);
			break;

		case SC_EOL_LF:
			ui.actionconvert_Unix_LF->setChecked(true);
			break;

		case SC_EOL_CR:
			ui.actionconvert_Mac_CR->setChecked(true);
			break;

		default:
			break;
		}
	}
}

//同步当前的编程语言到menu中
void CCNotePad::syncCurDocLexerToMenu(QWidget* pw)
{
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit == nullptr)
	{
		return;
	}

	QsciLexer * lexer = pEdit->lexer();
	if (lexer != nullptr)
	{
		QString lexerName(lexer->lexerTag());

		if (m_lexerNameToIndex.contains(lexerName))
		{
			m_lexerNameToIndex.value(lexerName).pAct->setChecked(true);
		}
		setLangsDescLable(lexerName);
	}
	else
	{
		ui.actionTxt->setChecked(true);
	}
}

void CCNotePad::cmpSelectFile()
{
	//此部分是对比软件的商业代码，开源版本不包含对比功能。

}

void CCNotePad::slot_compareFile()
{
	//此部分是对比软件的商业代码，开源版本不包含对比功能。
}

void CCNotePad::slot_compareDir()
{
	//此部分是对比软件的商业代码，开源版本不包含对比功能。
}

void CCNotePad::slot_binCompare()
{
	//此部分是对比软件的商业代码，开源版本不包含对比功能。
}


void CCNotePad::slot_preHexPage()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr && (TXT_TYPE == getDocTypeProperty(pw)))
	{
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (!pEdit->gotoPrePos())
		{
			QApplication::beep();
			ui.statusBar->showMessage(tr("no more pre pos"));
		}
	}
	else if (pw != nullptr && (HEX_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		HexFileMgr *fileMgr = nullptr;
		//只有0才是成功
		if (0 == FileManager::getInstance().loadFilePreNextPage(1, filePath, fileMgr))
		{
			ScintillaHexEditView* pEdit = dynamic_cast<ScintillaHexEditView*>(pw);
			showHexFile(pEdit, fileMgr);
		}
	}
	if (pw != nullptr && (BIG_TEXT_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		TextFileMgr *fileMgr = nullptr;
		//只有0才是成功
		if (0 == FileManager::getInstance().loadFilePreNextPage(1, filePath, fileMgr))
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			showBigTextFile(pEdit, fileMgr);
	}
	}
}

void CCNotePad::slot_nextHexPage()
{
	QWidget* pw = ui.editTabWidget->currentWidget();

	if (pw != nullptr && (TXT_TYPE == getDocTypeProperty(pw)))
	{
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (!pEdit->gotoNextPos())
		{
			QApplication::beep();
			ui.statusBar->showMessage(tr("no more next pos"));
		}
	}
	else if (pw != nullptr && (HEX_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		HexFileMgr *fileMgr = nullptr;

		//只有0才是成功
		int ret = FileManager::getInstance().loadFilePreNextPage(2, filePath, fileMgr);
		if (0 == ret)
		{
			ScintillaHexEditView* pEdit = dynamic_cast<ScintillaHexEditView*>(pw);
			showHexFile(pEdit, fileMgr);
			//ui.statusBar->showMessage(tr("Current offset is %1 , File Size is %2").arg(fileMgr->fileOffset).arg(fileMgr->fileSize));
		}
		else if (1 == ret)
		{
			ui.statusBar->showMessage(tr("The Last Page ! Current offset is %1 , load Contens Size is %2, File Total Size is %3").arg(fileMgr->fileOffset - fileMgr->contentRealSize).arg(fileMgr->contentRealSize).arg(fileMgr->fileSize));
			QApplication::beep();
		}
	}
	else if (pw != nullptr && (BIG_TEXT_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		TextFileMgr *fileMgr = nullptr;
		//只有0才是成功
		int ret = FileManager::getInstance().loadFilePreNextPage(2, filePath, fileMgr);
		if (0 == ret)
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			showBigTextFile(pEdit, fileMgr);
	}
		else if (1 == ret)
		{
			ui.statusBar->showMessage(tr("The Last Page ! Current offset is %1 , load Contens Size is %2, File Total Size is %3").arg(fileMgr->fileOffset - fileMgr->contentRealSize).arg(fileMgr->contentRealSize).arg(fileMgr->fileSize));
			QApplication::beep();
		}
	}
}

void CCNotePad::slot_gotoHexPage()
{
	if (m_pHexGotoWin.isNull())
	{
		m_pHexGotoWin = new HexFileGoto();

		HexFileGoto* pHexGoto = dynamic_cast<HexFileGoto*>(m_pHexGotoWin.data());

		pHexGoto->setAttribute(Qt::WA_DeleteOnClose);

		connect(pHexGoto, &HexFileGoto::gotoClick, this, &CCNotePad::slot_hexGotoFile);
	}
	m_pHexGotoWin.data()->activateWindow();
	m_pHexGotoWin.data()->show();
#ifdef uos
    adjustWInPos(m_pHexGotoWin.data());
#endif

	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr && (HEX_TYPE == getDocTypeProperty(pw)))
	{
		
	}
	else if (pw != nullptr && (BIG_TEXT_TYPE == getDocTypeProperty(pw)))
	{
		
	}
	else
	{
		ui.statusBar->showMessage(tr("Only Hex File Can Use it, Current Doc not a Hex File !"), 10000);
		QApplication::beep();
	}
}

void CCNotePad::slot_hexGotoFile(qint64 addr)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr && (HEX_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		HexFileMgr *fileMgr = nullptr;
		//只有0才是成功

		if (addr < 0)
		{
			ui.statusBar->showMessage(tr("Error file offset addr , please check !"), 8000);
			QApplication::beep();
			return;
		}

		int ret = FileManager::getInstance().loadFileFromAddr(filePath, addr, fileMgr);
		if (0 == ret)
		{
			ScintillaHexEditView* pEdit = dynamic_cast<ScintillaHexEditView*>(pw);
			showHexFile(pEdit, fileMgr);
		}
		else if (-2 == ret)
		{
			ui.statusBar->showMessage(tr("File Size is %1, addr %2 is exceeds file size").arg(fileMgr->fileSize).arg(addr));
			QApplication::beep();
		}
	}
	else if (pw != nullptr && (BIG_TEXT_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		TextFileMgr *fileMgr = nullptr;
		//只有0才是成功

		if (addr < 0)
		{
			ui.statusBar->showMessage(tr("Error file offset addr , please check !"), 8000);
			QApplication::beep();
			return;
		}

		int ret = FileManager::getInstance().loadFileFromAddr(filePath, addr, fileMgr);
		if (0 == ret)
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			showBigTextFile(pEdit, fileMgr);
		}
		else if (-2 == ret)
		{
			ui.statusBar->showMessage(tr("File Size is %1, addr %2 is exceeds file size").arg(fileMgr->fileSize).arg(addr));
			QApplication::beep();
		}
	}
	else
	{
		ui.statusBar->showMessage(tr("Current Text Doc Can Not Use it !"), 10000);
		QApplication::beep();
	}
}

void CCNotePad::slot_about()
{
	QMessageBox msgBox(this);
#if defined (Q_OS_MAC)
	msgBox.setText(tr("bugfix: https://github.com/cxasm/notepad-- \nchina: https://gitee.com/cxasm/notepad--"));
    msgBox.setDetailedText("Notepad-- v1.16.2");
#else
	msgBox.setWindowTitle(QString("Notepad-- v1.16.2"));
	msgBox.setText(tr("bugfix: https://github.com/cxasm/notepad-- \nchina: https://gitee.com/cxasm/notepad--"));
#endif
	msgBox.setTextInteractionFlags(Qt::TextSelectableByMouse);

	msgBox.exec();
}

void CCNotePad::addWatchFilePath(QString filePath)
{
	getRegularFilePath(filePath);
	m_fileWatch->addPath(filePath);
}

void CCNotePad::removeWatchFilePath(QString filePath)
{
	getRegularFilePath(filePath);
	m_fileWatch->removePath(filePath);
}

void CCNotePad::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
	{
		event->accept(); //可以在这个窗口部件上拖放对象
	}
	else
	{
		event->ignore();
	}
}

void CCNotePad::receiveEditDrop(QDropEvent* e)
{
	dropEvent(e);
}

void CCNotePad::dropEvent(QDropEvent* e)
{
	QList<QUrl> urls = e->mimeData()->urls();
	if (urls.isEmpty())
		return;

	QString fileName = urls.first().toLocalFile();

	if (fileName.isEmpty())
	{
		return;
	}

	if (!QFile::exists(fileName))
	{
		return;
	}

	openFile(fileName);
	e->accept();

	//qDebug() << ui.leftSrc->geometry() << ui.rightSrc->geometry() << QCursor::pos() << this->mapFromGlobal(QCursor::pos());
}


#ifdef Q_OS_WIN
static const ULONG_PTR CUSTOM_TYPE = 10000;
static const ULONG_PTR OPEN_NOTEPAD_TYPE = 10001;

bool CCNotePad::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	MSG *param = static_cast<MSG *>(message);

	switch (param->message)
	{
		case WM_COPYDATA:
		{
			COPYDATASTRUCT *cds = reinterpret_cast<COPYDATASTRUCT*>(param->lParam);
			if (cds->dwData == CUSTOM_TYPE)
			{
				QString openFilePath = QString::fromUtf8(reinterpret_cast<char*>(cds->lpData), cds->cbData);
				
				int retIndex = findFileIsOpenAtPad(openFilePath);
				if (-1 == retIndex)
				{
					openFile(openFilePath);
				}
				else
				{
					ui.statusBar->showMessage(tr("file %1 already open at tab %2").arg(openFilePath).arg(retIndex));
					ui.editTabWidget->setCurrentIndex(retIndex);
				}

				activateWindow();
				showNormal();
				//showMaximized();

				*result = 1;
				return true;
			}
			else if (cds->dwData == OPEN_NOTEPAD_TYPE)
			{
				activateWindow();
				showNormal();

				*result = 1;
				return true;
		}
	}
	}

	return QWidget::nativeEvent(eventType, message, result);
}
#endif

void CCNotePad::slot_batch_convert()
{
	EncodeConvert* newWin = new EncodeConvert(nullptr);
	newWin->setAttribute(Qt::WA_DeleteOnClose);
	newWin->setWindowModality(Qt::ApplicationModal);
	newWin->show();
#ifdef uos
    adjustWInPos(newWin);
#endif
}

//批量改名
void CCNotePad::slot_batch_rename()
{
	ReNameWin* newWin = new ReNameWin(nullptr);
	newWin->setAttribute(Qt::WA_DeleteOnClose);
	newWin->setWindowModality(Qt::ApplicationModal);
	newWin->show();
#ifdef uos
    adjustWInPos(newWin);
#endif
}

void CCNotePad::slot_options()
{
	OptionsView* p = new OptionsView();
	p->setAttribute(Qt::WA_DeleteOnClose);
	p->setWindowModality(Qt::ApplicationModal);
	connect(p, &OptionsView::sendTabFormatChange, this, &CCNotePad::slot_tabFormatChange);
	//connect(p, &OptionsView::signTxtFontChange, this, &CCNotePad::slot_txtFontChange);
	//connect(p, &OptionsView::signProLangFontChange, this, &CCNotePad::slot_proLangFontChange);

	p->show();
#ifdef uos
    adjustWInPos(p);
#endif
}

#if 0
//默认TXT文本的字体发生了变化
void CCNotePad::slot_txtFontChange(QFont &font)
{
	QsciLexerText::setGlobalDefaultFont(font);

	s_txtFont = font;

	QWidget* pw = nullptr;
	QsciLexer* lexer = nullptr;

	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		pw = ui.editTabWidget->widget(i);
		int docType = getDocTypeProperty(pw);

		if ((TXT_TYPE == docType) || (BIG_TEXT_TYPE == docType))
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			if (pEdit != nullptr )
			{
				lexer = pEdit->lexer();

				if (lexer != nullptr && (lexer->lexerId() == L_TXT))
				{
					QsciLexerText* t = dynamic_cast<QsciLexerText*>(lexer);
					if (t != nullptr)
					{
						t->setFont(font, 0);
					}
				}
			}
		}
	}
}
#endif

#if 0
//默认编程语言的文本的字体发生了变化
void CCNotePad::slot_proLangFontChange(QFont &font)
{
	QsciLexer::setProLangeDefaultFont(font);

	s_proLangFont = font;

	QWidget* pw = nullptr;
	QsciLexer* lexer = nullptr;

	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		pw = ui.editTabWidget->widget(i);
		int docType = getDocTypeProperty(pw);

		if ((TXT_TYPE == docType) || (BIG_TEXT_TYPE == docType))
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			if (pEdit != nullptr)
			{
				lexer = pEdit->lexer();

				if (lexer != nullptr && (lexer->lexerId() != L_TXT))
				{
					//这里还是有些问题，把所有的字体大小全部修改了。而没有只针对默认字体。
					lexer->setFont(font, -1);
				}
			}
		}
	}
}
#endif

//tab长度或者使用空格替换tab发生了变化
void CCNotePad::slot_tabFormatChange(bool tabLenChange, bool useTabChange)
{
	QWidget* pw = nullptr;
	int docType = 0;

	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		pw = ui.editTabWidget->widget(i);
		docType = getDocTypeProperty(pw);

		if ((TXT_TYPE == docType) || (BIG_TEXT_TYPE == docType))
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			if (pEdit != nullptr)
			{
				if (tabLenChange)
				{
					pEdit->execute(SCI_SETTABWIDTH, ScintillaEditView::s_tabLens);
				}
				if (useTabChange)
				{
					pEdit->execute(SCI_SETUSETABS, !ScintillaEditView::s_noUseTab);
				}
			}
		}
	}
}

void CCNotePad::slot_donate()
{
	donate* pWin = new donate();
	//pWin->setWindowIcon(QIcon(":/Resources/img/main.png"));
	pWin->setAttribute(Qt::WA_DeleteOnClose);
	pWin->show();
#ifdef uos
    adjustWInPos(pWin);
#endif
}

void CCNotePad::slot_registerCmd(int cmd, int code)
{
	if (cmd == 1)
	{
		//服务器返回注册码的消息。	//0 试用 1 正版 2 正版过期 3 错误key
		int status = JsonDeploy::getKeyValueFromNumSets(SOFT_STATUS);
		if (status != code)
		{
			JsonDeploy::updataKeyValueFromNumSets(SOFT_STATUS, code);
		}

		emit signRegisterReplay(code);
	}
}



void CCNotePad::slot_toLightBlueStyle()
{
	StyleSet::setLightStyle();

	//皮肤id
	JsonDeploy::updataKeyValueFromNumSets(SKIN_KEY,1);
}

void CCNotePad::slot_toDefaultStyle()
{
	StyleSet::setDefaultStyle();
	JsonDeploy::updataKeyValueFromNumSets(SKIN_KEY, 0);
}

void  CCNotePad::slot_toThinBlueStyle()
{
	StyleSet::setThinBlueStyle();
	JsonDeploy::updataKeyValueFromNumSets(SKIN_KEY, 2);
}

void CCNotePad::slot_toRiceYellow()
{
	StyleSet::setRiceYellowStyle();
	JsonDeploy::updataKeyValueFromNumSets(SKIN_KEY, 4);
}

void CCNotePad::slot_toYellow()
{
	StyleSet::setThinYellowStyle();
	JsonDeploy::updataKeyValueFromNumSets(SKIN_KEY, 3);
}

void CCNotePad::slot_toSilverStyle()
{
	StyleSet::setSilverStyle();
	JsonDeploy::updataKeyValueFromNumSets(SKIN_KEY, 5);
}

void CCNotePad::slot_toLavenderBlush()
{
	StyleSet::setLavenderBlushStyle();
	JsonDeploy::updataKeyValueFromNumSets(SKIN_KEY, 6);
}

void CCNotePad::slot_toMistyRose()
{
	StyleSet::setMistyRoseStyle();
	JsonDeploy::updataKeyValueFromNumSets(SKIN_KEY, 7);
}

//获取注册码
void CCNotePad::slot_register()
{

}

void CCNotePad::slot_langFormat()
{
	QString initTag;

	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
	if (pEdit != nullptr && (pEdit->lexer() != nullptr))
	{
		initTag = pEdit->lexer()->lexerTag();
	}

	QtLangSet* pWin = new QtLangSet(initTag,this);
	pWin->setAttribute(Qt::WA_DeleteOnClose);
	connect(pWin, &QtLangSet::viewStyleChange, this, &CCNotePad::slot_viewStyleChange);
	pWin->show();
}

void CCNotePad::slot_viewStyleChange(int lexerId, int styleId, QColor& fgColor, QColor& bkColor, QFont& font, bool fontChange)
{
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr && (pEdit->lexer() != nullptr))
		{
			QsciLexer* lexer = pEdit->lexer();

			if (lexer->lexerId() == lexerId)
			{
				if (fgColor.isValid())
				{
					lexer->setColor(fgColor, styleId);
				}
				if (bkColor.isValid())
				{
					lexer->setPaper(bkColor, styleId);
				}

				if (fontChange)
				{
					lexer->setFont(font, styleId);
				}
			}
		}
	}
}

//1:非脏新建文件(干净新文件）
void CCNotePad::restoreCleanNewFile(QString& fileName)
{
	int index = FileManager::getInstance().getNextNewFileId();
	newTxtFile(fileName, index);

	m_saveFile->setEnabled(false);
}

//2 非脏的已存在文件
void CCNotePad::restoreCleanExistFile(QString& filePath)
{
	openTextFile(filePath);
	m_saveFile->setEnabled(false);
}

//3 脏的新建文件。内容在tempFilePath中
void CCNotePad::restoreDirtyNewFile(QString& fileName, QString& tempFilePath)
{
	int index = FileManager::getInstance().getNextNewFileId();
	newTxtFile(fileName, index, tempFilePath);
	m_saveFile->setEnabled(true);
	m_saveAllFile->setEnabled(true);
}

//4 脏的老文件。内容在tempFilePath中
bool CCNotePad::restoreDirtyExistFile(QString& filePath, QString& tempFilePath)
{
	getRegularFilePath(filePath);

	QFileInfo fi(filePath);

	//如果文件大于300M,按照只读文件打开
	if (ScintillaEditView::s_bigTextSize <= 0 || ScintillaEditView::s_bigTextSize > 300)
	{
		ScintillaEditView::s_bigTextSize = 100;
	}

	if (fi.size() > ScintillaEditView::s_bigTextSize * 1024 * 1024)
	{
		return openBigTextFile(filePath);
	}

	QString fileLabel(fi.fileName());

	ScintillaEditView* pEdit = FileManager::getInstance().newEmptyDocument();
	pEdit->setNoteWidget(this);
	//必须要在editTabWidget->addTab之前，因为一旦add时会出发tabchange，其中没有doctype会导致错误
	setDocTypeProperty(pEdit, TXT_TYPE);

	CODE_ID code(UNKOWN);
	bool isReadOnly = false;
#ifdef _WIN32
	RC_LINE_FORM lineEnd(DOS_LINE);
#else
	RC_LINE_FORM lineEnd(UNIX_LINE);
#endif

	bool isChange = false;

	//如果非空，则从contentPath中加载文件内容。做恢复文件使用
	if (!tempFilePath.isEmpty())
	{
		int ret = FileManager::getInstance().loadFileDataInText(pEdit, tempFilePath, code, lineEnd, nullptr, false);
		if (ret != 0)
		{
			ui.statusBar->showMessage(tr("Restore Last Temp File %1 Failed").arg(tempFilePath));
		}
		isChange = true;
	}
	else
	{

		int ret = FileManager::getInstance().loadFileDataInText(pEdit, filePath, code, lineEnd, this, true);
		if (4 == ret)
		{
			delete pEdit;
			//用户同意以二进制格式打开文件
			return openHexFile(filePath);
		}
		else if (5 == ret)
		{
			isReadOnly = true;
			//只读模式
		}
		else if (0 != ret)
		{
			delete pEdit;
			return false;
		}
	}
	
	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon(TabNoNeedSave), getShortName(fileLabel));
	ui.editTabWidget->setCurrentIndex(curIndex);

	connect(pEdit, &ScintillaEditView::cursorPositionChanged, this, &CCNotePad::slot_LineNumIndexChange, Qt::QueuedConnection);
	enableEditTextChangeSign(pEdit);
	connect(pEdit, &ScintillaEditView::copyAvailable, this, &CCNotePad::slot_copyAvailable);
	connect(pEdit, SIGNAL(SCN_ZOOM()), pEdit, SLOT(updateLineNumberWidth()));

	//监控文件
	addWatchFilePath(filePath);

	setCodeBarLabel(code);
	setLineEndBarLabel(lineEnd);

	//注意顺序
	QVariant editTextEnd((int)lineEnd);
	pEdit->setProperty(Edit_Text_End, editTextEnd);
	setDocEolMode(pEdit, lineEnd);

	QVariant editViewFilePath(filePath);
	pEdit->setProperty(Edit_View_FilePath, editViewFilePath);

	ui.editTabWidget->setTabToolTip(curIndex, filePath);

	QVariant editViewNewFile(-1);
	pEdit->setProperty(Edit_File_New, editViewNewFile);

	setTextChangeProperty(pEdit, isChange);

	QVariant editTextCode((int)code);
	pEdit->setProperty(Edit_Text_Code, editTextCode);

	syncCurDocEncodeToMenu(pEdit);
	syncCurDocLineEndStatusToMenu(pEdit);
	syncCurDocLexerToMenu(pEdit);

	//设置自动转换和缩进参考线
	if (s_autoWarp == 1)
	{
		pEdit->setWrapMode(QsciScintilla::WrapWord);
	}

	if (s_indent == 1)
	{
		pEdit->setIndentGuide(true);
	}

	if (!isReadOnly)
	{
		setFileOpenAttrProperty(pEdit, OpenAttr::Text);
		setWindowTitleMode(filePath, OpenAttr::Text);
		ui.statusBar->showMessage(tr("File %1 Open Finished [Text Mode]").arg(filePath));
	}
	else
	{
		setFileOpenAttrProperty(pEdit, OpenAttr::TextReadOnly);
		setWindowTitleMode(filePath, OpenAttr::TextReadOnly);
		ui.statusBar->showMessage(tr("File %1 Open Finished [Text ReadOnly Mode] (Note: display up to 50K bytes ...)").arg(fi.fileName()));
	}

	if (pEdit->lexer() == nullptr)
	{
		autoSetDocLexer(pEdit);
	}

	if (isChange)
	{
		m_saveFile->setEnabled(true);
		m_saveAllFile->setEnabled(true);
	}
	return true;
}

//恢复上次打开的文件
//1:非脏新建文件 2 非脏的已存在文件 3 脏的新建文件 4 脏的老文件。
int CCNotePad::restoreLastFiles()
{
	if (s_restoreLastFile == 0)
	{
		return 0;
	}

	QString tempFileList = QString("notepad/temp/list");
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, tempFileList);
	qs.setIniCodec("UTF-8");

	QStringList fileList = qs.allKeys();
	//从小到大排序一下
	fileList.sort();

	QString key;
	QString value;

	foreach(key, fileList)
	{
		value = qs.value(key).toString();
		if (!value.isEmpty())
		{
			int type = value.right(1).toInt();
			QString path = value.left(value.size()-2);
			
			switch (type)
			{
			case 1:
				restoreCleanNewFile(path);
				break;
			case 2:
				restoreCleanExistFile(path);
				break;
			case 3:
			{
				QString qsSavePath = qs.fileName();
				QFileInfo fi(qsSavePath);
				QString saveDir = fi.dir().absolutePath();
				QString tempFileName = QString("%1/%2").arg(saveDir).arg(key);
				restoreDirtyNewFile(path, tempFileName);
			}
				break;
			case 4:
			{
				QString qsSavePath = qs.fileName();
				QFileInfo fi(qsSavePath);
				QString saveDir = fi.dir().absolutePath();
				QString tempFileName = QString("%1/%2").arg(saveDir).arg(key);
				restoreDirtyExistFile(path, tempFileName);
			}
				break;
			default:
				break;
			}
		}
	}
	return fileList.size();
}


//删除行首空格
void CCNotePad::slot_removeHeadBlank()
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	//静默调用
	pFind->removeLineHeadEndBlank(1);
}

//删除行尾空格
void CCNotePad::slot_removeEndBlank()
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	//静默调用
	pFind->removeLineHeadEndBlank(2);
}

//删除两端空格
void CCNotePad::slot_removeHeadEndBlank()
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	//静默调用
	pFind->removeLineHeadEndBlank(1);
	pFind->removeLineHeadEndBlank(2);
}

void CCNotePad::slot_columnBlockEdit()
{
	ColumnEdit* pWin = new ColumnEdit();
	pWin->setAttribute(Qt::WA_DeleteOnClose);
	pWin->setTabWidget(ui.editTabWidget);
	pWin->show();
}
