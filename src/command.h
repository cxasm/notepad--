#pragma once
#include <QString>

enum ChangeDire {
	NONE_CHANGE=-1,
	LEFT_CHANGE=0,
	RIGHT_CHANGE,
	BOTH_CHANGE,
};

class Command
{
public:
	virtual ~Command() = default;
	//获取当前命令的序号，同一个序号的，需要逐渐一起执行
	virtual int getOperIndex() = 0;
	virtual void undo() = 0;
	virtual QString desc() = 0;

	//获取左右是否变化的状态。
	ChangeDire getChangeStatus() {
		return m_changeStaus;
	}
	void setChangeStatus(ChangeDire c) {
		m_changeStaus = c;
	}

protected:
	ChangeDire m_changeStaus;
};

