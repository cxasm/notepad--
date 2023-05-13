#include "filemanager.h"
#include "scintillaeditview.h"
#include "scintillahexeditview.h"
#include "CmpareMode.h"
#include "ccnotepad.h"
#include "progresswin.h"

#include <QMessageBox>
#include <QFile>
#include <QtGlobal>
#include <qscilexer.h>
#include <QFileInfo>

LangType detectLanguage(QString& headContent, QString& filepath);

FileManager::FileManager():m_lastErrorCode(NONE_ERROR)
{
}

FileManager::~FileManager()
{
}

ScintillaEditView* FileManager::newEmptyDocument(bool isBigText)
{
	ScintillaEditView* pEdit = new ScintillaEditView(nullptr, isBigText);
	return pEdit;
}

ScintillaHexEditView* FileManager::newEmptyHexDocument()
{
	ScintillaHexEditView* pEdit = new ScintillaHexEditView(nullptr);
	return pEdit;
}

//从尾部找前面的换行符号。返回的是需要回溯的个数
//注意如果是LE编码，字节流是\n\0的格式。从尾部往前回溯，找到\n，要回退1个\0。\n\0是一个整体，不能分割开
//20230201发现一个bug,在LE模式下，不能单纯用\n做换行识别。因为发现其它字符也存在\n，必须要完整以\n\0才能确定是换行。
//同样发现BE模式下，是\0\n的格式，也不能单独使用\n做换行识别，因为发现其他字符也存在\n，必须要完整以\0\n才能确定是换行
int findLineEndPos(const char* buf, int size, CODE_ID code = UNKOWN)
{
	int ret = 0;
	bool isfound = false;
	for (int i = size - 1; i >= 0; --i)
	{
		if (buf[i] == '\n')
		{
			////如果是LE，还要确定\n的下一个是否是\0
			if ((code == UNICODE_LE) && ((i != size -1) && (buf[i+1] != '\0')))
			{
				++ret;
				continue;
			}
			////如果是BE，还要确定\n的前一个是否是\0
			if ((code == UNICODE_BE) && ((i != 0) && (buf[i - 1] != '\0')))
			{
				++ret;
				continue;
			}
			isfound = true;
			break;
		}
		++ret;
	}

	//如果没有找到，怀疑是mac格式，按照\r结尾解析
	if (!isfound)
	{
		for (int i = size - 1; i >= 0; --i)
		{
			if (buf[i] == '\r')
			{
				////如果是LE，还要确定\n的下一个是否是\0
				if ((code == UNICODE_LE) && ((i != size - 1) && (buf[i + 1] != '\0')))
				{
					continue;
				}
				////如果是BE，还要确定\n的前一个是否是\0
				if ((code == UNICODE_BE) && ((i != 0) && (buf[i - 1] != '\0')))
				{
					continue;
				}
				isfound = true;
				break;
			}
			++ret;
		}
	}

	if (isfound)
	{
		//注意好好想想，这里是--ret,而不是++ret。
		if (code == UNICODE_LE)
		{
			--ret;
		}
		//UNICODE_BE不需要处理

		return ret;
	}

	//说明是一个巨长的行，这种情况不是很好。直接读取了，不管是否进行了行截断
	return 0;
}

//从行首找后面的换行符号。返回的是需要前进的个数，即把前面掐掉一节，让返回在一行的行首位置
int findLineStartPos(const char* buf, int size, CODE_ID code = UNKOWN)
{
	int ret = 0;
	bool isfound = false;
	for (int i = 0; i < size; ++i)
	{
		++ret;
		if (buf[i] == '\n')
		{
			////如果是LE，还要确定\n的下一个是否是\0
			if ((code == UNICODE_LE) && ((i != size - 1) && (buf[i + 1] != '\0')))
			{
				continue;
			}
			////如果是BE，还要确定\n的前一个是否是\0
			if ((code == UNICODE_BE) && ((i != 0) && (buf[i - 1] != '\0')))
			{
				continue;
			}

			isfound = true;
			break;
		}
	}

	//如果没有找到，怀疑是mac格式，按照\r结尾解析
	if (!isfound)
	{
		for (int i = size - 1; i >= 0; --i)
		{
			++ret;
			if (buf[i] == '\r')
			{
				////如果是LE，还要确定\n的下一个是否是\0
				if ((code == UNICODE_LE) && ((i != size - 1) && (buf[i + 1] != '\0')))
				{
					continue;
				}
				////如果是BE，还要确定\n的前一个是否是\0
				if ((code == UNICODE_BE) && ((i != 0) && (buf[i - 1] != '\0')))
				{
					continue;
				}
				isfound = true;
				break;
			}
		}
	}

	if (isfound)
	{
		//注意好好想想，这里是++ret,而不是--ret。
		if (code == UNICODE_LE)
		{
			++ret;
		}
		return ret;
	}

	//说明是一个巨长的行，这种情况不是很好。直接读取了，不管是否进行了行截断
	return 0;
}


//返回第一个空闲的id，m_newFileIdList必须有序，从小到大，从0开始
//序号也从0开始，notepad++是从1开始
int FileManager::getNextNewFileId()
{
	if (m_newFileIdList.isEmpty())
	{
		return 0;
	}

	int index = 0;
	bool isFind = false;

	for (int i = 0; i < m_newFileIdList.size(); ++i)
	{
		if (m_newFileIdList.at(i).index > i)
		{
			index = i;
			isFind = true;
			break;
		}
	}

	if (!isFind)
	{
		index = m_newFileIdList.size();
	}

	return index;
}

//务必要保证不能重复id，所以newnode后，必须接着调用insertNewFileNode，避免重复了
void FileManager::insertNewFileNode(NewFileIdMgr node)
{
	m_newFileIdList.append(node);
	std::sort(m_newFileIdList.begin(), m_newFileIdList.end(), [](NewFileIdMgr& a, NewFileIdMgr& b) {
		return a.index < b.index;
	});
}

//删除newfile id的节点
void FileManager::delNewFileNode(int fileIndex)
{
	for (int i = 0; i < m_newFileIdList.size(); ++i)
	{
		if (m_newFileIdList.at(i).index == fileIndex)
		{
			m_newFileIdList.removeAt(i);
			break;
		}
	}
}

//和loadFileDataInText类似，但是不是从头开始读取文件，而是从startReadSize开始
//不检查编码，直接按照fileTextCode进行读取
int FileManager::loadFileDataInTextFromOffset(ScintillaEditView* editView, QString filePath, CODE_ID fileTextCode, QWidget* msgBoxParent, quint64 startReadSize)
{
	QFile file(filePath);

	//如果文件不存在，直接返回
	if (!file.exists())
	{
		return -1;
	}

	QFlags<QFileDevice::Permission> power = QFile::permissions(filePath);

	//直接以只读的方式打开，至于能不能保存，是保存时需要考虑的问题。
	//只需要在保存的时候获取admin权限即可
	QIODevice::OpenMode mode;

	mode = QIODevice::ExistingOnly | QIODevice::ReadOnly;

	if (!file.open(mode))
	{
		QMessageBox::warning(msgBoxParent, tr("Error"), tr("Open File %1 failed").arg(filePath));
		return 2;
	}

	quint64 fileSize = file.size();

	//如果文件是空的。检查一下，有可能在临时文件损坏情况下出现，外面需要使用
	if (fileSize == 0)
	{
		file.close();
		return 0;
	}

	//如果读取的内容，超过了当前文件大小，则直接返回。这里是返回0，视作成功，没有新内容要读
	if (startReadSize >= fileSize)
	{
		file.close();
		return 0;
	}

	QByteArray bytes;

	if (file.seek(startReadSize))
	{
		//读取后面所有的内容
		bytes = file.readAll();
	}

	file.close();

	QString text;

	Encode::tranStrToUNICODE(fileTextCode, bytes.data(), bytes.count(), text);
	
	editView->append(text);

	return 0;
}

#if 0

//这里是以文本方式加载文件。但是可能遇到的是二进制文件，里面会做判断
//二进制时hexAsk是否询问，当用户指定打开格式时，不需要询问
//MsgBoxParent::尽量把这个给一下，让MsgBox有图标，不那么难看。
int FileManager::loadFileDataInText(ScintillaEditView* editView, QString filePath, CODE_ID& fileTextCode, RC_LINE_FORM& lineEnd,CCNotePad * callbackObj, bool hexAsk, QWidget* msgBoxParent)
{
	QFile file(filePath);

	//如果文件不存在，直接返回
	if (!file.exists())
	{
		return -1;
	}

	QFlags<QFileDevice::Permission> power = QFile::permissions(filePath);

	//直接以只读的方式打开，至于能不能保存，是保存时需要考虑的问题。
	//只需要在保存的时候获取admin权限即可
	QIODevice::OpenMode mode;
	
	mode = QIODevice::ExistingOnly | QIODevice::ReadOnly;

	if (!file.open(mode))
	{
		qDebug() << file.error();
#ifdef Q_OS_WIN
		//打开失败，这里一般是权限问题导致。如果是windows，在外面申请权限后继续处理
		if (QFileDevice::OpenError == file.error())
			{
			if (callbackObj != nullptr)
			{
				return callbackObj->runAsAdmin(filePath);
			}
			return 1;
			}
#endif
#ifdef Q_OS_UNIX
		QMessageBox::warning(msgBoxParent, tr("Error"), tr("Open File %1 failed").arg(filePath));
#endif
		return 2;
	}

	qint64 fileSize = file.size();

	//如果文件是空的。检查一下，有可能在临时文件损坏情况下出现，外面需要使用
	if (fileSize == 0)
	{
		m_lastErrorCode = ERROR_TYPE::OPEN_EMPTY_FILE;
		file.close();
		return 0;
	}

	qint64 bufferSizeRequested = fileSize + qMin((qint64)(1 << 20), (qint64)(fileSize / 6));

	if (bufferSizeRequested > INT_MAX)
	{
		QMessageBox::warning(msgBoxParent, tr("Error"), tr("File is too big to be opened by Notepad--"));
		file.close();
		return 3;
	}

	QList<LineFileInfo> outputLineInfoVec;

	int maxLineSize = 0;
	int charsNums = 0;
	bool isHexFile = false;

	fileTextCode = CmpareMode::scanFileOutPut(fileTextCode,filePath, outputLineInfoVec, maxLineSize, charsNums, isHexFile);

	//如果文件是空的。检查一下，有可能在临时文件损坏情况下出现，外面需要使用
	if (charsNums == 0)
	{
		m_lastErrorCode = ERROR_TYPE::OPEN_EMPTY_FILE;
		file.close();
		return 0;
	}

	if (isHexFile && hexAsk)
	{
		//检测到文件很可能是二进制文件，询问用户，是否以二进制加载
		int ret = QMessageBox::question(msgBoxParent, tr("Open with Text or Hex? [Exist Garbled Code]"), tr("The file %1 is likely to be binary. Do you want to open it in binary?").arg(filePath), tr("Hex Open"), tr("Text Open"), tr("Cancel"));
		
		if (ret == 0)
		{
			//16进制打开
			file.close();
			return 4;
		}
		else if (ret == 1)
		{
			//继续以文本打开
	}
		else
		{
			//取消，不打开
			file.close();
			return 2;
		}
	}

	if (maxLineSize > 0)
	{
		//int textWidth = editView->execute(SCI_TEXTWIDTH, STYLE_DEFAULT, reinterpret_cast<sptr_t>("P"));
		editView->execute(SCI_SETSCROLLWIDTH, maxLineSize*10);
	}

	//以第一行的换行为文本的换行符
	lineEnd = UNKNOWN_LINE;

	if (!outputLineInfoVec.isEmpty())
	{
		lineEnd = static_cast<RC_LINE_FORM>(outputLineInfoVec.at(0).lineEndFormat);
	}

	if (lineEnd == UNKNOWN_LINE)
	{
#ifdef _WIN32
		lineEnd = DOS_LINE;
#else
		lineEnd = UNIX_LINE;
#endif
	}


	QString text;
	text.reserve(charsNums + 1);

	for (QList<LineFileInfo>::iterator it = outputLineInfoVec.begin(); it != outputLineInfoVec.end(); ++it)
	{
		text.append(it->unicodeStr);
	}

	file.close();


	//优先根据文件后缀来确定其语法风格
	LexerInfo lxdata = CCNotePad::getLangLexerIdByFileExt(filePath);

	if (lxdata.lexerId != L_TXT)
	{
		QsciLexer* lexer = editView->createLexer(lxdata.lexerId, lxdata.tagName);
		editView->setLexer(lexer);
	}
	else
	{
	//利用前面5行，进行一个编程语言的判断
	QString headContens;

	for (int i = 0; (i < outputLineInfoVec.size() && i < 5); ++i)
	{
		headContens.append(outputLineInfoVec.at(i).unicodeStr);
	}


	std::string headstr = headContens.toStdString();

	LangType _language = detectLanguageFromTextBegining((const unsigned char *)headstr.data(), headstr.length());

		if (_language >= 0 && _language < L_EXTERNAL)
	{
		QsciLexer* lexer = editView->createLexer(_language);
		editView->setLexer(lexer);
	}
	}
	
	//如果检测到时16进制文件，但是强行以二进制打开，则有限走setUtf8Text。
	if (!isHexFile)
	{
	editView->setText(text);
	}
	else
	{
		//20230203有github用户反馈，说存在乱码的文件被截断，所以后续还是不走截断
		editView->setText(text);

		return 6;
#if 0
		//这种情况，为了不编辑二进制模式，是可能只读的。
		if (1 == editView->setUtf8Text(text))
		{
			return 5;//只读模式
		}
#endif
	}
	
	return 0;
}

#endif

//20230304新增加：不再一行一行读取文件，而是一次性读取到内存，加快文本文件打开时的处理速度
//这里是以文本方式加载文件。但是可能遇到的是二进制文件，里面会做判断
//二进制时hexAsk是否询问，当用户指定打开格式时，不需要询问
//MsgBoxParent::尽量把这个给一下，让MsgBox有图标，不那么难看。
int FileManager::loadFileDataInText(ScintillaEditView* editView, QString filePath, CODE_ID& fileTextCode, RC_LINE_FORM& lineEnd, CCNotePad* callbackObj, bool hexAsk, QWidget* msgBoxParent)
{
	QFile file(filePath);

	//如果文件不存在，直接返回
	if (!file.exists())
	{
		return -1;
	}

	QFlags<QFileDevice::Permission> power = QFile::permissions(filePath);

	//直接以只读的方式打开，至于能不能保存，是保存时需要考虑的问题。
	//只需要在保存的时候获取admin权限即可
	QIODevice::OpenMode mode;

	mode = QIODevice::ExistingOnly | QIODevice::ReadOnly;

	if (!file.open(mode))
	{
		qDebug() << file.error();
#ifdef Q_OS_WIN
		//打开失败，这里一般是权限问题导致。如果是windows，在外面申请权限后继续处理
		if (QFileDevice::OpenError == file.error())
		{
			if (callbackObj != nullptr)
			{
				return callbackObj->runAsAdmin(filePath);
			}
		return 1;
	}
#endif
#ifdef Q_OS_UNIX
		QMessageBox::warning(msgBoxParent, tr("Error"), tr("Open File %1 failed").arg(filePath));
#endif
		return 2;
	}

	qint64 fileSize = file.size();

	//如果文件是空的。检查一下，有可能在临时文件损坏情况下出现，外面需要使用
	if (fileSize == 0)
	{
		m_lastErrorCode = ERROR_TYPE::OPEN_EMPTY_FILE;
		file.close();
		return 0;
	}

	qint64 bufferSizeRequested = fileSize + qMin((qint64)(1 << 20), (qint64)(fileSize / 6));

	if (bufferSizeRequested > INT_MAX)
	{
		QMessageBox::warning(msgBoxParent, tr("Error"), tr("File is too big to be opened by Notepad--"));
		file.close();
		return 3;
	}

	QString fileText;
	bool isErrorCode = false;

	fileTextCode = CmpareMode::scanFileOutPut(file, fileTextCode, filePath, fileText, isErrorCode);

	//如果文件是空的。检查一下，有可能在临时文件损坏情况下出现，外面需要使用
	if (fileText.size() == 0)
	{
		m_lastErrorCode = ERROR_TYPE::OPEN_EMPTY_FILE;
		file.close();
		return 0;
	}

	if (isErrorCode && hexAsk)
	{
		//检测到文件很可能是二进制文件，询问用户，是否以二进制加载
		int ret = QMessageBox::question(msgBoxParent, tr("Open with Text or Hex? [Exist Garbled Code]"), tr("The file %1 is likely to be binary. Do you want to open it in binary?").arg(filePath), tr("Hex Open"), tr("Text Open"), tr("Cancel"));

		if (ret == 0)
		{
			//16进制打开
			file.close();
			return 4;
		}
		else if (ret == 1)
		{
			//继续以文本打开
		}
		else
		{
			//取消，不打开
			file.close();
			return 2;
		}
	}

	//以第一行的换行为文本的换行符。暂时只考虑win unix 。mac \r 已经淘汰，暂时不管
	lineEnd = RC_LINE_FORM::UNKNOWN_LINE;

	int pos = fileText.indexOf("\n");
	if (pos >=1)
	{
		if (fileText[pos - 1] == QChar('\r'))
		{
			lineEnd = RC_LINE_FORM::DOS_LINE;
		}
		else
		{
			lineEnd = RC_LINE_FORM::UNIX_LINE;
		}
	}

	if (lineEnd == UNKNOWN_LINE)
	{
#ifdef _WIN32
		lineEnd = DOS_LINE;
#else
		lineEnd = UNIX_LINE;
#endif
	}

	file.close();


	//优先根据文件后缀来确定其语法风格
	LexerInfo lxdata = CCNotePad::getLangLexerIdByFileExt(filePath);

	if (lxdata.lexerId != L_TXT)
	{
		QsciLexer* lexer = editView->createLexer(lxdata.lexerId, lxdata.tagName);
		editView->setLexer(lexer);
	}
	else
	{
		//利用前面100个字符，进行一个编程语言的判断
		QString headContens = fileText.mid(0, 100);

		LangType _language = detectLanguage(headContens, filePath);

		if (_language >= 0 && _language < L_EXTERNAL)
		{
			QsciLexer* lexer = editView->createLexer(_language);
			editView->setLexer(lexer);
		}
	}

	//如果检测到时16进制文件，但是强行以二进制打开，则有限走setUtf8Text。
	if (!isErrorCode)
	{
		editView->setText(fileText);
	}
	else
	{
		//20230203有github用户反馈，说存在乱码的文件被截断，所以后续还是不走截断
		editView->setText(fileText);

		return 6;
	}

	return 0;
}


//加载文件，只为查找使用
int FileManager::loadFileForSearch(ScintillaEditView* editView, QString filePath)
{
	QFile file(filePath);

	QFlags<QFileDevice::Permission> power = QFile::permissions(filePath);

	QIODevice::OpenMode mode;

	if (!power.testFlag(QFile::WriteUser))
	{
		//文件不能写
		mode = QIODevice::ExistingOnly | QIODevice::ReadOnly;
	}
	else
	{
		mode = QIODevice::ExistingOnly | QIODevice::ReadWrite;
	}

	if (!file.open(mode))
	{
		//qDebug() << file.error();
		return 2;
	}

	qint64 fileSize = file.size();

	qint64 bufferSizeRequested = fileSize + qMin((qint64)(1 << 20), (qint64)(fileSize / 6));

	if (bufferSizeRequested > INT_MAX)
	{
		file.close();
		return 3;
	}

	bool existGrbledCode = false;
	QString outText;
	CODE_ID fileTextCode = CODE_ID::UNKOWN;

	fileTextCode = CmpareMode::scanFileOutPut(file, fileTextCode, filePath, outText, existGrbledCode);

	//20230218 这里必须指明一下编码，否则后续会导致编码被修改
	editView->setProperty(Edit_Text_Code, fileTextCode);

	if (existGrbledCode)
	{
		//qDebug() << filePath;
		file.close();
		return 4;
	}
	file.close();

	editView->setText(outText);

	return 0;
}

const int ONE_PAGE_BYTES = 4096;

//加载下一页或者上一页。(二进制模式）
int  FileManager::loadFilePreNextPage(int dir, QString& filePath, HexFileMgr* & hexFileOut)
{
	if (m_hexFileMgr.contains(filePath))
	{
		hexFileOut = m_hexFileMgr.value(filePath);

		//小于LITTLE_FILE_MAX的已经一次性全部在内存，没有上下页可以翻到
		if (hexFileOut->onetimeRead)
		{
			return 1;
		}
		qint64 pos = hexFileOut->fileOffset;

		if (dir == 1 && (pos >= 0))
		{
			//上一页
			pos = pos - hexFileOut->contentRealSize - ONE_PAGE_BYTES;
			if (pos < 0)
			{
				pos = 0;
			}
		}
		else if(dir == 2 && (pos < hexFileOut->fileSize))
		{
		
		}
		else
		{
			return 1;//没有上下页，已经是最后一页或最前一页
		}

		char* buf = new char[ONE_PAGE_BYTES+1];

		hexFileOut->file->seek(pos);
		qint64 ret = hexFileOut->file->read(buf, ONE_PAGE_BYTES);
		if (ret <= 0)
		{
			return -1;
		}
		else
		{
			//读取成功
			hexFileOut->fileOffset = hexFileOut->file->pos();

			if (hexFileOut->contentBuf != nullptr)
			{
				delete[]hexFileOut->contentBuf;
			}

			hexFileOut->contentBuf = buf;
			hexFileOut->contentRealSize = ret;
		}
		return 0;
	}
	return -1;
}

const int ONE_PAGE_TEXT_SIZE = 1000 * 1024;

//加载下一页或者上一页。(文本模式）
//返回值：0表示成功
int  FileManager::loadFilePreNextPage(int dir, QString& filePath, TextFileMgr* & textFileOut)
{
	if (m_bigTxtFileMgr.contains(filePath))
	{
		textFileOut = m_bigTxtFileMgr.value(filePath);

		qint64 pos = textFileOut->fileOffset;

		int canReadSize = 0;

		if (dir == 1 && (pos >= 0))
		{
			//读取上一页
			pos = pos - textFileOut->contentRealSize - ONE_PAGE_TEXT_SIZE;
			if (pos < 0)
			{
				//前面的内容不足以ONE_PAGE_TEXT_SIZE字节
				canReadSize = textFileOut->fileOffset - textFileOut->contentRealSize;
				if (canReadSize <= 0)
				{
					return 1;
				}
				pos = 0;
			}
			else
			{
				canReadSize = ONE_PAGE_TEXT_SIZE;
			}
		}
		else if (dir == 2 && (pos < textFileOut->fileSize))
		{
			canReadSize = ONE_PAGE_TEXT_SIZE;
		}
		else
		{
			return 1;
		}

		char* buf = new char[canReadSize + 1];
		buf[canReadSize] = '\0';

		textFileOut->file->seek(pos);
		qint64 ret = textFileOut->file->read(buf, canReadSize);
		if (ret <= 0)
		{
			return -1;
		}
		else
		{
			//读取成功
			//如果是往后读取
			if (dir == 2)
			{
				//读取了1M的内容，从内容尾部往前查找，找到第一个换行符号。如果没有怎么办？说明是一个巨长的行，不妙
				//如果是巨长的行，一行超过ONE_PAGE_TEXT_SIZE（1M),则可能存在单个字符截断的可能。
				buf[ret] = '\0';

				int preLineEndPos = 0;
				
				if (textFileOut->file->pos() < textFileOut->fileSize)//反之已经到尾部了，不需要往前找行首了
				{
					preLineEndPos = findLineEndPos(buf, ret, (CODE_ID)textFileOut->loadWithCode);
					if (preLineEndPos > 0)
					{
						//给后面的字符填\0，让字符串正常结尾\0
						buf[ret - preLineEndPos] = '\0';
					}
				}

				textFileOut->fileOffset = textFileOut->file->pos() -preLineEndPos;
				if (preLineEndPos > 0)
				{
					//文件seek到下一行的首位置。
					textFileOut->file->seek(textFileOut->fileOffset);
				}

				if (textFileOut->contentBuf != nullptr)
				{
					delete[]textFileOut->contentBuf;
				}

				textFileOut->contentBuf = buf;
				textFileOut->contentRealSize = ret - preLineEndPos;
			}
			else if (dir == 1)
			{
				//如果是往前读取
				//读取了1M的内容，从内容前面往后查找，找到第一个换行符号。如果没有怎么办？说明是一个巨长的行，不妙
				buf[ret] = '\0';

				int preLineStartPos = 0;

				if (textFileOut->file->pos() > canReadSize)//==canReadSize说明已经在文件最前面了。不在最前面，需要
				{
					preLineStartPos = findLineStartPos(buf, ret, (CODE_ID)textFileOut->loadWithCode);
					if (preLineStartPos > 0)
					{
						//把\n前面的内容去掉，通过内存move的方式。
						memmove(buf, buf+preLineStartPos,ret - preLineStartPos);
						buf[ret - preLineStartPos] = '\0';
					}
				}

				textFileOut->fileOffset = textFileOut->file->pos();

				if (textFileOut->contentBuf != nullptr)
				{
					delete[]textFileOut->contentBuf;
				}

				textFileOut->contentBuf = buf;
				textFileOut->contentRealSize = ret - preLineStartPos;
			}
		}
		return 0;
	}
	return -1;
}

//从指定地址开始加载文件
int FileManager::loadFileFromAddr(QString filePath, qint64 addr, HexFileMgr* & hexFileOut)
{
	if (m_hexFileMgr.contains(filePath))
	{
		hexFileOut = m_hexFileMgr.value(filePath);

		//超过文件大小
		if (addr < 0 || addr >= hexFileOut->fileSize)
		{
			return -2;
		}

		//4K对齐
		addr &= 0xfffffffffff0;

		char* buf = new char[ONE_PAGE_BYTES + 1];
		hexFileOut->file->seek(addr);

		qint64 ret = hexFileOut->file->read(buf, ONE_PAGE_BYTES);
		if (ret <= 0)
		{
			return -1;
		}
		else
		{
			//读取成功
			hexFileOut->fileOffset = hexFileOut->file->pos();

			if (hexFileOut->contentBuf != nullptr)
			{
				delete[]hexFileOut->contentBuf;
			}

			hexFileOut->contentBuf = buf;
			hexFileOut->contentRealSize = ret;
		}
		return 0;
	}

	return -1;
}

//从指定地址开始加载文本文件
int FileManager::loadFileFromAddr(QString filePath, qint64 addr, TextFileMgr* & textFileOut)
{
	if (m_bigTxtFileMgr.contains(filePath))
	{
		textFileOut = m_bigTxtFileMgr.value(filePath);

		//超过文件大小
		if (addr < 0 || addr >= textFileOut->fileSize)
		{
			return -2;
		}

		char* buf = new char[ONE_PAGE_TEXT_SIZE + 1];
		buf[ONE_PAGE_TEXT_SIZE] = '\0';

		textFileOut->file->seek(addr);

		qint64 ret = textFileOut->file->read(buf, ONE_PAGE_TEXT_SIZE);
		if (ret <= 0)//-1是出错。0也是没有读到
		{
			return -1;
		}
		else
		{
		
			int preLineEndPos = 0;
			buf[ret] = '\0';

			if (textFileOut->file->pos() < textFileOut->fileSize)//反之已经到尾部了，不需要往前找行了
			{
				preLineEndPos = findLineEndPos(buf, ret, (CODE_ID)textFileOut->loadWithCode);
				if (preLineEndPos > 0)
				{
					//给后面的字符填\0，让字符串正常结尾\0
					buf[ret - preLineEndPos] = '\0';
				}
			}

			//如果本来就在开头开始，则不需要计算findLineStartPos
			int preLineStartPos = 0;

			if (addr == 0)
			{

			}
			else
			{
				preLineStartPos = findLineStartPos(buf, ret, (CODE_ID)textFileOut->loadWithCode);
			if (preLineStartPos > 0 && preLineStartPos < ret) //preLineStartPos如果大于ret，则全部都被跳过了，不会显示，是个特例
			{
				memmove(buf, buf + preLineStartPos, ret - preLineStartPos);
				buf[ret - preLineStartPos] = '\0';
			}
			else
			{
				//如果没做调整，则后续不需要偏移，这里必须preLineStartPos赋0值
				preLineStartPos = 0;
			}
			}

			//只需要文件调到上一行的行位即可。
			textFileOut->fileOffset = textFileOut->file->pos() - preLineEndPos;
			if (preLineEndPos > 0)
			{
				//文件seek到下一行的首位置。
				textFileOut->file->seek(textFileOut->fileOffset);
			}

			if (textFileOut->contentBuf != nullptr)
			{
				delete[]textFileOut->contentBuf;
			}

			textFileOut->contentBuf = buf;
			textFileOut->contentRealSize = ret - preLineEndPos - preLineStartPos;
		}
		return 0;
	}

	return -1;
}

//加载二进制文件。从curPos开始，每行16个byte，每次读取64行，一共1024个byte
bool FileManager::loadFileData(QString filePath, HexFileMgr* & hexFileOut)
{
	QFile *file = new QFile(filePath);

	if (!file->open(QIODevice::ReadOnly | QIODevice::ExistingOnly))
	{
		return false;
	}



	int readBytes = 0;


	if (file->size() <= LITTLE_FILE_MAX)
	{
		readBytes = LITTLE_FILE_MAX;
	}
	else
	{
		//对于大于10K的文件，每次只读4K
		readBytes = ONE_PAGE_BYTES;
	}

	char* buf = new char[readBytes];
	qint64 ret = file->read(buf, readBytes);
	if (ret == -1)
	{
		//错误
		file->close();
		delete file;
		return false;
	}
	else
	{
		HexFileMgr* hexFile = nullptr;

		if (!m_hexFileMgr.contains(filePath))
		{
			hexFile = new HexFileMgr();
			hexFile->filePath = filePath;
			hexFile->file = file;
			hexFile->fileOffset = file->pos();
			hexFile->fileSize = file->size();
			hexFile->contentBuf = buf;
			hexFile->contentRealSize = ret;
			hexFile->onetimeRead = (file->size() <= LITTLE_FILE_MAX);
			m_hexFileMgr.insert(filePath, hexFile);
		}
		else
		{
			//理论上这里永远不走
			hexFile = m_hexFileMgr.value(filePath);
			hexFile->fileOffset = file->pos();
			hexFile->contentBuf = buf;
			hexFile->contentRealSize = ret;
			hexFile->onetimeRead = (file->size() <= LITTLE_FILE_MAX);
		}

		hexFileOut = hexFile;

		return true;
	}

	return false;
}

//加载大文本文件。从0开始读取ONE_PAGE_TEXT_SIZE 500K的内容
bool FileManager::loadFileData(QString filePath, TextFileMgr* & textFileOut, RC_LINE_FORM & lineEnd)
{
	QFile *file = new QFile(filePath);

	if (!file->open(QIODevice::ReadOnly | QIODevice::ExistingOnly))
	{
		return false;
	}

	int readBytes = ONE_PAGE_TEXT_SIZE;

	char* buf = new char[ONE_PAGE_TEXT_SIZE+1];
	buf[ONE_PAGE_TEXT_SIZE] = '\0';

	qint64 ret = file->read(buf, readBytes);
	if (ret <= 0)
	{
		//错误
		file->close();
		delete file;
		return false;
	}
	else
	{
		//读取了1M的内容，从尾部往找，找到第一个换行符号。如果没有怎么办？说明是一个巨长的行，不妙
		buf[ret] = '\0';

		//检测是否为unicode_le编码，要特殊对待。
		//bool isUnLeCode = CmpareMode::isUnicodeLeBomFile((uchar*)buf, 2);

		CODE_ID code = CmpareMode::getTextFileEncodeType((uchar*)buf, ret, filePath, true);

		int preLineEndPos = findLineEndPos(buf,ret, code);
		if (preLineEndPos > 0)
		{
			//给后面的字符填\0，让字符串正常结尾\0
			buf[ret - preLineEndPos] = '\0';

			if (ret - preLineEndPos >= 2)
			{
				if (buf[ret - preLineEndPos - 1] == '\n' && buf[ret - preLineEndPos - 2] == '\r')
				{
					lineEnd = DOS_LINE;
		}
				else if (buf[ret - preLineEndPos - 1] == '\n')
				{
					lineEnd = UNIX_LINE;
				}
				else if (buf[ret - preLineEndPos - 1] == '\r')
				{
					lineEnd = MAC_LINE;
				}
			}
		}

		TextFileMgr* txtFile = nullptr;

		if (!m_bigTxtFileMgr.contains(filePath))
		{
			txtFile = new TextFileMgr();
			txtFile->loadWithCode = code;
			
			txtFile->filePath = filePath;
			txtFile->file = file;
			txtFile->fileOffset = file->pos() - preLineEndPos;
			if (preLineEndPos > 0)
			{
				//文件seek到下一行的首位置。下次读的时候，头部肯定是一行的行首啦
				file->seek(txtFile->fileOffset);
			}
			txtFile->fileSize = file->size();
			txtFile->contentBuf = buf;
			txtFile->contentRealSize = ret - preLineEndPos;
			m_bigTxtFileMgr.insert(filePath, txtFile);
		}
		else
		{
			//理论上这里永远不走
			assert(false);
			txtFile = m_bigTxtFileMgr.value(filePath);
			txtFile->fileOffset = file->pos();
			txtFile->contentBuf = buf;
			txtFile->contentRealSize = ret;
		}

		textFileOut = txtFile;

		return true;
	}

	return false;
}

//返回行的数量
int getLineNumInBuf(char* buf, int size, CODE_ID code = UNKOWN)
{
	int lineNums = 0;

	for (int i = 0; i < size; ++i)
	{
		if (buf[i] == '\n')
		{
			////如果是LE，还要确定\n的下一个是否是\0
			if ((code == UNICODE_LE) && ((i != size - 1) && (buf[i + 1] != '\0')))
			{
				continue;
			}
			//如果是BE,简单\0\n是否连续存在，不能单纯检查\n,还有确定\n的前一个是不是\0
			if ((code == UNICODE_BE) && ((i != 0) && (buf[i - 1] != '\0')))
			{
				continue;
			}
			++lineNums;
		}
	}

	//如果没有找到，怀疑是mac格式，按照\r结尾解析
	if (lineNums == 0)
	{
		for (int i = 0; i < size; ++i)
		{
			if (buf[i] == '\r')
			{
				////如果是LE，还要确定\n的前面一个是否是\0
				if ((code == UNICODE_LE) && ((i != size - 1) && (buf[i + 1] != '\0')))
				{
					continue;
				}
				//如果是BE,简单\0\n是否连续存在，不能单纯检查\n,还有确定\n的前一个是不是\0
				if ((code == UNICODE_BE) && ((i != 0) && (buf[i - 1] != '\0')))
				{
					continue;
				}
				++lineNums;
			}
		}
	}

	return lineNums;
}

//创建大文件编辑模式的索引文件。0 成功，-1取消
int FileManager::createBlockIndex(BigTextEditFileMgr* txtFile)
{
	//每次filePtr 4M的速度进行建块
	qint64 fileSize = txtFile->file->size();

	qint64 curOffset = 0;

	uchar* curPtr = txtFile->filePtr;

	//检测是否为unicode_le编码，要特殊对待。
	//bool isUnLeCode = CmpareMode::isUnicodeLeBomFile(curPtr, 2);

	CODE_ID code = CmpareMode::getTextFileEncodeType(curPtr, fileSize, txtFile->filePath, true);
	txtFile->loadWithCode = code;
	
	const int blockBytes = BigTextEditFileMgr::BLOCK_SIZE * 1024 * 1024;

	int lineEndPos = 0;

	int steps = fileSize / blockBytes;

	txtFile->blocks.reserve(steps + 10);

	ProgressWin* loadFileProcessWin = nullptr;
	
	if (steps > 200)
	{
		loadFileProcessWin = new ProgressWin(nullptr);

		loadFileProcessWin->setWindowModality(Qt::ApplicationModal);

		loadFileProcessWin->info(tr("load bit text file tree in progress\nfile size %1, please wait ...").arg(tranFileSize(fileSize)));

		loadFileProcessWin->setTotalSteps(steps);

		loadFileProcessWin->show();
	}

	quint32 lineNumStart = 0;
	quint32 lineNum = 0;

	while ((curOffset + blockBytes) < fileSize)
	{
		BlockIndex bi;
		bi.fileOffset = curOffset;

		curOffset += blockBytes;

		lineEndPos = findLineEndPos((char*)curPtr+ bi.fileOffset, blockBytes, code);

		bi.fileSize = blockBytes - lineEndPos;

		lineNum = getLineNumInBuf((char*)curPtr + bi.fileOffset, bi.fileSize, code);

		curOffset -= lineEndPos;

		bi.lineNum = lineNum;
		bi.lineNumStart = lineNumStart;

		lineNumStart += lineNum;

		txtFile->blocks.append(bi);

		if (loadFileProcessWin != nullptr)
		{
			if (loadFileProcessWin->isCancel())
			{
				delete loadFileProcessWin;
				txtFile->blocks.clear();
				return -1;
			}
			loadFileProcessWin->moveStep(true);
		}
		
	}
	//最后一块
	int lastBlockBytes = fileSize - curOffset;

	BlockIndex bi;
	bi.fileOffset = curOffset;

	curOffset += lastBlockBytes;

	bi.fileSize = lastBlockBytes;

	lineNum = getLineNumInBuf((char*)curPtr + bi.fileOffset, bi.fileSize);

	bi.lineNum = lineNum;
	bi.lineNumStart = lineNumStart;

	txtFile->blocks.append(bi);

	if (loadFileProcessWin != nullptr)
	{
		delete loadFileProcessWin;
	}

	return 0;
}

//加载大文件，以索引的方式打开大文件
bool FileManager::loadFileDataWithIndex(QString filePath, BigTextEditFileMgr*& textFileOut)
{
	QFile* file = new QFile(filePath);
	file->open(QIODevice::ReadOnly);


	uchar* filePtr = file->map(0, file->size());

	BigTextEditFileMgr* txtFile = nullptr;

	if (!m_bigTxtEditFileMgr.contains(filePath))
	{
		txtFile = new BigTextEditFileMgr();
		txtFile->filePath = filePath;
		txtFile->file = file;
		txtFile->filePtr = filePtr;
		textFileOut = txtFile;

		if (-1 == createBlockIndex(txtFile))
		{
			//取消。
			delete txtFile;
			txtFile = nullptr;
			return false;
		}
		m_bigTxtEditFileMgr.insert(filePath, txtFile);
	}
	else
	{
		//理论上这里永远不走
		assert(false);
	}

	return true;
}

BigTextEditFileMgr* FileManager::getBigFileEditMgr(QString filepath)
{
	if (m_bigTxtEditFileMgr.contains(filepath))
	{
		return m_bigTxtEditFileMgr.value(filepath);
	}

	return nullptr;
}

TextFileMgr* FileManager::getSuperBigFileMgr(QString filepath)
{
	if (m_bigTxtFileMgr.contains(filepath))
	{
		return m_bigTxtFileMgr.value(filepath);
	}

	return nullptr;
}

int FileManager::getBigFileBlockId(QString filepath, quint32 lineNum)
{
	BigTextEditFileMgr* v = m_bigTxtEditFileMgr.value(filepath);

	for (int i = 0, s = v->blocks.size(); i < s; ++i)
	{
		const BlockIndex& k = v->blocks.at(i);
		if (lineNum >= k.lineNumStart && lineNum < (k.lineNumStart + k.lineNum))
		{
			return i;
		}
	}
	return -1;
}

HexFileMgr * FileManager::getHexFileHand(QString filepath)
{
	if (m_hexFileMgr.contains(filepath))
	{
		return m_hexFileMgr.value(filepath);
	}

	return nullptr;
}

void FileManager::closeHexFileHand(QString filepath)
{
	if (m_hexFileMgr.contains(filepath))
	{
		HexFileMgr* v = m_hexFileMgr.value(filepath);
		v->destory();
		delete v;
		m_hexFileMgr.remove(filepath);
	}
}

void FileManager::closeSuperBigTextFileHand(QString filepath)
{
	if (m_bigTxtFileMgr.contains(filepath))
	{
		TextFileMgr* v = m_bigTxtFileMgr.value(filepath);
		v->destory();
		delete v;
		m_bigTxtFileMgr.remove(filepath);
	}
}

void FileManager::closeBigTextRoFileHand(QString filepath)
{
	if (m_bigTxtEditFileMgr.contains(filepath))
	{
		BigTextEditFileMgr* v = m_bigTxtEditFileMgr.value(filepath);
		v->destory();
		delete v;
		m_bigTxtEditFileMgr.remove(filepath);
	}
}

//初步检查文件的编程语言。两个标准： 1 文件头部标签 2 文件特定名称
LangType detectLanguage(QString& headContent, QString& filepath)
{
	struct FirstLineLanguages
	{
		QString pattern;
		LangType lang;
	};

	const size_t FIRST_LINE_LANGUAGES = 5;
	FirstLineLanguages languages[FIRST_LINE_LANGUAGES] = {
		{ "<?xml",			L_XML },
		{ "<?php",			L_PHP },
		{ "<html",			L_HTML },
		{ "<!DOCTYPE html",	L_HTML },
		{ "<?",				L_PHP }
	};

	int foundPos = -1;
	for (int i = 0; i < FIRST_LINE_LANGUAGES; ++i)
	{
		foundPos = headContent.indexOf(languages[i].pattern);
		if (foundPos == 0)
		{
			return languages[i].lang;
		}
	}

	const size_t NAME_CHECK_LANGUAGES = 3;
	FirstLineLanguages NAME_LEXER[NAME_CHECK_LANGUAGES] = {
		{ "make",			L_MAKEFILE },
		{ "makefile",		L_MAKEFILE },
		{ "CMakeLists",		L_MAKEFILE },
	};

	QFileInfo fi(filepath);
	QString baseName = fi.baseName();

	for (int i = 0; i < NAME_CHECK_LANGUAGES; ++i)
	{
		if (0 == NAME_LEXER[i].pattern.compare(baseName, Qt::CaseInsensitive))
		{
			return NAME_LEXER[i].lang;
		}
	}

	// Unrecognized first line, we assume it is a text file for now
	return L_UNKNOWN;
}
