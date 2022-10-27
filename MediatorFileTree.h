#pragma once
#include <qobject.h>
#include "rcglobal.h"

class MediatorFileTree :public QObject
{
	Q_OBJECT
public:
	MediatorFileTree();
	virtual ~MediatorFileTree();

	void setLeftScrollValue(int value);
	void setRightScrollValue(int value);
	int getLeftScrollValue();
	int getRightScrollValue();

	void setLeftItemStatus(QString name, int status);
	void setRightItemStatus(QString name, int status);


signals:
	//滚动条同步
	void syncCurScrollValue(int direction);

	//同步展开和收起状态
	void syncExpandStatus(QString name, int direction, int status);

private:
	int m_leftScrollValue;
	int m_rightScrollValue;
};

