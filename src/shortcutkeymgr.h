#pragma once

#include <QMainWindow>
#include "ui_shortcutkeymgr.h"

enum Shortcut_Key_ID {
	New_File_ID=0,
	Open_File_ID,
	Save_File_ID,
	Save_All_File_ID,
	Close_ID,
	Close_All_ID,
	Cut_ID,
	Copy_ID,
	Paste_ID,
	Undo_ID,
	Redo_ID,
	Find_ID,
	Replace_ID,
	Dir_Find_ID,
	Mark_ID,
	Word_highlight_ID,
	Clear_all_highlight_ID,
	Zoom_In_ID,
	Zoom_Out_ID,
	Word_Wrap_ID,
	Show_Blank_ID,
	Indent_Guide_ID,
	Pre_Page_ID,
	Next_Page_ID,
	Goto_Page_ID,
	File_Compare_ID,
	Dir_Compare_ID,
	Bin_Compare_ID,
	Trans_code_ID,
	Batch_rename_ID,
	Format_Xml_ID,
	Format_Json_ID,
	ADD_DEL_LINE_COM,
	ADD_BK_COM,
	DEL_BK_COM,
	S_K_FOLD_1,
	S_K_FOLD_2,
	S_K_FOLD_3,
	S_K_FOLD_4,
	S_K_FOLD_5,
	S_K_FOLD_6,
	S_K_FOLD_7,
	S_K_FOLD_8,
	S_K_UNFOLD_1,
	S_K_UNFOLD_2,
	S_K_UNFOLD_3,
	S_K_UNFOLD_4,
	S_K_UNFOLD_5,
	S_K_UNFOLD_6,
	S_K_UNFOLD_7,
	S_K_UNFOLD_8,

	Shortcut_End_ID,//总是在最底下
};

static const char* New_File = "newfile";
static const char* Open_File = "openfile";
static const char* Save_File = "savefile";
static const char* Save_All_File = "saveall";
static const char* Close = "close";
static const char* Close_All = "closeall";
static const char* Cut = "cut";
static const char* Copy = "copy";
static const char* Paste = "paste";
static const char* Undo = "undo";
static const char* Redo = "redo";
static const char* Find = "find";
static const char* Replace = "replace";
static const char* DirFind = "dirfind";
static const char* Mark = "mark";
static const char* Word_highlight = "wordlight";
static const char* Clear_all_highlight = "clearwordlight";
static const char* Zoom_In = "zoomin";
static const char* Zoom_Out ="zoomout";
static const char* Word_Wrap = "wordwrap";
static const char* Show_Blank = "showblank";
static const char* Indent_Guide = "indentguide";
static const char* Pre_Page = "prepage";
static const char* Next_Page = "nextpage";
static const char* Goto_Page = "goto";
static const char* File_Compare = "filecmp";
static const char* Dir_Compare ="dircmp";
static const char* Bin_Compare ="bincmp";
static const char* Trans_code = "transcode";
static const char* Batch_rename = "batchrename";
static const char* Format_Xml = "formatxml";
static const char* Format_Json = "formatjson";

static const char* ADD_DELETE_LINE_COMMENT = "linecom";
static const char* ADD_BLOCK_COMMENT = "bkcom";
static const char* CANCEL_BLOCK_COMMENT = "cbkcom";

static const char* FOLD_1 = "fold_1";
static const char* FOLD_2 = "fold_2";
static const char* FOLD_3 = "fold_3";
static const char* FOLD_4 = "fold_4";
static const char* FOLD_5 = "fold_5";
static const char* FOLD_6 = "fold_6";
static const char* FOLD_7 = "fold_7";
static const char* FOLD_8 = "fold_8";
static const char* UNFOLD_1 = "unfold_1";
static const char* UNFOLD_2 = "unfold_2";
static const char* UNFOLD_3 = "unfold_3";
static const char* UNFOLD_4 = "unfold_4";
static const char* UNFOLD_5 = "unfold_5";
static const char* UNFOLD_6 = "unfold_6";
static const char* UNFOLD_7 = "unfold_7";
static const char* UNFOLD_8 = "unfold_8";

class ShortcutKeyMgr : public QMainWindow
{
	Q_OBJECT

public:
	ShortcutKeyMgr(QWidget *parent = nullptr);
	~ShortcutKeyMgr();
	static void initShortcutKeysMap();
	static QKeySequence getUserDefShortcutKey(QString iniTag);

private:
	
	void initNddShortcutTable();
	void initQscintShortcutTable();

	static void loadUserDefSet();
	int isKeySeqExist(int row, QString keySeq, int& type);
	bool ModifyShortCutKey(QString initTag, QString keySeqStr);
private slots:
	void slot_edit(QTableWidgetItem* item);

private:
	Ui::ShortcutKeyMgrClass ui;
	static QMap<QString, int>* s_shortcutKeysMap;

	QWidget* m_pNoteEdit;
};
