#pragma once

#include <QWidget>
#include <QContextMenuEvent>
#include <QMenu>
#include "ui_findresultwin.h"

class FindRecords;
struct FindRecord;

struct ResultLineInfo {
	int level;//缩进类型。0 1 2。0:title 1 filepath desc 2 result。
	int resultPos;//结果字段的开始offset。0和1是没有这个字段的。
	int resultEnd;//长度
	QString* pFilePath;
};

class FindResultWin : public QWidget
{
	Q_OBJECT

public:
	friend class FindResultView;
	FindResultWin(QWidget *parent = Q_NULLPTR);
	~FindResultWin();

	void appendResultsToShow(QVector<FindRecords*>* record, int hits, QString whatFind);
	int  getDefaultFontSize();
	void setDefaultFontSize(int defSize);
	void clear();
signals:
	void itemDoubleClicked(const QModelIndex &index);
	void showMsg(QString &msg);
	void lineDoubleClicked(QString* pFilePath, int pos, int end);

private slots:
	void on_lineDoubleClick(int line);
public slots:
	void slot_clearAllContents();

private:
#if 0
	void setItemBackground(QStandardItem* item, const QColor& color);
	void setItemForeground(QStandardItem * item, const QColor & color);

	void highlightFindText(int index, QString & srcText, QString & findText, Qt::CaseSensitivity cs);
	QString highlightFindText(FindRecord& record);
#endif
private:
	Ui::FindResultWin ui;
	QMenu *m_menu;
	QWidget* m_parent;

	QList<ResultLineInfo> m_resultLineInfo;
	QList<QString*> m_resultLineFilePath;

	int m_defaultFontSize;
	bool m_defFontSizeChange;
};
