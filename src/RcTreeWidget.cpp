#include "RcTreeWidget.h"
#include "MediatorFileTree.h"

#include <QScrollBar>
#include <QMenu>
#include <QProcess>

RcTreeWidget::RcTreeWidget(QWidget *parent):QTreeWidget(parent), m_userAddMenu(nullptr)
{

	connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &RcTreeWidget::slot_scrollValueChange);

	//收起和伸开子项
	connect(this, &QTreeWidget::itemCollapsed, this, &RcTreeWidget::slot_itemCollapsed);
	connect(this, &QTreeWidget::itemExpanded, this, &RcTreeWidget::slot_itemExpanded);

	setContextMenuPolicy(Qt::CustomContextMenu);  //设置枚举值
	connect(this, &QTreeWidget::customContextMenuRequested, this, &RcTreeWidget::slot_ShowPopMenu);
}

RcTreeWidget::~RcTreeWidget()
{

}

//给用户进行菜单增加的回调函数
void RcTreeWidget::setContextUserDefineItemMenuCallBack(std::function<void(int dire,QMenu*, QTreeWidgetItem*)>* userAddMenu)
{
	m_userAddMenu = userAddMenu;
}


//右键菜单
void RcTreeWidget::slot_ShowPopMenu(const QPoint& pos)
{
	QTreeWidgetItem* curItem = this->itemAt(pos);
	if (curItem != nullptr)
	{
		
		QMenu* menu = new QMenu(this);

		if (m_userAddMenu != nullptr)
		{
			(*m_userAddMenu)(m_direction, menu, curItem);
		}

		QAction* action = menu->addAction(tr("Show File in Explorer"), this, [&]() {
			QString path, cmd;

			path = QString("%1/%2").arg(m_rootDir).arg(curItem->data(0, Qt::ToolTipRole).toString());
			#ifdef _WIN32
				path = path.replace("/", "\\");
				cmd = QString("explorer.exe /select,%1").arg(path);
			#else
				path = path.replace("\\", "/");
				cmd = QString("open -R %1").arg(path);
			#endif
				QProcess process;
				process.startDetached(cmd);
			});

		//没有名称表示是对齐的item，不存在对应的文件，只是占位
		if (curItem->text(0).isEmpty())
		{
			action->setEnabled(false);
		}

		if (menu)
		{
			menu->setAttribute(Qt::WA_DeleteOnClose);
			menu->exec(QCursor::pos());
		}
	}
}


//点击收起的槽函数
void RcTreeWidget::slot_itemCollapsed(QTreeWidgetItem *item)
{
	QString name = item->data(0, Qt::ToolTipRole).toString();
	//左边变化，通知右边去改变
	if (m_direction == RC_LEFT)
	{
		//通知右边去收起
		m_mediator->setRightItemStatus(name,RC_COLLAPSED);
	}
	else
	{
		//通知左边去收起
		m_mediator->setLeftItemStatus(name, RC_COLLAPSED);
	}
}

//点击展开的槽函数
void RcTreeWidget::slot_itemExpanded(QTreeWidgetItem *item)
{
	QString name = item->data(0, Qt::ToolTipRole).toString();

	//左边变化，通知右边去改变
	if (m_direction == RC_LEFT)
	{
		//右边展开
		m_mediator->setRightItemStatus(name, RC_EXPANDED);
	}
	else
	{
		//左边收起
		m_mediator->setLeftItemStatus(name, RC_EXPANDED);
	}
}

void RcTreeWidget::setDirection(RC_DIRECTION direction)
{
	m_direction = direction;
}

void RcTreeWidget::setMediator(MediatorFileTree *mediator)
{
	m_mediator = mediator;

}

//滚动条值变化后的槽函数。一旦滚动则会出发这里，发送消息给中介，让中介去同步另外一方
void RcTreeWidget::slot_scrollValueChange(int value)
{
	if (m_direction == RC_LEFT)
	{
		if (m_mediator->getLeftScrollValue() != value)
		{
			m_mediator->setLeftScrollValue(value);
		}
	}
	else
	{
		if (m_mediator->getRightScrollValue() != value)
		{
			m_mediator->setRightScrollValue(value);
		}
	}
}

//注意，这里一旦开始调整后，又会引发滚动条值的变化
void RcTreeWidget::setVerticalValue(int value)
{
	//不相等才需要设置
	if (verticalScrollBar()->value() != value)
	{
		//超过最大值，只能设置为最大值
		if (value > verticalScrollBar()->maximum())
		{
			verticalScrollBar()->setValue(verticalScrollBar()->maximum());
		}
		else
		{
			verticalScrollBar()->setValue(value);
		}
	}
}
