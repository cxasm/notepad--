#include "QTreeWidgetSortItem.h"
#include "rcglobal.h"

/* 自己重新实现一个QTreeWidgetSortItem，主要是为了重载函数的排序功能，不适应默认的名字排序，而是使用
* 每个项自带的tip字符串排序。这样做是因为对齐的时候，有些空白对齐项目，并没有名称，就会导致无法排序
*/
int QTreeWidgetSortItem::s_sortType = 0;

bool QTreeWidgetSortItem::s_syncOrder = false;

QTreeWidgetSortItem::QTreeWidgetSortItem(int type):QTreeWidgetItem(type)
{

}

QTreeWidgetSortItem::QTreeWidgetSortItem(const QStringList &strings, int type):QTreeWidgetItem(strings, type)
{

}

QTreeWidgetSortItem::~QTreeWidgetSortItem()
{

}

bool QTreeWidgetSortItem::operator<(const QTreeWidgetItem & other) const
{
	if (s_sortType == 0)
	{
		if (!s_syncOrder) //按名称排序
		{
	//目录最大，放在文件前面。都是目录则按照名称排序
	if ((type() == RC_DIR) && (other.type() == RC_DIR))
	{
		goto cmp_name;
	}
	else if ((type() == RC_DIR) && (other.type() != RC_DIR))
	{
		return false;
	}
	else if ((type() != RC_DIR) && (other.type() == RC_DIR))
	{
		return true;
	}

cmp_name:

	QString a = this->data(0, Item_RelativePath).toString();
	QString b = other.data(0, Item_RelativePath).toString();

			return (a.compare(b, Qt::CaseInsensitive) > 0);
}
		else
		{
			//同步序号，按序号进行排序
#if 0
			if ((type() == RC_DIR) && (other.type() == RC_DIR))
			{
				QString a = this->data(0, Item_RelativePath).toString();
				QString b = other.data(0, Item_RelativePath).toString();

				return !(a.compare(b, Qt::CaseInsensitive) > 0);
			}
			else if ((type() == RC_DIR) && (other.type() != RC_DIR))
			{
				return true;
			}
			else if ((type() != RC_DIR) && (other.type() == RC_DIR))
			{
				return false;
			}

		cmp_index:
#endif
			//同步对方的操作。
			int a = this->data(0, Item_Index).toInt();
			int b = other.data(0, Item_Index).toInt();

			return(a > b);
		}
	}
	else if (s_sortType == 1) //按大小排序
	{
		//目录最大，放在文件前面。都是目录则按照名称排序
		if ((type() == RC_DIR) && (other.type() == RC_DIR))
		{

			qint64 a = this->data(0, DIR_ITEM_MAXSIZE_FILE).toULongLong();
			qint64 b = other.data(0, DIR_ITEM_MAXSIZE_FILE).toULongLong(); 

			return(a > b);
#if 0
			QString a = this->data(0, Item_RelativePath).toString();
			QString b = other.data(0, Item_RelativePath).toString();

			return (a.compare(b, Qt::CaseInsensitive) > 0);
#endif
		}
		else if ((type() == RC_DIR) && (other.type() != RC_DIR))
		{
			return false;
		}
		else if ((type() != RC_DIR) && (other.type() == RC_DIR))
		{
			return true;
		}

		int leftSize = this->text(1).toInt();
		int rightSize = other.text(1).toInt();

		return(leftSize > rightSize);
	}
	else if (s_sortType == 2) //by 修改日期
	{
		//目录最大，放在文件前面。都是目录则按照名称排序
		if ((type() == RC_DIR) && (other.type() == RC_DIR))
		{
			QString a = this->data(0, Item_RelativePath).toString();
			QString b = other.data(0, Item_RelativePath).toString();

			return (a.compare(b, Qt::CaseInsensitive) > 0);
		}
		else if ((type() == RC_DIR) && (other.type() != RC_DIR))
		{
			return false;
		}
		else if ((type() != RC_DIR) && (other.type() == RC_DIR))
		{
			return true;
		}

		QString a = this->text(2);
		QString b = other.text(2);

		return(a.compare(b, Qt::CaseInsensitive) > 0);
	}
	return false;
}
