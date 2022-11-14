#include "CmpareMode.h"
#include "Encode.h"
#include "rcglobal.h"

#include <QFile>
#include <QFileDevice> 
#include <QVector>
#include <QCryptographicHash>
#include <functional>
#include <QDataStream>
#include <QtConcurrent>


CmpareMode::CmpareMode()
{
}


CmpareMode::~CmpareMode()
{
	
}

//识别文字编码，并将文字按照原始编码格式，转换为QString。如果失败，默认按照utf8的格式进行转换；
bool CmpareMode::recognizeTextCode(QByteArray & text, LineFileInfo &lineInfo, QString &outUnicodeText)
{
	int lineNums = lineInfo.lineNums;

	int length = text.count();

	int result = false;

	//第一行时，检测一下文件编码，返回值也是文件的编码
	if (0 == lineNums)
	{
		int skip = 0;
		lineInfo.code = Encode::DetectEncode((uchar*) text.data(), length, skip);
		//根据编码跳过第一行前面的几个字符编码标识字段
		if (skip > 0)
		{
			text = text.mid(skip);
		}

		return Encode::tranStrToUNICODE((CODE_ID)lineInfo.code, text.data(), text.count(), outUnicodeText);

	}
	else
	{
		/*对于头部没有标识的行，需要每行进行详细检查，比较耗时
		*对于第一行已经是GBK的编码，标识出所有的确是GBK的行号
		*严格来说，如果以后要做国际版，不应该只考虑GBK，而是要考虑本地ASNI编码。
		*对中国而言，本地ASNI编码是GBK，对其它国家，比如日本/韩国而言，这些ASNI是它们本国
		*对应的本地编码。
		*/
//#if 0
//		//全部都在ascii范围以内，就作为ascii码。注意ASCII处理时其它地方时按照UTF8进行编码的
//		if (Encode::CheckTextIsAllAscii((uchar*)text.data(), length))
//		{
//			lineInfo.code = CODE_ID::ASCII;
//			return Encode::tranStrToUNICODE((CODE_ID)lineInfo.code, text.data(), length, outUnicodeText);
//		}
//		else
//		{
//#endif
			CODE_ID actualCode = Encode::CheckUnicodeWithoutBOM((uchar*)text.data(), length, outUnicodeText);
			if (CODE_ID::UTF8_NOBOM == actualCode)
			{
				lineInfo.code = CODE_ID::UTF8_NOBOM;
				result = true;
			}
			else if (CODE_ID::GBK == actualCode)
			{
				//如果发现存在GBK，则要以GBK作为字符编码。这里识别gbk是因为显示的时候，需要转化gbk进行显示
				lineInfo.code = CODE_ID::GBK;
				result = true;
			}
			else if (CODE_ID::ANSI == actualCode)
			{
				lineInfo.code = CODE_ID::UNKOWN; //这里就是乱码了。即不是utf8也不是GBK，也不能说乱码，目前其它国家未处理的码
				result = false;
			}
//#if 0
//		}
//#endif
	}

	return result;
}


CODE_ID CmpareMode::getTextFileEncodeType(uchar* fileFpr, int fileLength, QString filePath)
{

	if (fileLength >= 2 && fileFpr[0] == 0xFF && fileFpr[1] == 0xFE)
	{
		return CODE_ID::UNICODE_LE; //skip 2
	}
	else if (fileLength >= 2 && fileFpr[0] == 0xFE && fileFpr[1] == 0xFF)
	{
		return CODE_ID::UNICODE_BE; //skip 2
	}
	else if (fileLength >= 3 && fileFpr[0] == 0xEF && fileFpr[1] == 0xBB && fileFpr[2] == 0xBF)
	{
		return CODE_ID::UTF8_BOM; //skip 3 with BOM
	}

	//走到这里说明没有文件头BOM，进行全盘文件扫描
	if (!filePath.isEmpty())
	{
		return scanFileRealCode(filePath);
	}
	
	return CODE_ID::UNKOWN;
}

//20210802：发现如果是CODE_ID::UNICODE_LE，\r\n变成了\r\0\n\0，读取readLine遇到\n就结束了，而且toUnicode也会变成乱码失败
//所以UNICODE_LE需要单独处理。该函数只处理Unicode_LE编码文件，事先一定要检查文件编码
CODE_ID CmpareMode::readLineFromFileWithUnicodeLe(uchar* m_fileFpr, const int fileLength, QList<LineFileInfo>& lineInfoVec, QList<LineFileInfo>& blankLineInfoVec,int mode, int &maxLineSize)
{
	QCryptographicHash md4(QCryptographicHash::Md4);

	int lineNums = 0;
	CODE_ID code = CODE_ID::UNICODE_LE;

	int lineStartPos = 2; //uicode_le前面有2个特殊标识，故跳过2

	//获取一行在文件中
	auto getOneLineFromFile = [m_fileFpr](int& startPos, const int fileLength, QByteArray& ret)->bool{

		if (startPos < fileLength)
		{
			ret.clear();

			int lineLens = 0;

			bool isFindLine = false;

			for (int i = startPos; i < fileLength; ++i,++lineLens)
			{
				//遇到换行符号
				if (m_fileFpr[i] == 0x0A)
				{
					//lineLens需要加2，因为当前这个没有加，而且后面还有一个\0,这是le格式规定的
					ret.append((char*)(m_fileFpr + startPos), lineLens + 2);
					startPos += lineLens + 2;
					isFindLine = true;
					break;
				}
			}

			//没有找到一行
			if (!isFindLine)
			{
				//最后一行，可能没有带\r\0直接返回
				ret.append((char*)(m_fileFpr + startPos), fileLength - startPos);

				startPos = fileLength;
			}

			return true;
			
		}

		return false;
	};

	QByteArray line;

	auto work = [mode, &md4](LineFileInfo& lineInfo, const int n) {
		if (mode == 0)
		{
			md4.addData(lineInfo.unicodeStr.trimmed().toUtf8());
		}
		else if (mode == 1)
		{
			md4.addData(lineInfo.unicodeStr.left(lineInfo.unicodeStr.length() - n).toUtf8());
		}
		else if (mode == 2)
		{
			QString temp = lineInfo.unicodeStr;
			md4.addData(temp.replace(QRegExp("\\s"), QString("")).toUtf8());
		}
	};

	while (getOneLineFromFile(lineStartPos, fileLength,line)) {

		LineFileInfo lineInfo;

		lineInfo.lineNums = lineNums;

		/* 这种方式读取文件会包含后面的行尾 */
		int length = line.length();

		if (maxLineSize < length)
		{
			maxLineSize = length;
		}

		//如果是头部有标识的格式，则后续不用详细检查每行编码，直接按照头部标识走
		Encode::tranStrToUNICODE(code, line.data(), line.count(), lineInfo.unicodeStr);
		lineInfo.code = code;
	
		if (lineInfo.unicodeStr.endsWith("\r\r\n"))
		{
			//这里是一种错误，但确实可能出现
			if (length > 3)
			{
				/*if (mode == 0)
				{
					md4.addData(lineInfo.unicodeStr.trimmed().toUtf8());
				}
				else if (mode == 1)
				{
					md4.addData(lineInfo.unicodeStr.left(lineInfo.unicodeStr.length() - 3).toUtf8());
				}*/
				work(lineInfo, 3);
				}
			else
			{
				//空白行
				lineInfo.isLcsExist = false;
				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::DOS_LINE;
		}
		else if (lineInfo.unicodeStr.endsWith("\r\n"))
		{
			if (length > 2)
			{
				/*if (mode == 0)
				{
					md4.addData(lineInfo.unicodeStr.trimmed().toUtf8());
				}
				else if(mode == 1)
				{
					md4.addData(lineInfo.unicodeStr.left(lineInfo.unicodeStr.length() - 2).toUtf8());
				}*/
				work(lineInfo, 2);
				}
			else
			{
				//空白行
				lineInfo.isLcsExist = false;
				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::DOS_LINE;

		}
		else if (lineInfo.unicodeStr.endsWith("\n"))
		{
			if (length > 1)
			{
				/*if (mode == 0)
				{
					md4.addData(lineInfo.unicodeStr.trimmed().toUtf8());
				}
				else if (mode == 1)
				{
					md4.addData(lineInfo.unicodeStr.left(lineInfo.unicodeStr.length() - 1).toUtf8());
				}*/
				work(lineInfo, 1);
				}
			else
			{
				lineInfo.isLcsExist = false;

				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::UNIX_LINE;

		}
		else if (lineInfo.unicodeStr.endsWith("\r"))
		{
			if (length > 1)
			{
			/*	if (mode == 0)
				{
					md4.addData(lineInfo.unicodeStr.trimmed().toUtf8());
				}
				else if (mode == 1)
				{
					md4.addData(lineInfo.unicodeStr.left(lineInfo.unicodeStr.length() - 1).toUtf8());
				}*/
				work(lineInfo, 1);
				}
			else
			{
				lineInfo.isLcsExist = false;

				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::MAC_LINE;
		}
		else
		{
			if (length > 0)
			{
				/*if (mode == 0)
				{
					md4.addData(lineInfo.unicodeStr.trimmed().toUtf8());
				}
				else if (mode == 1)
				{
					md4.addData(lineInfo.unicodeStr.toUtf8());
				}*/
				work(lineInfo, 0);
				}
			else
			{
				lineInfo.isLcsExist = false;

				lineInfo.isEmptyLine = true;

			}
			lineInfo.lineEndFormat = RC_LINE_FORM::UNKNOWN_LINE;
		}

		if (lineInfo.isEmptyLine)
		{
			blankLineInfoVec.append(lineInfo);
		}
		else
		{
			lineInfo.md4 = md4.result();
			//qDebug() << lineInfo.md4;
			md4.reset();
			lineInfoVec.append(lineInfo);
		}
		++lineNums;
	}

	return code;
}


//读取每一行，将空白行和非空白行分开。非空白行取他们的行md4值（不包含尾部的换行符）
//返回值：文件扫描出来的字符编码
//在对比行的md5值时，忽略了后面的行尾类型。即只对比字符内容，忽略了行尾。
//20210802：发现如果是CODE_ID::UNICODE_LE，\r\n变成了\r\0\n\0，读取readLine遇到\n就结束了，而且toUnicode也会变成乱码失败
//所以UNICODE_LE需要单独处理。注意UNICODE_BE没有这个问题，因为BE是\0\r\0\n，0在前面就没有这个问题

//20210901 发现使用readLine的方式来读取一行不可靠。因为有些文件中一行中间有个\r，这种情况没有识别为多行。readLine是根据\n来识别的。
//进而导致中间的\r没有识别为多行，但是在编辑器中却多一行，导致对比错误。还是要自己来识别行。不依赖于readLine

//CODE_ID fileCode 事先预判定的编码
CODE_ID CmpareMode::readLineFromFile(uchar* m_fileFpr, const int fileLength, const CODE_ID fileCode, QList<LineFileInfo>&lineInfoVec, QList<LineFileInfo>&blankLineInfoVec, int mode, int &maxLineSize)
{
	QCryptographicHash md4(QCryptographicHash::Md4);

	int lineNums = 0;
	CODE_ID code = fileCode;
	bool isExistGbk = false;
	bool isExistUnKownCode = false;
	bool isExistUtf8 = false;


	int lineStartPos = 0; 

	//跳过前面的BOM头部。LE不在这里处理，在外面
	if (fileCode == CODE_ID::UNICODE_BE || fileCode == CODE_ID::UNICODE_LE)
	{
		lineStartPos = 2;
	}
	else if (fileCode == CODE_ID::UTF8_BOM)
	{
		lineStartPos = 3;
	}

	//获取一行在文件中
	auto getOneLineFromFile = [m_fileFpr](int& startPos, const int fileLength, const CODE_ID fileCode, QByteArray& ret)->bool {

		if (startPos < fileLength)
		{
			ret.clear();

			int lineLens = 0;
			bool isFindLine = false;

			for (int i = startPos; i < fileLength; ++i, ++lineLens)
			{
				//遇到符号CR
				if (m_fileFpr[i] == 0x0D)
				{
					//后一个是LF,即以CRLF结尾
					if ((i + 1 < fileLength) && (m_fileFpr[i+1] == 0x0A))
					{
						//lineLens需要加2，因为当前这个没有加，而且后面还有一个\n
						ret.append((char*)(m_fileFpr + startPos), lineLens + 2);
						startPos += lineLens + 2;
						isFindLine = true;
						break;
					}
					else if ((fileCode == UNICODE_BE)&&((i>0) && m_fileFpr[i-1] == '\0'))
					{
						//事先发现就是BE格式，以\0\r\0\n为结尾的
						if ((i + 2 < fileLength) && (m_fileFpr[i + 1] == 0x0) && (m_fileFpr[i + 2] == 0x0A))
						{
							//lineLens需要加3，因为当前这个没有加，而且后面还有一个\0\n
							ret.append((char*)(m_fileFpr + startPos), lineLens + 3);
							startPos += lineLens + 3;
							isFindLine = true;
							break;
						}
						else 
						{
							//虽然说是BE格式，但是后面没有以\0\n结尾，而是以\r结尾。这种多半就是错误。直接按\0\r结尾
							//lineLens需要加1，因为当前这个没有加
							ret.append((char*)(m_fileFpr + startPos), lineLens + 1);
							startPos += lineLens + 1;
							isFindLine = true;
							break;
						}

					}
					else
					{
						//直接以\r结尾了，后面没有\n或者\0\n。符合MAC格式，windows可能编码只有\r，没有\n的错误情况。
						//lineLens需要加1，因为当前这个没有加
						ret.append((char*)(m_fileFpr + startPos), lineLens + 1);
						startPos += lineLens + 1;
						isFindLine = true;
						break;
					}
				}
				else if(m_fileFpr[i] == 0x0A)
				{
					//没有先遇到\r，直接遇到\n.20210903发现忘记处理该情况le
					//lineLens需要加1，因为当前这个没有加
					ret.append((char*)(m_fileFpr + startPos), lineLens + 1);
					startPos += lineLens + 1;
					isFindLine = true;
					break;
				}
			}

			//没有找到一行
			if (!isFindLine)
			{
				//最后一行，可能没有带\r\0直接返回
				ret.append((char*)(m_fileFpr + startPos), fileLength - startPos);

				startPos = fileLength;
			}

			return true;

		}

		return false;
	};

	QByteArray line;

	auto work = [mode,&md4](LineFileInfo& lineInfo, const int n) {
		if (mode == 0)
		{
			md4.addData(lineInfo.unicodeStr.trimmed().toUtf8());
		}
		else if (mode == 1)
		{
			md4.addData(lineInfo.unicodeStr.left(lineInfo.unicodeStr.length() - n).toUtf8());
		}
		else if (mode == 2)
		{
			QString temp = lineInfo.unicodeStr;
			md4.addData(temp.replace(QRegExp("\\s"), QString("")).toUtf8());
		}
	};

	while (getOneLineFromFile(lineStartPos, fileLength, code, line)) {

		LineFileInfo lineInfo;

		lineInfo.lineNums = lineNums;

		/* 这种方式读取文件会包含后面的行尾 */
		int length = line.length();

		if (maxLineSize < length)
		{
			maxLineSize = length;
		}
		//外面必须把code先检测好了

		//if (fileCode == CODE_ID::UNICODE_BE /*|| fileCode == CODE_ID::UNICODE_LE */ || fileCode == CODE_ID::UTF8_BOM)
		if(fileCode != CODE_ID::UNKOWN)
		{
			//如果是头部有标识的格式，则后续不用详细检查每行编码，直接按照头部标识走
			Encode::tranStrToUNICODE(code, line.data(), line.count(), lineInfo.unicodeStr);
			lineInfo.code = fileCode;
		}
		else if(fileCode == CODE_ID::UNKOWN)
		{
			/*对于头部没有标识的行，需要每行进行详细检查，比较耗时
			*对于第一行已经是GBK的编码，标识出所有的确是GBK的行号
			*严格来说，如果以后要做国际版，不应该只考虑GBK，而是要考虑本地ASNI编码。
			*对中国而言，本地ASNI编码是GBK，对其它国家，比如日本/韩国而言，这些ASNI是它们本国
			*对应的本地编码。
			*/
			recognizeTextCode(line, lineInfo, lineInfo.unicodeStr);

			if (CODE_ID::UTF8_NOBOM == lineInfo.code)
			{
				isExistUtf8 = true;
			}
			else if (CODE_ID::GBK == lineInfo.code)
			{
				//如果发现存在GBK，则要以GBK作为字符编码。这里识别gbk是因为显示的时候，需要转化gbk进行显示
				isExistGbk = true;
			}
			else if (CODE_ID::UNKOWN == lineInfo.code)
			{
				isExistUnKownCode = true;
			}

		}

		if (lineInfo.unicodeStr.endsWith("\r\r\n"))
		{
			//这里是一种错误，但确实可能出现
			if (length > 3)
			{
				work(lineInfo,3);
			}
			else
			{
				//空白行
				lineInfo.isLcsExist = false;
				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::DOS_LINE;
		}
		else if (lineInfo.unicodeStr.endsWith("\r\n"))
		{
			if (length > 2)
			{
			
				work(lineInfo, 2);
			}
			else
			{
				//空白行
				lineInfo.isLcsExist = false;
				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::DOS_LINE;

		}
		else if (lineInfo.unicodeStr.endsWith("\n"))
		{
			if (length > 1)
			{
	
				work(lineInfo, 1);
			}
			else
			{
				lineInfo.isLcsExist = false;

				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::UNIX_LINE;

		}
		else if (lineInfo.unicodeStr.endsWith("\r"))
		{
			if (length > 1)
			{
				work(lineInfo, 1);
			}
			else
			{
				lineInfo.isLcsExist = false;

				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::MAC_LINE;
		}
		else
		{
			if (length > 0)
			{
				work(lineInfo, 0);
			}
			else
			{
				lineInfo.isLcsExist = false;

				lineInfo.isEmptyLine = true;

			}
			lineInfo.lineEndFormat = RC_LINE_FORM::UNKNOWN_LINE;
		}

		if (lineInfo.isEmptyLine)
		{
			blankLineInfoVec.append(lineInfo);
		}
		else
		{
			lineInfo.md4 = md4.result();
			md4.reset();
			lineInfoVec.append(lineInfo);
		}
		++lineNums;
	}

	//如果外部指定了格式，则直接返回外部格式
	if (fileCode != CODE_ID::UNKOWN)
	{
		return fileCode;
	}

	return judgeFinalTextCode(code, isExistUnKownCode, isExistGbk, isExistUtf8);
}

CODE_ID CmpareMode::judgeFinalTextCode(CODE_ID code, bool isExistUnKownCode, bool isExistGbk, bool isExistUtf8)
{
	//如果是三种有明确标识的字符编码，则严格按照标识的逻辑去读取。哪怕里面存在错误编码，也只能按照头部标识为准
	if (CODE_ID::UNICODE_LE == code || CODE_ID::UNICODE_BE == code || CODE_ID::UTF8_BOM == code || code == CODE_ID::GBK)
	{
		return code;
	}

	//剩下的是在文件头没有严格标识编码的文件
	//存在不能识别的编码，则应该是ASNI，需要用户指定编码
	if (isExistUnKownCode)
	{
		return CODE_ID::UNKOWN;
	}
	if (isExistGbk)
	{
		//如果没有错误码，而且发现gbk，则是gbk编码
		return CODE_ID::GBK;
	}
	//如果不存在错误和gbk，就是纯粹的ut8_nobom
	if (isExistUtf8)
	{
		return CODE_ID::UTF8_NOBOM;
	}

	return code;
}


//读取用于纯输出，不做比较。bool &isMaybeHexFile 是否是hex文件，不一定准确，做一个推测
// int& charsNums 输出字符个数
CODE_ID CmpareMode::readLineFromFile(uchar* m_fileFpr, const int fileLength, const CODE_ID fileCode, QList<LineFileInfo>&lineInfoVec, int& maxLineSize, int& charsNums, bool &isMaybeHexFile)
{
	int lineNums = 0;
	CODE_ID code = fileCode;
	bool isExistGbk = false;
	bool isExistUnKownCode = false;
	bool isExistUtf8 = false;
	int lineStartPos = 0;
	int errorCodeLines = 0;
	charsNums = 0;

	if (fileCode == CODE_ID::UNICODE_BE || fileCode == CODE_ID::UNICODE_LE)
	{
		lineStartPos = 2;
	}
	else if (fileCode == CODE_ID::UTF8_BOM)
	{
		lineStartPos = 3;
	}

	//获取一行在文件中
	auto getOneLineFromFile = [m_fileFpr](int& startPos, const int fileLength, const CODE_ID fileCode, QByteArray& ret)->bool {

		if (startPos < fileLength)
		{
			ret.clear();

			int lineLens = 0;
			bool isFindLine = false;

			for (int i = startPos; i < fileLength; ++i, ++lineLens)
			{
				//遇到符号CR
				if (m_fileFpr[i] == 0x0D)
				{
					//后一个是LF,即以CRLF结尾
					if ((i + 1 < fileLength) && (m_fileFpr[i + 1] == 0x0A))
					{
						//lineLens需要加2，因为当前这个没有加，而且后面还有一个\n
						ret.append((char*)(m_fileFpr + startPos), lineLens + 2);
						startPos += lineLens + 2;
						isFindLine = true;
						break;
					}
					else if ((fileCode == UNICODE_BE) && ((i > 0) && m_fileFpr[i - 1] == '\0'))
					{
						//事先发现就是BE格式，以\0\r\0\n为结尾的
						if ((i + 2 < fileLength) && (m_fileFpr[i + 1] == 0x0) && (m_fileFpr[i + 2] == 0x0A))
						{
							//lineLens需要加3，因为当前这个没有加，而且后面还有一个\0\n
							ret.append((char*)(m_fileFpr + startPos), lineLens + 3);
							startPos += lineLens + 3;
							isFindLine = true;
							break;
						}
						else
						{
							//虽然说是BE格式，但是后面没有以\0\n结尾，而是以\r结尾。这种多半就是错误。直接按\0\r结尾
							//lineLens需要加1，因为当前这个没有加
							ret.append((char*)(m_fileFpr + startPos), lineLens + 1);
							startPos += lineLens + 1;
							isFindLine = true;
							break;
						}

					}
					else
					{
						//直接以\r结尾了，后面没有\n或者\0\n。符合MAC格式，windows可能编码只有\r，没有\n的错误情况。
						//lineLens需要加1，因为当前这个没有加
						ret.append((char*)(m_fileFpr + startPos), lineLens + 1);
						startPos += lineLens + 1;
						isFindLine = true;
						break;
					}
				}
				else if (m_fileFpr[i] == 0x0A)
				{
					//没有先遇到\r，直接遇到\n.20210903发现忘记处理该情况le
					//lineLens需要加1，因为当前这个没有加
					ret.append((char*)(m_fileFpr + startPos), lineLens + 1);
					startPos += lineLens + 1;
					isFindLine = true;
					break;
				}
			}

			//没有找到一行
			if (!isFindLine)
			{
				//最后一行，可能没有带\r\0直接返回
				ret.append((char*)(m_fileFpr + startPos), fileLength - startPos);

				startPos = fileLength;
			}

			return true;

		}

		return false;
	};

	QByteArray line;


	while (getOneLineFromFile(lineStartPos, fileLength, code, line)) {

		LineFileInfo lineInfo;

		lineInfo.lineNums = lineNums;

		/* 这种方式读取文件会包含后面的行尾 */
		int length = line.length();

		if (maxLineSize < length)
		{
			maxLineSize = length;
		}

		//外面必须把code先检测好了

		//if (fileCode == CODE_ID::UNICODE_BE /*|| fileCode == CODE_ID::UNICODE_LE */ || fileCode == CODE_ID::UTF8_BOM)
		if(fileCode != CODE_ID::UNKOWN)
		{
			//如果是头部有标识的格式，则后续不用详细检查每行编码，直接按照头部标识走
			Encode::tranStrToUNICODE(code, line.data(), line.count(), lineInfo.unicodeStr);
			lineInfo.code = fileCode;
		}
		else if (fileCode == CODE_ID::UNKOWN)
		{
			/*对于头部没有标识的行，需要每行进行详细检查，比较耗时
			*对于第一行已经是GBK的编码，标识出所有的确是GBK的行号
			*严格来说，如果以后要做国际版，不应该只考虑GBK，而是要考虑本地ASNI编码。
			*对中国而言，本地ASNI编码是GBK，对其它国家，比如日本/韩国而言，这些ASNI是它们本国
			*对应的本地编码。
			*/
			recognizeTextCode(line, lineInfo, lineInfo.unicodeStr);

			if (CODE_ID::UTF8_NOBOM == lineInfo.code)
			{
				isExistUtf8 = true;
			}
			else if (CODE_ID::GBK == lineInfo.code)
			{
				//如果发现存在GBK，则要以GBK作为字符编码。这里识别gbk是因为显示的时候，需要转化gbk进行显示
				isExistGbk = true;
			}
			else if (CODE_ID::UNKOWN == lineInfo.code)
			{
				isExistUnKownCode = true;

				//增加错误行的计数
				errorCodeLines++;
			}

		}

		if (lineInfo.unicodeStr.endsWith("\r\r\n"))
		{
			//这里是一种错误，但确实可能出现
			if (length > 3)
			{

			}
			else
			{
				//空白行
				lineInfo.isLcsExist = false;
				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::DOS_LINE;
		}
		else if (lineInfo.unicodeStr.endsWith("\r\n"))
		{
			if (length > 2)
			{
				
			}
			else
			{
				//空白行
				lineInfo.isLcsExist = false;
				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::DOS_LINE;

		}
		else if (lineInfo.unicodeStr.endsWith("\n"))
		{
			if (length > 1)
			{
				
			}
			else
			{
				lineInfo.isLcsExist = false;

				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::UNIX_LINE;

		}
		else if (lineInfo.unicodeStr.endsWith("\r"))
		{
			if (length > 1)
			{
				
			}
			else
			{
				lineInfo.isLcsExist = false;

				lineInfo.isEmptyLine = true;
			}
			lineInfo.lineEndFormat = RC_LINE_FORM::MAC_LINE;
		}
		else
		{
			if (length > 0)
			{
				
			}
			else
			{
				lineInfo.isLcsExist = false;

				lineInfo.isEmptyLine = true;

			}
			lineInfo.lineEndFormat = RC_LINE_FORM::UNKNOWN_LINE;
		}

		lineInfoVec.append(lineInfo);
		
		charsNums += lineInfo.unicodeStr.size();
		++lineNums;
	}

	//如果超过一半的行都是错误的，则考虑为hex文件。
	if (lineNums >= 10 && (errorCodeLines * 100 / lineNums > 50))
	{
		isMaybeHexFile = true;
	}
	else
	{
		isMaybeHexFile = false;

		//如果前面三行中含有\0字符，也可能是二进制文件
		if (lineNums > 3)
		{
			for (int i = 0; i < 3; ++i)
			{
				if (lineInfoVec.at(i).unicodeStr.contains(QChar('\0')))
				{
					isMaybeHexFile = true;
					break;
				}
			}
		}
	}

	//如果用户外部强制编码，则直接按改编码返回
	if (fileCode != CODE_ID::UNKOWN)
	{
		return fileCode;
	}

	return judgeFinalTextCode(code, isExistUnKownCode, isExistGbk, isExistUtf8);
}


//扫描文件的字符编码，不输出文件
CODE_ID CmpareMode::scanFileRealCode(QString filePath)
{
	QFile file(filePath);
	file.open(QIODevice::ReadOnly);
	
	CODE_ID code = CODE_ID::UNKOWN;
	int lineNums = 0;

	bool isExistGbk = false;
	bool isExistUnKownCode = false;
	bool isExistUtf8 = false;


	while (!file.atEnd()) {

		LineFileInfo lineInfo;

		QByteArray line = file.readLine();

		lineInfo.lineNums = lineNums;

		/* 这种方式读取文件会包含后面的行尾 */
		//int length = line.length();

		//第一行时，检测一下文件编码，返回值也是文件的编码
		if (0 == lineNums)
		{
			recognizeTextCode(line, lineInfo, lineInfo.unicodeStr);
			code = (CODE_ID)lineInfo.code;

			//已经找到文本的标签，相信标签，之前返回
			if (code == CODE_ID::UNICODE_BE || code == CODE_ID::UNICODE_LE || code == CODE_ID::UTF8_BOM || code == CODE_ID::GBK)
			{
				break;
			}

		}
		else
		{
			/*对于头部没有标识的行，需要每行进行详细检查，比较耗时
			*对于第一行已经是GBK的编码，标识出所有的确是GBK的行号
			*严格来说，如果以后要做国际版，不应该只考虑GBK，而是要考虑本地ASNI编码。
			*对中国而言，本地ASNI编码是GBK，对其它国家，比如日本/韩国而言，这些ASNI是它们本国
			*对应的本地编码。
			*/
			recognizeTextCode(line, lineInfo, lineInfo.unicodeStr);

			if (CODE_ID::UTF8_NOBOM == lineInfo.code)
			{
				isExistUtf8 = true;
			}
			else if (CODE_ID::GBK == lineInfo.code)
			{
				//如果发现存在GBK，则要以GBK作为字符编码。这里识别gbk是因为显示的时候，需要转化gbk进行显示
				isExistGbk = true;
			}
			else if (CODE_ID::UNKOWN == lineInfo.code)
			{
				isExistUnKownCode = true;
				//20220127一旦发现错误编码，或者说不能识别的编码，则直接跳出。
				//因为肯定是不能识别的编码ASNI
				break;
			}

		}

		++lineNums;

		if (lineNums >= 1000)
		{
			break;
		}
	}

	file.close();

	return judgeFinalTextCode(code, isExistUnKownCode, isExistGbk, isExistUtf8);

}

//读取文件，并输出
//bytescharsNums:文件字符个数，不是文件大小
//20220908 自动判断是否是二进制文件。isHexFile 是输出
CODE_ID CmpareMode::scanFileOutPut(CODE_ID code, QString filePath, QList<LineFileInfo>& outputLineInfoVec, int &maxLineSize, int& charsNums, bool &isHexFile)
{
	QFile* file = new QFile(filePath);
	file->open(QIODevice::ReadOnly);
	uchar* m_fileFpr = file->map(0, file->size());

	if (code == UNKOWN)
	{
		code = getTextFileEncodeType(m_fileFpr, file->size(), filePath);
	}

	//UNICODE_LE格式需要单独处理
	if (code == UNICODE_LE)
	{
		readLineFromFileWithUnicodeLe(m_fileFpr, file->size(), outputLineInfoVec, outputLineInfoVec, 0, maxLineSize);
	}
	else
	{
		code = readLineFromFile(m_fileFpr, file->size(), code, outputLineInfoVec, maxLineSize, charsNums, isHexFile);
	}

	file->unmap(m_fileFpr);
	file->close();
	delete file;

	return code;
}
