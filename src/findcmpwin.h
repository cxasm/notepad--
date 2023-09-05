#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QVector>

#include "ui_findcmpwin.h"
#include "rcglobal.h"

#ifndef DEF_FIND_TAB_INDEX
#define DEF_FIND_TAB_INDEX

enum FindTabIndex {
	FIND_TAB,
	REPLACE_TAB,
};
#endif
class ScintillaEditView;

//struct FindCmpRecord {
//	int lineNum;
//	int pos;
//	QString lineContents;
//};
//
//class FindCmpRecords {
//public:
//	QString findFilePath;
//	QString findText;
//	ScintillaEditView* pEdit;
//	QVector<FindCmpRecord> records;
//	FindCmpRecords()
//	{
//		pEdit = nullptr;
//	}
//};


class QsciDisplayWindow;

class FindCmpWin : public QMainWindow
{
	Q_OBJECT

public:
	FindCmpWin(RC_DIRECTION dir, QWidget *parent = Q_NULLPTR);
	~FindCmpWin();

	
	void setCurrentTab(FindTabIndex index);
	void setWorkEdit(QsciDisplayWindow * editWidget);
	void setFindText(QString & text);
	void setFindHistory(QList<QString>* findHistory);

signals:
	void sgin_searchDirectionChange(RC_DIRECTION dir);
private:
	void updateParameterFromUI();
	void addFindHistory(QString & text);
	bool isFirstFind();
	//void addCurFindRecord(QsciDisplayWindow * pEdit, FindCmpRecords & recordRet);

private slots:
	void slot_findNext();
	void slot_findModeRegularBtChange(bool checked);
	void slot_isSearchLeft(bool checked);




private:
	Ui::FindCmpWin ui;

	QsciDisplayWindow *m_editWidget;

	//第一次查找，查找参数变化，认定为第一次查找
	bool m_isFindFirst;

	bool m_BackwardDir;
	bool m_matchWhole;
	bool m_matchCase;
	bool m_matchWrap;
	
	int m_searchMode; // 1 normal 2 regular expression

	//find的参数
	QString m_expr;
	bool m_re;
	bool m_cs;
	bool m_wo;
	bool m_wrap;
	bool m_forward;
	bool m_line;
	bool m_index;

	bool m_isFound;

	//上次点击的按钮id，如果两次点击的不一样
	//说明是不同的操作，如果是查找，则当前这次点击算是第一次查找
	QString m_lastClickBtName;
	QList<QString>* m_findHistory;
};
