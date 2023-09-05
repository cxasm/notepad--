#pragma once

#include <QWidget>
#include <QListWidgetItem>

#include "ui_filelistview.h"

struct FileListItemData {
	QWidget* pEditWidget;
	QListWidgetItem* pListItem;

	FileListItemData() = default;
	FileListItemData(QWidget* pwid, QListWidgetItem* pItem) :pEditWidget(pwid),pListItem(pItem)
	{

	}
};

class FileListView : public QWidget
{
	Q_OBJECT

public:
	FileListView(QWidget *parent = nullptr);
	virtual ~FileListView();

	void setNotepadWin(QWidget* pNotepad);

	void delFileItem(QString & filePath);

	QWidget* getWidgetByFilePath(QString filePath);

	void addFileItem(QString& filePath, QWidget* edit);

	void setCurItem(QString filePath);
signals:
	void itemDoubleClicked(QListWidgetItem* item);

private slots:
	void slot_ShowPopMenu(const QPoint& pos);

private:
	Ui::FileListViewClass ui;

	QWidget* m_pNotepad;

	QMap<QString, FileListItemData> m_fileEditMap;
};
