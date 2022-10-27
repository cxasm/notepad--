#pragma once

#include <QWidget>
#include <QContextMenuEvent>
#include <QMenu>
#include "ui_findresultwin.h"

class FindRecords;
class QStandardItem;
class NdStyledItemDelegate;
class QStandardItemModel;
struct FindRecord;

class FindResultWin : public QWidget
{
	Q_OBJECT

public:
	FindResultWin(QWidget *parent = Q_NULLPTR);
	~FindResultWin();

	void appendResultsToShow(FindRecords * record);
	void appendResultsToShow(QVector<FindRecords*>* record, int hits, QString whatFind);

signals:
	void itemDoubleClicked(const QModelIndex &index);
	void showMsg(QString &msg);

private slots:
	void slot_clearContents();
	void slot_selectAll();
	void slot_copyContents();
	void slot_copyItemContents();
	void slot_selectSection();
public slots:
	void slot_clearAllContents();

protected:
	void contextMenuEvent(QContextMenuEvent * event);

private:
	void setItemBackground(QStandardItem* item, const QColor& color);
	void setItemForeground(QStandardItem * item, const QColor & color);

	void highlightFindText(int index, QString & srcText, QString & findText, Qt::CaseSensitivity cs);
	QString highlightFindText(FindRecord& record);
private:
	Ui::FindResultWin ui;
	QMenu *m_menu;
	QAction* m_pSelectSectonAct;

	QStandardItemModel* m_model;
	NdStyledItemDelegate* m_delegate;
};
