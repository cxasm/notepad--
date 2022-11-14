#pragma once
#include <QObject>

class MediatorDisplay:public QObject
{
	Q_OBJECT
public:
	MediatorDisplay();
	virtual ~MediatorDisplay();

	void setLeftNum(int value);
	void setRightNum(int value);

	int getLeftNum();
	int getRightNum();

	void setLeftScrollValue(int value);
	void setRightScrollValue(int value);

	int getLeftScrollValue();
	int getRightScrollValue();

	void setLeftScrollXValue(int value);

	void setRightScrollXValue(int value);

	int getLeftScrollXValue();

	int getRightScrollXValue();

signals:
	//当前行同步
	void syncCurLineNum(int direction);

	//滚动条同步
	void syncCurScrollValue(int direction);

	void syncCurScrollXValue(int direction);

private:
	//左右当前行的值
	int m_leftLineNum;
	int m_rightLineNum;

	//左右滚动条的值
	int m_leftScrollValue;
	int m_rightScrollValue;

	//左右滚动条的值x
	int m_leftScrollXValue;
	int m_rightScrollXValue;
};

