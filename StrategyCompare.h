#pragma once
#include "CmpareMode.h"
#include "AbstractCompare.h"
#include <QMap>



class StrategyCompare
{
public:
	StrategyCompare();
	virtual ~StrategyCompare();
	virtual void BlockCmpLcs(const BlocksInfo & leftBlockInfo, uchar * leftFileData, QVector<LineFileInfo>& leftLinesInfo, const BlocksInfo & rightBlockInfo, uchar * rightFileData, QVector<LineFileInfo>& rightLinesInfo, BlockCmpPairResult & result);

private:
	void lessCmpMore(const BlocksInfo & lessBlockInfo, uchar * lessFileData, QVector<LineFileInfo>& lessLinesInfo, const BlocksInfo & moreBlockInfo, uchar * moreFileData, QVector<LineFileInfo>&moreLinesInfo, QMap<int, BlockCmpPairInfo>& result);
};

