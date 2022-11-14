#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QVector>


#include "ui_findwin.h"

enum FindTabIndex {
	FIND_TAB =0,
	REPLACE_TAB,
	DIR_FIND_TAB,
	MARK_TAB,
};
class ScintillaEditView;
class QsciScintilla;

struct FindRecord {
	int lineNum;
	int lineStartPos; //行开始位置
	int pos; //查找字段的开始位置
	int end; //查找字段的结束位置
	QString lineContents;
};

class FindRecords {
public:
	QString findFilePath;
	QString findText;
	//是否区分大小写
	//int caseSensitivity;
	//int totalMatch; //全词匹配

	ScintillaEditView* pEdit;
	QVector<FindRecord> records;
	FindRecords()
	{
		pEdit = nullptr;
	}
	~FindRecords()
	{
		pEdit = nullptr;
	}
};



class FindWin : public QMainWindow
{
	Q_OBJECT

public:
	FindWin(QWidget *parent = Q_NULLPTR);
	~FindWin();


	void setCurrentTab(FindTabIndex index);
	void setTabWidget(QTabWidget * editTabWidget);
	void setFindText(QString & text);
	void disableReplace();
	void setFindHistory(QList<QString>* findHistory);
	void markAllWord(QString& word);
	void removeLineHeadEndBlank(int mode);
	static void showCallTip(QsciScintilla * pEdit, int pos);
	void removeEmptyLine(bool isBlankContained);
	

protected:
	
	virtual void focusInEvent(QFocusEvent *ev);
	virtual void focusOutEvent(QFocusEvent *ev);
	virtual bool eventFilter(QObject * obj, QEvent * event);

	//bool eventFilter(QObject *, QEvent *);    //注意这里
signals:
	void sign_findAllInCurDoc(FindRecords* record);
	void sign_findAllInOpenDoc(QVector<FindRecords*>* record, int hits, QString findText);
	void sign_replaceSaveFile(QString filePath, ScintillaEditView * pEdit);
	void sign_clearResult();
	//void sign_markAllInCurDoc(FindRecords* record);

private:
	void updateParameterFromUI();
	void addFindHistory(QString & text);
	bool isFirstFind();
	void addCurFindRecord(ScintillaEditView * pEdit, FindRecords & recordRet, bool isMark=false);

	bool findTextInFile(QString & filePath, int & findNums, QVector<FindRecords*>* allfileInDirRecord);

	bool replaceTextInFile(QString & filePath, int & replaceNums, QVector<FindRecords*>* r = nullptr);

	int walkDirfile(QString path, int & foundTimes, bool isSkipBinary, bool isSkipHide, int skipMaxSize, bool isfilterFileType, QStringList & fileExtType, bool isSkipChildDirs, std::function<bool(QString&, int&, QVector<FindRecords*>*allfileInDirRecord)> foundCallBack, bool isAskAbort=true);

	QWidget* autoAdjustCurrentEditWin();

	void dealWithZeroFound(QsciScintilla * pEdit);

	void dealWithZeroFoundShowTip(QsciScintilla * pEdit, bool isShowTip=true);

	bool replaceFindNext(QsciScintilla* pEdit, bool showZeroFindTip);

	bool replace(ScintillaEditView* pEdit);
private slots:
	void slot_findNext();

	void slot_findCount();

	void slot_findAllInCurDoc();

	void slot_findAllInOpenDoc();

	void slot_replaceFindNext();

	void slot_replace();

	void slot_findModeRegularBtChange(bool checked);

	void slot_replaceModeRegularBtChange(bool checked);

	void slot_replaceAll();

	void slot_replaceAllInOpenDoc();

	void slot_markAll();

	void slot_clearMark();

	void slot_dirSelectDest();

	void slot_dealFileTypeChange(int state);

	void slot_dirFindAll();

	void slot_dirReplaceAll();

private:
	Ui::FindWin ui;

	QTabWidget *m_editTabWidget;

	//第一次查找，查找参数变化，认定为第一次查找
	bool m_isFindFirst;

	bool m_BackwardDir;
	bool m_matchWhole;
	bool m_matchCase;
	bool m_matchWrap;
	
	int m_searchMode; // 1 normal 2 regular expression 3 Exteral

	//find的参数
	QString m_expr;
	bool m_re;
	bool m_cs;
	bool m_wo;
	bool m_wrap;
	bool m_forward;//是否向前查找。注意如果向后，要为false
	bool m_line;
	bool m_index;
	bool m_extend;
	//bool m_show;
	//bool m_posix;
	//bool m_cxx11;

	bool m_isFound;

	QString m_replaceWithText;

	//上次点击的按钮id，如果两次点击的不一样
	//说明是不同的操作，如果是查找，则当前这次点击算是第一次查找
	QString m_lastClickBtName;

	QList<QString>* m_findHistory;

	ScintillaEditView* pEditTemp;


	QWidget* m_curEditWin;

	bool m_isStatic;//是否静默处理，不弹确认对话框
};
