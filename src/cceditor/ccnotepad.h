#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QToolButton>
#include <QMap>
#include <QCloseEvent>
#include <QActionGroup>
#include <qscilexer.h>
#include <QFileSystemWatcher>
#include <QPointer>
#include <QSharedMemory>
#include <QTranslator>
#include <QTimer>
#include <QSettings>
#include <QListWidgetItem>

#include "rcglobal.h"
#include "ui_ccnotepad.h"
#include "common.h"
#include "extlexermanager.h"
#include "scintillaeditview.h"
#include "findwin.h"
#include "pluginGl.h"


//class ScintillaEditView;
class ScintillaHexEditView;
class FindRecords;
class FindResultWin;
class QAction;
class CompareDirs;
class CompareWin;
struct HexFileMgr;
struct TextFileMgr;
struct BigTextEditFileMgr;


enum OpenAttr {
	Text = 1,
	HexReadOnly,
	BigTextReadOnly,
	BigTextReadWrite,
	SuperBigTextReadOnly,
	TextReadOnly
};

enum SpaceTab {
	Tab2Space = 0,
	Space2TabLeading,
	Space2TabAll,
};

enum LINE_SORT_TYPE {
	SORTLINES_LEXICOGRAPHIC_ASCENDING,
	SORTLINES_LEXICOGRAPHIC_DESCENDING,

	SORTLINES_LEXICO_CASE_INSENS_ASCENDING,
	SORTLINES_LEXICO_CASE_INSENS_DESCENDING,

	SORTLINES_REVERSE_ORDER,
};

struct FileExtLexer
{
	QString ext;
	LangType id;
};

const int FileExtMapLexerIdLen = L_EXTERNAL;

//1 文本 2 hex
enum NddDocType {
	TXT_TYPE = 1,
	//BIG_TEXT_RO_TYPE,//大文本，只读模式 BIG_TEXT_RO_TYPE。是只读模式
	BIG_TEXT_RO_TYPE,//大文本，只读模式,可以显示行号，可以跳转。理论上4G-8G比较合适。再大就属于超大文本
	BIG_EDIT_RW_TYPE,//大文本，读写模式。目前还不支持
	SUPER_BIG_TEXT_RO_TYPE,//超大文本，只读模式，理论上任意多大文件都可以。不一定支持行号。4G以上的文件。

	HEX_TYPE,
};

//打开模式。1 文本 2 二进制 3 大文本只读 4 文本只读
//const char* Open_Attr = "openid";
class FileListView;

class CCNotePad : public QMainWindow
{
	Q_OBJECT

public:
	CCNotePad(bool isMainWindows = true, QWidget *parent = Q_NULLPTR);
	~CCNotePad();

	void quickshow();

	void setCodeBarLabel(CODE_ID id);
	void setLineEndBarLabel(RC_LINE_FORM lineEnd);

    void initLexerNameToIndex();
	
	static LexerInfo getLangLexerIdByFileExt(QString filePath);
#if 0
	static QFont & getTxtFont()
	{
		return s_txtFont;
	}

	static QFont & getProLangFont()
	{
		return s_proLangFont;
	}
#endif
	
	bool openFile(QString filePath, int lineNum=-1);
	bool tryRestoreFile(QString filePath);

	void initTabNewOne();

	void setShareMem(QSharedMemory* v)
	{
		m_shareMem = v;
	}

	QSharedMemory* getShareMem()
	{
		return m_shareMem;
	}

	void receiveEditDrop(QDropEvent* e);
#ifdef Q_OS_WIN
	int runAsAdmin(const QString & filePath);
	void checkAppFont();
#endif

	void syncCurSkinToMenu(int id);

	int restoreLastFiles();

	ScintillaEditView * getCurEditView();
	void getCurUseLexerTags(QVector<QString>& tag);

	void clearHighlightWord(QString signWord, ScintillaEditView* pEdit = nullptr);
	bool closeFileByEditWidget(QWidget* pEdit);
	void showChangePageTips(QWidget* pEdit);
	int markAtBack(QString keyword);
	int findAtBack(QString keyword);
	int replaceAtBack(QStringList& keyword, QStringList& replace);
	void updateThemes();

	void setGlobalFgColor(int style);
	void setGlobalBgColor(int style);
	void setGlobalFont(int style);

	void changeMarkColor(int sytleId);
	void setUserDefShortcutKey(int shortcutId);
signals:
	void signSendRegisterKey(QString key);
	void signRegisterReplay(int code);
	void signLinkNetServer();
public slots:
	void slot_changeChinese();
	void slot_changeEnglish();
	
	void slot_actionNewFile_toggle(bool checked);
	void slot_actionOpenFile_toggle(bool checked);
	void slot_actionSaveFile_toggle(bool checked);
	void slot_actionSaveAsFile_toggle(bool checked);
	void slot_actionRenameFile_toggle(bool checked);
	void slot_actionClose(bool);
	void slot_actionCloseNonCurDoc();
	void slot_actionCloseLeftAll();
	void slot_actionCloseRightAll();
	void slot_quit(bool);

	void slot_closeAllFile(bool);
	void slot_batch_convert();
	void slot_batch_rename();
	void slot_options();
	void slot_donate();
    //	void slot_registerCmd(int cmd, int code);
	void slot_viewStyleChange(QString tag, int styleId, QColor & fgColor, QColor & bkColor, QFont & font, bool fontChange);
	void slot_viewLexerChange(QString tag);
	void slot_findInDir();
	void slot_showFileInExplorer();
	void slot_wordHighlight();

	void slot_clearWordHighlight();
	void slot_clearMark();
	void slot_zoomValueChange();


protected:
	void closeEvent(QCloseEvent *event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* e) override;
	bool eventFilter(QObject *watched, QEvent *event)override;
#ifdef Q_OS_WIN
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
	bool nativeOpenfile(QString openFilePath);
#endif
#ifdef uos
void adjustWInPos(QWidget* pWin);
#endif

private slots:
	void slot_editViewMofidyChange();
	void slot_tabClose(int index);
	void slot_LineNumIndexChange(int line, int index);
	void slot_saveAllFile();
	void slot_autoSaveFile(bool);
	void slot_timerAutoSave();

	void slot_tabCurrentChanged(int index);
	void slot_copyAvailable(bool select);
	void slot_cut();
	void slot_copy();
	void slot_paste();
	void slot_selectAll();
	void slot_undo();
	void slot_redo();
	void slot_zoomin();
	void slot_zoomout();
	void slot_wordwrap(bool checked);
	void slot_allWhite(bool checked);
	void slot_indentGuide(bool checked);
	void slot_find();


	void slot_replace();
	void slot_markHighlight();
	
	void slot_findResultPosChangeed(Qt::DockWidgetArea area);
	void slot_findResultItemDoubleClick(const QModelIndex & index);
	void slot_showFindAllInCurDocResult(FindRecords * record);
	void slot_showfindAllInOpenDocResult(QVector<FindRecords*>* record, int hits, QString whatFind);
	void slot_clearFindResult();
	void slot_convertWinLineEnd(bool);
	void slot_convertUnixLineEnd(bool);
	void slot_convertMacLineEnd(bool);
	void slot_openReceneFile();
	void slot_gotoline();
	void slot_show_spaces(bool check);
	void slot_show_line_end(bool check);
	void slot_load_with_gbk();
	void slot_load_with_utf8();
	void slot_load_with_utf8_bom();
	void slot_load_with_utf16_be();
	void slot_load_with_utf16_le();
	void slot_load_with_big5();
	void slot_encode_gbk();
	void slot_encode_utf8();
	void slot_encode_utf8_bom();
	void slot_encode_utf16_be();
	void slot_encode_utf16_le();
	void slot_encode_big5();

	void slot_lexerActTrig(QAction * action);
	void slot_compareFile();
	void slot_compareDir();
	void slot_binCompare();
	void slot_tabBarClicked(int index);
	void slot_reOpenTextMode();
	void slot_reOpenHexMode();
	void slot_selectLeftFile();
	void slot_selectRightFile();

	void slot_openFileInNewWin();
	void slot_about();
	void slot_aboutNdd();
	void slot_fileChange(QString filePath);
	void slot_tabBarDoubleClicked(int index);

	void slot_slectionChanged();
	void slot_preHexPage();
	void slot_nextHexPage();
	void slot_gotoHexPage();
	void slot_hexGotoFile(qint64 addr);
	void slot_tabFormatChange(bool tabLenChange, bool useTabChange);
	void slot_searchResultShow();
	void slot_saveFile(QString fileName, ScintillaEditView * pEdit);
	/*void slot_skinStyleGroup(QAction * action);*/
	void slot_changeIconSize(QAction * action);
	void slot_langFormat();

	void slot_removeHeadBlank();
	void slot_removeEndBlank();
	void slot_removeHeadEndBlank();
	void slot_columnBlockEdit();
	void slot_defineLangs();

	void slot_uppercase();
	void slot_lowercase();
	void slot_properCase();
	void slot_properCaseBlend();
	void slot_sentenceCase();
	void slot_sentenceCaseBlend();
	void slot_invertCase();
	void slot_randomCase();
	void slot_removeEmptyLineCbc();
	void slot_removeEmptyLine();
	void slot_column_mode();
	void slot_tabToSpace();
	void slot_spaceToTabAll();
	void slot_spaceToTabLeading();

	
	void slot_dupCurLine();
	void slot_removeDupLine();
	void slot_splitLines();
	void slot_joinLines();
	void slot_moveUpCurLine();
	void slot_moveDownCurLine();
	void slot_insertBlankAbvCur();
	void slot_insertBlankBelCur();
	
	void slot_reverseLineOrder();
	void slot_sortLexAsc();
	void slot_sortLexAscIgnCase();
	void slot_sortLexDesc();
	void slot_sortLexDescIngCase();

	void slot_findNext();
	void slot_findPrev();
	void slot_escQuit();
	void slot_formatXml();
	void slot_formatJson();

	void slot_clearHistoryOpenList();

	void slot_bookMarkAction();
	void slot_loadBookMarkMenu();
	void slot_markColorGroup(QAction * action);
	void slot_loadMarkColor();
	void slot_saveSearchHistory();
	void slot_fileListView(bool check);
	void slot_fileListItemDoubleClick(QListWidgetItem* item);
	void slot_showToolBar(bool);
	void slot_dynamicLoadToolMenu();
	void slot_batchFind();
#ifdef NO_PLUGIN
	void slot_pluginMgr();
	void onPlugWork(bool check);
	void sendParaToPlugin(NDD_PROC_DATA& procData);
#endif
	void slot_showWebAddr(bool check);
	void slot_langFileSuffix();
	void slot_shortcutManager();
	void on_lineEndChange(int index);

private:
	void initFindResultDockWin();
	void enableEditTextChangeSign(ScintillaEditView * pEdit);
	void disEnableEditTextChangeSign(ScintillaEditView * pEdit);
	bool saveFile(QString fileName, ScintillaEditView * pEdit, bool isBakWrite=true, bool isStatic=false, bool isClearSwpFile=false);
	void updateProAfterSaveNewFile(int curTabIndex, QString fileName, ScintillaEditView * pEdit);
	void setShoctIcon(int iconSize=24);
	void initToolBar();

	void setTxtLexer(ScintillaEditView * pEdit);
	void saveTabEdit(int tabIndex);
	void saveReceneOpenFile();
	void updateSaveAllToolBarStatus();
	void initReceneOpenFileMenu();
	
	int findFileIsOpenAtPad(QString filePath);
	bool isNewFileNameExist(QString& fileName);
	void updateCurTabSaveStatus();
	void setSaveButtonStatus(bool needSave);
	void setSaveAllButtonStatus(bool needSave);
	void tabClose(int index, bool isInQuit=false);
	void setDocEolMode(ScintillaEditView * pEdit, RC_LINE_FORM endStatus);
	bool convertDocLineEnd(RC_LINE_FORM endStatus);
	void transDocToEncord(CODE_ID destCode);

	void syncCurDocEncodeToMenu(QWidget * curEdit);
	void syncCurDocLineEndStatusToMenu(QWidget * curEdit);
	void syncCurDocLexerToMenu(QWidget* pEdit);

	void cmpSelectFile();

	void autoSetDocLexer(ScintillaEditView * pEdit, int defLexerId=-1);

	void updateTitleToCurDocFilePath();
	void addWatchFilePath(QString filePath);
	void removeWatchFilePath(QString filePath);

	bool checkRoladFile(ScintillaEditView * pEdit);
	void reloadEditFile(ScintillaEditView * pEidt);
	int initFindWindow(FindTabIndex type= FIND_TAB);

	void setToFileRightMenu();

	QString getShortName(const QString& name);

	bool reloadTextFileWithCode(CODE_ID code);

	bool openSuperBigTextFile(QString filePath);

	bool openBigTextRoFile(QString filePath);

	void setWindowTitleMode(QString filePath, OpenAttr attr);

	bool openTextFile(QString filePath, bool isCheckHex = true, CODE_ID code=CODE_ID::UNKOWN);
	bool openHexFile(QString filePath);
	
	bool showHexFile(ScintillaHexEditView * pEdit, HexFileMgr * hexFile);

	bool showBigTextFile(ScintillaEditView * pEdit, TextFileMgr * hexFile);
	bool showBigTextFile(ScintillaEditView* pEdit, BigTextEditFileMgr* txtFile, int blockIndex);

	void initNotePadSqlOptions();
	void saveNotePadSqlOptions();
	//void saveDefFont();
	void savePadUseTimes();
	void saveTempFile(ScintillaEditView * pEdit, int index, QSettings& qs);
	void closeFileStatic(int index, QSettings& qs);
	void closeAllFileStatic();

	void restoreCleanNewFile(QString & fileName);
	void restoreCleanExistFile(QString & filePath);
	void restoreDirtyNewFile(QString & fileName, QString & tempFilePath, int lexid=L_TXT);
	bool restoreDirtyExistFile(QString & fileName, QString & tempFilePath);
	
	ScintillaEditView* newTxtFile(QString Name, int index, QString contentPath="");
	void setLangsDescLable(QString &langDesc);
	void transCurUpperOrLower(TextCaseType type);
	void removeEmptyLine(bool isBlankContained);
	void spaceTabConvert(SpaceTab type);
	void dealLineSort(LINE_SORT_TYPE type);

	void find(FindTabIndex findType);

	void registerEscKeyShort(QWidget * parent);
	void closeAllFileWhenQuit(bool isQuit=false);
	void initFileListDockWin();
	void addFileListView(QString file, QWidget* pw);
	void delFileListView(QString file);
	void fileListSetCurItem(QString filePath);
	void syncFileTabToListView();
	void setZoomLabelValue(int zoomValue);
	void zoomto(int zoomValue);
	void tabClose(QWidget* pEdit);

	void init_toolsMenu();
	void changeBlankShowStatus(int showBlank);
	void syncBlankShowStatus();
#ifdef NO_PLUGIN
	void loadPluginLib();
	void loadPluginProcs(QString strLibDir, QMenu* pMenu);
	void onPlugFound(NDD_PROC_DATA& procData, QMenu* pUserData);
#endif

	void setUserDefShortcutKey();
	void setNormalTextEditInitPro(ScintillaEditView* pEdit, QString filePath, CODE_ID code, RC_LINE_FORM lineEnd, bool isReadOnly, bool isModifyed);
	void dealRecentFileMenuWhenColseFile(QString closeFilePath);
private:
	Ui::CCNotePad ui;

	QLabel* m_codeStatusLabel;
	QComboBox* m_lineEndLabel;
	QLabel* m_lineNumLabel;
	QLabel* m_langDescLabel;
	QLabel* m_zoomLabel;

	QMenu* m_tabRightClickMenu;

	QDockWidget* m_dockSelectTreeWin;
	FindResultWin* m_pResultWin;

	QPointer<QDockWidget> m_dockFileListWin;
	FileListView* m_fileListView;

	QList<QString> m_receneOpenFileList;

	QActionGroup *m_pEncodeActGroup;
	QActionGroup *m_pLineEndActGroup;
	QActionGroup *m_pLexerActGroup;
	QActionGroup *m_pIconSize;

	QAction* m_quitAction;

	//关闭时，询问保存文件，用户点击了取消，此时不能走关闭程序
	//设置一个标志记录该状态，确定最终是否要退出
	bool m_isQuitCancel;

    QMap<QString, LexerNode> m_lexerNameToIndex;

	//监控文件被修改的对象
	QFileSystemWatcher* m_fileWatch;

	QString m_cmpLeftFilePath;
	QString m_cmpRightFilePath;

	QPointer<QMainWindow> m_pFindWin;
	QPointer <QWidget> m_columnEditWin;

	QSharedMemory* m_shareMem;

	//是否主窗口。可以打开多个，但是只有第一个是主窗口
	bool m_isMainWindows;

	//最近打开的对比文件和目录列表。做一个环形区
//保存在数据库中
	int m_receneDirStartPos;
	int m_receneFileStartPos;
	QList<QAction*> m_receneDirList;
	QList<QAction*> m_receneFileList;

	QMap<QString, QAction*> m_receneRecrod;

	QAction* m_openInNewWinAct;
	QAction* m_showFileDirAct;

	QAction* m_openWithText;
	QAction* m_openWithHex;

	QAction* m_selectLeftCmp;
	QAction* m_selectRightCmp;


	//所有打开的notebook均保存起来。关闭时切换share里面保存的地址
	static QList<CCNotePad*> *s_padInstances;

	//当前打开的二进制文件，保存在这里
	QMap<QString, HexFileMgr*> m_hexFileMgr;

	QPointer<QWidget> m_pHexGotoWin;

	static QStringList s_findHistroy;
	static int s_padTimes;

	int m_curSoftLangs; //当前语言0:自动 1 中文 2 英语

	static int s_autoWarp; //自动换行
	static int s_indent; //自动缩进
	static int s_showblank; //显示空白
	static int s_zoomValue;
	
	
	QTranslator* m_translator;
	QTimer * m_timerAutoSave;

	QToolButton* m_newFile;
	QToolButton* m_openFile;
	QToolButton* m_saveFile;
	QToolButton* m_saveAllFile;
	QToolButton* m_autoSaveAFile;
	QToolButton* m_closeFile;
	QToolButton* m_closeAllFile;
	QToolButton* m_cutFile;
	QToolButton* m_copyFile;
	QToolButton* m_pasteFile;
	QToolButton* m_undo;
	QToolButton* m_redo;
	QToolButton* m_findText;
	QToolButton* m_replaceText;
	QToolButton* m_markText;
	QToolButton* m_signText;
	QToolButton* m_clearMark;
	QToolButton* m_zoomin;
	QToolButton* m_zoomout;
	QToolButton* m_wordwrap;
	QToolButton* m_allWhite;
	QToolButton* m_indentGuide;
	QToolButton* m_preHexPage;
	QToolButton* m_nextHexPage;
	QToolButton* m_gotoHexPage;

	QToolButton* m_transcode;
	QToolButton* m_rename;

	QAction* m_formatXml;
	QAction* m_formatJson;


	QPointer<QMainWindow> m_batchFindWin;

	int m_curIconSize;
	int m_curColorIndex;

	bool m_isInReloadFile;

	bool m_isToolMenuLoaded;

	bool m_isInitBookMarkAct;

	QList<QAction*>m_styleMarkActList;
	QList<NDD_PROC_DATA> m_pluginList;

public:
		static QString s_lastOpenDirPath;
	static int s_restoreLastFile; //自动恢复上次打开的文件
	static int s_curStyleId;
	static int s_curMarkColorId;
	static int s_hightWebAddr;//高亮网页地址
};

