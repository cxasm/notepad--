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

#include "rcglobal.h"
#include "ui_ccnotepad.h"
#include "common.h"
#include "extlexermanager.h"
#include "scintillaeditview.h"
#include "findwin.h"


//class ScintillaEditView;
class ScintillaHexEditView;
class FindRecords;
class FindResultWin;
class QAction;
class CompareDirs;
class CompareWin;
struct HexFileMgr;
struct TextFileMgr;

#define uos 1

#ifdef Q_OS_WIN
#undef uos
#endif

#ifdef Q_OS_MAC
#undef uos
#endif

enum OpenAttr {
	Text = 1,
	HexReadOnly,
	BigTextReadOnly,
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



//打开模式。1 文本 2 二进制 3 大文本只读 4 文本只读
//const char* Open_Attr = "openid";


class CCNotePad : public QMainWindow
{
	Q_OBJECT

public:
	CCNotePad(bool isMainWindows = true, QWidget *parent = Q_NULLPTR);
	~CCNotePad();

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
	
	bool openFile(QString filePath);
	
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

signals:
	void signSendRegisterKey(QString key);
	void signRegisterReplay(int code);
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
	void slot_registerCmd(int cmd, int code);
	void slot_viewStyleChange(QString tag, int styleId, QColor & fgColor, QColor & bkColor, QFont & font, bool fontChange);
	void slot_viewLexerChange(QString tag);
	void slot_findInDir();
protected:
	void closeEvent(QCloseEvent *event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* e) override;
#ifdef Q_OS_WIN
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
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
	void slot_clearMark();
	void slot_wordHighlight();
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
	void slot_show_all(bool check);
	void slot_load_with_gbk();
	void slot_load_with_utf8();
	void slot_load_with_utf8_bom();
	void slot_load_with_utf16_be();
	void slot_load_with_utf16_le();
	void slot_encode_gbk();
	void slot_encode_utf8();
	void slot_encode_utf8_bom();
	void slot_encode_utf16_be();
	void slot_encode_utf16_le();
	void slot_lexerActTrig(QAction * action);
	void slot_compareFile();
	void slot_compareDir();
	void slot_binCompare();
	void slot_tabBarClicked(int index);
	void slot_reOpenTextMode();
	void slot_reOpenHexMode();
	void slot_selectLeftFile();
	void slot_selectRightFile();
	void slot_showFileInExplorer();
	void slot_openFileInNewWin();
	void slot_about();
	void slot_fileChange(QString filePath);

	void slot_toLightBlueStyle();
	void slot_toDefaultStyle();
	void slot_toThinBlueStyle();
	void slot_toRiceYellow();
	void slot_toYellow();
	void slot_toSilverStyle();
	void slot_toLavenderBlush();
	void slot_toMistyRose();
	void slot_register();
	void slot_slectionChanged();
	void slot_preHexPage();
	void slot_nextHexPage();
	void slot_gotoHexPage();
	void slot_hexGotoFile(qint64 addr);
	void slot_tabFormatChange(bool tabLenChange, bool useTabChange);
	void slot_searchResultShow();
	//void slot_txtFontChange(QFont & font);
	//void slot_proLangFontChange(QFont &font);
	void slot_saveFile(QString fileName, ScintillaEditView * pEdit);
	void slot_skinStyleGroup(QAction * action);
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


private:
	void initFindResultDockWin();
	void enableEditTextChangeSign(ScintillaEditView * pEdit);
	void disEnableEditTextChangeSign(ScintillaEditView * pEdit);
	bool saveFile(QString fileName, ScintillaEditView * pEdit, bool isBakWrite=true, bool isStatic=false, bool isClearSwpFile=false);
	void updateProAfterSaveNewFile(int curTabIndex, QString fileName, ScintillaEditView * pEdit);
	void initToolBar();

	void setTxtLexer(ScintillaEditView * pEdit);
	
	void saveReceneOpenFile();
	void updateSaveAllToolBarStatus();
	void initReceneOpenFileMenu();
	
	int findFileIsOpenAtPad(QString filePath);
	void updateCurTabSaveStatus();
	void setSaveButtonStatus(bool needSave);
	void setSaveAllButtonStatus(bool needSave);
	void setDocEolMode(ScintillaEditView * pEdit, RC_LINE_FORM endStatus);
	void convertDocLineEnd(RC_LINE_FORM endStatus);
	void transDocToEncord(CODE_ID destCode);

	void syncCurDocEncodeToMenu(QWidget * curEdit);
	void syncCurDocLineEndStatusToMenu(QWidget * curEdit);
	void syncCurDocLexerToMenu(QWidget* pEdit);

	void cmpSelectFile();

	void autoSetDocLexer(ScintillaEditView * pEdit);

	void updateTitleToCurDocFilePath();
	void addWatchFilePath(QString filePath);
	void removeWatchFilePath(QString filePath);

	bool checkRoladFile(ScintillaEditView * pEdit);
	void reloadEditFile(ScintillaEditView * pEidt);
	void initFindWindow();
	void setToFileRightMenu();


	QString getShortName(const QString& name);

	bool reloadTextFileWithCode(CODE_ID code);

	bool openBigTextFile(QString filePath);

	void setWindowTitleMode(QString filePath, OpenAttr attr);

	bool openTextFile(QString filePath, bool isCheckHex = true, CODE_ID code=CODE_ID::UNKOWN);
	bool openHexFile(QString filePath);
	bool showHexFile(ScintillaHexEditView * pEdit, HexFileMgr * hexFile);

	bool showBigTextFile(ScintillaEditView * pEdit, TextFileMgr * hexFile);

	void initNotePadSqlOptions();
	void saveNotePadSqlOptions();
	//void saveDefFont();
	void savePadUseTimes();
	void saveTempFile(ScintillaEditView * pEdit, int index, QSettings& qs);
	void closeFileStatic(int index, QSettings& qs);
	void closeAllFileStatic();

	void restoreCleanNewFile(QString & fileName);
	void restoreCleanExistFile(QString & filePath);
	void restoreDirtyNewFile(QString & fileName, QString & tempFilePath);
	bool restoreDirtyExistFile(QString & fileName, QString & tempFilePath);
	
	ScintillaEditView* newTxtFile(QString Name, int index, QString contentPath="");
	void setLangsDescLable(QString &langDesc);
	void transCurUpperOrLower(TextCaseType type);
	void removeEmptyLine(bool isBlankContained);
	void spaceTabConvert(SpaceTab type);
	void dealLineSort(LINE_SORT_TYPE type);

	void find(FindTabIndex findType);
private:
	Ui::CCNotePad ui;

	QLabel* m_codeStatusLabel;
	QLabel* m_lineEndLabel;
	QLabel* m_lineNumLabel;
	QLabel* m_langDescLabel;

	QToolButton* m_saveFile;
	QToolButton* m_saveAllFile;
	QToolButton* m_autoSaveAFile;
	QToolButton* m_cutFile;
	QToolButton* m_copyFile;
	QToolButton* m_undo;
	QToolButton* m_redo;

	QMenu* m_tabRightClickMenu;

	QDockWidget* m_dockSelectTreeWin;
	FindResultWin* m_pResultWin;

	//一个用于查找，一个用于排序
	QMap <QString,QAction*> m_receneOpenFile;
	QList<QString> m_receneOpenFileList;

	QActionGroup *m_pEncodeActGroup;
	QActionGroup *m_pLineEndActGroup;
	QActionGroup *m_pLexerActGroup;

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

	QToolButton* m_wordwrap;
	QToolButton* m_allWhite;
	QToolButton* m_indentGuide;

	//所有打开的notebook均保存起来。关闭时切换share里面保存的地址
	static QList<CCNotePad*> *s_padInstances;

	//当前打开的二进制文件，保存在这里
	QMap<QString, HexFileMgr*> m_hexFileMgr;

	QPointer<QWidget> m_pHexGotoWin;

	QToolButton* m_preHexPage;
	QToolButton* m_nextHexPage;
	QToolButton* m_gotoHexPage;

	static QList<QString> s_findHistroy;

	static int s_padTimes;

	//static QFont s_txtFont;

	//static QFont s_proLangFont;

	int m_curSoftLangs; //当前语言0:自动 1 中文 2 英语

	static int s_autoWarp; //自动换行
	static int s_indent; //自动缩进
	
	//QString m_txtFontStr;//从数据库读取的字体字符串
	//QString m_proLangFontStr;//从数据库读取的字体字符串

	QTranslator* m_translator;

	QTimer * m_timerAutoSave;

	int m_initWidth;
	int m_initHeight;

public:
		static QString s_lastOpenDirPath;
	static int s_restoreLastFile; //自动恢复上次打开的文件

};

