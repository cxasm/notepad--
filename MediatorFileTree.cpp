#include "MediatorFileTree.h"

/* 文件夹对比界面的中介者，使用中介者是为了让消息经过中介对象统一调度，避免左右互相依赖导致交互混乱
*/

MediatorFileTree::MediatorFileTree() :QObject(nullptr)
{

}

MediatorFileTree::~MediatorFileTree()
{

}

void MediatorFileTree::setLeftScrollValue(int value)
{
	m_leftScrollValue = value;

	//如果左右不相等，则推动对方去同步
	if (m_leftScrollValue != m_rightScrollValue)
	{
		emit syncCurScrollValue(0);
	}

}

void MediatorFileTree::setRightScrollValue(int value)
{
	m_rightScrollValue = value;

	if (m_leftScrollValue != m_rightScrollValue)
	{
		emit syncCurScrollValue(1);
	}
}

int MediatorFileTree::getLeftScrollValue()
{
	return m_leftScrollValue;
}

int MediatorFileTree::getRightScrollValue()
{
	return m_rightScrollValue;
}


//设置item。中介本身不保存ITEM，因为太多，发送消息给外面空间
void MediatorFileTree::setLeftItemStatus(QString name, int status)
{
	emit syncExpandStatus(name, RC_LEFT, status);
}

void MediatorFileTree::setRightItemStatus(QString name, int status)
{
	emit syncExpandStatus(name, RC_RIGHT, status);
}
