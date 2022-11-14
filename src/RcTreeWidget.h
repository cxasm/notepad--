#pragma once
#include <qtreewidget.h>
#include <QPoint>
#include <functional>
#include <QMenu>

#include "rcglobal.h"

class MediatorFileTree;

class RcTreeWidget :public QTreeWidget
{
	
public:
	RcTreeWidget(QWidget *parent=nullptr);
	~RcTreeWidget();

	void setContextUserDefineItemMenuCallBack(std::function<void(int dire, QMenu*, QTreeWidgetItem*)>* userAddMenu);

	void setDirection(RC_DIRECTION direction);
	void setMediator(MediatorFileTree * mediator);
	void setVerticalValue(int value);

	void setRootDir(QString dir)
	{
		m_rootDir = dir;
	}

	QString getRootDir()
	{
		return m_rootDir;
	}

public slots:
	void slot_scrollValueChange(int value);
	void slot_itemCollapsed(QTreeWidgetItem * item);
	void slot_itemExpanded(QTreeWidgetItem * item);
	void slot_ShowPopMenu(const QPoint& pos);
private:
	MediatorFileTree * m_mediator;
	RC_DIRECTION m_direction;
	QString m_rootDir;

	std::function<void(int dire, QMenu*, QTreeWidgetItem*)>* m_userAddMenu;
};

