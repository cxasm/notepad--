#include "Encode.h"
#include <QTextCodec>
#include <QtDebug>

/* 检查字符串编码的类。看了大量文献，结论如下：
*如果是UTF BOM格式，或者UNICODE格式，其文件头部前几个字节（2-3）有一定的标识。由此标识直接按对应编码处理。
*如果没有标识，默认就是UTF8(NO BOM) 与 ANSI(现在只考虑GBK）进行对比。
*此时需要做统计分析。对所有行进行UTF8解析，如果按照UTF8解析错位再按照GBK解析。如果解析出GBK那么大概率认为文件是GBK编码的。
*/

Encode::Encode()
{

}

Encode::~Encode()
{

}

CODE_ID Encode::getCodeByName(QString name)
{
	CODE_ID id;

	if (name == "unknown")
	{
		id = CODE_ID::UNKOWN;
	}
	else if (name == "UTF16-LE")
	{
		id = CODE_ID::UNICODE_LE;
	}
	else if (name == "UTF16-BE")
	{
		id = CODE_ID::UNICODE_BE;
	}
	else if (name == "UTF8")
	{
		id = CODE_ID::UTF8_NOBOM;
	}
	else if (name == "UTF8-BOM")
	{
		id = CODE_ID::UTF8_BOM;
	}
	else if (name == "GBK")
	{
		id = CODE_ID::GBK;
	}
	else if (name == "EUC-JP")
	{
		id = CODE_ID::EUC_JP;
	}
	else if (name == "Shift-JIS")
	{
		id = CODE_ID::Shift_JIS;
	}
	else if (name == "EUC-KR")
	{
		id = CODE_ID::EUC_KR;
	}
	else if (name == "KOI8-R")
	{
		id = CODE_ID::KOI8_R;
	}
	else if (name == "TSCII")
	{
		id = CODE_ID::TSCII;
	}
	else if (name == "TIS-620")
	{
		id = CODE_ID::TIS_620;
	}
	else
	{
		id = CODE_ID::UNKOWN;
	}

	return id;

}

QString Encode::getLineEndById(RC_LINE_FORM id)
{
	QString ret;

	switch (id)
	{
	case PAD_LINE:
	case UNKNOWN_LINE:
#ifdef WIN32
		ret = "Windows(CR LF)";
#else
		ret = "Unix(LF)";
#endif
		ret = "NULL";
		break;
	case UNIX_LINE:
		ret = "Unix(LF)";
		break;
	case DOS_LINE:
		ret = "Windows(CR LF)";
		break;
	case MAC_LINE:
		ret = "Mac(CR)";
		break;
	default:
		break;
	}
	return  ret;
}

QString Encode::getCodeNameById(CODE_ID id)
{
	QString ret;

	switch (id)
	{
	case UNKOWN:
		ret = "unknown";
		break;
	case ANSI:
		ret = "unknown";
		break;
	case UNICODE_LE:
		ret = "UTF16-LE";
		break;
	case UNICODE_BE:
		ret = "UTF16-BE";
		break;
	case UTF8_NOBOM:
		ret = "UTF8";
		break;
	case UTF8_BOM:
		ret = "UTF8-BOM";
		break;
	case GBK:
		ret = "GBK";
		break;
	case EUC_JP:
		ret = "EUC-JP";
		break;
	case Shift_JIS:
		ret = "Shift-JIS";
		break;
	case EUC_KR:
		ret = "EUC-KR";
		break;
	case KOI8_R:
		ret = "KOI8-R";
		break;
	case TSCII:
		ret = "TSCII";
		break;
	case TIS_620:
		ret = "TIS-620";
		break;
	default:
		ret = "unknown";
		break;
	}

	return ret;
}

QByteArray Encode::getEncodeStartFlagByte(CODE_ID code)
{
	QByteArray ret;

	switch (code)
	{
		case UNICODE_LE:
		{
			ret.append((char)0xFF);
			ret.append((char)0xFE);
		}
		break;

		case UNICODE_BE:
		{
		ret.append((char)0xFE);
		ret.append((char)0xFF);
		}
		break;

		case UTF8_BOM:
		{
			ret.append((char)0xEF);
			ret.append((char)0xBB);
			ret.append((char)0xBF);
		}
		break;

		default:
			break;
	}

	return ret;
}

CODE_ID Encode::DetectEncode(const uchar* pBuffer, int length, int &skip)
{
	if (pBuffer[0] == 0xFF && pBuffer[1] == 0xFE)
	{
		skip = 2;
		return CODE_ID::UNICODE_LE; //skip 2
	}
	if (pBuffer[0] == 0xFE && pBuffer[1] == 0xFF)
	{
		skip = 2;
		return CODE_ID::UNICODE_BE; //skip 2
	}
	if (pBuffer[0] == 0xEF && pBuffer[1] == 0xBB && pBuffer[2] == 0xBF)
	{
		skip = 3;
		return CODE_ID::UTF8_BOM; //skip 3 with BOM
	}

	// 不能知道是不是UTF8
	CODE_ID code = CheckUnicodeWithoutBOM(pBuffer, length);

	skip = 0;
	return code; //skip 0

}

bool Encode::tranGbkToUNICODE(const char* pText, int length, QString &out)
{
	QTextCodec::ConverterState state;
	
	QTextCodec *codec = QTextCodec::codecForName("GBK");
	out = codec->toUnicode((const char *)pText, length, &state);
	if (state.invalidChars > 0) {
		return false;
	}
	return true;
}

bool Encode::tranUtf8ToUNICODE(const char* pText, int length, QString &out)
{
	QTextCodec::ConverterState state;

	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	out = codec->toUnicode((const char *)pText, length, &state);
	if (state.invalidChars > 0) {
		return false;
	}
	return true;
}

//与getCodeNameById类似，但是返回的是QT系统支持的编码的字符串名称
QString Encode::getQtCodecNameById(CODE_ID id)
{
	QString ret;

	switch (id)
	{
	case UNKOWN:
	case ANSI:
		ret = "unknown";
		break;
	case UNICODE_LE:
		ret = "UTF16-LE";
		break;
	case UNICODE_BE:
		ret = "UTF16-BE";
		break;
	case UTF8_NOBOM://qt没有这种
	case UTF8_BOM:
		ret = "UTF8";
		break;
	case GBK:
		ret = "GBK";
		break;
	case EUC_JP:
		ret = "EUC-JP";
		break;
	case Shift_JIS:
		ret = "Shift-JIS";
		break;
	case EUC_KR:
		ret = "EUC-KR";
		break;
	case KOI8_R:
		ret = "KOI8-R";
		break;
	case TSCII:
		ret = "TSCII";
		break;
	case TIS_620:
		ret = "TIS-620";
		break;
	default:
		ret = "unknown";
		break;
	}

	return ret;
}

//将指定编码的字符串转换到unicode
bool Encode::tranStrToUNICODE(CODE_ID code, const char* pText, int length, QString &out)
{
	if (length < 0)
	{
		return false;
	}

	QTextCodec::ConverterState state;
	QTextCodec *codec = nullptr;

	QString textCodeName = getQtCodecNameById(code);
	if (textCodeName.isEmpty() || textCodeName == "unknown")
	{
		//对于其它非识别编码，统一转换为utf8。减去让用户选择的麻烦
		//这里其实是有问题的。先这样简单处理
		codec = QTextCodec::codecForName("UTF-8");
	}
	else
	{
		codec = QTextCodec::codecForName(textCodeName.toStdString().c_str());
	}
	
	if (codec == nullptr)
	{
		return false;
	}

	out = codec->toUnicode((const char *)pText, length, &state);
	if (state.invalidChars > 0) {
		return false;
	}
	return true;
}

/* 这里其实是穷举字符串的字符编码；ASNI utf8。目前只检测GBK和utf8;其它语种没有穷举
*GB2312 GBK GB18030 三种差别见https://cloud.tencent.com/developer/article/1343240
*关于编码的详细说明，见https://blog.csdn.net/libaineu2004/article/details/19245205
*/
//这里是有限检查utf8的，如果出现gbk，说明一定不是utf8，因为utf8检查到错误码。
CODE_ID Encode::CheckUnicodeWithoutBOM(const uchar* pText, int length)
{
	QTextCodec::ConverterState state;
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	const QString text = codec->toUnicode((const char *)pText, length, &state);
	if (state.invalidChars > 0) {
		/*不是UTF-8格式的文件，这里优先判断是不是UTF8，再判断是不是GBK；我们先做中文版；如果后续要做
		*国际版，其实不应该只检查GBK，而是应该检查本地ASCI码，包括ascii码*/
		QTextCodec::ConverterState state1;
		QTextCodec *codec1 = QTextCodec::codecForName("GBK");
		codec1->toUnicode((const char *)pText, length, &state1);
		if (state1.invalidChars > 0) {
			return CODE_ID::ANSI;
		}
		else
		{
			return CODE_ID::GBK;
		}
	}

	return CODE_ID::UTF8_NOBOM;
}

CODE_ID Encode::CheckUnicodeWithoutBOM(const uchar* pText, int length, QString &outUnicodeText)
{
	QTextCodec::ConverterState state;
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	outUnicodeText = codec->toUnicode((const char *)pText, length, &state);
	if (state.invalidChars > 0) {
		/*不是UTF-8格式的文件，这里优先判断是不是UTF8，再判断是不是GBK；我们先做中文版；如果后续要做
		*国际版，其实不应该只检查GBK，而是因为检查本地ASCI码，包括ascii码*/
		QTextCodec::ConverterState state1;
		QTextCodec *codec1 = QTextCodec::codecForName("GBK");
		QString gbkStr = codec1->toUnicode((const char *)pText, length, &state1);
		if (state1.invalidChars > 0) {
			//如果也不是gbk，姑且按照utf8直接返回
			return CODE_ID::ANSI;
		}
		else
		{
			outUnicodeText = gbkStr;
			return CODE_ID::GBK;
		}
	}

	return CODE_ID::UTF8_NOBOM;
}

//检查是否全是ascii字符码
bool Encode::CheckTextIsAllAscii(const uchar* pText, int length)
{
	for (int i = 0; i < length; ++i)
	{
		if (*(pText + i) < 0 || *(pText + i) > 0x7F)
		{
			return false;
		}
	}
	return true;
}
