#include "MediatorDisplay.h"


//这里如果直接让左右互相同步，互相影响，可能导致混乱。需要一个中间调停者模式，作为中间人去控制同步消息
//中介者模式意图：用一个中介对象来封装一系列的对象交互，中介者使各对象不需要显式地相互引用，从而使其耦合松散，而且可以独立地改变它们之间的交互。
//主要解决：对象与对象之间存在大量的关联关系，这样势必会导致系统的结构变得很复杂，同时若一个对象发生改变，我们也需要跟踪与之相关联的对象，同时做出相应的处理。
//之前不使用该模式时，左右显示同步互相交互，时而发生消息混乱

MediatorDisplay::MediatorDisplay():QObject(nullptr)
{
	m_leftLineNum = 0;
	m_rightLineNum = 0;
	m_leftScrollValue = 0;
	m_rightScrollValue = 0;

}

MediatorDisplay::~MediatorDisplay()
{
}

void MediatorDisplay::setLeftNum(int value)
{
	 m_leftLineNum = value;
	 if (m_leftLineNum != m_rightLineNum)
	 {
		 emit syncCurLineNum(0);
	 }
	
}

void MediatorDisplay::setRightNum(int value)
{
	m_rightLineNum = value;
	if (m_leftLineNum != m_rightLineNum)
	{
		emit syncCurLineNum(1);
	}
}

int MediatorDisplay::getLeftNum()
{
	return m_leftLineNum;
}

int MediatorDisplay::getRightNum()
{
	return m_rightLineNum;
}

void MediatorDisplay::setLeftScrollValue(int value)
{
	if (m_leftScrollValue != value)
	{
		m_leftScrollValue = value;
	}
	

	//如果左右不相等，则推动对方去同步
	if (m_leftScrollValue != m_rightScrollValue)
	{
		emit syncCurScrollValue(0);
	}

}

void MediatorDisplay::setRightScrollValue(int value)
{
	if (m_rightScrollValue != value)
	{
		m_rightScrollValue = value;
	}
	

	if (m_leftScrollValue != m_rightScrollValue)
	{
		emit syncCurScrollValue(1);
	}
}

int MediatorDisplay::getLeftScrollValue()
{
	return m_leftScrollValue;
}

int MediatorDisplay::getRightScrollValue()
{
	return m_rightScrollValue;
}


void MediatorDisplay::setLeftScrollXValue(int value)
{
	if (m_leftScrollXValue != value)
	{
		m_leftScrollXValue = value;
	}


	//如果左右不相等，则推动对方去同步
	if (m_leftScrollXValue != m_rightScrollXValue)
	{
		emit syncCurScrollXValue(0);
	}

}

void MediatorDisplay::setRightScrollXValue(int value)
{
	if (m_rightScrollXValue != value)
	{
		m_rightScrollXValue = value;
	}


	if (m_leftScrollXValue != m_rightScrollXValue)
	{
		emit syncCurScrollXValue(1);
	}
}

int MediatorDisplay::getLeftScrollXValue()
{
	return m_leftScrollXValue;
}

int MediatorDisplay::getRightScrollXValue()
{
	return m_rightScrollXValue;
}
