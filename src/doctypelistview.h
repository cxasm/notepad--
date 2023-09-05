#pragma once

#include <QWidget>
#include <QMap>
#include <QString>
#include <QMenu>

#include "ui_doctypelistview.h"

class DocTypeListView : public QWidget
{
	Q_OBJECT

public:
	DocTypeListView(QWidget *parent = Q_NULLPTR);
	virtual ~DocTypeListView();

	static void initSupportFileTypes();
	static bool isSupportExt(QString ext);
	static bool isHexExt(QString ext);
	static QMap<QString,bool> * s_supportFileExts;

	//记录常见的16进制文件的结尾
	static QMap<QString,bool> * s_binFileExts;

protected:
	void save();

private slots:
	void slot_curRowChanged(int row);
	void slot_customContextMenuRequested(const QPoint& pos);
	void slot_addCustomType();
	void slot_add();
	void slot_sub();
	void slot_customListItemClicked(QListWidgetItem* item);
	void slot_typListItemClicked(QListWidgetItem* item);


private:
	Ui::DocTypeListView ui;

	static QStringList s_supportFileExt;
	static QStringList s_extBindFileType;

	QMenu *m_menu;

	bool m_isDirty; //是否修改过
};
