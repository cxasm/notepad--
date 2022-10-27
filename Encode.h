#pragma once
#include <QtGlobal>

//#define GBK_
#include "rcglobal.h"


class Encode
{
public:
	Encode();
	~Encode();
	static CODE_ID getCodeByName(QString name);
	static QString getLineEndById(RC_LINE_FORM id);
	static QString getCodeNameById(CODE_ID id);
	static QByteArray getEncodeStartFlagByte(CODE_ID code);
	static CODE_ID DetectEncode(const uchar* pBuffer, int length,int &skip);
	static bool tranGbkToUNICODE(const char* pText, int length, QString &out);
	static bool tranUtf8ToUNICODE(const char * pText, int length, QString & out);
	static QString getQtCodecNameById(CODE_ID id);
	static bool tranStrToUNICODE(CODE_ID code, const char * pText, int length, QString & out);
	static CODE_ID CheckUnicodeWithoutBOM(const uchar * pText, int length);
	static CODE_ID CheckUnicodeWithoutBOM(const uchar * pText, int length, QString & outUnicodeText);
	static bool CheckTextIsAllAscii(const uchar * pText, int length);
};

