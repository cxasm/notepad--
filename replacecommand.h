#pragma once
#include "command.h"
#include "rcglobal.h"

class CompareWin;

class ReplaceOperRecords {
public:
	RC_DIRECTION dir;
	int startPos; //开始位置
	int srcEndPos;//原始结束位置
	char* srcContents; //原始文字内容

	int replaceLens;//替换长度
	char* replaceContents;//替换内容

	ReplaceOperRecords() :startPos(0), srcEndPos(0), srcContents(nullptr), replaceLens(0), replaceContents(nullptr)
	{

	}

	~ReplaceOperRecords()
	{
		if (srcContents != nullptr)
		{
			delete[]srcContents;
			srcContents = nullptr;
		}
		if (replaceContents != nullptr)
		{
			delete[]replaceContents;
			replaceContents = nullptr;
		}
	}

	ReplaceOperRecords(const ReplaceOperRecords& other) = delete;
};


class ReplaceCommand :public Command
{
public:
	ReplaceCommand(CompareWin* operWin);
	virtual ~ReplaceCommand();

	virtual int getOperIndex()override;
	virtual void undo()override;

	virtual QString desc()override;
	void setDesc(QString v);

	void addRecord(ReplaceOperRecords* v);
	void setOperIndex(int v);


private:
	QList<ReplaceOperRecords*> m_record;
	CompareWin* m_operWin;
	int m_index;

	QString m_desc;

	ReplaceCommand(const ReplaceCommand& o) = delete;
	ReplaceCommand& operator=(const ReplaceCommand& o) = delete;
};

