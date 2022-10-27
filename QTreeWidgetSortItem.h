#pragma once
#include <qtreewidget.h>
class QTreeWidgetSortItem : public QTreeWidgetItem
{
public:
	QTreeWidgetSortItem(int type);
	QTreeWidgetSortItem(const QStringList &strings, int type);
	virtual ~QTreeWidgetSortItem();
	virtual bool operator<(const QTreeWidgetItem &other) const override;

	static void setSortColumn(int index)
	{
		s_sortType = index;
	}
	static void setSyncOrder(bool v)
	{
		s_syncOrder = v;
	}
private :
	static int s_sortType;
	static bool s_syncOrder;
};

