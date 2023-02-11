#include <memory>
#include "ccnotepad.h"
#include "filemanager.h"
#include "Encode.h"
#include "findwin.h"
#include "nddsetting.h"
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
#include "langstyledefine.h"
#include "extlexermanager.h"
#include "aboutndd.h"
#include "filelistview.h"
#include "bigfilemessage.h"
#include "batchfindreplace.h"
#include "langextset.h"
#include "shortcutkeymgr.h"
#include "CmpareMode.h"

#ifdef NO_PLUGIN
#include "pluginmgr.h"
#include "plugin.h"
#include "pluginGl.h"
#endif


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
#include <QShortcut>
#ifdef Q_OS_WIN
#include <QXmlQuery>
#include <QXmlFormatter>
#endif
#include <QBuffer>
#include <QXmlStreamReader>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWidgetAction>
#include <QListWidgetItem>
#include <QLibrary>

#include "Sorters.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <Windows.h>
#endif
#include <memory>


#ifdef Q_OS_WIN
extern bool s_isAdminAuth;

inline std::wstring StringToWString(const std::string& str)
{
#if 0
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wide = new wchar_t[len + 1];
	memset(wide, '\0', sizeof(wchar_t) * (len + 1));
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, len);
	std::wstring w_str(wide);
	delete[] wide;
	return w_str;
#endif

	QString temp = QString::fromStdString(str);
	return temp.toStdWString();
}
#endif


int CCNotePad::s_padTimes = 0;
int CCNotePad::s_zoomValue = 0;

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
		ret = QObject::tr("Big Text ReadOnly Mode");
		break;
	case BigTextReadWrite:
		ret = QObject::tr("Big Text ReadWrite Mode");
		break;
	case SuperBigTextReadOnly:
		ret = QObject::tr("Super Big Text ReadOnly Mode");
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

#ifdef STYLE_BLUDE
const char *NewFileIcon32 = ":/Resources/edit/styleblue/newfile.png";
const char *OpenFileIcon32 = ":/Resources/edit/styleblue/openfile.png";
const char *NeedSaveBarIcon32 = ":/Resources/edit/styleblue/needsavebar.png";
const char *NoNeedSaveBarIcon32 = ":/Resources/edit/styleblue/needsavebar.png";
const char *NeedSaveAllBarIcon32 = ":/Resources/edit/styleblue/needsaveall.png";
const char *NoNeedSaveAllBarIcon32 = ":/Resources/edit/styleblue/needsaveall.png";
const char* AutoTimeSaveBarIcon32 = ":/Resources/edit/styleblue/autosave.png";
const char *CloseFileIcon32 = ":/Resources/edit/styleblue/closefile.png";
const char *CloseAllFileIcon32 = ":/Resources/edit/styleblue/closeall.png";
const char *CutIcon32 = ":/Resources/edit/styleblue/cut.png";
const char *CopyFileIcon32 = ":/Resources/edit/styleblue/copy.png";
const char *PasteIcon32 = ":/Resources/edit/styleblue/paste.png";
const char *UndoIcon32 = ":/Resources/edit/styleblue/undo.png";
const char *RedoIcon32 = ":/Resources/edit/styleblue/redo.png";
const char *FindIcon32 = ":/Resources/edit/styleblue/find.png";
const char *ReplaceIcon32 = ":/Resources/edit/styleblue/replace.png";
const char* MarkIcon32 = ":/Resources/edit/styleblue/mark.png";
const char* SignIcon32 = ":/Resources/edit/styleblue/sign.png";
const char* ClearSignIcon32 = ":/Resources/edit/styleblue/clearsign.png";
const char *ZoominIcon32 = ":/Resources/edit/styleblue/zoomin.png";
const char *ZoomoutIcon32 = ":/Resources/edit/styleblue/zoomout.png";
const char *CrlfIcon32 = ":/Resources/edit/styleblue/crlf.png";
const char *WhiteIcon32 = ":/Resources/edit/styleblue/white.png";
const char *IndentIcon32 = ":/Resources/edit/styleblue/indentGuide.png";
const char *FileCompareIcon32 = ":/Resources/edit/styleblue/filecompare.png";
const char *DirCompareIcon32 = ":/Resources/edit/styleblue/dircompare.png";
const char *BinCmpIcon32 = ":/Resources/edit/styleblue/bincmp.png";
const char *TransCodeIcon32 = ":/Resources/edit/styleblue/transcode.png";
const char *RenameIcon32 = ":/Resources/edit/styleblue/rename.png";
const char *PreHexIcon32 = ":/Resources/edit/styleblue/pre.png";
const char *NextHexIcon32 = ":/Resources/edit/styleblue/next.png";
const char *GotoHexIcon32 = ":/Resources/edit/styleblue/goto.png";
const char *TabNeedSave32 = ":/Resources/edit/global/needsave.png";
const char *TabNoNeedSave32 = ":/Resources/edit/global/noneedsave.png";
#endif

const char *NewFileIcon32 = ":/Resources/edit/styledeepblue/newfile.png";
const char *OpenFileIcon32 = ":/Resources/edit/styledeepblue/openfile.png";
const char *NeedSaveBarIcon32 = ":/Resources/edit/styledeepblue/needsavebar.png";
const char *NoNeedSaveBarIcon32 = ":/Resources/edit/styledeepblue/needsavebar.png";
const char *NeedSaveAllBarIcon32 = ":/Resources/edit/styledeepblue/needsaveall.png";
const char *NoNeedSaveAllBarIcon32 = ":/Resources/edit/styledeepblue/needsaveall.png";
const char* AutoTimeSaveBarIcon32 = ":/Resources/edit/styledeepblue/autosave.png";
const char *CloseFileIcon32 = ":/Resources/edit/styledeepblue/closefile.png";
const char *CloseAllFileIcon32 = ":/Resources/edit/styledeepblue/closeall.png";
const char *CutIcon32 = ":/Resources/edit/styledeepblue/cut.png";
const char *CopyFileIcon32 = ":/Resources/edit/styledeepblue/copy.png";
const char *PasteIcon32 = ":/Resources/edit/styledeepblue/paste.png";
const char *UndoIcon32 = ":/Resources/edit/styledeepblue/undo.png";
const char *RedoIcon32 = ":/Resources/edit/styledeepblue/redo.png";
const char *FindIcon32 = ":/Resources/edit/styledeepblue/find.png";
const char *ReplaceIcon32 = ":/Resources/edit/styledeepblue/replace.png";
const char* MarkIcon32 = ":/Resources/edit/styledeepblue/mark.png";
const char* SignIcon32 = ":/Resources/edit/styledeepblue/sign.png";
const char* ClearSignIcon32 = ":/Resources/edit/styledeepblue/clearsign.png";
const char *ZoominIcon32 = ":/Resources/edit/styledeepblue/zoomin.png";
const char *ZoomoutIcon32 = ":/Resources/edit/styledeepblue/zoomout.png";
const char *CrlfIcon32 = ":/Resources/edit/styledeepblue/crlf.png";
const char *WhiteIcon32 = ":/Resources/edit/styledeepblue/white.png";
const char *IndentIcon32 = ":/Resources/edit/styledeepblue/indentGuide.png";
const char *FileCompareIcon32 = ":/Resources/edit/styledeepblue/filecompare.png";
const char *DirCompareIcon32 = ":/Resources/edit/styledeepblue/dircompare.png";
const char *BinCmpIcon32 = ":/Resources/edit/styledeepblue/bincmp.png";
const char *TransCodeIcon32 = ":/Resources/edit/styledeepblue/transcode.png";
const char *RenameIcon32 = ":/Resources/edit/styledeepblue/rename.png";
const char *PreHexIcon32 = ":/Resources/edit/styledeepblue/pre.png";
const char *NextHexIcon32 = ":/Resources/edit/styledeepblue/next.png";
const char *GotoHexIcon32 = ":/Resources/edit/styledeepblue/goto.png";
//const char *TabNeedSave32 = ":/Resources/edit/global/needsave.png";
//const char *TabNoNeedSave32 = ":/Resources/edit/global/noneedsave.png";

#if 0
const char *NewFileIconDark32 = ":/Resources/edit/styledark/newfile.png";
const char *OpenFileIconDark32 = ":/Resources/edit/styledark/openfile.png";
const char *NeedSaveBarIconDark32 = ":/Resources/edit/styledark/needsavebar.png";
const char *NoNeedSaveBarIconDark32 = ":/Resources/edit/styledark/needsavebar.png";
const char *NeedSaveAllBarIconDark32 = ":/Resources/edit/styledark/needsaveall.png";
const char *NoNeedSaveAllBarIconDark32 = ":/Resources/edit/styledark/needsaveall.png";
const char* AutoTimeSaveBarIconDark32 = ":/Resources/edit/styledark/autosave.png";
const char *CloseFileIconDark32 = ":/Resources/edit/styledark/closefile.png";
const char *CloseAllFileIconDark32 = ":/Resources/edit/styledark/closeall.png";
const char *CutIconDark32 = ":/Resources/edit/styledark/cut.png";
const char *CopyFileIconDark32 = ":/Resources/edit/styledark/copy.png";
const char *PasteIconDark32 = ":/Resources/edit/styledark/paste.png";
const char *UndoIconDark32 = ":/Resources/edit/styledark/undo.png";
const char *RedoIconDark32 = ":/Resources/edit/styledark/redo.png";
const char *FindIconDark32 = ":/Resources/edit/styledark/find.png";
const char *ReplaceIconDark32 = ":/Resources/edit/styledark/replace.png";
const char* MarkIconDark32 = ":/Resources/edit/styledark/mark.png";
const char* SignIconDark32 = ":/Resources/edit/styledark/sign.png";
const char* ClearSignIconDark32 = ":/Resources/edit/styledark/clearsign.png";
const char *ZoominIconDark32 = ":/Resources/edit/styledark/zoomin.png";
const char *ZoomoutIconDark32 = ":/Resources/edit/styledark/zoomout.png";
const char *CrlfIconDark32 = ":/Resources/edit/styledark/crlf.png";
const char *WhiteIconDark32 = ":/Resources/edit/styledark/white.png";
const char *IndentIconDark32 = ":/Resources/edit/styledark/indentGuide.png";
const char *FileCompareIconDark32 = ":/Resources/edit/styledark/filecompare.png";
const char *DirCompareIconDark32 = ":/Resources/edit/styledark/dircompare.png";
const char *BinCmpIconDark32 = ":/Resources/edit/styledark/bincmp.png";
const char *TransCodeIconDark32 = ":/Resources/edit/styledark/transcode.png";
const char *RenameIconDark32 = ":/Resources/edit/styledark/rename.png";
const char *PreHexIconDark32 = ":/Resources/edit/styledark/pre.png";
const char *NextHexIconDark32 = ":/Resources/edit/styledark/next.png";
const char *GotoHexIconDark32 = ":/Resources/edit/styledark/goto.png";
const char *TabNeedSaveDark32 = ":/Resources/edit/global/needsave.png";
const char *TabNoNeedSaveDark32 = ":/Resources/edit/global/noneedsavedark.png";
#endif

const char *TabNeedSaveDark32 = ":/notepad/needsave.png";
const char *TabNoNeedSaveDark32 = ":/notepad/noneedsave.png";


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
//static QMap<QString, int> s_fileTypeToLangMap; //使用ExtLexerManager进行了替换

QStringList CCNotePad::s_findHistroy;

int CCNotePad::s_autoWarp = 0; //自动换行
int CCNotePad::s_indent = 0; //自动缩进
int CCNotePad::s_showblank = 0; //显示空白
int CCNotePad::s_restoreLastFile = 1;//自动恢复上次打开的文件
int CCNotePad::s_curStyleId = 0;
int CCNotePad::s_curMarkColorId = SCE_UNIVERSAL_FOUND_STYLE_EXT5;
int CCNotePad::s_hightWebAddr = 0;

//lexerName to index


//这里是静态的默认文件后缀类型与词法类型。还有一个动态的，用来管理用户新增语言的部分
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
{QString("nsh"), L_NSIS},
{QString("v"), L_VERILOG},
{QString("rs"), L_RUST},
{QString("frm"), L_VB},
{QString("NULL"), L_EXTERNAL},
};

RC_LINE_FORM getLineEndTypeFromBigText(QString& text)
{
	for (int i = 0, s = text.size(); i < s; ++i)
	{
		if (text.at(i) == '\n' && ((i > 1) && text.at(i - 1) == '\r'))
		{
			return DOS_LINE;
		}
		else if (text.at(i) == '\n' && ((i > 1) && text.at(i - 1) != '\r'))
		{
			return UNIX_LINE;
		}
		else if (text.at(i) == '\r' && ((i != (s - 1)) && text.at(i + 1) != '\n'))
		{
			return MAC_LINE;
		}
	}
	//默认windws
	return DOS_LINE;
}

//根据文件的后缀来确定文件的编程语言，进而设置默认的LEXER
void initFileTypeLangMap()
{
	if (0 == ExtLexerManager::getInstance()->size())
	{
		//先加载静态的关联文件后缀
		for (int i = 0; i < FileExtMapLexerIdLen; ++i)
		{
			if (s_fileExtMapLexerId[i].id == L_EXTERNAL)
			{
				break;
			}
			else
			{
				FileExtLexer& v = s_fileExtMapLexerId[i];

				//标准的定义可以忽略后面的tag,因为标准lexer的tag都是存在的。
				ExtLexerManager::getInstance()->addNewExtType(v.ext, v.id, ScintillaEditView::getTagByLexerId(v.id));
			}
		}
		//在加载动态的关联部分，这部分是用户自定义的类型。这里最好不要放在多个文件，否则会慢，单独放一个文件即可。
		//把新语言tagName,和关联ext单独存放起来ext_tag.ini。只读取一个文件就能获取所有，避免遍历慢
		QString extsFile = QString("notepad/userlang/ext_tag");//ext_tag是存在所有tag ext的文件
		QSettings qs(QSettings::IniFormat, QSettings::UserScope, extsFile);
		qs.setIniCodec("UTF-8");

		QStringList keys = qs.allKeys();
		//LangType lexId = L_USER_TXT;
		bool ok = true;
		QString tagName;
		LangType lexerId;

		for (int i = 0, s = keys.size(); i < s; ++i)
		{
			const QString& tagName = keys.at(i);

			QStringList exts = qs.value(tagName).toStringList();
			lexerId = (LangType)exts.takeLast().toInt(&ok);
			QString ext;
			if (ok)
			{
				foreach(ext, exts)
				{

					ExtLexerManager::getInstance()->addNewExtType(ext, lexerId, tagName);
	}
}
		}

		//最后加载用户自定义的文件后缀名和语法关联文件
		LangExtSet::loadExtRelevanceToMagr();
	}
}

void  CCNotePad::initLexerNameToIndex()
{
	if (m_lexerNameToIndex.isEmpty())
	{
		LexerNode* pNodes = new LexerNode[100];

		int i = 0;

		//pNodes[i].pAct = ui.actionAVS;
		//pNodes[i].index = L_AVS;
		QVariant data((int)L_AVS);
		//ui.actionAVS->setData(data);
		//m_lexerNameToIndex.insert("avs", pNodes[i]);
  //      ++i;

		pNodes[i].pAct = ui.actionAssembly;
		pNodes[i].index = L_ASM;
		data.setValue(int(L_ASM));
		ui.actionAssembly->setData(data);
		m_lexerNameToIndex.insert("asm", pNodes[i]);
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

		pNodes[i].pAct = ui.actionR_2;
		pNodes[i].index = L_R;
		data.setValue(int(L_R));
		ui.actionR_2->setData(data);
		m_lexerNameToIndex.insert("r", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionRuby;
		pNodes[i].index = L_RUBY;
		data.setValue(int(L_RUBY));
		ui.actionRuby->setData(data);
		m_lexerNameToIndex.insert("ruby", pNodes[i]);
		++i;

		pNodes[i].pAct = ui.actionRust;
		pNodes[i].index = L_RUST;
		data.setValue(int(L_RUST));
		ui.actionRust->setData(data);
		m_lexerNameToIndex.insert("rust", pNodes[i]);
		++i;
		

		pNodes[i].pAct = ui.actionShell;
		pNodes[i].index = L_BASH;
		data.setValue(int(L_BASH));
		ui.actionShell->setData(data);
		m_lexerNameToIndex.insert("shell", pNodes[i]);
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

		pNodes[i].pAct = ui.actionVb;
		pNodes[i].index = L_VB;
		data.setValue(int(L_VB));
		ui.actionVb->setData(data);
		m_lexerNameToIndex.insert("vb", pNodes[i]);
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

		pNodes[i].pAct = ui.actionUserDefine;
		pNodes[i].index = L_USER_DEFINE;
		data.setValue(int(L_USER_DEFINE));
		ui.actionUserDefine->setData(data);
		m_lexerNameToIndex.insert("UserDefine", pNodes[i]);
		++i;
		delete[]pNodes;

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

//需要临时写一些文件，保存在该目录中。目前就是管理员提权时，保存之前的文件。
QString getGlboalTempSaveDir()
{
	//就是app/notepad/temp的目录
	QString tempFileList = QString("notepad/temp/list");
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, tempFileList);
	QString qsSavePath = qs.fileName();
	QFileInfo fi(qsSavePath);
	return fi.dir().absolutePath();
}

void setEditShowBlankStatus(ScintillaEditView* pEdit, int blankSet)
{
	if (blankSet == 1)
	{
		pEdit->setWhitespaceVisibility(QsciScintilla::WsVisible);
		pEdit->setEolVisibility(false);
	}
	else if(blankSet == 2)
	{
		pEdit->setWhitespaceVisibility(QsciScintilla::WsInvisible);
		pEdit->setEolVisibility(true);
	}
	else if (blankSet == 3)
	{
		pEdit->setWhitespaceVisibility(QsciScintilla::WsVisible);
		pEdit->setEolVisibility(true);
	}
	else if (blankSet == 0)
	{
		pEdit->setWhitespaceVisibility(QsciScintilla::WsInvisible);
		pEdit->setEolVisibility(false);
	}
}

//根据文件类型给出语言id
LexerInfo CCNotePad::getLangLexerIdByFileExt(QString filePath)
{
	QFileInfo fi(filePath);
	QString ext = fi.suffix();

	LexerInfo lexer(L_TXT,"txt");

	if(ExtLexerManager::getInstance()->getLexerTypeByExt(ext, lexer))
	{
		return lexer;
	}

	return lexer;
}

CCNotePad::CCNotePad(bool isMainWindows, QWidget *parent)
	: QMainWindow(parent), m_cutFile(nullptr),m_copyFile(nullptr), m_dockSelectTreeWin(nullptr), \
	m_pResultWin(nullptr),m_isQuitCancel(false), m_tabRightClickMenu(nullptr), m_shareMem(nullptr),m_isMainWindows(isMainWindows),\
	m_openInNewWinAct(nullptr), m_showFileDirAct(nullptr), m_timerAutoSave(nullptr), m_curColorIndex(-1), m_fileListView(nullptr), m_isInReloadFile(false), m_isToolMenuLoaded(false)
{
	ui.setupUi(this);

#ifdef Q_OS_MAC
    setWindowIcon(QIcon(":/mac.icns"));
#endif

	NddSetting::init();

	m_translator = new QTranslator(this);

		m_curSoftLangs = NddSetting::getKeyValueFromNumSets(LANGS_KEY);

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
		
#if 0
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
	ui.editTabWidget->installEventFilter(this);

	m_fileWatch = new QFileSystemWatcher(this);
	connect(m_fileWatch,&QFileSystemWatcher::fileChanged,this, &CCNotePad::slot_fileChange);


	//只有主窗口才监控openwith的文件
	if (isMainWindows)
	{
		initNotePadSqlOptions();
	}

	slot_loadBookMarkMenu();

	QByteArray lastGeo = NddSetting::getKeyByteArrayValue(WIN_POS);

	if (!lastGeo.isEmpty())
	{
		restoreGeometry(lastGeo);
	}
#endif
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
	NddSetting::close();
}

//先快速让窗口展示处理，后续再去做复杂的初始化
void CCNotePad::quickshow()
{
	QByteArray lastGeo = NddSetting::getKeyByteArrayValue(WIN_POS);

	if (!lastGeo.isEmpty())
	{
		restoreGeometry(lastGeo);
	}

	show();

	QCoreApplication::processEvents();

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


	QTabBar* pBar = ui.editTabWidget->tabBar();

	connect(pBar, &QTabBar::tabCloseRequested, this, &CCNotePad::slot_tabClose);


	m_codeStatusLabel = new QLabel("UTF8", ui.statusBar);
	m_lineEndLabel = new QComboBox(ui.statusBar);
	m_lineEndLabel->addItems(QStringList() << "Windows(CR LF)" << "Unix(LF)" << "Mac(CR)");
#ifdef Q_OS_WIN
	m_lineEndLabel->setCurrentIndex(0);
#else
	m_lineEndLabel->setCurrentIndex(1);
#endif
	connect(m_lineEndLabel, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CCNotePad::on_lineEndChange);

	m_lineNumLabel = new QLabel(tr("Ln:0	Col:0"), ui.statusBar);

	m_langDescLabel = new QLabel("Txt", ui.statusBar);

	m_zoomLabel = new QLabel("Zoom", ui.statusBar);

	m_codeStatusLabel->setMinimumWidth(120);
	m_lineEndLabel->setMinimumWidth(100);
	m_lineNumLabel->setMinimumWidth(120);
	m_langDescLabel->setMinimumWidth(100);
	m_zoomLabel->setMinimumWidth(100);

	//0在前面，越小越在左边
	ui.statusBar->insertPermanentWidget(0, m_zoomLabel);
	ui.statusBar->insertPermanentWidget(1, m_langDescLabel);
	ui.statusBar->insertPermanentWidget(2, m_lineNumLabel);
	ui.statusBar->insertPermanentWidget(3, m_lineEndLabel);
	ui.statusBar->insertPermanentWidget(4, m_codeStatusLabel);
	

	initToolBar();

	m_saveFile->setEnabled(false);
	m_saveAllFile->setEnabled(false);

	initReceneOpenFileMenu();

	//最后加入退出菜单
	ui.menuFile->addSeparator();
	m_quitAction = ui.menuFile->addAction(tr("Quit"), this, &CCNotePad::slot_quit);
	m_quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

	connect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged, Qt::UniqueConnection);
	connect(ui.editTabWidget, &QTabWidget::tabBarClicked, this, &CCNotePad::slot_tabBarClicked, Qt::QueuedConnection);
	ui.editTabWidget->installEventFilter(this);

	m_fileWatch = new QFileSystemWatcher(this);
	connect(m_fileWatch, &QFileSystemWatcher::fileChanged, this, &CCNotePad::slot_fileChange);


	//只有主窗口才监控openwith的文件
	if (m_isMainWindows)
	{

		initNotePadSqlOptions();

		


	}

	m_isInitBookMarkAct = false;

	slot_loadBookMarkMenu();

	slot_loadMarkColor();

	m_isToolMenuLoaded = false;

	init_toolsMenu();

	this->setContextMenuPolicy(Qt::NoContextMenu);
	
	//恢复文件列表
	if (1 == NddSetting::getKeyValueFromNumSets(FILELISTSHOW))
	{
		initFileListDockWin();
	}

	//隐藏工具栏
	if (0 == NddSetting::getKeyValueFromNumSets(TOOLBARSHOW))
	{
		ui.mainToolBar->setVisible(false);
		ui.actionShow_ToolBar->setChecked(false);
	}
	//高亮web地址。默认为0不高亮
	if (1 == NddSetting::getKeyValueFromNumSets(SHOWWEBADDR))
	{
		s_hightWebAddr = 1;
		ui.actionShow_Web_Addr->setChecked(true);
	}

	//恢复用户自定义快捷键
	setUserDefShortcutKey();
}

void CCNotePad::on_lineEndChange(int index)
{

	if (index == 0)
	{
		if (convertDocLineEnd(DOS_LINE))
		{
			ui.actionconver_windows_CR_LF->setChecked(true);
		}
	}
	else if (index == 1)
	{
		if (convertDocLineEnd(UNIX_LINE))
		{
			ui.actionconvert_Unix_LF->setChecked(true);
		}
	}
	else if (index == 2)
	{
		if (convertDocLineEnd(MAC_LINE))
		{
			ui.actionconvert_Mac_CR->setChecked(true);
		}
	}
}

void CCNotePad::setUserDefShortcutKey(int shortcutId)
{
	QKeySequence keySeq;

	switch (shortcutId)
	{
	case New_File_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(New_File);
			ui.actionNewFile->setShortcut(keySeq);

		break;
	case Open_File_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Open_File);
			ui.actionOpenFile->setShortcut(keySeq);
		
		break;
	case Save_File_ID:
		break;
	case Save_All_File_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Save_All_File);
			ui.actionSave_as->setShortcut(keySeq);
		
		break;
	case Close_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Close);
			ui.actionClose->setShortcut(keySeq);
		
		break;
	case Close_All_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Close_All);
			ui.actionClose_All->setShortcut(keySeq);
		
		break;
	case Cut_ID:
		break;
	case Copy_ID:
		break;
	case Paste_ID:
		break;
	case Undo_ID:
		break;
	case Redo_ID:
		break;
	case Find_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Find);
			ui.actionFind->setShortcut(keySeq);
		
		break;
	case Replace_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Replace);
			ui.actionReplace->setShortcut(keySeq);
		
		break;
	case Dir_Find_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(DirFind);
			ui.actionFind_In_Dir->setShortcut(keySeq);
		
		break;
	case Mark_ID:
		
		break;
	case Word_highlight_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Word_highlight);
			m_signText->setShortcut(keySeq);
		
		break;
	case Clear_all_highlight_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Clear_all_highlight);
			m_clearMark->setShortcut(keySeq);
		
		break;
	case Zoom_In_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Zoom_In);
			m_zoomin->setShortcut(keySeq);
		
		break;
	case Zoom_Out_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Zoom_Out);
			m_zoomout->setShortcut(keySeq);
		
		break;
	case Word_Wrap_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Word_Wrap);
			ui.actionWrap->setShortcut(keySeq);
		
		break;
	case Show_Blank_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Show_Blank);
			ui.actionShowAll->setShortcut(keySeq);
		
		break;
	case Indent_Guide_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Indent_Guide);
			m_indentGuide->setShortcut(keySeq);
		
		break;
	case Pre_Page_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Pre_Page);
			m_preHexPage->setShortcut(keySeq);
		
		break;
	case Next_Page_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Next_Page);
			m_nextHexPage->setShortcut(keySeq);
		
		break;
	case Goto_Page_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Goto_Page);
			ui.actionGoline->setShortcut(keySeq);
		
		break;

	case File_Compare_ID:
		break;
	case Dir_Compare_ID:
		break;
	case Bin_Compare_ID:
		break;

	case Trans_code_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Trans_code);
			m_transcode->setShortcut(keySeq);
		
		break;
	case Batch_rename_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Batch_rename);
			m_rename->setShortcut(keySeq);
		
		break;

	case Format_Xml_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Format_Xml);
			m_formatXml->setShortcut(keySeq);
		
		break;

	case Format_Json_ID:
		keySeq = ShortcutKeyMgr::getUserDefShortcutKey(Format_Json);
			m_formatJson->setShortcut(keySeq);
		
		break;

	default:
		break;
	}
}
//设置用户自定义快捷键
void CCNotePad::setUserDefShortcutKey()
{
	ShortcutKeyMgr::initShortcutKeysMap();

	for (int i = New_File_ID; i < Shortcut_End_ID; ++i)
	{
		setUserDefShortcutKey(i);
	}
}

void CCNotePad::init_toolsMenu()
{
	slot_dynamicLoadToolMenu();
	//connect(ui.menuTools,&QMenu::aboutToShow,this,&CCNotePad::slot_dynamicLoadToolMenu);
}

enum ToolMenuAct {
	BATCH_FIND = 1,
};
//动态加载工具菜单项
void CCNotePad::slot_dynamicLoadToolMenu()
{
	if (!m_isToolMenuLoaded)
	{
		m_isToolMenuLoaded = true;

#ifdef NO_PLUGIN
		connect(ui.actionPlugin_Manager, &QAction::triggered, this, &CCNotePad::slot_pluginMgr);
#endif

		QMenu* formatMenu = new QMenu(tr("Format Language"), this);
		m_formatXml = formatMenu->addAction(tr("Format Xml"), this, &CCNotePad::slot_formatXml);
		m_formatJson = formatMenu->addAction(tr("Format Json"), this, &CCNotePad::slot_formatJson);
		ui.menuTools->addMenu(formatMenu);

		QAction* pAct = nullptr;
		pAct = ui.menuTools->addAction(tr("Batch Find"), this, &CCNotePad::slot_batchFind);
		pAct->setData(BATCH_FIND);

#ifdef NO_PLUGIN
		//动态加载插件
		loadPluginLib();
#endif
	}
}

#ifdef NO_PLUGIN
void  CCNotePad::slot_pluginMgr()
{
	PluginMgr* pWin = new PluginMgr(this, m_pluginList);
	pWin->setAttribute(Qt::WA_DeleteOnClose);
	pWin->show();
}


void CCNotePad::loadPluginLib()
{
	QString strDir = qApp->applicationDirPath();
	QDir dir(strDir);
	if (dir.cd("./plugin"))
	{
		strDir = dir.absolutePath();

		loadPluginProcs(strDir,ui.menuPlugin);
	}
}

void CCNotePad::onPlugFound(NDD_PROC_DATA& procData, QMenu* pUserData)
{
	QMenu* pMenu = pUserData;

	if (pMenu == NULL)
	{
		return;
	}

	//创建action
	if (procData.m_menuType == 0)
	{
		QAction* pAction = new QAction(procData.m_strPlugName, pMenu);
		pMenu->addAction(pAction);
	pAction->setText(procData.m_strPlugName);
	pAction->setData(procData.m_strFilePath);
	connect(pAction, &QAction::triggered, this, &CCNotePad::onPlugWork);
	}
	else if (procData.m_menuType == 1)
	{
		//创建二级菜单
		QMenu* pluginMenu = new QMenu(procData.m_strPlugName, pMenu);
		pMenu->addMenu(pluginMenu);

		//菜单句柄通过procData传递到插件中
		procData.m_rootMenu = pluginMenu;
		sendParaToPlugin(procData);
	}
	else
	{
		return;
	}

	

	m_pluginList.append(procData);
}

//真正执行插件的工作
void CCNotePad::onPlugWork(bool check)
{
	QAction* pAct = dynamic_cast<QAction*>(sender());
	if (pAct != nullptr)
	{
		QString plugPath = pAct->data().toString();

		QLibrary* pLib = new QLibrary(plugPath);

		NDD_PROC_MAIN_CALLBACK pMainCallBack;
		pMainCallBack = (NDD_PROC_MAIN_CALLBACK)pLib->resolve("NDD_PROC_MAIN");

		if (pMainCallBack != NULL)
		{
			std::function<QsciScintilla* ()> foundCallBack = std::bind(&CCNotePad::getCurEditView, this);

			pMainCallBack(this, plugPath, foundCallBack, nullptr);
		}
		else
		{
			ui.statusBar->showMessage(tr("plugin %1 load failed !").arg(plugPath), 10000);
		}
	
	}
}

//把插件需要的参数，传递到插件中去
void CCNotePad::sendParaToPlugin(NDD_PROC_DATA& procData)
{
	QString plugPath = procData.m_strFilePath;

	QLibrary* pLib = new QLibrary(plugPath);

	NDD_PROC_MAIN_CALLBACK pMainCallBack;
	pMainCallBack = (NDD_PROC_MAIN_CALLBACK)pLib->resolve("NDD_PROC_MAIN");

		if (pMainCallBack != NULL)
		{
			std::function<QsciScintilla* ()> foundCallBack = std::bind(&CCNotePad::getCurEditView, this);

			pMainCallBack(this, plugPath, foundCallBack, &procData);
		}
		else
		{
			ui.statusBar->showMessage(tr("plugin %1 load failed !").arg(plugPath), 10000);
		}
}

void CCNotePad::loadPluginProcs(QString strLibDir, QMenu* pMenu)
{
	std::function<void(NDD_PROC_DATA&, QMenu*)> foundCallBack = std::bind(&CCNotePad::onPlugFound, this, std::placeholders::_1, std::placeholders::_2);

	int nRet = loadProc(strLibDir, foundCallBack, pMenu);
	if (nRet > 0)
	{
		ui.statusBar->showMessage(tr("load plugin in dir %1 success, plugin num %2").arg(strLibDir).arg(nRet));
	}
}
#endif

//批量查找替换
void CCNotePad::slot_batchFind()
{
#ifdef uos
	bool isPosAdjust = false;
#endif

	if (m_batchFindWin.isNull())
	{
		m_batchFindWin = new BatchFindReplace(this);
		m_batchFindWin->setAttribute(Qt::WA_DeleteOnClose);

		BatchFindReplace* pWin = dynamic_cast<BatchFindReplace*>(m_batchFindWin.data());
		pWin->setTabWidget(ui.editTabWidget);

		QByteArray lastGeo = NddSetting::getWinPos(BATCH_FIND_REPLACE_POS);
		if (!lastGeo.isEmpty())
		{
			m_batchFindWin->restoreGeometry(lastGeo);
#ifdef uos
			isPosAdjust = true;
#endif
	}

	}
	m_batchFindWin->show();

#ifdef uos
	if (!isPosAdjust)
	{
	adjustWInPos(m_batchFindWin);
	}
#endif 

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

enum BookMarkActId {
	SET_REMOVE=1,
	NEXT_MARK,
	PREV_MARK,
	CLEAR_MARK,
	CUT_MARK_LINE,
	COPY_MARK_LINE,
	PASTE_MARK_LINE,
	DELETE_MARK_LINE,
	DELETE_UNMARK,
	CLIP_MARK,
};
void CCNotePad::slot_bookMarkAction()
{
	QAction* pAct = dynamic_cast<QAction*>(sender());
	if (pAct != nullptr)
	{
		QWidget* pw = ui.editTabWidget->currentWidget();
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit == nullptr)
		{
			return;
		}

		BookMarkActId id = (BookMarkActId)pAct->data().toInt();
		switch (id)
		{
		case SET_REMOVE:
			pEdit->bookmarkToggle(-1);
			break;
		case NEXT_MARK:
			pEdit->bookmarkNext(true);
			break;
		case PREV_MARK:
			pEdit->bookmarkNext(false);
			break;
		case CLEAR_MARK:
			pEdit->bookmarkClearAll();
			break;
		case CUT_MARK_LINE:
			pEdit->cutMarkedLines();
			break;
		case COPY_MARK_LINE:
			pEdit->copyMarkedLines();
			break;
		case PASTE_MARK_LINE:
			pEdit->pasteToMarkedLines();
			break;
		case DELETE_MARK_LINE:
			pEdit->deleteMarkedLines(true);
			break;
		case DELETE_UNMARK:
			pEdit->deleteMarkedLines(false);
			break;
		case CLIP_MARK:
			pEdit->inverseMarks();
			break;
		default:
			break;
		}
	}
}
//动态加载书签的菜单项
void CCNotePad::slot_loadBookMarkMenu()
{

	if (!m_isInitBookMarkAct)
	{
		m_isInitBookMarkAct = true;

		QAction* pAct = nullptr;
		pAct = ui.menuBook_Mark->addAction(tr("Set/Remove BookMark"), this, &CCNotePad::slot_bookMarkAction, QKeySequence("Ctrl+F2"));
		pAct->setData(SET_REMOVE);

		pAct = ui.menuBook_Mark->addAction(tr("Next BookMark"), this, &CCNotePad::slot_bookMarkAction, QKeySequence("F2"));
		pAct->setData(NEXT_MARK);

		pAct = ui.menuBook_Mark->addAction(tr("Prev BookMark"), this, &CCNotePad::slot_bookMarkAction, QKeySequence("Shift+F2"));
		pAct->setData(PREV_MARK);

		pAct = ui.menuBook_Mark->addAction(tr("ClearAll BookMark"), this, &CCNotePad::slot_bookMarkAction);
		pAct->setData(CLEAR_MARK);

		pAct = ui.menuBook_Mark->addAction(tr("Cut BookMark Lines"), this, &CCNotePad::slot_bookMarkAction);
		pAct->setData(CUT_MARK_LINE);

		pAct = ui.menuBook_Mark->addAction(tr("Copy BookMark Lines"), this, &CCNotePad::slot_bookMarkAction);
		pAct->setData(COPY_MARK_LINE);

		pAct = ui.menuBook_Mark->addAction(tr("Paste BookMark Lines"), this, &CCNotePad::slot_bookMarkAction);
		pAct->setData(PASTE_MARK_LINE);

		pAct = ui.menuBook_Mark->addAction(tr("Delete BookMark Lines"), this, &CCNotePad::slot_bookMarkAction);
		pAct->setData(DELETE_MARK_LINE);

		pAct = ui.menuBook_Mark->addAction(tr("Delete UnBookMark Lines"), this, &CCNotePad::slot_bookMarkAction);
		pAct->setData(DELETE_UNMARK);

		pAct = ui.menuBook_Mark->addAction(tr("Clip BookMark"), this, &CCNotePad::slot_bookMarkAction);
		pAct->setData(CLIP_MARK);
	}
}

void CCNotePad::slot_markColorGroup(QAction *action)
{
	CCNotePad::s_curMarkColorId = action->data().toInt();
	slot_wordHighlight();
}

#define SCE_UNIVERSAL_FOUND_STYLE_START 20

//修改标记样式的颜色
void CCNotePad::changeMarkColor(int sytleId)
{
	if (sytleId < 5)
	{
		QPixmap colorBar(36, 36);
		colorBar.fill((&StyleSet::s_global_style->mark_style_1)[sytleId].bgColor);
		m_styleMarkActList.at(sytleId)->setIcon(colorBar);
	}
}


void CCNotePad::slot_loadMarkColor()
{
	if (m_curColorIndex == -1)
	{
		m_curColorIndex = 0;

		QPixmap colorBar(36, 36);

		QActionGroup* markColorGroup = new QActionGroup(this);
		connect(markColorGroup, &QActionGroup::triggered, this, &CCNotePad::slot_markColorGroup, Qt::QueuedConnection);

		int index = 1;
		auto initColorBar = [this, markColorGroup,&index](QPixmap& colorBar)->QAction* {
			QAction* action = new QAction(ui.menuMark_Color);
			action->setIcon(colorBar);
			action->setText(tr("Color %1").arg(index));
			action->setData(index+ SCE_UNIVERSAL_FOUND_STYLE_START);
			++index;
			ui.menuMark_Color->addAction(action);
			markColorGroup->addAction(action);
			return action;
		};

		m_styleMarkActList.clear();

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

	}
}

void CCNotePad::syncCurSkinToMenu(int id)
{
	s_curStyleId = id;
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
			NddSetting::updataKeyValueFromNumSets(LANGS_KEY, m_curSoftLangs);
	}

		//如果已经加载了，则冲加载，否则中英文切换不生效
		if (m_isToolMenuLoaded)
		{
			ui.menuTools->clear();
			m_isToolMenuLoaded = false;
			slot_dynamicLoadToolMenu();
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
		NddSetting::updataKeyValueFromNumSets(LANGS_KEY, m_curSoftLangs);
}

	//如果已经加载了，则冲加载，否则中英文切换不生效
	if (m_isToolMenuLoaded)
	{
		ui.menuTools->clear();
		m_isToolMenuLoaded = false;
		slot_dynamicLoadToolMenu();
}
}
#if 0
void CCNotePad::saveDefFont()
{
	if (s_txtFont.toString() != m_txtFontStr)
	{
		QString newFont = s_txtFont.toString();
		NddSetting::updataKeyValueFromSets(TXT_FONT, newFont);
	}

	if (s_proLangFont.toString() != m_proLangFontStr)
	{
		QString newFont = s_proLangFont.toString();
		NddSetting::updataKeyValueFromSets(PRO_LANG_FONT, newFont);
}
}
#endif

void CCNotePad::savePadUseTimes()
{
	QString key("padtimes");

	int times = NddSetting::getKeyValueFromNumSets(key);

	NddSetting::updataKeyValueFromNumSets(key, s_padTimes + times);
}

void CCNotePad::slot_searchResultShow()
{
	initFindResultDockWin();
		m_dockSelectTreeWin->show();
	}

//读取Sql的全局配置
void CCNotePad::initNotePadSqlOptions()
{
	//tab的长度，默认为4
	QString key("tablens");
	ScintillaEditView::s_tabLens = NddSetting::getKeyValueFromNumSets(key);

	//space replace tab空格替换tab，默认1
	QString key1("tabnouse");
	ScintillaEditView::s_noUseTab = (1 == NddSetting::getKeyValueFromNumSets(key1)) ? true : false;

	ScintillaEditView::s_bigTextSize = NddSetting::getKeyValueFromNumSets(MAX_BIG_TEXT);
	if (ScintillaEditView::s_bigTextSize < 50 || ScintillaEditView::s_bigTextSize > 300)
	{
		ScintillaEditView::s_bigTextSize = 100;
}

	s_restoreLastFile = NddSetting::getKeyValueFromNumSets(RESTORE_CLOSE_FILE);
}
//保存Sql的全局配置
void CCNotePad::saveNotePadSqlOptions()
{
	QString key("tablens");
	NddSetting::updataKeyValueFromNumSets(key, ScintillaEditView::s_tabLens);

	QString key1("tabnouse");
	NddSetting::updataKeyValueFromNumSets(key1, ScintillaEditView::s_noUseTab?1:0);

	NddSetting::updataKeyValueFromNumSets(MAX_BIG_TEXT, ScintillaEditView::s_bigTextSize);

	NddSetting::updataKeyValueFromNumSets(RESTORE_CLOSE_FILE, CCNotePad::s_restoreLastFile);
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

	int changeIndex = -1;

	for (int i = 0; i < ui.editTabWidget->count(); ++i)
	{
		pw = ui.editTabWidget->widget(i);
		if (pw != nullptr && (pw->property(Edit_View_FilePath) == filePath))
		{
				//这里只设置1个标志，下次获取焦点时，才判定询问是否需要重新加载
			pw->setProperty(Modify_Outside, QVariant(true));

			changeIndex = i;
				break;
			}
		}

	//如果就是当前文件，则直接弹窗提示
	if (ui.editTabWidget->currentIndex() == changeIndex)
	{
		checkRoladFile(dynamic_cast<ScintillaEditView*>(pw));
	}
}

void CCNotePad::slot_tabBarDoubleClicked(int index)
{
	if (-1 == index)
	{
		initTabNewOne();
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
		else if(BIG_TEXT_RO_TYPE == docType || SUPER_BIG_TEXT_RO_TYPE == docType)
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
	else if (Qt::MiddleButton == QGuiApplication::mouseButtons())
	{
		//关闭当前文件
		slot_tabClose(index);
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
	QString path;
	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr)
	{
		path = pw->property(Edit_View_FilePath).toString();
	}

	showFileInExplorer(path);
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
		pNewWin->quickshow();
		pNewWin->setShareMem(this->getShareMem());
		pNewWin->setAttribute(Qt::WA_DeleteOnClose);
		pNewWin->openFile(path);
		
#ifdef uos
    adjustWInPos(pNewWin);
#endif
		s_padInstances->append(pNewWin);
	}
}

//int defLexerId:如果失败，则按照该语法类型的id赋值；如果-1则不给与默认值。
//因为发现如果是新建的文件，而且手动设置了语法，其文件名还是*.txt，此时如果根据
//后缀名自动赋值与否，会导致手动设置的语法失效。
void CCNotePad::autoSetDocLexer(ScintillaEditView* pEdit, int defLexerId)
{
	QString filePath = pEdit->property(Edit_View_FilePath).toString();

	//OpenAttr openType = (OpenAttr)pEdit->property(Open_Attr).toInt();
	//if (OpenAttr::Text != openType && OpenAttr::BigTextReadOnly != openType)
	//{
	//	return;
	//}

	LexerInfo lxdata = getLangLexerIdByFileExt(filePath);

	QsciLexer* lexer = nullptr;

	//如果没有特殊语法，而且默认给与语法不是-1，则按照默认语法设置
	if (lxdata.lexerId == L_TXT && defLexerId != -1)
	{
		lexer = ScintillaEditView::createLexer(defLexerId);
	}
	else
	{
		lexer = ScintillaEditView::createLexer(lxdata.lexerId, lxdata.tagName);
	}

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
		QString filePath = getFilePathProperty(pw);
		//16进制的处理逻辑
		int docType = getDocTypeProperty(pw);
		if (HEX_TYPE == docType)
		{
			
			setWindowTitleMode(filePath, OpenAttr::HexReadOnly);
			fileListSetCurItem(filePath);
			return;
		}
		else if ((TXT_TYPE == docType)||(BIG_TEXT_RO_TYPE == docType)||(SUPER_BIG_TEXT_RO_TYPE == docType))
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
				setWindowTitleMode(filePath, (OpenAttr)pw->property(Open_Attr).toInt());
			}
			else if (BIG_TEXT_RO_TYPE == docType)
			{
				//setWindowTitle(QString("%1 (%2)").arg(pw->property(Edit_View_FilePath).toString()).arg(tr("Big Text File ReadOnly")));
				setWindowTitleMode(filePath, OpenAttr::BigTextReadOnly);
			}
			else if (SUPER_BIG_TEXT_RO_TYPE == docType)
			{
				//setWindowTitle(QString("%1 (%2)").arg(pw->property(Edit_View_FilePath).toString()).arg(tr("Big Text File ReadOnly")));
				setWindowTitleMode(filePath, OpenAttr::SuperBigTextReadOnly);
			}
			syncCurDocEncodeToMenu(pw);
			syncCurDocLineEndStatusToMenu(pw);
			syncCurDocLexerToMenu(pw);

			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

			//目前只用了0 2 两种换行模式。
			if (s_autoWarp != pEdit->wrapMode())
			{
				pEdit->setWrapMode((s_autoWarp == 0 ? QsciScintilla::WrapNone: QsciScintilla::WrapCharacter));
			}
			pEdit->viewport()->setFocus();

			fileListSetCurItem(filePath);
			}
			}
		}

//快捷按钮的初始化
void CCNotePad::setShoctIcon(int iconSize)
{
#if 0
	auto setDark32Icon = [this]()
	{
		//黑色图标
		m_newFile->setIcon(QIcon(NewFileIconDark32));
		m_openFile->setIcon(QIcon(OpenFileIconDark32));
		m_saveFile->setIcon(QIcon(NoNeedSaveBarIconDark32));
		m_saveAllFile->setIcon(QIcon(NoNeedSaveAllBarIconDark32));
		m_autoSaveAFile->setIcon(QIcon(AutoTimeSaveBarIconDark32));
		m_closeFile->setIcon(QIcon(CloseFileIconDark32));
		m_closeAllFile->setIcon(QIcon(CloseAllFileIconDark32));
		m_cutFile->setIcon(QIcon(CutIconDark32));
		m_copyFile->setIcon(QIcon(CopyFileIconDark32));
		m_pasteFile->setIcon(QIcon(PasteIconDark32));
		m_undo->setIcon(QIcon(UndoIconDark32));
		m_redo->setIcon(QIcon(RedoIconDark32));
		m_findText->setIcon(QIcon(FindIconDark32));
		m_replaceText->setIcon(QIcon(ReplaceIconDark32));
		m_markText->setIcon(QIcon(MarkIconDark32));
		m_signText->setIcon(QIcon(SignIconDark32));
		m_clearMark->setIcon(QIcon(ClearSignIconDark32));
		m_zoomin->setIcon(QIcon(ZoominIconDark32));
		m_zoomout->setIcon(QIcon(ZoomoutIconDark32));
		m_wordwrap->setIcon(QIcon(CrlfIconDark32));
		m_allWhite->setIcon(QIcon(WhiteIconDark32));
		m_indentGuide->setIcon(QIcon(IndentIconDark32));
		m_preHexPage->setIcon(QIcon(PreHexIconDark32));
		m_nextHexPage->setIcon(QIcon(NextHexIconDark32));
		m_gotoHexPage->setIcon(QIcon(GotoHexIconDark32));
		m_fileCompare->setIcon(QIcon(FileCompareIconDark32));
		m_dirCompare->setIcon(QIcon(DirCompareIconDark32));
		m_binCompare->setIcon(QIcon(BinCmpIconDark32));
		m_transcode->setIcon(QIcon(TransCodeIconDark32));
		m_rename->setIcon(QIcon(RenameIconDark32));
	};

	if (iconSize < 48)
	{
		if (iconSize == 36 && StyleSet::getCurrentSytleId() == DEEP_BLACK)
		{
			setDark32Icon();
}
		else
		{
		m_newFile->setIcon(QIcon(NewFileIcon));
		m_openFile->setIcon(QIcon(OpenFileIcon));
		m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon));
		m_saveAllFile->setIcon(QIcon(NoNeedSaveAllBarIcon));
		m_autoSaveAFile->setIcon(QIcon(AutoTimeSaveBarIcon));
		m_closeFile->setIcon(QIcon(CloseFileIcon));
		m_closeAllFile->setIcon(QIcon(CloseAllFileIcon));
		m_cutFile->setIcon(QIcon(CutIcon));
		m_copyFile->setIcon(QIcon(CopyFileIcon));
		m_pasteFile->setIcon(QIcon(PasteIcon));
		m_undo->setIcon(QIcon(UndoIcon));
		m_redo->setIcon(QIcon(RedoIcon));
		m_findText->setIcon(QIcon(FindIcon));
		m_replaceText->setIcon(QIcon(ReplaceIcon));
		m_markText->setIcon(QIcon(MarkIcon));
		m_signText->setIcon(QIcon(SignIcon));
		m_clearMark->setIcon(QIcon(ClearSignIcon));
		m_zoomin->setIcon(QIcon(ZoominIcon));
		m_zoomout->setIcon(QIcon(ZoomoutIcon));
		m_wordwrap->setIcon(QIcon(CrlfIcon));
		m_allWhite->setIcon(QIcon(WhiteIcon));
		m_indentGuide->setIcon(QIcon(IndentIcon));
		m_preHexPage->setIcon(QIcon(PreHexIcon));
		m_nextHexPage->setIcon(QIcon(NextHexIcon));
		m_gotoHexPage->setIcon(QIcon(GotoHexIcon));
		m_fileCompare->setIcon(QIcon(FileCompareIcon));
		m_dirCompare->setIcon(QIcon(DirCompareIcon));
		m_binCompare->setIcon(QIcon(BinCmpIcon));
		m_transcode->setIcon(QIcon(TransCodeIcon));
		m_rename->setIcon(QIcon(RenameIcon));
	}
	}
	else
	{
		if (StyleSet::getCurrentSytleId() != DEEP_BLACK)
		{
		m_newFile->setIcon(QIcon(NewFileIcon32));
		m_openFile->setIcon(QIcon(OpenFileIcon32));
		m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon32));
		m_saveAllFile->setIcon(QIcon(NoNeedSaveAllBarIcon32));
		m_autoSaveAFile->setIcon(QIcon(AutoTimeSaveBarIcon32));
		m_closeFile->setIcon(QIcon(CloseFileIcon32));
		m_closeAllFile->setIcon(QIcon(CloseAllFileIcon32));
		m_cutFile->setIcon(QIcon(CutIcon32));
		m_copyFile->setIcon(QIcon(CopyFileIcon32));
		m_pasteFile->setIcon(QIcon(PasteIcon32));
		m_undo->setIcon(QIcon(UndoIcon32));
		m_redo->setIcon(QIcon(RedoIcon32));
		m_findText->setIcon(QIcon(FindIcon32));
		m_replaceText->setIcon(QIcon(ReplaceIcon32));
		m_markText->setIcon(QIcon(MarkIcon32));
		m_signText->setIcon(QIcon(SignIcon32));
		m_clearMark->setIcon(QIcon(ClearSignIcon32));
		m_zoomin->setIcon(QIcon(ZoominIcon32));
		m_zoomout->setIcon(QIcon(ZoomoutIcon32));
		m_wordwrap->setIcon(QIcon(CrlfIcon32));
		m_allWhite->setIcon(QIcon(WhiteIcon32));
		m_indentGuide->setIcon(QIcon(IndentIcon32));
		m_preHexPage->setIcon(QIcon(PreHexIcon32));
		m_nextHexPage->setIcon(QIcon(NextHexIcon32));
		m_gotoHexPage->setIcon(QIcon(GotoHexIcon32));
		m_fileCompare->setIcon(QIcon(FileCompareIcon32));
		m_dirCompare->setIcon(QIcon(DirCompareIcon32));
		m_binCompare->setIcon(QIcon(BinCmpIcon32));
		m_transcode->setIcon(QIcon(TransCodeIcon32));
		m_rename->setIcon(QIcon(RenameIcon32));
	}
		else
		{
			setDark32Icon();
	}
}
#endif

	if (iconSize < 48)
	{
		m_newFile->setIcon(QIcon(NewFileIcon));
		m_openFile->setIcon(QIcon(OpenFileIcon));
		m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon));
		m_saveAllFile->setIcon(QIcon(NoNeedSaveAllBarIcon));
		m_autoSaveAFile->setIcon(QIcon(AutoTimeSaveBarIcon));
		m_closeFile->setIcon(QIcon(CloseFileIcon));
		m_closeAllFile->setIcon(QIcon(CloseAllFileIcon));
		m_cutFile->setIcon(QIcon(CutIcon));
		m_copyFile->setIcon(QIcon(CopyFileIcon));
		m_pasteFile->setIcon(QIcon(PasteIcon));
		m_undo->setIcon(QIcon(UndoIcon));
		m_redo->setIcon(QIcon(RedoIcon));
		m_findText->setIcon(QIcon(FindIcon));
		m_replaceText->setIcon(QIcon(ReplaceIcon));
		m_markText->setIcon(QIcon(MarkIcon));
		m_signText->setIcon(QIcon(SignIcon));
		m_clearMark->setIcon(QIcon(ClearSignIcon));
		m_zoomin->setIcon(QIcon(ZoominIcon));
		m_zoomout->setIcon(QIcon(ZoomoutIcon));
		m_wordwrap->setIcon(QIcon(CrlfIcon));
		m_allWhite->setIcon(QIcon(WhiteIcon));
		m_indentGuide->setIcon(QIcon(IndentIcon));
		m_preHexPage->setIcon(QIcon(PreHexIcon));
		m_nextHexPage->setIcon(QIcon(NextHexIcon));
		m_gotoHexPage->setIcon(QIcon(GotoHexIcon));
		m_transcode->setIcon(QIcon(TransCodeIcon));
		m_rename->setIcon(QIcon(RenameIcon));
}
	else
	{
		m_newFile->setIcon(QIcon(NewFileIcon32));
		m_openFile->setIcon(QIcon(OpenFileIcon32));
		m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon32));
		m_saveAllFile->setIcon(QIcon(NoNeedSaveAllBarIcon32));
		m_autoSaveAFile->setIcon(QIcon(AutoTimeSaveBarIcon32));
		m_closeFile->setIcon(QIcon(CloseFileIcon32));
		m_closeAllFile->setIcon(QIcon(CloseAllFileIcon32));
		m_cutFile->setIcon(QIcon(CutIcon32));
		m_copyFile->setIcon(QIcon(CopyFileIcon32));
		m_pasteFile->setIcon(QIcon(PasteIcon32));
		m_undo->setIcon(QIcon(UndoIcon32));
		m_redo->setIcon(QIcon(RedoIcon32));
		m_findText->setIcon(QIcon(FindIcon32));
		m_replaceText->setIcon(QIcon(ReplaceIcon32));
		m_markText->setIcon(QIcon(MarkIcon32));
		m_signText->setIcon(QIcon(SignIcon32));
		m_clearMark->setIcon(QIcon(ClearSignIcon32));
		m_zoomin->setIcon(QIcon(ZoominIcon32));
		m_zoomout->setIcon(QIcon(ZoomoutIcon32));
		m_wordwrap->setIcon(QIcon(CrlfIcon32));
		m_allWhite->setIcon(QIcon(WhiteIcon32));
		m_indentGuide->setIcon(QIcon(IndentIcon32));
		m_preHexPage->setIcon(QIcon(PreHexIcon32));
		m_nextHexPage->setIcon(QIcon(NextHexIcon32));
		m_gotoHexPage->setIcon(QIcon(GotoHexIcon32));
		m_transcode->setIcon(QIcon(TransCodeIcon32));
		m_rename->setIcon(QIcon(RenameIcon32));
	}
}

void CCNotePad::initToolBar()
{
	int iconIndex = NddSetting::getKeyValueFromNumSets(ICON_SIZE);
	
	int ICON_SIZE = 24;

	if (iconIndex == 0)
	{
		ui.action24->setChecked(true);
	}
	else if (iconIndex == 1)
	{
		ICON_SIZE = 36;
		ui.action36->setChecked(true);
	}
	else if (iconIndex == 2)
	{
		ICON_SIZE = 48;
		ui.action48->setChecked(true);
	}

	m_curIconSize = ICON_SIZE;

	s_autoWarp = NddSetting::getKeyValueFromNumSets(AUTOWARP_KEY);
	s_zoomValue = NddSetting::getKeyValueFromNumSets(ZOOMVALUE);
	if (s_zoomValue > 10 || s_zoomValue < -5)
	{
		s_zoomValue = 0;
		NddSetting::updataKeyValueFromNumSets(ZOOMVALUE, s_zoomValue);
	}

	s_indent = NddSetting::getKeyValueFromNumSets(INDENT_KEY);
	s_showblank = NddSetting::getKeyValueFromNumSets(SHOWSPACE_KEY);

	m_newFile = new QToolButton(ui.mainToolBar);
	connect(m_newFile, &QAbstractButton::clicked, this, &CCNotePad::slot_actionNewFile_toggle);
	m_newFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_newFile->setToolTip(tr("New File"));
	ui.mainToolBar->addWidget(m_newFile);

	m_openFile = new QToolButton(ui.mainToolBar);
	connect(m_openFile, &QAbstractButton::clicked, this, &CCNotePad::slot_actionOpenFile_toggle);
	m_openFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_openFile->setToolTip(tr("Open File"));
	ui.mainToolBar->addWidget(m_openFile);

	m_saveFile = new QToolButton(ui.mainToolBar);
	connect(m_saveFile, &QAbstractButton::clicked, this, &CCNotePad::slot_actionSaveFile_toggle);
	m_saveFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_saveFile->setToolTip(tr("Save File"));
	ui.mainToolBar->addWidget(m_saveFile);

	m_saveAllFile = new QToolButton(ui.mainToolBar);
	connect(m_saveAllFile, &QAbstractButton::clicked, this, &CCNotePad::slot_saveAllFile);
	m_saveAllFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_saveAllFile->setToolTip(tr("Save All File"));
	ui.mainToolBar->addWidget(m_saveAllFile);

	m_autoSaveAFile = new QToolButton(ui.mainToolBar);
	m_autoSaveAFile->setCheckable(true);
	connect(m_autoSaveAFile, &QAbstractButton::clicked, this, &CCNotePad::slot_autoSaveFile);
	m_autoSaveAFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_autoSaveAFile->setToolTip(tr("Cycle Auto Save"));
	ui.mainToolBar->addWidget(m_autoSaveAFile);
	

	m_closeFile = new QToolButton(ui.mainToolBar);
	connect(m_closeFile, &QAbstractButton::clicked, this, &CCNotePad::slot_actionClose);
	m_closeFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_closeFile->setToolTip(tr("Close"));
	ui.mainToolBar->addWidget(m_closeFile);

	m_closeAllFile = new QToolButton(ui.mainToolBar);
	connect(m_closeAllFile, &QAbstractButton::clicked, this, &CCNotePad::slot_closeAllFile);
	m_closeAllFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_closeAllFile->setToolTip(tr("Close All"));
	ui.mainToolBar->addWidget(m_closeAllFile);

	ui.mainToolBar->addSeparator();

	m_cutFile = new QToolButton(ui.mainToolBar);
	connect(m_cutFile, &QAbstractButton::clicked, this, &CCNotePad::slot_cut);
	m_cutFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_cutFile->setToolTip(tr("Cut"));
	ui.mainToolBar->addWidget(m_cutFile);

	m_copyFile = new QToolButton(ui.mainToolBar);
	connect(m_copyFile, &QAbstractButton::clicked, this, &CCNotePad::slot_copy);
	m_copyFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_copyFile->setIcon(QIcon(CopyFileIcon));
	m_copyFile->setToolTip(tr("Copy"));
	ui.mainToolBar->addWidget(m_copyFile);

	m_pasteFile = new QToolButton(ui.mainToolBar);
	connect(m_pasteFile, &QAbstractButton::clicked, this, &CCNotePad::slot_paste);
	m_pasteFile->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_pasteFile->setToolTip(tr("Paste"));
	ui.mainToolBar->addWidget(m_pasteFile);

	ui.mainToolBar->addSeparator();

	m_undo = new QToolButton(ui.mainToolBar);
	connect(m_undo, &QAbstractButton::clicked, this, &CCNotePad::slot_undo);
	m_undo->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_undo->setToolTip(tr("Undo"));
	ui.mainToolBar->addWidget(m_undo);

	m_redo = new QToolButton(ui.mainToolBar);
	connect(m_redo, &QAbstractButton::clicked, this, &CCNotePad::slot_redo);
	m_redo->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_redo->setToolTip(tr("Redo"));
	ui.mainToolBar->addWidget(m_redo);

	ui.mainToolBar->addSeparator();

	m_findText = new QToolButton(ui.mainToolBar);
	connect(m_findText, &QAbstractButton::clicked, this, &CCNotePad::slot_find);
	m_findText->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_findText->setToolTip(tr("Find"));
	ui.mainToolBar->addWidget(m_findText);

	m_replaceText = new QToolButton(ui.mainToolBar);
	connect(m_replaceText, &QAbstractButton::clicked, this, &CCNotePad::slot_replace);
	m_replaceText->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_replaceText->setToolTip(tr("Replace"));
	ui.mainToolBar->addWidget(m_replaceText);

	m_markText = new QToolButton(ui.mainToolBar);
	connect(m_markText, &QAbstractButton::clicked, this, &CCNotePad::slot_markHighlight);
	m_markText->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_markText->setToolTip(tr("Mark"));
	ui.mainToolBar->addWidget(m_markText);

	ui.mainToolBar->addSeparator();

	//选择单词高亮
	m_signText = new QToolButton(ui.mainToolBar);
	connect(m_signText, &QAbstractButton::clicked, this, &CCNotePad::slot_wordHighlight);
	m_signText->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_signText->setToolTip(tr("word highlight(F8)"));
	m_signText->setShortcut(QKeySequence(Qt::Key_F8));
	ui.mainToolBar->addWidget(m_signText);

	//选择单词高亮
	m_clearMark = new QToolButton(ui.mainToolBar);
	connect(m_clearMark, &QAbstractButton::clicked, this, &CCNotePad::slot_clearMark);
	m_clearMark->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_clearMark->setToolTip(tr("clear all highlight(F7)"));
	m_clearMark->setShortcut(QKeySequence(Qt::Key_F7));
	ui.mainToolBar->addWidget(m_clearMark);

	
	ui.mainToolBar->addSeparator();

	m_zoomin = new QToolButton(ui.mainToolBar);
	connect(m_zoomin, &QAbstractButton::clicked, this, &CCNotePad::slot_zoomin);
	m_zoomin->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_zoomin->setToolTip(tr("Zoom In"));
	ui.mainToolBar->addWidget(m_zoomin);

	m_zoomout = new QToolButton(ui.mainToolBar);
	connect(m_zoomout, &QAbstractButton::clicked, this, &CCNotePad::slot_zoomout);
	m_zoomout->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_zoomout->setToolTip(tr("Zoom Out"));
	ui.mainToolBar->addWidget(m_zoomout);

	ui.mainToolBar->addSeparator();

	m_wordwrap = new QToolButton(ui.mainToolBar);
	m_wordwrap->setCheckable(true);
	m_wordwrap->setChecked((s_autoWarp != QsciScintilla::WrapNone));
	connect(m_wordwrap, &QAbstractButton::clicked, this, &CCNotePad::slot_wordwrap);
	m_wordwrap->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_wordwrap->setToolTip(tr("Word Wrap"));
	ui.mainToolBar->addWidget(m_wordwrap);

	ui.actionWrap->setChecked((s_autoWarp != QsciScintilla::WrapNone));

	m_allWhite = new QToolButton(ui.mainToolBar);
	m_allWhite->setCheckable(true);
	m_allWhite->setChecked((s_showblank == 3));

	//这里使用clicked信号。修改setChecked时，不会触发该信号，避免循环触发。
	//注意统一:凡是需要两个地方控制一个状态，同步的地方。按钮使用clicked信号，action使用triggered。
	//因为这两个信号，不会被setchecked修改，这样其他地方调用setchecked时，避免信号循环冲突
	connect(m_allWhite, &QAbstractButton::clicked, this, &CCNotePad::slot_allWhite);
	m_allWhite->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_allWhite->setToolTip(tr("Show Blank"));
	ui.mainToolBar->addWidget(m_allWhite);

	ui.actionShowAll->setChecked((s_showblank == 3));

	m_indentGuide = new QToolButton(ui.mainToolBar);
	m_indentGuide->setCheckable(true);
	m_indentGuide->setChecked((s_indent == 1));
	connect(m_indentGuide, &QAbstractButton::toggled, this, &CCNotePad::slot_indentGuide);
	m_indentGuide->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_indentGuide->setToolTip(tr("Indent Guide"));
	ui.mainToolBar->addWidget(m_indentGuide);

	ui.mainToolBar->addSeparator();

	m_preHexPage = new QToolButton(ui.mainToolBar);
	connect(m_preHexPage, &QAbstractButton::clicked, this, &CCNotePad::slot_preHexPage);
	m_preHexPage->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_preHexPage->setToolTip(tr("Pre Hex Page"));
	ui.mainToolBar->addWidget(m_preHexPage);

	m_nextHexPage = new QToolButton(ui.mainToolBar);
	connect(m_nextHexPage, &QAbstractButton::clicked, this, &CCNotePad::slot_nextHexPage);
	m_nextHexPage->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_nextHexPage->setToolTip(tr("Next Hex Page"));
	ui.mainToolBar->addWidget(m_nextHexPage);

	m_gotoHexPage = new QToolButton(ui.mainToolBar);
	connect(m_gotoHexPage, &QAbstractButton::clicked, this, &CCNotePad::slot_gotoHexPage);
	m_gotoHexPage->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_gotoHexPage->setToolTip(tr("Goto Hex Page"));
	ui.mainToolBar->addWidget(m_gotoHexPage);

	ui.mainToolBar->addSeparator();


	m_transcode = new QToolButton(ui.mainToolBar);
	connect(m_transcode, &QAbstractButton::clicked, this, &CCNotePad::slot_batch_convert);
	m_transcode->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_transcode->setToolTip(tr("transform encoding"));
	ui.mainToolBar->addWidget(m_transcode);

	m_rename = new QToolButton(ui.mainToolBar);
	connect(m_rename, &QAbstractButton::clicked, this, &CCNotePad::slot_batch_rename);
	m_rename->setFixedSize(ICON_SIZE, ICON_SIZE);
	m_rename->setToolTip(tr("batch rename file"));
	ui.mainToolBar->addWidget(m_rename);

	setShoctIcon(ICON_SIZE);
	

	//编码里面只能有一个当前被选中
	m_pEncodeActGroup = new QActionGroup(this);
	m_pEncodeActGroup->addAction(ui.actionencode_in_GBK);
	m_pEncodeActGroup->addAction(ui.actionencode_in_uft8);
	m_pEncodeActGroup->addAction(ui.actionencode_in_UTF8_BOM);
	m_pEncodeActGroup->addAction(ui.actionencode_in_UCS_BE_BOM);
	m_pEncodeActGroup->addAction(ui.actionencode_in_UCS_2_LE_BOM);
	m_pEncodeActGroup->addAction(ui.actionBig5);
	m_pEncodeActGroup->setExclusive(true);

	m_pLineEndActGroup = new QActionGroup(this);
	m_pLineEndActGroup->addAction(ui.actionconver_windows_CR_LF);
	m_pLineEndActGroup->addAction(ui.actionconvert_Unix_LF);
	m_pLineEndActGroup->addAction(ui.actionconvert_Mac_CR);
	m_pEncodeActGroup->setExclusive(true);

	m_pIconSize = new QActionGroup(this);
	m_pIconSize->addAction(ui.action24);
	m_pIconSize->addAction(ui.action36);
	m_pIconSize->addAction(ui.action48);
	m_pIconSize->setExclusive(true);
	connect(m_pIconSize, &QActionGroup::triggered, this, &CCNotePad::slot_changeIconSize, Qt::QueuedConnection);


	m_pLexerActGroup = new QActionGroup(this);
	/*m_pLexerActGroup->addAction(ui.actionAVS);*/
	m_pLexerActGroup->addAction(ui.actionAssembly);
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
	m_pLexerActGroup->addAction(ui.actionR_2);
	m_pLexerActGroup->addAction(ui.actionRuby);
	m_pLexerActGroup->addAction(ui.actionRust);
	m_pLexerActGroup->addAction(ui.actionSpice);
	m_pLexerActGroup->addAction(ui.actionSql);
	m_pLexerActGroup->addAction(ui.actionTcl);
	m_pLexerActGroup->addAction(ui.actionTex);
	m_pLexerActGroup->addAction(ui.actionVb);
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
	m_pLexerActGroup->addAction(ui.actionUserDefine);

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

	syncBlankShowStatus();
}


//void CCNotePad::slot_skinStyleGroup(QAction* /*action*/)
//{
//	//切换图标
//	setShoctIcon(m_curIconSize);
//
//	if (s_curStyleId != StyleSet::m_curStyleId)
//	{
//		//if (DEEP_BLACK == StyleSet::m_curStyleId)
//		//{
//		//	//如果不存在暗黑配置，则需要修正一次。后续如果存在了，则不需要再修正颜色
//		//	if (!QtLangSet::isExistDarkLangSetings())
//		//	{
//		//	QtLangSet::setAllLangFontFgColorToDarkStyle();
//		//}
//	//}
//		s_curStyleId = StyleSet::m_curStyleId;
//	}
//
//	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
//	{
//		QWidget* pw = ui.editTabWidget->widget(i);
//		ScintillaEditView *pEdit = dynamic_cast<ScintillaEditView*>(pw);
//		if (pEdit != nullptr)
//		{
//			pEdit->adjuctSkinStyle();
//			autoSetDocLexer(pEdit);
//		}
//		else
//		{
//			ScintillaHexEditView* pEdit = dynamic_cast<ScintillaHexEditView*>(pw);
//			if (pEdit != nullptr)
//			{
//				pEdit->adjuctSkinStyle();
//	}
//	}
//	}
//}

void CCNotePad::setZoomLabelValue(int zoomValue)
{
	m_zoomLabel->setText(tr("Zoom: %1%").arg(zoomValue));
		}

void CCNotePad::slot_changeIconSize(QAction *action)
{
	int size = 24;
	if (action == ui.action24)
	{
		size = 24;

		NddSetting::updataKeyValueFromNumSets(ICON_SIZE,0);
		ui.action24->setChecked(true);
	}
	else if (action == ui.action36)
	{
		size = 36;
		NddSetting::updataKeyValueFromNumSets(ICON_SIZE, 1);
		ui.action36->setChecked(true);
	}
	else if (action == ui.action48)
	{
		size = 48;
		NddSetting::updataKeyValueFromNumSets(ICON_SIZE, 2);
		ui.action48->setChecked(true);
	}

	m_curIconSize = size;

	const QObjectList & childs = ui.mainToolBar->children();

	QToolButton* toolBt = nullptr;

	for (int i = 0; i < childs.size(); ++i)
	{
		toolBt = dynamic_cast<QToolButton*>(childs.at(i));
		if (toolBt != nullptr)
		{
			toolBt->setFixedSize(size, size);
		}
	}

	setShoctIcon(size);
}

void CCNotePad::setTxtLexer(ScintillaEditView* pEdit)
{
	QsciLexer* lexer = ScintillaEditView::createLexer(L_TXT, "");
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
				//用户自定义的不在其中。不能设置为用户自定义的语法，不明确。
				return;
			}

			delete curLexer;
		}

		QsciLexer * lexer = ScintillaEditView::createLexer(lexerId,"");
		if (lexer != nullptr)
		{
			pEdit->setLexer(lexer);
			QString tag = lexer->lexerTag();
			setLangsDescLable(tag);
		}
		else
		{
			//默认按txt处理
			setTxtLexer(pEdit);
	}
		
}
}

//保存最近打开文件到数据库。文件只有在关闭时，才写入最近列表。不关闭的下次自动恢复打开
void CCNotePad::saveReceneOpenFile()
{
	QString rFile("recentopenfile");

	const int maxRecord = 15;

	if (NddSetting::isDbExist())
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
			NddSetting::updataKeyValueFromSets(rFile, fileSaveText);
		}
		else
		{
			NddSetting::updataKeyValueFromSets(rFile, "");
	}
}
}

//从数据库读取最近对比的文件列表
void CCNotePad::initReceneOpenFileMenu()
{
	QString rFile("recentopenfile");

	if (NddSetting::isDbExist())
	{
		QString fileStr = NddSetting::getKeyValueFromSets(rFile);

		QStringList fileList = fileStr.split('|');

		for (QString var : fileList)
		{
			if (!var.isEmpty() && (-1 == m_receneOpenFileList.indexOf(var)))
			{
				QAction* act = ui.menuRecene_File->addAction(var, this, &CCNotePad::slot_openReceneFile);
				act->setObjectName(var);
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

//判断新建名称是否已经存在，是 true 
bool CCNotePad::isNewFileNameExist(QString& fileName)
{

	for (int i = ui.editTabWidget->count() -1; i >=0 ; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		if (pw != nullptr && (-1 != getFileNewIndexProperty(pw)))
		{
			if (getFilePathProperty(pw) == fileName)
			{
				return true;
			}
		}
	}

	return false;
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
	else if(codeStr == "BIG5-HKSCS")
	{
		codeStr = tr("Big5(Traditional Chinese)");
	}
	m_codeStatusLabel->setText(codeStr);
}

void  CCNotePad::setLineEndBarLabel(RC_LINE_FORM lineEnd)
{
	QString endStr = Encode::getLineEndById(lineEnd);

	if (m_lineEndLabel->currentText() != endStr)
	{
		//这里要禁止currentIndexChanged,避免循环触发
		disconnect(m_lineEndLabel, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CCNotePad::on_lineEndChange);
		m_lineEndLabel->setCurrentText(endStr);
		connect(m_lineEndLabel, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CCNotePad::on_lineEndChange);
}
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
	tabClose(pEidt);
	openFile(filePath);
}

bool CCNotePad::checkRoladFile(ScintillaEditView* pEdit)
{
	if (pEdit != nullptr && pEdit->property(Modify_Outside).toBool())
	{
		//防止该函数重入，导致时序错误
		if (m_isInReloadFile)
		{
			return false;
		}

		m_isInReloadFile = true;

		QString filePath = pEdit->property(Edit_View_FilePath).toString();

		QApplication::beep();

		if (QMessageBox::Yes == QMessageBox::question(this, tr("Reload"), tr("\"%1\" This file has been modified by another program. Do you want to reload it?").arg(filePath)))
		{
			//reloadEditFile 里面会关闭和新增tab，触发一系列的currentChanged
			disconnect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged);
			reloadEditFile(pEdit);
			connect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged, Qt::UniqueConnection);
		}
		else
		{
			pEdit->setProperty(Modify_Outside, QVariant(false));
		}

		m_isInReloadFile = false;

		return true;
	}

	return false;
}

//这个函数是在paint中调用，所以不要直连，否则调用QMessagebox后崩溃
void  CCNotePad::slot_LineNumIndexChange(int line, int index)
{
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(sender());
	if (pEdit == nullptr)
	{
		return;
	}
	QString lineNums;

	int type = getDocTypeProperty(pEdit);

	switch (type)
	{
	case TXT_TYPE:
		//文本文件可能被修改。
	checkRoladFile(pEdit);
		lineNums = tr("Ln: %1	Col: %2").arg(line + 1).arg(index);
		break;
	case BIG_TEXT_RO_TYPE:
		//大文本分块加载，只读格式
	{
		quint32 bLineStart = pEdit->getBigTextBlockStartLine();
		lineNums = tr("Ln: %1	Col: %2").arg(bLineStart + line + 1).arg(index);
}
		break;
	case BIG_EDIT_RW_TYPE:
		break;//暂时没有
	case SUPER_BIG_TEXT_RO_TYPE:
	case HEX_TYPE:
		//这两种是没有行号的，只有列号
		lineNums = tr("Ln: %1	Col: %2").arg("unknown").arg(index);
		break;
	default:
		break;
	}
	m_lineNumLabel->setText(lineNums);
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

QAction* findItemInMenuByName(QMenu* menu, QString name)
{
	return menu->findChild<QAction*>(name);
}

void CCNotePad::dealRecentFileMenuWhenColseFile(QString closeFilePath)
{
	QAction* act = nullptr;

	//如果关闭的文件，已经在最近列表中，则移动到最前面即可
	int index = m_receneOpenFileList.indexOf(closeFilePath);
	if (-1 != index)
	{
		QString filePath = m_receneOpenFileList.takeAt(index);

		act = findItemInMenuByName(ui.menuRecene_File, filePath);

		if (act != nullptr)
		{
			ui.menuRecene_File->removeAction(act);
		}
	}
	else
	{
		act = new QAction(closeFilePath, ui.menuFile);
		act->setObjectName(closeFilePath);

		connect(act, &QAction::triggered, this, &CCNotePad::slot_openReceneFile);
	}


	//在菜单最近列表上面添加。如果最近列表是空的，则放在退出菜单之上
	if (m_receneOpenFileList.isEmpty())
	{
		ui.menuRecene_File->insertAction(nullptr, act);
	}
	else
	{
		//放在列表最上面
		QString curTopActionPath = m_receneOpenFileList.first();

		QAction* topAct = findItemInMenuByName(ui.menuRecene_File, curTopActionPath);
		if (topAct != nullptr)
		{
			ui.menuRecene_File->insertAction(topAct, act);
		}
	}

	m_receneOpenFileList.push_front(closeFilePath);

	//不能无限制变大，及时删除一部分
	if (m_receneOpenFileList.size() > 15)
	{
		QString k = m_receneOpenFileList.takeLast();
		QAction* lastAct = findItemInMenuByName(ui.menuRecene_File, k);
		if (lastAct != nullptr)
		{
			ui.menuRecene_File->removeAction(act);
			lastAct->deleteLater();
		}

	}
}

//isInQuit::是否在主程序退出状态
void CCNotePad::tabClose(int index, bool isInQuit)
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

		//关闭文件后，打开一个新的页面
		if (!isInQuit)
		{
		initTabNewOne();
		}
		delFileListView(filePath);
		return;
	}
	else if (BIG_TEXT_RO_TYPE == type)
	{
		ui.editTabWidget->removeTab(index);
		pw->deleteLater();

		FileManager::getInstance().closeBigTextRoFileHand(filePath);
		if (!isInQuit)
		{
		initTabNewOne();
		}
		delFileListView(filePath);
		return;
	}
	else if (SUPER_BIG_TEXT_RO_TYPE == type)
	{
		ui.editTabWidget->removeTab(index);
		pw->deleteLater();

		FileManager::getInstance().closeSuperBigTextFileHand(filePath);
		if (!isInQuit)
		{
			initTabNewOne();
		}
		delFileListView(filePath);
		return;
	}

	//关闭之前，检查是否要保存。如果文档为脏，则询问是否要保存
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
	if ((pEdit != nullptr) && (pEdit->property(Edit_Text_Change).toBool()))
	{
		QApplication::beep();

		int ret = QMessageBox::question(this, tr("Do you want to save changes to before closing?"), tr("If you don't save the changes you made in file %1, you'll lose them forever.").arg(filePath), tr("Yes"), tr("No"), tr("Cancel"));
	
		//保存
		if (ret == 0)
		{
			saveTabEdit(index);

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
		dealRecentFileMenuWhenColseFile(filePath);
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

	delFileListView(filePath);

	if (!isInQuit)
	{
	initTabNewOne();
}
}
//点击tab上的关闭事件执行槽函数。注意这个index是其在tab中的序号。
//当中间有删除时，是会动态变化的。所以不能以这个id为一直的固定索引
void CCNotePad::slot_tabClose(int index)
{
	tabClose(index);
}

void CCNotePad::tabClose(QWidget* pEdit)
{
	for (int i = 0; i < ui.editTabWidget->count(); ++i)
	{
		if (pEdit == ui.editTabWidget->widget(i))
		{
			tabClose(i);
			break;
		}
	}
	
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
		int ret = FileManager::getInstance().loadFileDataInText(pEdit, contentPath, code, lineEnd, nullptr, false, this);
		if (ret != 0)
		{
			ui.statusBar->showMessage(tr("Restore Last Temp File %1 Failed").arg(contentPath));
		}
		isChange = true;
	}

	connect(pEdit, &ScintillaEditView::cursorPositionChanged, this, &CCNotePad::slot_LineNumIndexChange, Qt::QueuedConnection);
	connect(pEdit, &ScintillaEditView::copyAvailable, this, &CCNotePad::slot_copyAvailable);
	connect(pEdit, SIGNAL(SCN_ZOOM()), this, SLOT(slot_zoomValueChange()));

	enableEditTextChangeSign(pEdit);

	QString label = name;

	disconnect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged);

	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon((StyleSet::getCurrentSytleId() != DEEP_BLACK)? TabNoNeedSave:TabNoNeedSaveDark32), label);

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

	connect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged, Qt::UniqueConnection);

	//设置自动转换和缩进参考线
	if (s_autoWarp != QsciScintilla::WrapNone)
	{
		pEdit->setWrapMode(QsciScintilla::WrapCharacter);
	}

	setEditShowBlankStatus(pEdit, s_showblank);

	if (s_zoomValue != 0)
	{
		pEdit->zoomTo(s_zoomValue);
	}

	autoSetDocLexer(pEdit);

	int zoomValue = 100 + 10 * s_zoomValue;
	ui.statusBar->showMessage(tr("New File Finished [Text Mode] Zoom %1%").arg(zoomValue), 8000);
	setZoomLabelValue(zoomValue);

	addFileListView(name, pEdit);

	pEdit->viewport()->setFocus();

	return pEdit;
}

void CCNotePad::slot_actionNewFile_toggle(bool /*checked*/)
{
	int index = FileManager::getInstance().getNextNewFileId();
	int nameId = index;
	QString name;

	while (true)
	{
		name = QString("New %1").arg(nameId);

		//检测一下是否重名New 文件，如果存在，则重新命名。注意id肯定是唯一的，但是名称其实可以重复
		if (!isNewFileNameExist(name))
		{
			break;
		}
		++nameId;
	}

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

//重新加载文件以指定的编码方式。单纯的修改编码，不视作文件做了修改。
bool CCNotePad::reloadTextFileWithCode(CODE_ID code)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	//16进制的处理逻辑

	int docType = getDocTypeProperty(pw);

	if (HEX_TYPE == docType)
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

		disEnableEditTextChangeSign(pEdit);

		pEdit->clear();

		if (docType == TXT_TYPE)
		{
			int errCode = FileManager::getInstance().loadFileDataInText(pEdit, filePath, code, lineEnd, this, false,this);
		if (errCode == 5)
		{
			//只读模式。暂时什么也不做
		}
			else if (errCode != 0)
		{
			delete pEdit;
			return false;
		}
		}
		else if (BIG_TEXT_RO_TYPE == docType)
		{
			//大文本索引加载模式，不需要再读取文本。只需要进行编码的转换即可
			BigTextEditFileMgr* fileMgr = FileManager::getInstance().getBigFileEditMgr(filePath);
			if (fileMgr != nullptr)
			{
				fileMgr->loadWithCode = code;
				showBigTextFile(pEdit, fileMgr, fileMgr->m_curBlockIndex);
			}
			else
			{
				return false;
			}
		}
		else if (SUPER_BIG_TEXT_RO_TYPE == docType)
		{
			TextFileMgr* fileMgr = FileManager::getInstance().getSuperBigFileMgr(filePath);
			if (fileMgr != nullptr)
			{
				fileMgr->loadWithCode = code;
				showBigTextFile(pEdit, fileMgr);

				//如果切换了编码，可能乱码，把当前的行号缓存清空一下，因为旧行号已经没有意义了。
				pEdit->clearSuperBitLineCache();

				pEdit->showBigTextLineAddr(fileMgr->fileOffset - fileMgr->contentRealSize, fileMgr->fileOffset);
				
			}
			else
			{
				return false;
			}
		}

		if (pEdit->lexer() == nullptr)
		{
			autoSetDocLexer(pEdit);
	}
		enableEditTextChangeSign(pEdit);
	}

	setCodeBarLabel(code);

	QVariant editTextCode((int)code);
	pEdit->setProperty(Edit_Text_Code, editTextCode);

	return true;
}

const int MAX_TEXT_FILE_SIZE = 100 * 1024 * 1024;

//大文本打开只读模式。20230126新增，这种模式打开时建立索引;todo:后续可多线程在后台建立索引
bool CCNotePad::openBigTextRoFile(QString filePath)
{
	QFileInfo fi(filePath);
	QString fileLabel(fi.fileName());

	//如果4M一个分块，则1024则是4G文件，2048则是8G文件。目前暂时最大支持8G的文件，进行文本编辑。
	BigTextEditFileMgr* txtFile = nullptr;

	RC_LINE_FORM lineEnd(UNKNOWN_LINE);

	if (!FileManager::getInstance().loadFileDataWithIndex(filePath, txtFile))
	{
		return false;
	}

	ScintillaEditView* pEdit = FileManager::getInstance().newEmptyDocument(true);
	pEdit->setReadOnly(true);
	pEdit->setNoteWidget(this);

	//必须要在editTabWidget->addTab之前，因为一旦add时会出发tabchange，其中没有doctype会导致错误
	pEdit->execute(SCI_SETSCROLLWIDTH, 80 * 10);
	setDocTypeProperty(pEdit, BIG_TEXT_RO_TYPE);

	showBigTextFile(pEdit, txtFile,0);

	lineEnd = (RC_LINE_FORM)txtFile->lineEndType;

	disconnect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged);
	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon((StyleSet::getCurrentSytleId() != DEEP_BLACK) ? TabNoNeedSave : TabNoNeedSaveDark32), getShortName(fileLabel));

	ui.editTabWidget->setCurrentIndex(curIndex);
	connect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged, Qt::UniqueConnection);

	connect(pEdit, &ScintillaEditView::cursorPositionChanged, this, &CCNotePad::slot_LineNumIndexChange, Qt::QueuedConnection);

	autoSetDocLexer(pEdit);

	//pEdit->showBigEidTextLineNum(txtFile);

	QVariant editViewFilePath(filePath);
	pEdit->setProperty(Edit_View_FilePath, editViewFilePath);

	//setWindowTitle(QString("%1 (%2)").arg(filePath).arg(tr("Big Text File ReadOnly")));

	ui.editTabWidget->setTabToolTip(curIndex, filePath);

	QVariant editViewNewFile(-1);
	pEdit->setProperty(Edit_File_New, editViewNewFile);

	QVariant editTextChange(false);
	pEdit->setProperty(Edit_Text_Change, editTextChange);

	setCodeTypeProperty(pEdit, txtFile->loadWithCode);
	setCodeBarLabel((CODE_ID)txtFile->loadWithCode);

	setLineEndBarLabel(lineEnd);
	setEndTypeProperty(pEdit, lineEnd);
	setDocEolMode(pEdit, lineEnd);

	syncCurDocEncodeToMenu(pEdit);
	setFileOpenAttrProperty(pEdit, OpenAttr::BigTextReadOnly);
	setWindowTitleMode(filePath, OpenAttr::BigTextReadOnly);


	//设置自动转换和缩进参考线
	if (s_autoWarp != QsciScintilla::WrapNone)
	{
		pEdit->setWrapMode(QsciScintilla::WrapCharacter);
	}

	setEditShowBlankStatus(pEdit, s_showblank);

	if (s_indent == 1)
	{
		pEdit->setIndentGuide(true);
	}

	if (s_zoomValue != 0)
	{
		pEdit->zoomTo(s_zoomValue);
	}

	addFileListView(filePath, pEdit);

	return true;
}

//按照超大文本文件进行只读打开
bool CCNotePad::openSuperBigTextFile(QString filePath)
{
	QFileInfo fi(filePath);
	QString fileLabel(fi.fileName());

	TextFileMgr* txtFile = nullptr;

	RC_LINE_FORM lineEnd(UNKNOWN_LINE);

	if (!FileManager::getInstance().loadFileData(filePath, txtFile, lineEnd))
	{
		return false;
	}

	ScintillaEditView* pEdit = FileManager::getInstance().newEmptyDocument(true);
	pEdit->setReadOnly(true);
	pEdit->setNoteWidget(this);

	//必须要在editTabWidget->addTab之前，因为一旦add时会出发tabchange，其中没有doctype会导致错误
	pEdit->execute(SCI_SETSCROLLWIDTH, 80 * 10);
	setDocTypeProperty(pEdit, SUPER_BIG_TEXT_RO_TYPE);

	showBigTextFile(pEdit, txtFile);
	lineEnd = (RC_LINE_FORM)txtFile->lineEndType;

	disconnect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged);
	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon((StyleSet::getCurrentSytleId() != DEEP_BLACK) ? TabNoNeedSave : TabNoNeedSaveDark32), getShortName(fileLabel));

	ui.editTabWidget->setCurrentIndex(curIndex);
	connect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged, Qt::UniqueConnection);
	connect(pEdit, &ScintillaEditView::cursorPositionChanged, this, &CCNotePad::slot_LineNumIndexChange, Qt::QueuedConnection);


	autoSetDocLexer(pEdit);

	pEdit->showBigTextLineAddr(txtFile->fileOffset - txtFile->contentRealSize, txtFile->fileOffset);

	QVariant editViewFilePath(filePath);
	pEdit->setProperty(Edit_View_FilePath, editViewFilePath);

	//setWindowTitle(QString("%1 (%2)").arg(filePath).arg(tr("Big Text File ReadOnly")));
	
	ui.editTabWidget->setTabToolTip(curIndex, filePath);

	QVariant editViewNewFile(-1);
	pEdit->setProperty(Edit_File_New, editViewNewFile);

	QVariant editTextChange(false);
	pEdit->setProperty(Edit_Text_Change, editTextChange);

	setCodeTypeProperty(pEdit, txtFile->loadWithCode);
	setCodeBarLabel((CODE_ID)txtFile->loadWithCode);

	setLineEndBarLabel(lineEnd);
	setEndTypeProperty(pEdit, lineEnd);
	setDocEolMode(pEdit, lineEnd);

	syncCurDocEncodeToMenu(pEdit);
	setFileOpenAttrProperty(pEdit, OpenAttr::SuperBigTextReadOnly);
	setWindowTitleMode(filePath, OpenAttr::SuperBigTextReadOnly);


	//设置自动转换和缩进参考线
	if (s_autoWarp != QsciScintilla::WrapNone)
	{
		pEdit->setWrapMode(QsciScintilla::WrapCharacter);
	}

	setEditShowBlankStatus(pEdit, s_showblank);


	if (s_indent == 1)
	{
		pEdit->setIndentGuide(true);
	}

	if (s_zoomValue != 0)
	{
		pEdit->zoomTo(s_zoomValue);
	}

	addFileListView(filePath, pEdit);

	return true;
}


void CCNotePad::showChangePageTips(QWidget* pEdit)
{
	int type = getDocTypeProperty(pEdit);
	
	if ((BIG_TEXT_RO_TYPE == type) || (SUPER_BIG_TEXT_RO_TYPE == type) || (HEX_TYPE == type))
	{
		ui.statusBar->showMessage(tr("Use < (Prev) or > (Next) and Goto Buttons to Change Page Num ."), 10000);
	}
}

void CCNotePad::setWindowTitleMode(QString filePath, OpenAttr attr)
{
	QString title = QString("%1 [%2]").arg(filePath).arg(OpenAttrToString(attr));
	setWindowTitle(title);
}

const quint64 MAX_TRY_OPEN_FILE_SIZE = 1024 * 1024 * 1024;

//打开普通文本文件。
bool CCNotePad::openTextFile(QString filePath, bool isCheckHex, CODE_ID code)
{
	getRegularFilePath(filePath);

	//先检测交换文件是否存在，如果存在，说明上次崩溃了，提示用户恢复
	QString swapfile = getSwapFilePath(filePath);

	bool isNeedRestoreFile = false;

	QFileInfo fi(filePath);

	//如果文件大于设定最大值,询问是否只读文件打开
	if (ScintillaEditView::s_bigTextSize <= 0 || ScintillaEditView::s_bigTextSize > 300)
	{
		ScintillaEditView::s_bigTextSize = 100;
	}

	if (fi.size() > ScintillaEditView::s_bigTextSize*1024*1024)
	{
		//文件如果小于1G，询问用户如何打开。如果大于1G，无条件分块加载
		if (fi.size() < MAX_TRY_OPEN_FILE_SIZE)
		{
			BigFileMessage askMsgBox(this);
			askMsgBox.setTip(tr("File %1 \nFile Size %2 > %3M, How to Open it ?").arg(filePath).arg(tranFileSize(fi.size())).arg(ScintillaEditView::s_bigTextSize));
			int openMode = askMsgBox.exec();

			//放弃打开
			if (openMode == -1)
			{
				return false;
			}
			else if (openMode == TXT_TYPE)
			{
				//正常普通文本打开，不做什么，继续往下走
			}
			else if (openMode == BIG_TEXT_RO_TYPE)
			{
				//大文本只读打开。20230125新增，做了内部索引，适合4G-8G左右的文件。
				return openBigTextRoFile(filePath);
	}
			else if (openMode == SUPER_BIG_TEXT_RO_TYPE)
			{
				//超大文本编辑模式。8G以上
				return openSuperBigTextFile(filePath);
			}
			else
			{
				//二进制打开
				return openHexFile(filePath);
			}
		}
		else
		{
			//如果小于8G，则大文本只读打开;反之则超大文本只读打开
			if (fi.size() <= 8 * MAX_TRY_OPEN_FILE_SIZE)
			{
				return openBigTextRoFile(filePath);
		}
			else
			{
				return openSuperBigTextFile(filePath);
	}
		}
	}

	if (QFile::exists(swapfile))
	{
		QFileInfo spfi(swapfile);

		//如果存在交换文件，而且修改时间更晚，询问用户是否需要恢复
		if ((spfi.size() > 0) /*&& (spfi.lastModified() >= fi.lastModified())*/)
		{
			//无条件备份一下文件，swap/原始文件都备份。避免用户文件丢失！！！
			QString srcBakFile = QString("%1_bak").arg(filePath);
			QString swapBakFile = QString("%1_bak").arg(swapfile);

			QFile::copy(filePath, srcBakFile);
			QFile::copy(swapfile, swapBakFile);

			int ret = QMessageBox::question(this, tr("Recover File?"), tr("File %1 abnormally closed last time , Restore it ?").arg(filePath), tr("Restore"), tr("No"));
			//使用历史存档恢复文件
			if (ret == 0)
			{
				isNeedRestoreFile = true;
			}
		}
	}

	ScintillaEditView* pEdit = FileManager::getInstance().newEmptyDocument();
	pEdit->setNoteWidget(this);

	//必须要在editTabWidget->addTab之前，因为一旦add时会出发tabchange，其中没有doctype会导致错误
	setDocTypeProperty(pEdit, TXT_TYPE);

	RC_LINE_FORM lineEnd;

	bool isReadOnly = false;

	//如果需要恢复，则加载交换文件的内容。
	if (!isNeedRestoreFile)
	{
		int ret = FileManager::getInstance().loadFileDataInText(pEdit, filePath, code, lineEnd, this, isCheckHex,this);
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
		else if (6 == ret)
		{
			//可能存在乱码，给出警告。还是以编辑模式打开
			ui.statusBar->showMessage(tr("File %1 open success. But Exist Garbled code !"));
		}
		else if (0 != ret)
		{
			delete pEdit;
		return false;
	}
	}
	else
	{
		//使用上次的swap文件恢复当前文件
		if (0 != FileManager::getInstance().loadFileDataInText(pEdit, swapfile, code, lineEnd,this,false,this))
		{
			ui.statusBar->showMessage(tr("File %1 Open Failed").arg(swapfile));
			delete pEdit;
			return false;
		}
		//恢复时直接使用新内容保存到原始文件中
		saveFile(filePath, pEdit, false);
	}
	
	//下面函数太长，进行一个重构到setNormalTextEditInitPro，后面其他地方也需要使用
	setNormalTextEditInitPro(pEdit, filePath, code, lineEnd, isReadOnly,false);

	return true;
}

//初始化普通可编辑文件的基本属性
//fileLabel:label显示名称
//filePath:对应的文件路径名
//code 文件编码
//lineEnd 文件换行符
//isReadOnly 是否只读
//isModifyed 是否修改过的脏状态
void CCNotePad::setNormalTextEditInitPro(ScintillaEditView* pEdit, QString filePath, CODE_ID code, RC_LINE_FORM lineEnd, bool isReadOnly, bool isModifyed)
{
	//防止addTab触发currentChanged信号，应发不必要的连锁反应
	disconnect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged);

	QFileInfo fi(filePath);
	QString fileLabel(fi.fileName());

	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon(TabNoNeedSave), getShortName(fileLabel));
	ui.editTabWidget->setCurrentWidget(pEdit);
	connect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged, Qt::UniqueConnection);

	connect(pEdit, &ScintillaEditView::cursorPositionChanged, this, &CCNotePad::slot_LineNumIndexChange, Qt::QueuedConnection);
	enableEditTextChangeSign(pEdit);
	connect(pEdit, &ScintillaEditView::copyAvailable, this, &CCNotePad::slot_copyAvailable);
	connect(pEdit, SIGNAL(SCN_ZOOM()), this, SLOT(slot_zoomValueChange()));

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

	setTextChangeProperty(pEdit, isModifyed);

	//如果是脏，还需要设置保存等按钮
	if (isModifyed)
	{
		m_saveFile->setEnabled(true);
		m_saveAllFile->setEnabled(true);
		ui.editTabWidget->setTabIcon(curIndex, QIcon(TabNeedSave));
	}

	QVariant editTextCode((int)code);
	pEdit->setProperty(Edit_Text_Code, editTextCode);

	syncCurDocEncodeToMenu(pEdit);
	syncCurDocLineEndStatusToMenu(pEdit);
	syncCurDocLexerToMenu(pEdit);

	//设置自动转换和缩进参考线
	if (s_autoWarp != QsciScintilla::WrapNone)
	{
		pEdit->setWrapMode(QsciScintilla::WrapCharacter);
	}

	setEditShowBlankStatus(pEdit, s_showblank);


	if (s_zoomValue != 0)
	{
		pEdit->zoomTo(s_zoomValue);
	}

	if (!isReadOnly)
	{
		setFileOpenAttrProperty(pEdit, OpenAttr::Text);
		setWindowTitleMode(filePath, OpenAttr::Text);
		int zoomValue = 100 + 10 * s_zoomValue;
		ui.statusBar->showMessage(tr("File %1 Open Finished [Text Mode] Zoom %2%").arg(filePath).arg(zoomValue), 8000);
		setZoomLabelValue(zoomValue);
	}
	else
	{
		setFileOpenAttrProperty(pEdit, OpenAttr::TextReadOnly);
		setWindowTitleMode(filePath, OpenAttr::TextReadOnly);
		ui.statusBar->showMessage(tr("File %1 Open Finished [Text ReadOnly Mode] (Note: display up to 50K bytes ...)").arg(fi.fileName()), 8000);
	}

	if (pEdit->lexer() == nullptr)
	{
		autoSetDocLexer(pEdit);
	}

	//缩进线要在autoSetDocLexer之后，发现lexer会修改缩进参考线
	if (s_indent == 1)
	{
		pEdit->setIndentGuide(true);
	}

	addFileListView(filePath, pEdit);
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

//显示超大文本文件只读
bool CCNotePad::showBigTextFile(ScintillaEditView* pEdit, TextFileMgr* txtFile)
{
	qint64 addr = txtFile->fileOffset - txtFile->contentRealSize;


	CODE_ID code = (CODE_ID)txtFile->loadWithCode;

	//不知道编码，则需要自动判断编码
	if (txtFile->loadWithCode == UNKOWN)
	{
		//自动从头部或文件中判断编码
		code = CmpareMode::getTextFileEncodeType((uchar *)txtFile->contentBuf, txtFile->contentRealSize, txtFile->filePath);
	}

	QString outUtf8Text;

	bool tranSucess = true;


	//UNICODE_LE格式需要单独处理
	if (code == UNICODE_LE)
	{
		tranSucess = CmpareMode::tranUnicodeLeToUtf8Bytes((uchar*)txtFile->contentBuf, txtFile->contentRealSize, outUtf8Text);
	}
	else
	{
		//如果还是unknown,则没法了，默认按照Utf8解析。
		if (code == UNKOWN)
		{
			code = UTF8_NOBOM;
		}
		tranSucess = Encode::tranStrToUNICODE(code, (const char*)txtFile->contentBuf, txtFile->contentRealSize, outUtf8Text);
	}

	if (txtFile->loadWithCode != code)
	{
		txtFile->loadWithCode = code;
	}

	//获取行结尾信息
	if (txtFile->lineEndType == RC_LINE_FORM::UNKNOWN_LINE)
	{
		txtFile->lineEndType = getLineEndTypeFromBigText(outUtf8Text);
	}

	pEdit->setText(outUtf8Text);

	if (tranSucess)
	{
	ui.statusBar->showMessage(tr("Current offset is %1 , load Contens Size is %2, File Total Size is %3").arg(addr).arg(txtFile->contentRealSize).arg(txtFile->fileSize));
	}
	else
	{
		//文件乱码
		if (txtFile->contentBuf == 0)
		{
			QMessageBox::warning(this, tr("Format Error"), tr("Not a txt format file , load with big txt is garbled code!"));
		}
		else
		{
			ui.statusBar->showMessage(tr("Not a txt format file , load with big txt is garbled code!"));
		}
	}

	//pEdit->setUtf8Text(txtFile->contentBuf, txtFile->contentRealSize);

	return true;
}

//显示大文本文件,可编辑。, int blockIndex显示第几块。txtFile->loadWithCode 如果是UNKONW,则自动判断编码；反之以code指定的加载
bool CCNotePad::showBigTextFile(ScintillaEditView* pEdit, BigTextEditFileMgr* txtFile, int blockIndex)
{
	if (blockIndex >= 0 && blockIndex < txtFile->blocks.size())
	{
		
		BlockIndex bi = txtFile->blocks.at(blockIndex);

		CODE_ID code = (CODE_ID)txtFile->loadWithCode;

		//不知道编码，则需要自动判断编码
		if (txtFile->loadWithCode == UNKOWN)
		{
			//自动从头部或文件中判断编码。如果是第0块即文件开头，才能从头部检测
			code = CmpareMode::getTextFileEncodeType(txtFile->filePtr + bi.fileOffset, bi.fileSize, txtFile->filePath, (blockIndex == 0));
		}

		QString outUtf8Text;

		bool tranSucess = true;

		//UNICODE_LE格式需要单独处理
		if (code == UNICODE_LE)
		{
			tranSucess = CmpareMode::tranUnicodeLeToUtf8Bytes(txtFile->filePtr + bi.fileOffset, bi.fileSize, outUtf8Text, (blockIndex == 0));
		}
		else
		{
			//如果还是unknown,则没法了，默认按照Utf8解析。
			if (code == UNKOWN)
			{
				code = UTF8_NOBOM;
			}
			tranSucess = Encode::tranStrToUNICODE(code, (const char*)txtFile->filePtr + bi.fileOffset, bi.fileSize, outUtf8Text);
		}

		if (txtFile->loadWithCode != code)
		{
			txtFile->loadWithCode = code;
		}

		//获取行结尾信息
		if (txtFile->lineEndType == RC_LINE_FORM::UNKNOWN_LINE)
		{
			txtFile->lineEndType = getLineEndTypeFromBigText(outUtf8Text);
		}
		//int ret = pEdit->setUtf8Text((char*)txtFile->filePtr + bi.fileOffset, bi.fileSize);

		pEdit->setText(outUtf8Text);

		pEdit->showBigTextRoLineNum(txtFile, blockIndex);
		pEdit->setBigTextBlockStartLine(bi.lineNumStart);
		txtFile->m_curBlockIndex = blockIndex;

		if (tranSucess)
		{
			ui.statusBar->showMessage(tr("Current offset is %1 , line nums is %2 - %3 load Contens Size is %4, File Total Size is %5").arg(bi.fileOffset).arg(bi.lineNumStart + 1).arg(bi.lineNumStart + bi.lineNum + 1).arg(bi.fileSize).arg(txtFile->file->size()));
		}
		else
		{
			//文件乱码
			if (blockIndex == 0)
			{
				QMessageBox::warning(this, tr("Format Error"), tr("Not a txt format file , load with big txt is garbled code!"));
			}
			else
			{
				ui.statusBar->showMessage(tr("Not a txt format file , load with big txt is garbled code!"));
			}
		}
		return true;
	}

	BlockIndex bi = txtFile->blocks.at(txtFile->m_curBlockIndex);
	ui.statusBar->showMessage(tr("Current offset is %1 , line nums is %2 - %3 load Contens Size is %4, File Total Size is %5").arg(bi.fileOffset).arg(bi.lineNumStart + 1).arg(bi.lineNumStart + bi.lineNum + 1).arg(bi.fileSize).arg(txtFile->file->size()));

	QApplication::beep();
	return false;
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

	disconnect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged);
	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon((StyleSet::getCurrentSytleId() != DEEP_BLACK) ? TabNoNeedSave : TabNoNeedSaveDark32), getShortName(fileLabel));
	
	ui.editTabWidget->setCurrentIndex(curIndex);
	connect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged, Qt::UniqueConnection);

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

	addFileListView(filePath, pEdit);

	return true;
}

void CCNotePad::slot_fileListView(bool check)
{
	if (check)
	{
		initFileListDockWin();
		syncFileTabToListView();
}
	else
	{
		if (!m_dockFileListWin.isNull())
		{
			m_dockFileListWin->close();
		}
	}
}


void CCNotePad::addFileListView(QString file, QWidget* pw)
{
	if (!m_dockFileListWin.isNull())
	{
		m_fileListView->addFileItem(file, pw);
	}
}
void CCNotePad::delFileListView(QString file)
{
	if (!m_dockFileListWin.isNull())
	{
		m_fileListView->delFileItem(file);
	}
}

void CCNotePad::syncFileTabToListView()
{
	if (m_dockFileListWin.isNull())
	{
		return;
	}

	for (int i = 0; i < ui.editTabWidget->count(); ++i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		QString filePath = getFilePathProperty(pw);
		m_fileListView->addFileItem(filePath,pw);
	}
}

void CCNotePad::fileListSetCurItem(QString filePath)
{
	if (!m_dockFileListWin.isNull())
	{
		m_fileListView->setCurItem(filePath);
	}
}

//双击文件列表，定位到对应的文件
void CCNotePad::slot_fileListItemDoubleClick(QListWidgetItem* item)
{
	if (!m_dockFileListWin.isNull())
	{
		QWidget *pWid = m_fileListView->getWidgetByFilePath(item->text());
		if (pWid != nullptr)
		{
			ui.editTabWidget->setCurrentWidget(pWid);
		}
	}
}

//在文件列表类中使用，关闭pEdit所在的编辑器
bool  CCNotePad::closeFileByEditWidget(QWidget* pEdit)
{
	int index = ui.editTabWidget->indexOf(pEdit);
	if (index != -1)
	{
		slot_tabClose(index);
		return true;
	}
	return false;
}

void  CCNotePad::initFileListDockWin()
{
	//停靠窗口1
	if (m_dockFileListWin.isNull())
	{
		m_dockFileListWin = new QDockWidget(tr("File List"), this);
		connect(m_dockFileListWin, &QDockWidget::dockLocationChanged, this, [](Qt::DockWidgetArea area) {
			NddSetting::updataKeyValueFromNumSets(FILELISTPOS, area);
		});

		connect(m_dockFileListWin, &QObject::destroyed, this, [this] {
			if (ui.actionFileListView->isChecked())
			{
				ui.actionFileListView->setChecked(false);
			}
		});
		m_dockFileListWin->setAttribute(Qt::WA_DeleteOnClose);
		m_dockFileListWin->layout()->setMargin(0);
		m_dockFileListWin->layout()->setSpacing(0);

		//暂时不提供关闭，因为关闭后需要同步菜单的check状态

		m_dockFileListWin->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
		m_dockFileListWin->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

		m_fileListView = new FileListView(m_dockFileListWin);
		m_fileListView->setNotepadWin(this);

		connect(m_fileListView, &FileListView::itemDoubleClicked, this, &CCNotePad::slot_fileListItemDoubleClick);

		m_dockFileListWin->setWidget(m_fileListView);

		int lastArea = NddSetting::getKeyValueFromNumSets(FILELISTPOS);
		if (lastArea == 0)
		{
			lastArea = Qt::LeftDockWidgetArea;
		}

		addDockWidget((Qt::DockWidgetArea)lastArea, m_dockFileListWin);

		if (!ui.actionFileListView->isChecked())
		{
			ui.actionFileListView->setChecked(true);
	}
	}
	m_dockFileListWin->show();
}

static QString fileSuffix(const QString& filePath)
{
	QFileInfo fi(filePath);
	return fi.suffix();
}

//打开之前保存的恢复文件
bool CCNotePad::tryRestoreFile(QString filePath)
{
	getRegularFilePath(filePath);

	QFileInfo fi(filePath);
	QString fileName = fi.fileName();

	QString tempDir = getGlboalTempSaveDir();
	QString restoreTempFile = QString("%1\\%2").arg(tempDir).arg(fi.fileName());
	
	QFileInfo restoreFi(restoreTempFile);

	//存在恢复文件，则加载打开
	if (restoreFi.exists())
	{
		ScintillaEditView* pEdit = FileManager::getInstance().newEmptyDocument();
		pEdit->setNoteWidget(this);

		//使用上次的swap文件恢复当前文件
		CODE_ID code;
		RC_LINE_FORM lineEnd;

		if (0 != FileManager::getInstance().loadFileDataInText(pEdit, restoreTempFile, code, lineEnd, nullptr,false))
		{
			ui.statusBar->showMessage(tr("File %1 Open Failed").arg(restoreTempFile));
			delete pEdit;
			QFile::remove(restoreTempFile);
			return openFile(filePath);
		}
		else
		{
			//打开成功
			setNormalTextEditInitPro(pEdit, filePath, code, lineEnd, false,true);
			//删除临时备份文件
			QFile::remove(restoreTempFile);
			return true;
		}
	}
	return openFile(filePath);
}
bool CCNotePad::openFile(QString filePath, int lineNum)
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

		QWidget* pw = ui.editTabWidget->currentWidget();
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr)
		{
			//必须要先获得焦点，否则无法执行行跳转
			pEdit->viewport()->setFocus();

			if (lineNum != -1)
			{
				pEdit->execute(SCI_GOTOLINE, lineNum - 1);
			}
		}

		ui.statusBar->showMessage(tr("file %1 already open at tab %2").arg(filePath).arg(retIndex),8000);
		
		return true;
	}

	//如果是已知的二进制文件，以二进制打开
	if (DocTypeListView::isHexExt(fileSuffix(filePath)))
	{
		return openHexFile(filePath);
	}

	//非已知的后缀文件，暂时无条件以文本模式打开
	int ret = openTextFile(filePath);
	if (ret && lineNum != -1)
	{
		QWidget* pw = ui.editTabWidget->currentWidget();
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr)
		{
			pEdit->execute(SCI_GOTOLINE, lineNum-1);
	}
	}
	return ret;
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

#ifdef _WIN32
void hide_file(const QString& szFile)
{
#ifdef UNICODE
	std::wstring wstr = szFile.toStdWString();
	::SetFileAttributes(wstr.c_str(), FILE_ATTRIBUTE_HIDDEN);
#else
	::SetFileAttributes(szFile.toStdString()c_str(), FILE_ATTRIBUTE_HIDDEN);
#endif // !UNICODE
}
#endif // _WIN32

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
		//linux也不是拥有者，可写权限就行
		QFlags<QFileDevice::Permission> power = QFile::permissions(fileName);

		if (!power.testFlag(QFile::WriteUser))
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

	auto saveWork = [this, &pEdit,isStatic](QFile& file, QString &fileName, bool isSwapFile=false)->bool{

	if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
	{
		QApplication::beep();
		if (!isStatic)
		{
#ifdef Q_OS_WIN
			//打开失败，这里一般是权限问题导致。如果是windows，在外面申请权限后继续处理
			if (QFileDevice::OpenError == file.error())
			{
				//先把当前文件的内容，保存到临时的目录中。
				QString tempDir = getGlboalTempSaveDir();
				QFileInfo fi(fileName);
				QString saveTempFile = QString("%1\\%2").arg(tempDir).arg(fi.fileName());
				saveFile(saveTempFile, pEdit, false, true, false);

				//后面新打开的文件，再去读取该文件。
				this->runAsAdmin(fileName);
				
				return false;
		}
#endif
			if (isSwapFile)
			{
				//如果是交换文件写失败，询问是否继续直接写文件
				return (QMessageBox::Yes == QMessageBox::question(this, tr("Error"), tr("Save Swap File %1 failed. Write the target file directly ?").arg(fileName)));

			}
			else
			{
			QMessageBox::warning(this, tr("Error"), tr("Save File %1 failed. You may not have write privileges \nPlease save as a new file!").arg(fileName));
		}
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
	else if (dstCode == CODE_ID::BIG5)
	{
		QTextCodec::setCodecForLocale(QTextCodec::codecForName("BIG5-HKSCS"));
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
		QString swapFilePath = getSwapFilePath(fileName);
		QFile swapfile(swapFilePath);
		//老文件则先写入交换文件，避免断电后破坏文件不能恢复
		//再写入原本文件
		bool success = saveWork(swapfile, fileName, true);
		if (success)
		{
#ifdef _WIN32
			if (!isClearSwpFile)
			{
				hide_file(swapFilePath);
			}
#endif
			success = saveWork(srcfile, fileName);
		if (!success)
		{
			return false;
		}
		}
		if (success && isClearSwpFile)
		{
			QFile::remove(swapFilePath);
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

	autoSetDocLexer(pEdit);

	addWatchFilePath(fileName);
}

//
void CCNotePad::saveTabEdit(int tabIndex)
{
	QWidget* pw = ui.editTabWidget->widget(tabIndex);

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
					QMessageBox::warning(this, tr("Error"), tr("file %1 already open at tab %2, please select other file name.").arg(fileName).arg(retIndex));
					return;
				}

				if (!saveFile(fileName, pEdit))
				{
					return;
				}
				updateProAfterSaveNewFile(tabIndex, fileName, pEdit);
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
		ui.editTabWidget->setTabIcon(tabIndex, QIcon((StyleSet::getCurrentSytleId() != DEEP_BLACK) ? TabNoNeedSave : TabNoNeedSaveDark32));
		//m_saveFile->setIcon(QIcon(NoNeedSaveBarIcon));
		m_saveFile->setEnabled(false);


		updateSaveAllToolBarStatus();

		//只有保存后再打开文本变化监控
		enableEditTextChangeSign(pEdit);
	}
}

//保存文件的执行
void CCNotePad::slot_actionSaveFile_toggle(bool /*checked*/)
{
	int index = ui.editTabWidget->currentIndex();
	saveTabEdit(index);
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

			QString curFilePath = getFilePathProperty(pEdit);
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As ..."), curFilePath, filter);

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

		ui.editTabWidget->setTabIcon(index, QIcon((StyleSet::getCurrentSytleId() != DEEP_BLACK) ? TabNoNeedSave : TabNoNeedSaveDark32));
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
	close();
#if 0
	slot_closeAllFile(true);

	//中途取消了关闭，则不退出
	if (m_isQuitCancel)
	{
		m_isQuitCancel = false;
		return;
	}

	exit(0);
#endif
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
		saveTabEdit(i);
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
//5和3一样，但是多了一个语法设置保存。
//20230119 对于1非脏的新建文件，不再保存。
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
			//不再保存新建的非脏文件。因为一定是空的，意义不大
			//qs.setValue(QString("%1").arg(index), QString("%1|1").arg(fileName));
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
			int lexId = L_TXT;
			if (pEdit->lexer() != nullptr)
			{
				lexId = pEdit->lexer()->lexerId();
				if (lexId > L_TXT)
				{
					lexId = L_TXT;
				}
			}

			if (lexId == L_TXT)
			{
			qs.setValue(QString("%1").arg(index), QString("%1|3").arg(fileName));
		}
			else
			{
				qs.setValue(QString("%1").arg(index), QString("%1|%2|5").arg(fileName).arg(lexId));
	}
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
	else if (BIG_TEXT_RO_TYPE == type)
	{
		ui.editTabWidget->removeTab(index);
		pw->deleteLater();

		FileManager::getInstance().closeBigTextRoFileHand(filePath);
		return;
	}
	else if (SUPER_BIG_TEXT_RO_TYPE == type)
	{
		ui.editTabWidget->removeTab(index);
		pw->deleteLater();

		FileManager::getInstance().closeSuperBigTextFileHand(filePath);
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

	//这里是在退出程序的过程中，不需要更新最近打开文件菜单列表

	//取消监控文件
	removeWatchFilePath(filePath);
	pw->deleteLater();
}
void CCNotePad::closeAllFileStatic()
{
	QString tempFileList = QString("notepad/temp/list");
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, tempFileList);
	qs.setIniCodec("UTF-8");
	QString qsSavePath = qs.fileName();
	QFileInfo fi(qsSavePath);
	QDir saveDir = fi.dir();
	//检查文件夹temp是否存在，不然就创建。发现第一次时，没有该文件夹，文件保存时失败。
	if (!saveDir.exists())
	{
		saveDir.mkdir(saveDir.absolutePath());
	}

	qs.clear();

	//从尾部开始依次调用保存所有文件。没修改的不需要保存
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		ui.editTabWidget->setCurrentIndex(i);
		closeFileStatic(i,qs);
	}
}

//退出时关闭所有文件
void CCNotePad::closeAllFileWhenQuit(bool isQuit)
{
	m_isQuitCancel = false;

	//从尾部开始依次调用保存所有文件。没修改的不需要保存
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		ui.editTabWidget->setCurrentIndex(i);

		tabClose(i, isQuit);

		if (m_isQuitCancel)
		{
			break;
		}
	}
}

//关闭所有文件的槽函数
void CCNotePad::slot_closeAllFile(bool)
{
	closeAllFileWhenQuit(false);
}

void CCNotePad::closeEvent(QCloseEvent * event)
{
	if (!m_pFindWin.isNull())
	{
		QByteArray curGeo = m_pFindWin->saveGeometry();
		NddSetting::updataKeyByteArrayValue(FINDWINSIZE, curGeo);

		m_pFindWin.data()->close();
	}

	if (!m_pHexGotoWin.isNull())
	{
		m_pHexGotoWin.data()->close();
	}

	if (!m_columnEditWin.isNull())
	{
		m_columnEditWin.data()->close();
	}
	

	//关闭的时候，filelistwin还存在
	if (!m_dockFileListWin.isNull())
	{
		NddSetting::updataKeyValueFromNumSets(FILELISTSHOW, 1);
		m_dockFileListWin.data()->close();
	}
	else
	{
		NddSetting::updataKeyValueFromNumSets(FILELISTSHOW, 0);
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
		closeAllFileWhenQuit(true);
	}
#else
	if ((s_restoreLastFile == 1) && m_isMainWindows)
	{
		closeAllFileStatic();
		m_isQuitCancel = false;
	}
	else
	{
		closeAllFileWhenQuit(true);
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

	//保存大小

	QByteArray curGeo = saveGeometry();
	NddSetting::updataKeyByteArrayValue(WIN_POS, curGeo);

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
	++s_zoomValue;

	if (s_zoomValue < -10)
		s_zoomValue = -10;
	else if (s_zoomValue > 20)
		s_zoomValue = 20;

	zoomto(s_zoomValue);
}

//ctrl+鼠标放大缩小zoom，由pedit发送的消息
//任何一个编辑框修改，其余的编辑框也需要同步修改
void CCNotePad::slot_zoomValueChange()
	{
	ScintillaEditView* pSrcEdit = dynamic_cast<ScintillaEditView*>(sender());
	if (pSrcEdit != nullptr)
	{
		pSrcEdit->updateLineNumberWidth();

		int curZoomValue = pSrcEdit->execute(SCI_GETZOOM);

		if (s_zoomValue != curZoomValue)
			{
				s_zoomValue = curZoomValue;
				NddSetting::updataKeyValueFromNumSets(ZOOMVALUE, s_zoomValue);
			int zoomValue = 100 + 10 * curZoomValue;
			ui.statusBar->showMessage(tr("Current Zoom Value is %1%").arg(zoomValue));
			setZoomLabelValue(zoomValue);
	}

		//修改其余的pedit
		for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
		{
			QWidget* pw = ui.editTabWidget->widget(i);
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

			if ((pEdit != nullptr) && (pEdit != pSrcEdit))
			{
				//zoomTo 会触发SCN_ZOOM，而zoomTo会触发slot_zoomValueChange，避免循环触发
				disconnect(pEdit, SIGNAL(SCN_ZOOM()), this, SLOT(slot_zoomValueChange()));
				pEdit->zoomTo(s_zoomValue);
				pEdit->updateLineNumberWidth();
				connect(pEdit, SIGNAL(SCN_ZOOM()), this, SLOT(slot_zoomValueChange()));
}
	}
}
}

void CCNotePad::zoomto(int zoomValue)
{
	NddSetting::updataKeyValueFromNumSets(ZOOMVALUE, zoomValue);
	int value = 100 + 10 * zoomValue;
	ui.statusBar->showMessage(tr("Current Zoom Value is %1%").arg(value));
	setZoomLabelValue(value);


	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
			//zoomTo 会触发SCN_ZOOM，而zoomTo会触发slot_zoomValueChange，避免循环触发
			disconnect(pEdit, SIGNAL(SCN_ZOOM()), this, SLOT(slot_zoomValueChange()));
			pEdit->zoomTo(zoomValue);
			pEdit->updateLineNumberWidth();
			connect(pEdit, SIGNAL(SCN_ZOOM()), this, SLOT(slot_zoomValueChange()));
	}
}
	}

void CCNotePad::slot_zoomout()
{
	--s_zoomValue;

	if (s_zoomValue < -10)
		s_zoomValue = -10;
	else if (s_zoomValue > 20)
		s_zoomValue = 20;

	zoomto(s_zoomValue);
}

//只切换了当前文档。换行大批量切换，可能会非常耗时，所以不全部换行。在文档切换的时候，需要检查下当前文档的自动换行状态。
void CCNotePad::slot_wordwrap(bool checked)
{
	QWidget* pw = ui.editTabWidget->currentWidget();

	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		if (checked)
		{
			pEdit->setWrapMode(QsciScintilla::WrapCharacter);
		}
		else
		{
			pEdit->setWrapMode(QsciScintilla::WrapNone);
		}
	}
	
	s_autoWarp = (checked) ? QsciScintilla::WrapCharacter : QsciScintilla::WrapNone;
	NddSetting::updataKeyValueFromNumSets(AUTOWARP_KEY, s_autoWarp);

	//同步两个按钮的状态
	if (ui.actionWrap->isChecked() != checked)
	{
		ui.actionWrap->setChecked(checked);
}

	if (m_wordwrap != nullptr && m_wordwrap->isChecked() != checked)
{
		m_wordwrap->setChecked(checked);
	}
}

void CCNotePad::syncBlankShowStatus()
	{
	//检查3个地方按钮的状态。这里务必注意，action使用trigged信号，setcheck修改不会触发该信号。避免循环
	//同样道理，button使用click信号，也是避免setcheck触发click信号，避免信号循环处理。
	ui.actionShowSpaces->setChecked((0 != (s_showblank & 0x1)));
	ui.actionshow_end_of_line->setChecked((0 != (s_showblank & 0x2)));
	ui.actionShowAll->setChecked((3 == s_showblank));
	m_allWhite->setChecked((3 == s_showblank));
	}

//这里是对所有文档进行了空白的开启等。后续如果发现有效率问题，要学自动换行那样，可以只修当前文档；其余文档在切换是更新
void CCNotePad::changeBlankShowStatus(int showBlank)
{
	//对比判断防止循环触发，这个是有必要的。
	if (s_showblank == showBlank)
	{
		return;
	}
	//0 全部不开启。1开始空白 2 开启行尾 3 全部开启
	s_showblank = showBlank;

	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
	if (pEdit != nullptr)
	{
			setEditShowBlankStatus(pEdit, s_showblank);
		}
		}
	
	NddSetting::updataKeyValueFromNumSets(SHOWSPACE_KEY, s_showblank);

	syncBlankShowStatus();
}

//有2个函数，都触发该函数。一个是ui.actionShowAll,一个是按钮m_allWhite。
//状态以ui.actionShowAll为主。m_allWhite为辅助同步按钮状态。避免二者的信号互相干扰。
void CCNotePad::slot_allWhite(bool checked)
{

	//0 全部不开启。1开始空白 2 开启行尾 3 全部开启
	changeBlankShowStatus((checked ? 3 : 0));
}

//只控制空格和TAB，不控制行尾
void CCNotePad::slot_show_spaces(bool checked)
	{
	int showblank = s_showblank;
		if (checked)
		{
		showblank |= 0x1;
		}
		else
		{
		showblank &= 0xe;
		}
	changeBlankShowStatus(showblank);
	}


//全部都会生效，和自动换行不一样，会全部设置
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
	NddSetting::updataKeyValueFromNumSets(INDENT_KEY, s_indent);
}

void CCNotePad::find(FindTabIndex findType)
{
	initFindWindow(findType);
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
#ifdef uos
	pFind->activateWindow();
#endif
	pFind->showNormal();
#ifdef uos
	adjustWInPos(pFind);
#endif

	pFind->setFocus();
	pFind->setCurrentTab(findType);
}

void CCNotePad::slot_findInDir()
{
	find(DIR_FIND_TAB);
}

void CCNotePad::slot_find()
{
	initFindWindow(FIND_TAB);
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

//在后台查找关键字
int CCNotePad::findAtBack(QString keyword)
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	return pFind->findAtBack(keyword);

}
//在后台替换关键字

//在后台替换关键字
int CCNotePad::replaceAtBack(QStringList& keyword, QStringList& replace)
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	return pFind->replaceAtBack(keyword, replace);
}

//在后台高亮关键字
int CCNotePad::markAtBack(QString keyword)
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	return pFind->markAllWord(keyword);
}

//返回值：0 正常 1 选择自动为空
int CCNotePad::initFindWindow(FindTabIndex type)
{
	int ret = 0;
	FindWin* pFind = nullptr;
	QWidget* pw = ui.editTabWidget->currentWidget();
	int docType = getDocTypeProperty(pw);

	if (m_pFindWin.isNull())
	{
		m_pFindWin = new FindWin(this);
		connect(m_pFindWin,&QObject::destroyed,this,&CCNotePad::slot_saveSearchHistory);

		QByteArray lastGeo = NddSetting::getKeyByteArrayValue(FINDWINSIZE);

		if (!lastGeo.isEmpty())
		{
			m_pFindWin->restoreGeometry(lastGeo);
		}

		pFind = dynamic_cast<FindWin*>(m_pFindWin.data());

		if (s_findHistroy.isEmpty())
		{
			//从历史查找记录文件中加载
			QString searchHistory = QString("notepad/searchHistory");//历史查找记录
			QSettings qs(QSettings::IniFormat, QSettings::UserScope, searchHistory);
			qs.setIniCodec("UTF-8");

			if (qs.contains("keys"))
			{
				QVariant history = qs.value("keys", "");
				s_findHistroy = history.toStringList();
			}
		}

		pFind->setFindHistory(&s_findHistroy);
	
		pFind->setTabWidget(ui.editTabWidget);

		if((TXT_TYPE == docType) || (BIG_TEXT_RO_TYPE == docType) || (SUPER_BIG_TEXT_RO_TYPE == docType))
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
		//注册一个ESC的退出按钮事件
			//设置查找的快捷键
		QShortcut *escSc = new QShortcut(m_pFindWin);
		escSc->setKey(QKeySequence(Qt::Key_Escape));
		escSc->setContext(Qt::WidgetWithChildrenShortcut);
		connect(escSc, &QShortcut::activated, this, [this]() {m_pFindWin->close(); });
	}
	else
	{
		pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	}

	
	if ((TXT_TYPE == docType) || (BIG_TEXT_RO_TYPE == docType) || (SUPER_BIG_TEXT_RO_TYPE == docType))
	{
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr && pEdit->hasSelectedText())
		{
			QString text = pEdit->selectedText();
			if (FIND_TAB == type)
			{
			pFind->setFindText(text);
				//如果字段比较短，则字段填充到替换中
				if (text.size() <= 60)
				{
					pFind->setReplaceFindText(text);
		}
	}
			else if (REPLACE_TAB == type)
			{
				pFind->setReplaceFindText(text);
				//如果字段比较短，则字段填充到替换中
				if (text.size() <= 60)
				{
					pFind->setFindText(text);
				}
			}
			else if (DIR_FIND_TAB == type)
			{
				pFind->setDirFindText(text);
			}
		}
		else
		{
			QString text;
			pFind->setFindText(text);
			pFind->setReplaceFindText(text);
			pFind->setDirFindText(text);
			ret = 1;
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
		else
		{
			QString text;
			pFind->setFindText(text);
			ret = 1;
	}
}
	return ret;
}

//保存历史查找记录
void CCNotePad::slot_saveSearchHistory()
{
	//从历史查找记录文件中加载
	QString searchHistory = QString("notepad/searchHistory");//历史查找记录
	QSettings qs(QSettings::IniFormat, QSettings::UserScope, searchHistory);
	qs.setIniCodec("UTF-8");

	if (s_findHistroy.count() > 15)
	{
		s_findHistroy = s_findHistroy.mid(0, 15);
	}
	qs.setValue("keys", s_findHistroy);
	qs.sync();
}

void CCNotePad::slot_replace()
{
	initFindWindow(REPLACE_TAB);
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	pFind->setCurrentTab(REPLACE_TAB);
	pFind->activateWindow();
	pFind->showNormal();
#ifdef uos
    adjustWInPos(pFind);
#endif
}

//标记高亮
void CCNotePad::slot_markHighlight()
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	pFind->setCurrentTab(MARK_TAB);
	pFind->activateWindow();
	pFind->showNormal();
#ifdef uos
    adjustWInPos(pFind);
#endif
}

//取消所有标记高亮
void CCNotePad::slot_clearMark()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	int docType = getDocTypeProperty(pw);

	if ((TXT_TYPE == docType) || (BIG_TEXT_RO_TYPE == docType) || (SUPER_BIG_TEXT_RO_TYPE == docType))
	{
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		int docEnd = pEdit->length();
		for (int i = 0; i < 5; ++i)
		{
			pEdit->execute(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE_EXT5+i);
		pEdit->execute(SCI_INDICATORCLEARRANGE, 0, docEnd);
		}
		pEdit->releaseAllMark();
	}
}

void CCNotePad::clearHighlightWord(QString signWord, ScintillaEditView* pEdit)
{
	if (pEdit == nullptr)
	{
	QWidget* pw = ui.editTabWidget->currentWidget();
		pEdit = dynamic_cast<ScintillaEditView*>(pw);

		if (HEX_TYPE == getDocTypeProperty(pw) || (pEdit == nullptr))
		{
		return;
	}
	}
	
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

	if (isClearMark)
	{
		//取消高亮
			FindRecords* r = curMarkRecord.at(i);

			pEdit->execute(SCI_SETINDICATORCURRENT, r->hightLightColor);

			for (int j = 0; j < r->records.size(); ++j)
			{
				const FindRecord& oneRecord = r->records.at(j);
				pEdit->execute(SCI_INDICATORCLEARRANGE, oneRecord.pos, oneRecord.end - oneRecord.pos);
			}

			//必须删除释放，否则内存泄露
			delete r;
			curMarkRecord.removeAt(i);
	}
	
		}
void CCNotePad::slot_clearWordHighlight()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	int docType = getDocTypeProperty(pw);
	QString signWord;
	ScintillaEditView* pEdit;
	int srcPostion = -1;

	if ((TXT_TYPE == docType) || (BIG_TEXT_RO_TYPE == docType) || (SUPER_BIG_TEXT_RO_TYPE == docType))
	{
		pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr && pEdit->hasSelectedText())
		{
			signWord = pEdit->selectedText();
		}
	}
	else if (HEX_TYPE == docType)
	{
		return;
	}

	if (signWord.isEmpty())
	{
		//如果没有选择内容，则自动获取当前的单词，进行1个匹配。注意中文也是一样，检测到前后的空格，看做1个单词
		srcPostion = pEdit->execute(SCI_GETCURRENTPOS);
		signWord = pEdit->wordAtPosition(srcPostion);
	}

	if (!signWord.isEmpty())
	{
		clearHighlightWord(signWord, pEdit);
	}
}

void CCNotePad::slot_wordHighlight()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	int docType = getDocTypeProperty(pw);
	QString signWord;
	ScintillaEditView* pEdit;
	int srcPostion = -1;

	if ((TXT_TYPE == docType) || (BIG_TEXT_RO_TYPE == docType) || (SUPER_BIG_TEXT_RO_TYPE == docType))
	{
		pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr && pEdit->hasSelectedText())
		{
			signWord = pEdit->selectedText();
		}
	}
	else if (HEX_TYPE == docType)
	{
		return;
	}

	if (signWord.isEmpty())
	{
		//如果没有选择内容，则自动获取当前的单词，进行1个匹配。注意中文也是一样，检测到前后的空格，看做1个单词
		srcPostion = pEdit->execute(SCI_GETCURRENTPOS);
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

		bool isNeedReColor = false;

		//取消高亮
		if (isClearMark)
		{
			FindRecords* r = curMarkRecord.at(i);
			isNeedReColor = (r->hightLightColor != CCNotePad::s_curMarkColorId);

			pEdit->execute(SCI_SETINDICATORCURRENT, r->hightLightColor);

			for (int j = 0; j < r->records.size(); ++j)
			{
				const FindRecord& oneRecord = r->records.at(j);
				pEdit->execute(SCI_INDICATORCLEARRANGE, oneRecord.pos, oneRecord.end - oneRecord.pos);
			}

			//必须删除释放，否则内存泄露
			delete r;
			curMarkRecord.removeAt(i);
		}

		//如果就颜色和当前颜色不一样，则还需要重新高亮
		if(!isClearMark || isNeedReColor)
		{
			//反之高亮
			initFindWindow();
			FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
			//静默调用
			pFind->markAllWord(signWord);
			}
		}
	}

void CCNotePad::slot_findResultPosChangeed(Qt::DockWidgetArea area)
{
	NddSetting::updataKeyValueFromNumSets(FINDRESULTPOS,area);
}

void  CCNotePad::initFindResultDockWin()
{
	//停靠窗口1
	if (m_dockSelectTreeWin == nullptr)
	{

		m_dockSelectTreeWin = new QDockWidget(tr("Find result"), this);
		connect(m_dockSelectTreeWin, &QDockWidget::dockLocationChanged, this, &CCNotePad::slot_findResultPosChangeed);

		m_dockSelectTreeWin->layout()->setMargin(0);
		m_dockSelectTreeWin->layout()->setSpacing(0);

		//暂时不提供关闭，因为关闭后需要同步菜单的check状态

		m_dockSelectTreeWin->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
		m_dockSelectTreeWin->setAllowedAreas(Qt::LeftDockWidgetArea| Qt::RightDockWidgetArea |Qt::BottomDockWidgetArea);

		m_pResultWin = new FindResultWin(m_dockSelectTreeWin);
		connect(m_pResultWin, &FindResultWin::itemDoubleClicked, this, &CCNotePad::slot_findResultItemDoubleClick);
		connect(m_pResultWin, &FindResultWin::showMsg, this, [this](QString& msg) {
			ui.statusBar->showMessage(msg,5000);
		});

		m_dockSelectTreeWin->setWidget(m_pResultWin);

		int lastArea = NddSetting::getKeyValueFromNumSets(FINDRESULTPOS);
		if (lastArea == 0)
		{
			lastArea = Qt::BottomDockWidgetArea;
	}

		addDockWidget((Qt::DockWidgetArea)lastArea, m_dockSelectTreeWin);
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

bool CCNotePad::convertDocLineEnd(RC_LINE_FORM endStatus)
{
	QWidget* pw = ui.editTabWidget->currentWidget();

	if (HEX_TYPE == getDocTypeProperty(pw))
	{
		ui.statusBar->showMessage(tr("Only Text File Can Use it, Current Doc is a Hex File !"), 10000);
		QApplication::beep();
		return false;
	}

	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
	if (pEdit != nullptr && pEdit->isReadOnly())
	{
		ui.statusBar->showMessage(tr("The ReadOnly document does not allow this operation."), 8000);
		QApplication::beep();
		return false;
	}

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
		return false;
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
	return true;
}


void CCNotePad::slot_gotoline()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

	if (pEdit != nullptr)
	{
		int lineCounts = 2147483647;

		bool ok;
		int num = QInputDialog::getInt(this, tr("Go to line"), tr("Line Num:"), 1, 1, lineCounts, 1, &ok);
		if (ok)
		{

			if (TXT_TYPE == getDocTypeProperty(pw))
			{
				pEdit->execute(SCI_GOTOLINE, num - 1);
		}
			else if (BIG_TEXT_RO_TYPE == getDocTypeProperty(pw))
			{
				//如果是大文本只读加载的，则逻辑不一样，需要根据行号定位到块，再定位到行
				int blockid = FileManager::getInstance().getBigFileBlockId(getFilePathProperty(pw), num - 1);
				if (blockid != -1)
				{
					BigTextEditFileMgr* mgr = FileManager::getInstance().getBigFileEditMgr(getFilePathProperty(pw));
					const BlockIndex& v = mgr->blocks.at(blockid);

					showBigTextFile(pEdit, mgr, blockid);

					int offsetLineNum = (num - v.lineNumStart);

					pEdit->execute(SCI_SETFIRSTVISIBLELINE, (long)offsetLineNum);

					pEdit->execute(SCI_GOTOLINE, offsetLineNum - 1);
					
	}
				else
				{
					BigTextEditFileMgr* mgr = FileManager::getInstance().getBigFileEditMgr(getFilePathProperty(pw));
					const BlockIndex& v = mgr->blocks.last();

					QApplication::beep();
					ui.statusBar->showMessage(tr("out of file line range,mar line num is %1 !").arg(v.lineNum + v.lineNumStart -1));
}
			}
			else
			{
				//超大文本不支持跳转行号，只支持跳转地址。先留着
			}

		}
	}
}

void CCNotePad::slot_show_line_end(bool checked)
{
	int showblank = s_showblank;
		if (checked)
		{
		showblank |= 0x2;
		}
		else
		{
		showblank &= 0xd;
		}
	changeBlankShowStatus(showblank);
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

		case BIG5:
			ui.actionBig5->setChecked(true);
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
		else
		{
			m_lexerNameToIndex.value("UserDefine").pAct->setChecked(true);
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
		else
		{
			QApplication::beep();
	}
	}
	else if (pw != nullptr && (SUPER_BIG_TEXT_RO_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		TextFileMgr *fileMgr = nullptr;
		//只有0才是成功
		if (0 == FileManager::getInstance().loadFilePreNextPage(1, filePath, fileMgr))
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			showBigTextFile(pEdit, fileMgr);
			pEdit->showBigTextLineAddr(fileMgr->fileOffset - fileMgr->contentRealSize, fileMgr->fileOffset);
	}
	}
	else if (pw != nullptr && (BIG_TEXT_RO_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		BigTextEditFileMgr* fileMgr = FileManager::getInstance().getBigFileEditMgr(filePath);
		if(fileMgr != nullptr)
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			
			int id = fileMgr->m_curBlockIndex - 1;
			showBigTextFile(pEdit, fileMgr, id);
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
	else if (pw != nullptr && (SUPER_BIG_TEXT_RO_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		TextFileMgr *fileMgr = nullptr;
		//只有0才是成功
		int ret = FileManager::getInstance().loadFilePreNextPage(2, filePath, fileMgr);
		if (0 == ret)
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			showBigTextFile(pEdit, fileMgr);
			pEdit->showBigTextLineAddr(fileMgr->fileOffset - fileMgr->contentRealSize, fileMgr->fileOffset);
	}
		else if (1 == ret)
		{
			ui.statusBar->showMessage(tr("The Last Page ! Current offset is %1 , load Contens Size is %2, File Total Size is %3").arg(fileMgr->fileOffset - fileMgr->contentRealSize).arg(fileMgr->contentRealSize).arg(fileMgr->fileSize));
			QApplication::beep();
		}
	}
	else if (pw != nullptr && (BIG_TEXT_RO_TYPE == getDocTypeProperty(pw)))
	{
		QString filePath = getFilePathProperty(pw);
		BigTextEditFileMgr* fileMgr = FileManager::getInstance().getBigFileEditMgr(filePath);
		if (fileMgr != nullptr)
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);

			int id = fileMgr->m_curBlockIndex + 1;
			showBigTextFile(pEdit, fileMgr, id);
}
	}
}

//菜单上面的GOTO按钮的执行槽函数
void CCNotePad::slot_gotoHexPage()
{
	if (m_pHexGotoWin.isNull())
	{
		m_pHexGotoWin = new HexFileGoto();

		HexFileGoto* pHexGoto = dynamic_cast<HexFileGoto*>(m_pHexGotoWin.data());

		pHexGoto->setAttribute(Qt::WA_DeleteOnClose);

		connect(pHexGoto, &HexFileGoto::gotoClick, this, &CCNotePad::slot_hexGotoFile);

		registerEscKeyShort(m_pHexGotoWin);
	}
	

	QWidget* pw = ui.editTabWidget->currentWidget();
	if (pw != nullptr && (HEX_TYPE == getDocTypeProperty(pw)))
	{
	m_pHexGotoWin.data()->activateWindow();
	m_pHexGotoWin.data()->show();

#ifdef uos
    adjustWInPos(m_pHexGotoWin.data());
#endif
	}
	else if (pw != nullptr && (SUPER_BIG_TEXT_RO_TYPE == getDocTypeProperty(pw)))
	{
		m_pHexGotoWin.data()->activateWindow();
		m_pHexGotoWin.data()->show();

#ifdef uos
		adjustWInPos(m_pHexGotoWin.data());
#endif
	}
	else if (pw != nullptr && (BIG_TEXT_RO_TYPE == getDocTypeProperty(pw)))
	{
		//这里直接按跳转到行号进行处理
		slot_gotoline();
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
	else if (pw != nullptr && (SUPER_BIG_TEXT_RO_TYPE == getDocTypeProperty(pw)))
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
			pEdit->showBigTextLineAddr(fileMgr->fileOffset - fileMgr->contentRealSize, fileMgr->fileOffset);
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
	QString msg = tr(R"(
<a href="https://gitee.com/cxasm/notepad--">https://gitee.com/cxasm/notepad--</a>
		<br/>
<a href="https://github.com/cxasm/notepad--">https://github.com/cxasm/notepad--</a>
	)");

#if defined (Q_OS_MAC)
	msgBox.setText(msg);
	msgBox.setDetailedText(QString("Notepad-- %1").arg(VersionStr));
#else
	msgBox.setWindowTitle(QString("Notepad-- %1").arg(VersionStr));
	msgBox.setText(msg);
#endif

	msgBox.exec();
}

void CCNotePad::slot_aboutNdd()
{
	AboutNdd* pWin = new AboutNdd();
	pWin->setAttribute(Qt::WA_DeleteOnClose);
	QString title = tr("Notepad-- Version %1").arg(VersionStr);
	pWin->setWindowTitle(title);
	pWin->appendText(title);
//	int status = NddSetting::getKeyValueFromNumSets(SOFT_STATUS);
//	if (1 == status)
//	{
//		pWin->appendText(tr("Registered Version"));
//	}
//	else
//	{
//		pWin->appendText(tr("Free Trial"));
//	}
	pWin->show();
	registerEscKeyShort(pWin);
#ifdef uos
	adjustWInPos(pWin);
#endif
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

	for (int i = 0; i < urls.size(); ++i)
	{
		QString fileName = urls.at(i).toLocalFile();

	if (fileName.isEmpty())
	{
				continue;
	}

	if (!QFile::exists(fileName))
	{
				continue;
	}

		QFileInfo fi(fileName);

		if (!fi.isFile())
		{
			ui.statusBar->showMessage(tr("%1 is not a file, skip open it...").arg(fileName));
			continue;
		}

	openFile(fileName);
	}
	
	e->accept();
}

//目前只有一个Tabwidget的双击事件。
bool CCNotePad::eventFilter(QObject * watched, QEvent * event)
{
	if (watched == ui.editTabWidget && event->type() == QEvent::MouseButtonDblClick)
	{
		slot_actionNewFile_toggle(true);
		m_saveFile->setEnabled(false);
		return true; // 注意这里一定要返回true，表示你要过滤该事件原本的实现
	}
	return false;
}


#ifdef Q_OS_WIN
static const ULONG_PTR CUSTOM_TYPE = 10000;
static const ULONG_PTR OPEN_NOTEPAD_TYPE = 10001;
static const ULONG_PTR CUSTOM_TYPE_FILE_LINENUM = 10002;

bool CCNotePad::nativeOpenfile(QString openFilePath)
{
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
	//窗口如果最小化，则在任务栏下面闪动
	QApplication::alert(this);

	//发现在release模式下，必须要先最小再最大，窗口才能跑到最前面。而调试时则没有该现象。可能是哪里有个问题。
	if (!this->isMinimized())
	{
		this->showMinimized();
	}

	if (this->isMaximized())
	{
		this->showMaximized();
	}
	else
	{
		this->showNormal();
	}

	this->activateWindow();
	return true;
}
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
				nativeOpenfile(openFilePath);
				*result = 1;
				return true;
			}
			else if (cds->dwData == OPEN_NOTEPAD_TYPE)
			{
				activateWindow();
				QApplication::alert(this);

				*result = 1;
				return true;
			}
			else if (cds->dwData == CUSTOM_TYPE_FILE_LINENUM)
			{
				QString openFilePath = QString::fromUtf8(reinterpret_cast<char*>(cds->lpData), cds->cbData);
				QStringList paraList = openFilePath.split("|");
				if (paraList.size() == 2)
				{
					nativeOpenfile(paraList.at(0));
					bool ok = true;
					int lineNum = paraList.at(1).toInt(&ok);
					if (ok)
					{
						QWidget* pw = ui.editTabWidget->currentWidget();
						ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
						if (pEdit != nullptr)
						{
							pEdit->execute(SCI_GOTOLINE, lineNum - 1);
						}
					}
				}
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
	registerEscKeyShort(newWin);
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
	registerEscKeyShort(newWin);
#ifdef uos
    adjustWInPos(newWin);
#endif
}

void CCNotePad::slot_options()
{
	OptionsView* p = new OptionsView(this,nullptr);
	p->setAttribute(Qt::WA_DeleteOnClose);
	//p->setWindowModality(Qt::ApplicationModal);
	connect(p, &OptionsView::sendTabFormatChange, this, &CCNotePad::slot_tabFormatChange);
	//connect(p, &OptionsView::signTxtFontChange, this, &CCNotePad::slot_txtFontChange);
	//connect(p, &OptionsView::signProLangFontChange, this, &CCNotePad::slot_proLangFontChange);

	p->show();
	registerEscKeyShort(p);
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

		if ((TXT_TYPE == docType) || (BIG_TEXT_RO_TYPE == docType))
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

		if ((TXT_TYPE == docType) || (BIG_TEXT_RO_TYPE == docType))
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

		if ((TXT_TYPE == docType) || (BIG_TEXT_RO_TYPE == docType) || (SUPER_BIG_TEXT_RO_TYPE == docType))
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
					pEdit->setIndentationsUseTabs(!ScintillaEditView::s_noUseTab);
				}
			}
		}
	}
}

void CCNotePad::slot_donate()
{
	Donate* pWin = new Donate();
	pWin->setAttribute(Qt::WA_DeleteOnClose);
	pWin->show();
	registerEscKeyShort(pWin);

#ifdef uos
    adjustWInPos(pWin);
#endif
}

// void CCNotePad::slot_registerCmd(int cmd, int code)
// {
//	if (cmd == 1)
//	{
//		//服务器返回注册码的消息。	//0 试用 1 正版 2 正版过期 3 错误key
//		int status = NddSetting::getKeyValueFromNumSets(SOFT_STATUS);
//		if (status != code)
//		{
//			NddSetting::updataKeyValueFromNumSets(SOFT_STATUS, code);
//		}
//
//		emit signRegisterReplay(code);
//	}
//}

//获取注册码
//void CCNotePad::slot_register()
//{
//}

//当前正在使用的所有语言的tags
void CCNotePad::getCurUseLexerTags(QVector<QString>& tags)
{
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr && (pEdit->lexer() != nullptr))
		{
			QString v = pEdit->lexer()->lexerTag();

			if (-1 == tags.indexOf(v))
			{
				tags.append(v);
			}
		}
	}
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
	connect(pWin, &QtLangSet::viewLexerChange, this, &CCNotePad::slot_viewLexerChange);
	pWin->show();
	registerEscKeyShort(pWin);
#ifdef uos
    adjustWInPos(pWin);
#endif
	pWin->selectInitLangTag(initTag);
}

void CCNotePad::slot_viewStyleChange(QString tag, int styleId, QColor& fgColor, QColor& bkColor, QFont& font, bool fontChange)
{
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr && (pEdit->lexer() != nullptr))
		{
			QsciLexer* lexer = pEdit->lexer();

			if (lexer->lexerTag() == tag)
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

void CCNotePad::slot_viewLexerChange(QString tag)
{
	int lexerId = -1;
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr && (pEdit->lexer() != nullptr))
		{
			QsciLexer* lexer = pEdit->lexer();

			
			if (lexer != nullptr && lexer->lexerTag() == tag)
			{
				lexerId = lexer->lexerId();
				delete lexer;
				autoSetDocLexer(pEdit, lexerId);
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

//2 非脏的老文件,直接打开
void CCNotePad::restoreCleanExistFile(QString& filePath)
{
	openTextFile(filePath);
	m_saveFile->setEnabled(false);
}

//3 脏的新建文件。内容在tempFilePath中
void CCNotePad::restoreDirtyNewFile(QString& fileName, QString& tempFilePath, int lexid)
{
	int index = FileManager::getInstance().getNextNewFileId();
	ScintillaEditView* pEdit = newTxtFile(fileName, index, tempFilePath);
	if (lexid != L_TXT)
	{
		QsciLexer* lexer = pEdit->createLexer(lexid);
		if (lexer != nullptr)
		{
			if (nullptr != pEdit->lexer())
			{
				delete pEdit->lexer();
			}

			pEdit->setLexer(lexer);
			syncCurDocLexerToMenu(pEdit);
		}
	}
	setTextChangeProperty(pEdit,true);
	m_saveFile->setEnabled(true);
	m_saveAllFile->setEnabled(true);

	int tabIndex = ui.editTabWidget->indexOf(pEdit);
	if (tabIndex != -1)
	{
		ui.editTabWidget->setTabIcon(tabIndex, QIcon(TabNeedSave));
}
}

//4 脏的老文件。内容在tempFilePath中
bool CCNotePad::restoreDirtyExistFile(QString& filePath, QString& tempFilePath)
{
	getRegularFilePath(filePath);

#if 0 //这里不需要判断了，大文件压根不能编辑，没有保存临时文件的说法
	QFileInfo fi(tempFilePath);
	//如果文件大于300M,按照只读文件打开
	if (ScintillaEditView::s_bigTextSize <= 0 || ScintillaEditView::s_bigTextSize > 300)
	{
		ScintillaEditView::s_bigTextSize = 100;
	}

	if (fi.size() > ScintillaEditView::s_bigTextSize * 1024 * 1024)
	{
		return openBigTextFile(tempFilePath);
	}
#endif
	QFileInfo fi(filePath);
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
	bool isLoadOrgin = false;//是否加载原始文件。只有临时文件被破坏或大小为空，才需要加载老的

	//如果非空，则从contentPath中加载文件内容。做恢复文件使用
	if (!tempFilePath.isEmpty())
	{
		int ret = FileManager::getInstance().loadFileDataInText(pEdit, tempFilePath, code, lineEnd, nullptr, false,this);
		if (ret != 0)
		{
			isLoadOrgin = true;
			ui.statusBar->showMessage(tr("Restore Last Temp File %1 Failed").arg(tempFilePath), 5000);
		}
		else
		{
			//如果是空的临时文件，则直接打开原始文件，临时文件估计被破坏或是空的，则直接读取原始文件
			if (ERROR_TYPE::OPEN_EMPTY_FILE == FileManager::getInstance().getLastErrorCode())
			{
				isLoadOrgin = true;
				FileManager::getInstance().resetLastErrorCode();
	}
	else
	{
				isChange = true;
			}
		}
	}

	if(isLoadOrgin)
	{

		int ret = FileManager::getInstance().loadFileDataInText(pEdit, filePath, code, lineEnd, this, true, this);
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
	
	disconnect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged);
	int curIndex = ui.editTabWidget->addTab(pEdit, QIcon((StyleSet::getCurrentSytleId() != DEEP_BLACK) ? TabNoNeedSave : TabNoNeedSaveDark32), getShortName(fileLabel));
	ui.editTabWidget->setCurrentIndex(curIndex);
	connect(ui.editTabWidget, &QTabWidget::currentChanged, this, &CCNotePad::slot_tabCurrentChanged, Qt::UniqueConnection);

	connect(pEdit, &ScintillaEditView::cursorPositionChanged, this, &CCNotePad::slot_LineNumIndexChange, Qt::QueuedConnection);
	enableEditTextChangeSign(pEdit);
	connect(pEdit, &ScintillaEditView::copyAvailable, this, &CCNotePad::slot_copyAvailable);
	connect(pEdit, SIGNAL(SCN_ZOOM()), this, SLOT(slot_zoomValueChange()));

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

	if (isChange)
	{
		ui.editTabWidget->setTabIcon(curIndex, QIcon(TabNeedSave));
	}
	
	QVariant editTextCode((int)code);
	pEdit->setProperty(Edit_Text_Code, editTextCode);

	syncCurDocEncodeToMenu(pEdit);
	syncCurDocLineEndStatusToMenu(pEdit);
	syncCurDocLexerToMenu(pEdit);

	//设置自动转换和缩进参考线
	if (s_autoWarp != QsciScintilla::WrapNone)
	{
		pEdit->setWrapMode(QsciScintilla::WrapCharacter);
	}

	setEditShowBlankStatus(pEdit, s_showblank);


	if (s_indent == 1)
	{
		pEdit->setIndentGuide(true);
	}

	if (s_zoomValue != 0)
	{
		pEdit->zoomTo(s_zoomValue);
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
	//从小到大排序一下。这里是按照ASCII排序，不得行。
	// 需要转换为数字0-N进行排序，否则排序结果错误。
	QList<int> fileIdList;
	for (int i = 0; i < fileList.size(); ++i)
	{
		fileIdList.append(fileList.at(i).toInt());
	}
	std::sort(fileIdList.begin(), fileIdList.end(), [](int& a, int& b) {
		return a < b;
		});

	int key;
	QString value;

	foreach(key, fileIdList)
	{
		value = qs.value(QString::number(key)).toString();
		if (!value.isEmpty())
		{
			bool ok = false;
			int type = value.right(1).toInt(&ok);

			if (!ok)
			{
				continue;
			}
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
			case 5:
			{
				//与3一样，不过需要恢复语法
				int pos = path.lastIndexOf("|");
				QString filePath = path.mid(0,pos);
				int lexerId = path.mid(pos + 1).toInt(&ok);
				if (!ok)
				{
					continue;
				}

				QString qsSavePath = qs.fileName();
				QFileInfo fi(qsSavePath);
				QString saveDir = fi.dir().absolutePath();
				QString tempFileName = QString("%1/%2").arg(saveDir).arg(key);
				restoreDirtyNewFile(filePath, tempFileName, lexerId);
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
	if (m_columnEditWin.isNull())
	{
		m_columnEditWin = new ColumnEdit();
		m_columnEditWin->setAttribute(Qt::WA_DeleteOnClose);
		ColumnEdit* pWin = dynamic_cast<ColumnEdit*>(m_columnEditWin.data());
	pWin->setTabWidget(ui.editTabWidget);
	}
	m_columnEditWin->show();
	registerEscKeyShort(m_columnEditWin);
#ifdef uos
    adjustWInPos(m_columnEditWin);
#endif
}

void CCNotePad::slot_defineLangs()
{
	LangStyleDefine* pWin = new LangStyleDefine(this);
	pWin->setAttribute(Qt::WA_DeleteOnClose);
	pWin->show();
	registerEscKeyShort(pWin);
#ifdef uos
    adjustWInPos(pWin);
#endif
}

void CCNotePad::transCurUpperOrLower(TextCaseType type)
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
	if (pEdit != nullptr)
	{
		if (pEdit->isReadOnly())
		{
			ui.statusBar->showMessage(tr("The ReadOnly document does not allow this operation."), 8000);
			QApplication::beep();
			return;
		}
		pEdit->convertSelectedTextTo(type);
	}
}

void CCNotePad::slot_uppercase()
{
	transCurUpperOrLower(UPPERCASE);
}
void CCNotePad::slot_lowercase()
{
	transCurUpperOrLower(LOWERCASE);
}
void CCNotePad::slot_properCase()
{
	transCurUpperOrLower(TITLECASE_FORCE);
}
void CCNotePad::slot_properCaseBlend()
{
	transCurUpperOrLower(TITLECASE_BLEND);
}
void CCNotePad::slot_sentenceCase()
{
	transCurUpperOrLower(SENTENCECASE_FORCE);
}
void CCNotePad::slot_sentenceCaseBlend()
{
	transCurUpperOrLower(SENTENCECASE_BLEND);
}
void CCNotePad::slot_invertCase()
{
	transCurUpperOrLower(INVERTCASE);
}
void CCNotePad::slot_randomCase()
{
	transCurUpperOrLower(RANDOMCASE);
}

void CCNotePad::slot_removeEmptyLine()
{
	removeEmptyLine(false);
}


void CCNotePad::slot_removeEmptyLineCbc()
{
	removeEmptyLine(true);
}

void CCNotePad::removeEmptyLine(bool isBlankContained)
{
	initFindWindow();
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	//静默调用
	pFind->removeEmptyLine(isBlankContained);
}

void CCNotePad::slot_column_mode()
{
	QMessageBox::about(this, tr("Column Edit Mode Tips"), tr("\"ALT+Mouse Click\" or \"Alt+Shift+Arrow keys\" Switch to mode!"));
}

void CCNotePad::slot_tabToSpace()
{
	spaceTabConvert(Tab2Space);
}

void CCNotePad::slot_spaceToTabAll()
{
	spaceTabConvert(Space2TabAll);
}

void CCNotePad::slot_spaceToTabLeading()
{
	spaceTabConvert(Space2TabLeading);
}

ScintillaEditView* CCNotePad::getCurEditView()
{
	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* _pEditView = dynamic_cast<ScintillaEditView*>(pw);
	if (_pEditView != nullptr)
	{
		if (_pEditView->isReadOnly())
		{
			ui.statusBar->showMessage(tr("The ReadOnly document does not allow this operation."), 8000);
			QApplication::beep();
			return nullptr;
		}
		return _pEditView;
	}
	return nullptr;
}

//tab space 互转
void CCNotePad::spaceTabConvert(SpaceTab type)
{

	QWidget* pw = ui.editTabWidget->currentWidget();
	ScintillaEditView* _pEditView = dynamic_cast<ScintillaEditView*>(pw);
	if (_pEditView != nullptr)
	{
		if (_pEditView->isReadOnly())
		{
			ui.statusBar->showMessage(tr("The ReadOnly document does not allow this operation."), 8000);
			QApplication::beep();
			return;
		}

		intptr_t tabWidth = _pEditView->execute(SCI_GETTABWIDTH);
		intptr_t currentPos = _pEditView->execute(SCI_GETCURRENTPOS);
		intptr_t docLength = _pEditView->execute(SCI_GETLENGTH) + 1;
		if (docLength < 2)
			return;

		intptr_t count = 0;
		intptr_t column = 0;
		intptr_t newCurrentPos = 0;
		intptr_t tabStop = tabWidth - 1;   // remember, counting from zero !
		bool onlyLeading = false;

		char * source = new char[docLength];
		if (source == NULL)
			return;
		_pEditView->execute(SCI_GETTEXT, docLength, reinterpret_cast<sptr_t>(source));

		if (type == Tab2Space)
		{
			// count how many tabs are there
			for (const char * ch = source; *ch; ++ch)
			{
				if (*ch == '\t')
					++count;
			}
			if (count == 0)
			{
				delete[] source;
				return;
			}
		}
		// allocate tabwidth-1 chars extra per tab, just to be safe
		size_t newlen = docLength + count * (tabWidth - 1) + 1;
		char * destination = new char[newlen];
		if (destination == NULL)
		{
			delete[] source;
			return;
		}
		char * dest = destination;

		switch (type)
		{
		case Tab2Space:
		{
			// rip through each line of the file
			for (int i = 0; source[i] != '\0'; ++i)
			{
				if (source[i] == '\t')
				{
					intptr_t insertTabs = tabWidth - (column % tabWidth);
					for (int j = 0; j < insertTabs; ++j)
					{
						*dest++ = ' ';
						if (i <= currentPos)
							++newCurrentPos;
					}
					column += insertTabs;
				}
				else
				{
					*dest++ = source[i];
					if (i <= currentPos)
						++newCurrentPos;
					if ((source[i] == '\n') || (source[i] == '\r'))
						column = 0;
					else if ((source[i] & 0xC0) != 0x80)  // UTF_8 support: count only bytes that don't start with 10......
						++column;
				}
			}
			*dest = '\0';
			break;
		}
		case Space2TabLeading:
		{
			onlyLeading = true;
		}
		case Space2TabAll:
		{
			bool nextChar = false;
			int counter = 0;
			bool nonSpaceFound = false;
			for (int i = 0; source[i] != '\0'; ++i)
			{
				if (nonSpaceFound == false)
				{
					while (source[i + counter] == ' ')
					{
						if ((column + counter) == tabStop)
						{
							tabStop += tabWidth;
							if (counter >= 1)        // counter is counted from 0, so counter >= max-1
							{
								*dest++ = '\t';
								i += counter;
								column += counter + 1;
								counter = 0;
								nextChar = true;
								if (i <= currentPos)
									++newCurrentPos;
								break;
							}
							else if (source[i + 1] == ' ' || source[i + 1] == '\t')  // if followed by space or TAB, convert even a single space to TAB
							{
								*dest++ = '\t';
								i++;
								column += 1;
								counter = 0;
								if (i <= currentPos)
									++newCurrentPos;
							}
							else       // single space, don't convert it to TAB
							{
								*dest++ = source[i];
								column += 1;
								counter = 0;
								nextChar = true;
								if (i <= currentPos)
									++newCurrentPos;
								break;
							}
						}
						else
							++counter;
					}

					if (nextChar == true)
					{
						nextChar = false;
						continue;
					}

					if (source[i] == ' ' && source[i + counter] == '\t') // spaces "absorbed" by a TAB on the right
					{
						*dest++ = '\t';
						i += counter;
						column = tabStop + 1;
						tabStop += tabWidth;
						counter = 0;
						if (i <= currentPos)
							++newCurrentPos;
						continue;
					}
				}

				if (onlyLeading == true && nonSpaceFound == false)
					nonSpaceFound = true;

				if (source[i] == '\n' || source[i] == '\r')
				{
					*dest++ = source[i];
					column = 0;
					tabStop = tabWidth - 1;
					nonSpaceFound = false;
				}
				else if (source[i] == '\t')
				{
					*dest++ = source[i];
					column = tabStop + 1;
					tabStop += tabWidth;
					counter = 0;
				}
				else
				{
					*dest++ = source[i];
					counter = 0;
					if ((source[i] & 0xC0) != 0x80)   // UTF_8 support: count only bytes that don't start with 10......
					{
						++column;

						if (column > 0 && column % tabWidth == 0)
							tabStop += tabWidth;
					}
				}

				if (i <= currentPos)
					++newCurrentPos;
			}
			*dest = '\0';
			break;
		}
		}

		_pEditView->execute(SCI_BEGINUNDOACTION);
		_pEditView->execute(SCI_SETTEXT, 0, reinterpret_cast<sptr_t>(destination));
		_pEditView->execute(SCI_GOTOPOS, newCurrentPos);

		_pEditView->execute(SCI_ENDUNDOACTION);

		// clean up
		delete[] source;
		delete[] destination;
	}
}


void CCNotePad::slot_dupCurLine()
{
	qDebug() << "dup atcion called";

	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView != nullptr)
	{
		_pEditView->execute(SCI_LINEDUPLICATE);
	}
}

void CCNotePad::slot_removeDupLine()
{
	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView != nullptr)
	{
		_pEditView->execute(SCI_BEGINUNDOACTION);
		_pEditView->removeAnyDuplicateLines();
		_pEditView->execute(SCI_ENDUNDOACTION);
	}
}

void CCNotePad::slot_splitLines()
{
	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView != nullptr)
	{
		if (_pEditView->execute(SCI_GETSELECTIONS) == 1)
		{
			std::pair<size_t, size_t> lineRange = _pEditView->getSelectionLinesRange();
			auto anchorPos = _pEditView->execute(SCI_POSITIONFROMLINE, lineRange.first);
			auto caretPos = _pEditView->execute(SCI_GETLINEENDPOSITION, lineRange.second);
			_pEditView->execute(SCI_SETSELECTION, caretPos, anchorPos);
			_pEditView->execute(SCI_TARGETFROMSELECTION);
			size_t edgeMode = _pEditView->execute(SCI_GETEDGEMODE);
			if (edgeMode == EDGE_NONE)
			{
				_pEditView->execute(SCI_LINESSPLIT, 0);
			}
			else
			{
				auto textWidth = _pEditView->execute(SCI_TEXTWIDTH, STYLE_DEFAULT, reinterpret_cast<sptr_t>("P"));
				auto edgeCol = _pEditView->execute(SCI_GETEDGECOLUMN); // will work for edgeMode == EDGE_BACKGROUND
				if (edgeMode == EDGE_MULTILINE)
				{
					//暂时这样。后续有问题再说
				}
				++edgeCol;  // compensate for zero-based column number
				_pEditView->execute(SCI_LINESSPLIT, textWidth * edgeCol);
			}
		}
	}
}
void CCNotePad::slot_joinLines()
{
	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView != nullptr)
	{
		const std::pair<size_t, size_t> lineRange = _pEditView->getSelectionLinesRange();
		if (lineRange.first != lineRange.second)
		{
			auto anchorPos = _pEditView->execute(SCI_POSITIONFROMLINE, lineRange.first);
			auto caretPos = _pEditView->execute(SCI_GETLINEENDPOSITION, lineRange.second);
			_pEditView->execute(SCI_SETSELECTION, caretPos, anchorPos);
			_pEditView->execute(SCI_TARGETFROMSELECTION);
			_pEditView->execute(SCI_LINESJOIN);
		}
	}
}

void CCNotePad::slot_moveUpCurLine()
{
	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView != nullptr)
	{
		_pEditView->execute(SCI_MOVESELECTEDLINESUP);
	}
}

void CCNotePad::slot_moveDownCurLine()
{
	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView != nullptr)
	{
		_pEditView->execute(SCI_MOVESELECTEDLINESDOWN);

		// Ensure the selection is within view
		_pEditView->execute(SCI_SCROLLRANGE, _pEditView->execute(SCI_GETSELECTIONEND), _pEditView->execute(SCI_GETSELECTIONSTART));
	}
}

void CCNotePad::slot_insertBlankAbvCur()
{
	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView != nullptr)
	{
		_pEditView->insertNewLineAboveCurrentLine();
}
}
void CCNotePad::slot_insertBlankBelCur()
{
	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView != nullptr)
	{
		_pEditView->insertNewLineBelowCurrentLine();
}
}


void CCNotePad::dealLineSort(LINE_SORT_TYPE type)
{

	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView == nullptr)
	{
		return;
}

	size_t fromLine = 0, toLine = 0;
	size_t fromColumn = 0, toColumn = 0;

	bool hasLineSelection = false;
	if (_pEditView->execute(SCI_GETSELECTIONS) > 1)
{
		if (_pEditView->execute(SCI_SELECTIONISRECTANGLE))
		{
			size_t rectSelAnchor = _pEditView->execute(SCI_GETRECTANGULARSELECTIONANCHOR);
			size_t rectSelCaret = _pEditView->execute(SCI_GETRECTANGULARSELECTIONCARET);
			size_t anchorLine = _pEditView->execute(SCI_LINEFROMPOSITION, rectSelAnchor);
			size_t caretLine = _pEditView->execute(SCI_LINEFROMPOSITION, rectSelCaret);
			fromLine = std::min(anchorLine, caretLine);
			toLine = std::max(anchorLine, caretLine);
			size_t anchorLineOffset = rectSelAnchor - _pEditView->execute(SCI_POSITIONFROMLINE, anchorLine) + _pEditView->execute(SCI_GETRECTANGULARSELECTIONANCHORVIRTUALSPACE);
			size_t caretLineOffset = rectSelCaret - _pEditView->execute(SCI_POSITIONFROMLINE, caretLine) + _pEditView->execute(SCI_GETRECTANGULARSELECTIONCARETVIRTUALSPACE);
			fromColumn = std::min(anchorLineOffset, caretLineOffset);
			toColumn = std::max(anchorLineOffset, caretLineOffset);
}
		else
{
			return;
}
	}
	else
{
		auto selStart = _pEditView->execute(SCI_GETSELECTIONSTART);
		auto selEnd = _pEditView->execute(SCI_GETSELECTIONEND);
		hasLineSelection = selStart != selEnd;
		if (hasLineSelection)
		{
			const  std::pair<size_t, size_t> lineRange = _pEditView->getSelectionLinesRange();
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
			toLine = _pEditView->execute(SCI_GETLINECOUNT) - 1;
}
	}

	LINE_SORT_TYPE id = type;

	bool isDescending = ((id == SORTLINES_LEXICOGRAPHIC_DESCENDING) || (id == SORTLINES_LEXICO_CASE_INSENS_DESCENDING));

	_pEditView->execute(SCI_BEGINUNDOACTION);
	std::unique_ptr<ISorter> pSorter;

	if (id == SORTLINES_LEXICOGRAPHIC_DESCENDING || id == SORTLINES_LEXICOGRAPHIC_ASCENDING)
{
		pSorter = std::unique_ptr<ISorter>(new LexicographicSorter(isDescending, fromColumn, toColumn));
}
	else if (id == SORTLINES_LEXICO_CASE_INSENS_DESCENDING || id == SORTLINES_LEXICO_CASE_INSENS_ASCENDING)
{
		pSorter = std::unique_ptr<ISorter>(new LexicographicCaseInsensitiveSorter(isDescending, fromColumn, toColumn));
}
	else if (id == SORTLINES_REVERSE_ORDER)
{
		pSorter = std::unique_ptr<ISorter>(new ReverseSorter(isDescending, fromColumn, toColumn));
}

	try
{
		_pEditView->sortLines(fromLine, toLine, pSorter.get());
}
	catch (size_t& failedLineIndex)
{
		size_t lineNo = 1 + fromLine + failedLineIndex;

		QMessageBox::warning(this, tr("SortingError"), tr("Unable to perform numeric sorting due to line %1.").arg(lineNo));
}

	_pEditView->execute(SCI_ENDUNDOACTION);

	if (hasLineSelection) // there was 1 selection, so we restore it
{
		auto posStart = _pEditView->execute(SCI_POSITIONFROMLINE, fromLine);
		auto posEnd = _pEditView->execute(SCI_GETLINEENDPOSITION, toLine);
		_pEditView->execute(SCI_SETSELECTIONSTART, posStart);
		_pEditView->execute(SCI_SETSELECTIONEND, posEnd);
}
}

void CCNotePad::slot_reverseLineOrder()
{
	dealLineSort(SORTLINES_REVERSE_ORDER);
}

void CCNotePad::slot_sortLexAsc()
{
	dealLineSort(SORTLINES_LEXICOGRAPHIC_ASCENDING);
}

void CCNotePad::slot_sortLexAscIgnCase()
{
	dealLineSort(SORTLINES_LEXICO_CASE_INSENS_ASCENDING);
}

void CCNotePad::slot_sortLexDesc()
{
	dealLineSort(SORTLINES_LEXICOGRAPHIC_DESCENDING);
}

void CCNotePad::slot_sortLexDescIngCase()
{
	dealLineSort(SORTLINES_LEXICO_CASE_INSENS_DESCENDING);
}

//这里是从F3 F4快捷按下时的查找槽函数。
void CCNotePad::slot_findNext()
{
	//先检查一下，当前查找窗口是否存在而且显示。存在则直接调用查找框的功能。
	//避免F3和查找框里面的干扰
	if (!m_pFindWin.isNull() && m_pFindWin->isVisible())
	{
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	pFind->setCurrentTab(FIND_TAB);
	pFind->findNext();
		return;
}

	int ret = initFindWindow();

	if (ret == 0)
	{
		FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
		pFind->setCurrentTab(FIND_TAB);
		pFind->findNext();
	}
	else
	{
		ui.statusBar->showMessage(tr("what find is null !"), 8000);
	}
}

void CCNotePad::slot_findPrev()
{
	//先检查一下，当前查找窗口是否存在而且显示。存在则直接调用查找框的功能。
	//避免F4和查找框里面的干扰
	if (!m_pFindWin.isNull() && m_pFindWin->isVisible())
	{
	FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
	pFind->setCurrentTab(FIND_TAB);
	pFind->findPrev();
		return;
}

	int ret = initFindWindow();
	if (ret == 0)
	{
		FindWin* pFind = dynamic_cast<FindWin*>(m_pFindWin.data());
		pFind->setCurrentTab(FIND_TAB);
		pFind->findPrev();
	}
	else
	{
		ui.statusBar->showMessage(tr("what find is null !"), 8000);
	}
}

//繁体BIG5
void CCNotePad::slot_load_with_big5()
{
	reloadTextFileWithCode(CODE_ID::BIG5);
}

//转成繁体BIG5
void CCNotePad::slot_encode_big5()
{
	transDocToEncord(BIG5);
}

//为窗口注册ECS退出快捷键
void CCNotePad::registerEscKeyShort(QWidget *parent)
{
	QShortcut *escSc = new QShortcut(parent);
	escSc->setKey(QKeySequence(Qt::Key_Escape));
	escSc->setContext(Qt::WidgetWithChildrenShortcut);
	connect(escSc, &QShortcut::activated, this, &CCNotePad::slot_escQuit);
}

void CCNotePad::slot_escQuit()
{
	QShortcut * escSc = dynamic_cast<QShortcut *>(sender());
	if (escSc != nullptr && escSc->parentWidget() != nullptr)
	{
		escSc->parentWidget()->close();
	}
}

//格式化xml语言
void CCNotePad::slot_formatXml()
{
	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView == nullptr || _pEditView->isReadOnly())
	{
		return;
	}

	QString text = _pEditView->text();
	if (text.isEmpty())
	{
		return;
	}

	QString xmlOut;

	QXmlStreamReader reader(text);
	QXmlStreamWriter writer(&xmlOut);
	writer.setAutoFormatting(true);

	while (!reader.atEnd()) {
		reader.readNext();
		if (!reader.isWhitespace()) {
			writer.writeCurrentToken(reader);
		}
	}

	if (reader.hasError()) 
	{
		ui.statusBar->showMessage(tr("XML format error, please check!"), 8000);
		QApplication::beep();
		return;
	}

	_pEditView->setText(xmlOut);
	
}

void CCNotePad::slot_formatJson()
{
	ScintillaEditView* _pEditView = getCurEditView();
	if (_pEditView == nullptr || _pEditView->isReadOnly())
	{
		return;
	}

	QString text = _pEditView->text();
	if (text.isEmpty())
	{
		return;
	}

	QByteArray bytes = text.toUtf8();

	QJsonParseError parseError;
	QJsonDocument doc = QJsonDocument::fromJson(bytes, &parseError);

	if (QJsonParseError::NoError != parseError.error)
	{
		ui.statusBar->showMessage(tr("JSON format error, please check!"),8000);
		QApplication::beep();
		return;
	}

	QString content(doc.toJson(QJsonDocument::Indented));
	_pEditView->setText(content);
}

//清空历史打开记录
void CCNotePad::slot_clearHistoryOpenList()
{

	ui.menuRecene_File->clear();
	m_receneOpenFileList.clear();

}

void CCNotePad::slot_showToolBar(bool check)
{
	ui.mainToolBar->setVisible(check);

	NddSetting::updataKeyValueFromNumSets(TOOLBARSHOW, check?1:0);
}

void CCNotePad::slot_showWebAddr(bool check)
{
	CCNotePad::s_hightWebAddr = check ? 1 : 0;

	NddSetting::updataKeyValueFromNumSets(SHOWWEBADDR, s_hightWebAddr);
}

//更新当前主题的样式
void CCNotePad::updateThemes()
{
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);

		int docType = getDocTypeProperty(pw);

		if (docType != HEX_TYPE)
		{
			ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
			if (pEdit != nullptr)
			{
				pEdit->updateThemes();
			}
		}
		else
		{
			ScintillaHexEditView* pEdit = dynamic_cast<ScintillaHexEditView*>(pw);
			if (pEdit != nullptr)
			{
				pEdit->updateThemes();
			}
		}
	}

	//切换主题后，如果存在查找框，则暴力关闭一下，因为查找框的颜色实时生效还存在问题
	if (m_dockSelectTreeWin != nullptr)
	{
		m_dockSelectTreeWin->deleteLater();
		m_dockSelectTreeWin = nullptr;
}
}

void CCNotePad::setGlobalFgColor(int style)
{
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr)
		{
			pEdit->setGlobalFgColor(style);
		}
	}
}

void CCNotePad::setGlobalBgColor(int style)
{
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr)
		{
			pEdit->setGlobalBgColor(style);
		}
	}
}

void CCNotePad::setGlobalFont(int style)
{
	for (int i = ui.editTabWidget->count() - 1; i >= 0; --i)
	{
		QWidget* pw = ui.editTabWidget->widget(i);
		ScintillaEditView* pEdit = dynamic_cast<ScintillaEditView*>(pw);
		if (pEdit != nullptr)
		{
			pEdit->setGlobalFont(style);
		}
	}
}

//文件后缀的编程语法关联
void CCNotePad::slot_langFileSuffix()
{
	LangExtSet* pWin = new LangExtSet(this);
	pWin->setAttribute(Qt::WA_DeleteOnClose);
#ifdef uos
    adjustWInPos(pWin);
#endif
	pWin->show();
}

//快捷键管理
void CCNotePad::slot_shortcutManager()
{
	ShortcutKeyMgr* pWin = new ShortcutKeyMgr(this);
	pWin->setAttribute(Qt::WA_DeleteOnClose);
#ifdef uos
    adjustWInPos(pWin);
#endif
	pWin->show();
}
