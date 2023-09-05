#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QCloseEvent>
#include "ui_batchfindreplace.h"

class CCNotePad;

class BatchFindReplace : public QMainWindow
{
	Q_OBJECT

public:
	BatchFindReplace(QWidget *parent = nullptr);
	virtual ~BatchFindReplace();
	void setTabWidget(QTabWidget* editTabWidget);

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void on_freshBtClick();
	void on_findBtClick();
	void on_replaceBtClick();
	void on_swapFindReplace();
	void on_export();
	void on_import();
	void on_mark();
	void on_clearMark();

private:
	bool tranInputKeyword(QString& keyWord, QStringList& outputKeyWordList);
	void appendToFindReplaceTable(QStringList& findKeyword);
	void appendToFindTable(QString findKeyword);
	void insertToReplaceTable(int row, QString replaceKeyword);
	void insertToFindReplaceTable(QStringList& replaceKeyword);

	QWidget* autoAdjustCurrentEditWin();
private:
	Ui::BatchFindReplaceClass ui;

	QTabWidget* m_editTabWidget;
	QWidget* m_curEditWin;

	CCNotePad* m_mainNotepad;
};
