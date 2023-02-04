#pragma once
#include <qobject.h>
#include<QVector>
#include<QMap>
#include <QFuture>

#include <functional>
#include "rcglobal.h"
#include "Encode.h"


class BlockUserData;
class QFile;


const int LEFT = 0;
const int RIGHT = 1;

//对比bin二进制文件。
const int MAX_BIN_SIZE = 1024 * 1024 * 10; //最大10M

typedef void(* CALL_FUNC)(void *, uchar *, int);

typedef struct lineFileInfo_ {
	qint32 lineNums; //行号码
	bool isLcsExist;//是否属于lcsline的一部分
	bool isEmptyLine; //是否是空白行，只包含换行符的行
	int code; //该行的字符编码
	int lineEndFormat; //行尾：见RC_LINE_FORM
	QByteArray md4;
	QString unicodeStr; //这个是包含行尾的换行符的
	lineFileInfo_()
	{
		isLcsExist = false;
		isEmptyLine = false;
		code = UNKOWN;
		lineEndFormat = UNKNOWN_LINE;
	}
}LineFileInfo;



const int EMPTY_FILE = 0;
const int SCAN_SUCCESS = 1;

class CmpareMode;

typedef struct ThreadFileCmpParameter_ {
	QString leftPath;
	QString rightPath;
	CmpareMode *resultCmpObj;

	ThreadFileCmpParameter_(QString leftPath_, QString rightPath_)
	{
		leftPath = leftPath_;
		rightPath = rightPath_;
		resultCmpObj = nullptr;
	}

}ThreadFileCmpParameter;

class CmpareMode :public QObject
{
	Q_OBJECT
public:
	CmpareMode();
	virtual ~CmpareMode();

	static CODE_ID readLineFromFile(uchar * m_fileFpr, const int fileLength, const CODE_ID fileCode, QList<LineFileInfo>& lineInfoVec, QList<LineFileInfo>& blankLineInfoVec, int mode, int& maxLineSize);
	static CODE_ID judgeFinalTextCode(CODE_ID code, bool isExistUnKownCode, bool isExistGbk, bool isExistUtf8);
	static CODE_ID readLineFromFile(uchar * m_fileFpr, const int fileLength, const CODE_ID fileCode, QList<LineFileInfo>& lineInfoVec,int& maxLineSize, int& charsNums, bool &isMaybeHexFile);

	static CODE_ID scanFileRealCode(QString filePath);
	static CODE_ID scanFileOutPut(CODE_ID code, QString filePath, QList<LineFileInfo>& outputLineInfoVec, int & maxLineSize, int & charsNums, bool &isHexFile);

	static CODE_ID getTextFileEncodeType(uchar* fileFpr, int fileLength, QString filePath="", bool isCheckHead = true);
	static bool tranUnicodeLeToUtf8Bytes(uchar* fileFpr, const int fileLength, QString& outUtf8Bytes, bool isSkipHead=false);
	static bool isUnicodeLeBomFile(uchar* fileFpr, int fileLength);
private:

	static bool recognizeTextCode(QByteArray & text, LineFileInfo & lineInfo, QString & outUnicodeText);
	quint32 static readLineFromFileWithUnicodeLe(uchar* m_fileFpr, const int fileLength, QList<LineFileInfo>& lineInfoVec, QList<LineFileInfo>& blankLineInfoVec,int mode, int &maxLineSize);

	
};

